#ifndef	SAVEPCX_H
#define	SAVEPCX_H

/*
	Save an 8-bit PCX file... Useful for saving screenshots.

	This code is pretty good, I spent quite some time making sure
	the header is filled correctly. I found that many PCX writers
	'forget' certain bytes, such as the palette ID byte.

	Return value: 0 on failure, anything else means success.
*/


int savepcx(char *filename, s_screen *screen, char *pal);


#endif

