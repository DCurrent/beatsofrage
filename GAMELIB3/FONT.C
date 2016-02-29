#include <malloc.h>
#include <stdarg.h>
#include <string.h>

#include "types.h"
#include "screen.h"
#include "loadimg.h"
#include "bitmap.h"
#include "sprite.h"
#include "spriteq.h"


#define		MAX_FONTS		4
#define		FONT_LAYER		0xFFFFFFFF


typedef struct{
	s_sprite *	token[256];
	int		token_width[256];
	int		font_loaded;
}s_font;

s_font fonts[MAX_FONTS];




void font_unload(int which){
	int i;

	which %= MAX_FONTS;

	for(i=0; i<256; i++){
		if(fonts[which].token[i]) free(fonts[which].token[i]);
		fonts[which].token[i] = NULL;
	}
	fonts[which].font_loaded = 0;
}



int font_load(int which, char *filename, char *packfile){
	int x, y;
	int index = 0;
	int size;
	int cx = 0, cy = 0;
	s_bitmap *bitmap;
	s_screen *screen;
	int rval = 0;
	int tw, th;

	which %= MAX_FONTS;

	font_unload(which);

	if((screen = loadscreen(filename, packfile, NULL)) == NULL) goto err;
	tw = screen->width/16;
	th = screen->height/16;
	if(!(bitmap = allocbitmap(tw,th))) goto err;


	// grab tokens
	for(y=0; y<16; y++){
		for(x=0; x<16; x++){
			getbitmap(x*tw, y*th, tw,th, bitmap, screen);
			clipbitmap(bitmap, &cx, NULL, &cy, NULL);
			size = fakey_encodesprite(bitmap);
			fonts[which].token[index] = (s_sprite*)malloc(size);
			if(!fonts[which].token[index]){
				font_unload(which);
				goto err;
			}
			encodesprite(-cx,-cy, bitmap, fonts[which].token[index]);
			fonts[which].token_width[index] = fonts[which].token[index]->width + (tw/10);
			if(fonts[which].token_width[index] <= 1) fonts[which].token_width[index] = tw/3;
			++index;
		}
	}

	rval = 1;
	fonts[which].font_loaded = 1;

err:
	freebitmap(bitmap);
	freescreen(screen);

	return rval;
}




void font_printf(int x, int y, int which, char *format, ...){
	static char b[1024];
	char * buf = b;
	va_list arglist;

	which %= MAX_FONTS;

	if(!fonts[which].font_loaded) return;
	
	va_start(arglist, format);
	vsprintf(buf, format, arglist);
	va_end(arglist);

	while(*buf){
		spriteq_add(x,y, FONT_LAYER, fonts[which].token[*buf], 0, NULL);
		x += fonts[which].token_width[*buf];
		buf++;
	}
}




// Print to a screen rather than queueing the sprites
void screen_printf(s_screen * screen, int x, int y, int which, char *format, ...){
	static char b[1024];
	char * buf = b;
	va_list arglist;
	int ox = x;

	which %= MAX_FONTS;

	if(!fonts[which].font_loaded) return;
	
	va_start(arglist, format);
	vsprintf(buf, format, arglist);
	va_end(arglist);

	while(*buf){
		if(*buf>=32){
			putsprite(x, y, fonts[which].token[*buf], screen);
			x += fonts[which].token_width[*buf];
		}
		else if(*buf=='\n'){
			x = ox;
			y += 10;
		}
		buf++;
	}
}



