#ifndef PACKFILE_H
#define PACKFILE_H


#define	NUMPACKHANDLES	8
#define PACKVERSION	0x00000000

#ifndef		SEEK_SET
#define		SEEK_SET	0
#define		SEEK_CUR	1
#define		SEEK_END	2
#endif

#ifndef		NULL
#define		NULL		0L
#endif


// All of these return -1 on error

int openpackfile(char *filename, char *packfilename);
int readpackfile(int handle, void *buf, int len);
int closepackfile(int handle);
int seekpackfile(int handle, int offset, int whence);


#endif


