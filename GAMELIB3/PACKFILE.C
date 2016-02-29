/*
	Code to read files from packfiles.
	Last update: 21-jun-2002


	============= Format description (a bit cryptical) ================

	dword	4B434150 ("PACK")
	dword	version
	?????	filedata
	{
		dword	structsize
		dword	filestart
		dword	filesize
		bytes	name
	} rep
	dword	headerstart
*/


#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>

#pragma pack (1)

#define		MAXPACKHANDLES		8
#define		PACKMAGIC		0x4B434150
#define		PACKVERSION		0x00000000


// Actual file handles.
static int packhandle[MAXPACKHANDLES] = { -1,-1,-1,-1,-1,-1,-1,-1 };

// Own file pointers and sizes
static unsigned int packfilepointer[MAXPACKHANDLES];
static unsigned int packfilesize[MAXPACKHANDLES];




// Convert slashes (UNIX) to backslashes (DOS).
// Return a pointer to buffer with filename converted to DOS format.
static char * slashback(char *sz){
    int i=0;
    static char new[256];

    while(sz[i] && i<255){
	new[i] = sz[i];
	if(new[i]=='/') new[i]='\\';
	++i;
    }
    new[i] = 0;

    return new;
}




typedef struct pnamestruct{
	unsigned int	pns_len;	// Length of the struct in bytes
	unsigned int	filestart;	// Start position of referenced file
	unsigned int	filesize;	// Size of referenced file
	char		namebuf[80];	// Buffer to hold the file's name
}pnamestruct;


int openpackfile(char *filename, char *packfilename){

    int h;
    int handle;
    unsigned int magic, version, headerstart;
    pnamestruct pn;
    unsigned int p;

    for(h=0; h<MAXPACKHANDLES && packhandle[h]>-1; h++); // Find free handle
    if(h>=MAXPACKHANDLES) return -1;			// No free handles

    // Convert slashes to backslashes
    filename = slashback(filename);

    packfilepointer[h] = 0;


    // Separate file present?
    if((handle=open(filename,O_RDONLY|O_BINARY))!=-1){
	if((packfilesize[h]=lseek(handle,0,SEEK_END))==-1){
	    close(handle);
	    return -1;
	}
	if(lseek(handle,0,SEEK_SET)==-1){
	    close(handle);
	    return -1;
	}      
	packhandle[h] = handle;
	return h;
    }


    // Try to open packfile
    if((handle=open(packfilename,O_RDONLY|O_BINARY))==-1) return -1;

    // Read magic dword ("PACK" identifier)
    if(read(handle,&magic,4)!=4 || magic!=PACKMAGIC){
	close(handle);
	return -1;
    }
    // Read version from packfile
    if(read(handle,&version,4)!=4 || version!=PACKVERSION){
	close(handle);
	return -1;
    }

    // Seek to position of headerstart indicator
    if(lseek(handle,-4,SEEK_END)==-1){
	close(handle);
	return -1;
    }
    // Read headerstart
    if(read(handle,&headerstart,4)!=4){
	close(handle);
	return -1;
    }
    // Seek to headerstart
    if(lseek(handle,headerstart,SEEK_SET)==-1){
	close(handle);
	return -1;
    }

    p = headerstart;

    // Search for filename
    while(read(handle,&pn,sizeof(pn))>12){
	if(stricmp(filename,pn.namebuf)==0){
	    packhandle[h] = handle;
	    packfilesize[h] = pn.filesize;
	    lseek(handle,pn.filestart,SEEK_SET);
	    return h;
	}
	p += pn.pns_len;
	if(lseek(handle,p,SEEK_SET)==-1){
	    close(handle);
	    return -1;
	}
    }
    // Filename not found
    close(handle);
    return -1;
}




int readpackfile(int handle, void *buf, int len){
    int realhandle;

    if(handle<0 || handle>=MAXPACKHANDLES) return -1;
    if(len<0) return -1;
    if(len==0) return 0;

    realhandle = packhandle[handle];
    if(realhandle == -1) return -1;

    if(len + packfilepointer[handle] > packfilesize[handle])
	len = packfilesize[handle] - packfilepointer[handle];

    if((len = read(realhandle,buf,len))==-1) return -1;
    packfilepointer[handle] += len;
    return len;
}



int closepackfile(int handle){
    if(handle<0 || handle>=MAXPACKHANDLES) return -1;
    if(packhandle[handle] == -1) return -1;
    close(packhandle[handle]); 
    packhandle[handle] = -1;
    return 0;
}



int seekpackfile(int handle, int offset, int whence){
    int realhandle;
    int desiredoffs;

    if(handle<0 || handle>=MAXPACKHANDLES) return -1;
    realhandle = packhandle[handle];
    if(realhandle == -1) return -1;

    switch(whence){
	case SEEK_SET:
		desiredoffs = offset;
		if(desiredoffs>packfilesize[handle])
		   desiredoffs = packfilesize[handle];
		else if(desiredoffs<0)
		   desiredoffs = 0;
		break;
	case SEEK_CUR:
		desiredoffs = packfilepointer[handle] + offset;
		if(desiredoffs>packfilesize[handle])
		   desiredoffs = packfilesize[handle];
		else if(desiredoffs<0)
		   desiredoffs = 0;
		break;
	case SEEK_END:
		desiredoffs = packfilesize[handle] + offset;
		if(desiredoffs>packfilesize[handle])
		   desiredoffs = packfilesize[handle];
		else if(desiredoffs<0)
		   desiredoffs = 0;
		break;
	default: return -1;		
    }
    desiredoffs -= packfilepointer[handle];
    if((lseek(realhandle,desiredoffs,SEEK_CUR))==-1) return -1;
    packfilepointer[handle] += desiredoffs;
    return packfilepointer[handle];
}






