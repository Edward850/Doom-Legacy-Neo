#ifndef __FILESRCH_H__
#define __FILESRCH_H__

#include "doomdef.h"
#include "d_netfil.h"

#define MAXSEARCHDEPTH 20

filestatus_t filesearch(char *filename, char *startpath, unsigned char *wantedmd5sum, boolean completepath, int maxsearchdepth);

#endif // __FILESRCH_H__
