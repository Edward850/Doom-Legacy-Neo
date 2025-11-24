#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "filesrch.h"
#include "d_netfil.h"

//
// filesearch:
//
// ATTENTION : make sure there is enouth space in filename to put a full path (255 or 512)
// if needmd5check==0 there is no md5 check
// if changestring then filename will be change with the full path and name
// maxsearchdepth==0 only search given directory, no subdirs
// return FS_NOTFOUND
//        FS_MD5SUMBAD;
//        FS_FOUND

filestatus_t filesearch(char *filename, char *startpath, unsigned char *wantedmd5sum, boolean completepath, int maxsearchdepth)
{
    DIR **dirhandle;
    struct dirent *dent;
    struct stat fstat;
    int found=0;
    char *searchname = strdup( filename);
    filestatus_t retval = FS_NOTFOUND;
    int depthleft=maxsearchdepth;
    char searchpath[1024];
    int *searchpathindex;

    dirhandle = (DIR**) malloc( maxsearchdepth * sizeof( DIR*));
    searchpathindex = (int*) malloc( maxsearchdepth * sizeof( int));
    
    strcpy( searchpath,startpath);
    searchpathindex[--depthleft] = strlen( searchpath) + 1;

    dirhandle[depthleft] = opendir( searchpath);

    if(searchpath[searchpathindex[depthleft]-2] != '/')
    {
        searchpath[searchpathindex[depthleft]-1] = '/';
        searchpath[searchpathindex[depthleft]] = 0;
    }
    else
    {
        searchpathindex[depthleft]--;
    }

    while( (!found) && (depthleft < maxsearchdepth))
    {
        searchpath[searchpathindex[depthleft]]=0;
        dent = readdir( dirhandle[depthleft]);
        if( dent)
        {
            strcpy(&searchpath[searchpathindex[depthleft]],dent->d_name);
        }

        if( !dent)
        {
            closedir( dirhandle[depthleft++]);
        } 
        else if( dent->d_name[0]=='.' &&
             (dent->d_name[1]=='\0' ||
              (dent->d_name[1]=='.' &&
               dent->d_name[2]=='\0')))
        {
            // we don't want to scan uptree
        }
        else if( stat(searchpath,&fstat) < 0) // do we want to follow symlinks? if not: change it to lstat
        {
            // was the file (re)moved? can't stat it
        } 
        else if( S_ISDIR(fstat.st_mode) && depthleft)
        {
            strcpy(&searchpath[searchpathindex[depthleft]],dent->d_name);
            searchpathindex[--depthleft] = strlen(searchpath) + 1;

            if( !(dirhandle[depthleft] = opendir(searchpath)))
            {
                // can't open it... maybe no read-permissions
                // go back to previous dir
                depthleft++;
            }

            searchpath[searchpathindex[depthleft]-1]='/';
            searchpath[searchpathindex[depthleft]]=0;
        }
        else if (!strcasecmp(searchname, dent->d_name))
        {
            switch( checkfilemd5(searchpath, wantedmd5sum))
            {
                case FS_FOUND:
                    if(completepath)
                    {
                        strcpy(filename,searchpath);
                    }
                    else
                    {
                        strcpy(filename,dent->d_name);
                    }
                    retval=FS_FOUND;
                    found=1;
                    break;
                case FS_MD5SUMBAD:
                    retval = FS_MD5SUMBAD;
                    break;
                default:
                    // prevent some compiler warnings
                    break;
            }
        }
    }

    for(; depthleft<maxsearchdepth; closedir(dirhandle[depthleft++]));

    free(searchname);
    free(searchpathindex);
    free(dirhandle);

    return retval;
}
