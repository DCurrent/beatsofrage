/*
	Save an 8-bit PCX file... Useful for saving screenshots.

	This code is pretty good, I spent quite some time making sure
	the header is filled correctly. I found that many PCX writers
	'forget' certain bytes, such as the palette ID byte.

	Last update: 5-feb-2003

	Changes:
	- Used per-line encoding, which is more compatible.
	- Added byte-padding for odd-width images (untested).
*/


#include "types.h"
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>


#define		RLE_BUF_SIZE		2048


#pragma pack (0)


typedef struct pcx_header{
	char	manufacturer;		// Always 0x0A!
	char	version;		// 5?
	char	encoding;		// 1 (RLE-encoded)
	char	bpp;			// 8 (our pics are always 8-bit)
	short	xmin, ymin;		// Always 0
	short	xmax, ymax;		// Size-1
	short	hres, vres;		// Screen resolution (DPI)
	char	lowcolorpal[48];	// Palette for 16-color images, unused
	char	reserved;
	char	colorplanes;		// 1 for 8-bit
	short	bpl;			// Bytes per line, width padded even
	short	paltype;		// 0
	short	hscreensize, vscreensize;
	char	unused2[54];
}pcx_header;




int savepcx(char *filename, s_screen *screen, char *pal){
	int success = 1;
	char *RLEbuf = NULL;
	pcx_header * head = NULL;
	int r, p;
	int blocksize, blockrun;
	int handle = -1;
	char *lineptr;
	int x, y;


	if(!screen) return 0;
	if(!pal) return 0;


	// Allocate buffers
	if((RLEbuf=malloc(RLE_BUF_SIZE))==NULL){
		success = 0;
		goto END;
	}
	if((head=(pcx_header *)malloc(sizeof(pcx_header)))==NULL){
		success = 0;
		goto END;
	}


	// Fill the header

	memset(head, 0, sizeof(pcx_header));
	head->manufacturer = 0x0A;
	head->version = 5;
	head->encoding = 1;
	head->bpp = 8;
	head->xmin = 0;
	head->ymin = 0;
	head->xmax = screen->width - 1;
	head->ymax = screen->height - 1;
	head->hres = screen->width;
	head->vres = screen->height;
	head->hscreensize = screen->width;
	head->vscreensize = screen->height;
	head->colorplanes = 1;
	head->bpl = screen->width+1 & 0xFFFE;
	head->paltype = 0;



	if((handle=open(filename, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0))==NULL){
		success = 0;
		goto END;
	}
	if(write(handle, head, sizeof(pcx_header))!=sizeof(pcx_header)){
		success = 0;
		goto END;
	}


	// Start the coding loop

	y = 0;
	x = 0;
	lineptr = screen->data + y*screen->width;
	while(y < screen->height){

		// To start of RLE buffer
		r = 0;

		while(r<RLE_BUF_SIZE-4 && x<screen->width){
			if(lineptr[x]>=0xC0 || (x<screen->width-1 && lineptr[x+1]==lineptr[x])){
				RLEbuf[r+1] = lineptr[x];
				RLEbuf[r] = 0;
				while(x<screen->width && lineptr[x]==RLEbuf[r+1] && RLEbuf[r]<63){
					++RLEbuf[r];
					++x;
				}
				RLEbuf[r] |= 0xC0;
				r+=2;
			}
			else{
				RLEbuf[r] = lineptr[x];
				++r;
				++x;
			}
		}

		// Reached end of line?
		if(x>=screen->width){
			++y;
			lineptr = screen->data + y*screen->width;
			x = 0;

			// Odd-width byte padding?
			if(screen->width&1){
				RLEbuf[r] = 0;
				++r;
			}
		}

		// Write RLE code to file
		if(write(handle, RLEbuf, r)!=r){
			success = 0;
			goto END;
		}
	}


	// Write palette ID
	p = 0x0C;
	if(write(handle, &p, 1)!=1){
		success = 0;
		goto END;
	}

	// Write palette
	if(write(handle, pal, 768)!=768){
		success = 0;
		goto END;
	}

END:
	if(handle!=-1) close(handle);
	if(RLEbuf) free(RLEbuf);
	if(head) free(head);

	return success;
}





