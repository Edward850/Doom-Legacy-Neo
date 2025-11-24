// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: w_wad.c,v 1.33 2002/07/29 21:52:25 hurdler Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Portions Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// $Log: w_wad.c,v $
// Revision 1.33  2002/07/29 21:52:25  hurdler
// Someone want to have a look at this bugs
//
// Revision 1.32  2002/07/26 15:21:36  hurdler
// near RC release
//
// Revision 1.31  2002/01/14 18:54:45  hurdler
// I'm tired to answer that question ;)
//
// Revision 1.30  2001/07/28 16:18:37  bpereira
// no message
//
// Revision 1.29  2001/05/27 13:42:48  bpereira
// no message
//
// Revision 1.28  2001/05/21 14:57:05  crashrl
// Readded directory crawling file search function
//
// Revision 1.27  2001/05/16 22:33:34  bock
// Initial FreeBSD support.
//
// Revision 1.26  2001/05/16 17:12:52  crashrl
// Added md5-sum support, removed recursiv wad search
//
// Revision 1.25  2001/04/17 22:26:07  calumr
// Initial Mac add
//
// Revision 1.24  2001/03/03 06:17:34  bpereira
// no message
//
// Revision 1.23  2001/02/28 17:50:55  bpereira
// no message
//
// Revision 1.22  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.21  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.20  2000/10/04 16:19:24  hurdler
// Change all those "3dfx names" to more appropriate names
//
// Revision 1.19  2000/09/28 20:57:19  bpereira
// no message
//
// Revision 1.18  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.17  2000/08/11 21:37:17  hurdler
// fix win32 compilation problem
//
// Revision 1.16  2000/08/11 19:10:13  metzgermeister
// *** empty log message ***
//
// Revision 1.15  2000/07/01 09:23:49  bpereira
// no message
//
// Revision 1.14  2000/05/09 20:57:58  hurdler
// use my own code for colormap (next time, join with Boris own code)
// (necessary due to a small bug in Boris' code (not found) which shows strange effects under linux)
//
// Revision 1.13  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.12  2000/04/27 17:43:19  hurdler
// colormap code in hardware mode is now the default
//
// Revision 1.11  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.10  2000/04/13 23:47:47  stroggonmeth
// See logs
//
// Revision 1.9  2000/04/11 19:07:25  stroggonmeth
// Finished my logs, fixed a crashing bug.
//
// Revision 1.8  2000/04/09 02:30:57  stroggonmeth
// Fixed missing sprite def
//
// Revision 1.7  2000/04/08 11:27:29  hurdler
// fix some boom stuffs
//
// Revision 1.6  2000/04/07 01:39:53  stroggonmeth
// Fixed crashing bug in Linux.
// Made W_ColormapNumForName search in the other direction to find newer colormaps.
//
// Revision 1.5  2000/04/06 20:40:22  hurdler
// Mostly remove warnings under windows
//
// Revision 1.4  2000/04/05 15:47:47  stroggonmeth
// Added hack for Dehacked lumps. Transparent sprites are now affected by colormaps.
//
// Revision 1.3  2000/04/04 00:32:48  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Handles WAD file header, directory, lump I/O.
//
//-----------------------------------------------------------------------------


// added for linux 19990220 by Kin
#ifdef LINUX
#define O_BINARY 0
#endif

#ifndef __APPLE_CC__
#ifndef FREEBSD
#include <malloc.h>
#endif
#endif
#include <fcntl.h>
#ifndef __WIN32__
#include <unistd.h>
#endif

#include "doomdef.h"
#include "doomtype.h"
#include "w_wad.h"
#include "z_zone.h"

#include "i_video.h"        //rendermode
#include "d_netfil.h"
#include "dehacked.h"
#include "r_defs.h"
#include "i_system.h"

#include "md5.h"

#ifdef HWRENDER
#include "hardware/hw_main.h"
#endif

#ifdef LINUX
int strupr(char *n); // from dosstr.c
int strlwr(char *n); // from dosstr.c
#endif

//===========================================================================
//                                                                    GLOBALS
//===========================================================================
int          numwadfiles;             // number of active wadfiles
wadfile_t*   wadfiles[MAX_WADFILES];  // 0 to numwadfiles-1 are valid

// W_Shutdown
// Closes all of the WAD files before quitting
// If not done on a Mac then open wad files
// can prevent removable media they are on from
// being ejected
void W_Shutdown(void)
{
    while (numwadfiles--)
    {
        close(wadfiles[numwadfiles]->handle);
    }
}

//===========================================================================
//                                                        LUMP BASED ROUTINES
//===========================================================================

// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//
// If filename starts with a tilde, the file is handled
//  specially to allow map reloads.
// But: the reload feature is a fragile hack...

int                     reloadlump;
char*                   reloadname;

//  Allocate a wadfile, setup the lumpinfo (directory) and
//  lumpcache, add the wadfile to the current active wadfiles
//
//  now returns index into wadfiles[], you can get wadfile_t*
//  with:
//       wadfiles[<return value>]
//
//  return -1 in case of problem
//
// BP: Can now load dehacked files (ext .deh)
int W_LoadWadFile (char *filename)
{
    int              handle;
    FILE             *fhandle;
    int              numlumps;
    lumpinfo_t*      lumpinfo;
    lumpcache_t*     lumpcache;
    wadfile_t*       wadfile;
    int              i;
    int              length;
    struct stat      bufstat;
    char             filenamebuf[MAX_WADPATH];
#ifdef HWRENDER    
    GlidePatch_t*    grPatch;
#endif

    //
    // check if limit of active wadfiles
    //
    if (numwadfiles>=MAX_WADFILES)
    {
        CONS_Printf ("Maximum wad files reached\n");
        return -1;
    }

    strncpy(filenamebuf, filename, MAX_WADPATH);
    filename = filenamebuf;
    // open wad file
    if ( (handle = open (filename,O_RDONLY|O_BINARY,0666)) == -1)
    {
        nameonly(filename); // leave full path here
        if( findfile(filename, NULL, true) )
        {
            if ( (handle = open (filename,O_RDONLY|O_BINARY,0666)) == -1)
            {
                CONS_Printf ("Can't open %s\n", filename);
                return -1;
            }
        }
        else
        {
            CONS_Printf ("File %s not found.\n", filename);
            return -1;
        }
    }

    // detect dehacked file with the "deh" extension
    if( stricmp(&filename[strlen(filename)-3],"deh")==0 )
    {
        // this code emulate a wadfile with one lump name "DEHACKED" 
        // at position 0 and size of the whole file
        // this allow deh fiel to be like all wad, can be copied by network 
        // and loaded at the console
        fstat(handle,&bufstat);
        numlumps = 1; 
        lumpinfo = Z_Malloc (sizeof(lumpinfo_t),PU_STATIC,NULL);
        lumpinfo->position = 0;
        lumpinfo->size = bufstat.st_size;
        strcpy(lumpinfo->name,"DEHACKED");
    }
    else 
    {   // assume wad file
        wadinfo_t        header;
        lumpinfo_t*      lump_p;
        filelump_t*      fileinfo;

        // read the header
        read (handle, &header, sizeof(header));
        if (strncmp(header.identification,"IWAD",4))
        {
            // Homebrew levels?
            if (strncmp(header.identification,"PWAD",4))
            {
                CONS_Printf ("%s doesn't have IWAD or PWAD id\n", filename);
                return -1;
            }
            // ???modifiedgame = true;
        }
        header.numlumps = LONG(header.numlumps);
        header.infotableofs = LONG(header.infotableofs);

        // read wad file directory
        length = header.numlumps*sizeof(filelump_t);
        fileinfo = alloca (length);
        lseek (handle, header.infotableofs, SEEK_SET);
        read (handle, fileinfo, length);
        numlumps = header.numlumps;
        
        // fill in lumpinfo for this wad
        lump_p = lumpinfo = Z_Malloc (numlumps*sizeof(lumpinfo_t),PU_STATIC,NULL);
        for (i=0 ; i<numlumps ; i++,lump_p++, fileinfo++)
        {
            //lump_p->handle   = handle;
            lump_p->position = LONG(fileinfo->filepos);
            lump_p->size     = LONG(fileinfo->size);
            strncpy (lump_p->name, fileinfo->name, 8);
        }
    }
    //
    //  link wad file to search files
    //
    fstat(handle,&bufstat);
    wadfile = Z_Malloc (sizeof (wadfile_t),PU_STATIC,NULL);
    wadfile->filename = Z_StrDup(filename);
    wadfile->handle = handle;
    wadfile->numlumps = numlumps;
    wadfile->lumpinfo = lumpinfo;
    wadfile->filesize = bufstat.st_size;

    //
    //  generate md5sum
    // 
    fhandle = fopen(filenamebuf, "rb");
    {
        int t=I_GetTime();
        md5_stream (fhandle, wadfile->md5sum);
        if( devparm )
            CONS_Printf("md5 calc for %s took %f second\n",
                        wadfile->filename,(float)(I_GetTime()-t)/TICRATE);
    }
    fclose(fhandle);
    
    //
    //  set up caching
    //
    length = numlumps * sizeof(lumpcache_t);
    lumpcache = Z_Malloc (length,PU_STATIC,NULL);

    memset (lumpcache, 0, length);
    wadfile->lumpcache = lumpcache;

#ifdef HWRENDER
    //faB: now allocates GlidePatch info structures STATIC from the start,
    //     because these were causing a lot of fragmentation of the heap,
    //     considering they are never freed.
    length = numlumps * sizeof(GlidePatch_t);
    grPatch = Z_Malloc (length, PU_HWRPATCHINFO, 0);    //never freed
    // set mipmap.downloaded to false
    memset (grPatch, 0, length);
    for (i=0; i<numlumps; i++)
    {
        //store the software patch lump number for each GlidePatch
        grPatch[i].patchlump = (numwadfiles<<16) + i;
    }
    wadfile->hwrcache = grPatch;
#endif

    //
    //  add the wadfile
    //
    wadfiles[numwadfiles++] = wadfile;

    CONS_Printf ("Added file %s (%i lumps)\n", filename, numlumps);
    W_LoadDehackedLumps( numwadfiles-1 );
    return numwadfiles-1;
}


// !!!NOT CHECKED WITH NEW WAD SYSTEM
//
// W_Reload
// Flushes any of the reloadable lumps in memory
//  and reloads the directory.
//
void W_Reload (void)
{
    wadinfo_t           header;
    int                 lumpcount;
    lumpinfo_t*         lump_p;
    int                 i;
    int                 handle;
    int                 length;
    filelump_t*         fileinfo;
    lumpcache_t*        lumpcache;

    if (!reloadname)
        return;

    if ( (handle = open (reloadname,O_RDONLY | O_BINARY)) == -1)
        I_Error ("W_Reload: couldn't open %s",reloadname);

    read (handle, &header, sizeof(header));
    lumpcount = LONG(header.numlumps);
    header.infotableofs = LONG(header.infotableofs);
    length = lumpcount*sizeof(filelump_t);
    fileinfo = alloca (length);
    lseek (handle, header.infotableofs, SEEK_SET);
    read (handle, fileinfo, length);

    // Fill in lumpinfo
    lump_p = wadfiles[reloadlump>>16]->lumpinfo + (reloadlump&0xFFFF);

    lumpcache = wadfiles[reloadlump>>16]->lumpcache;

    for (i=reloadlump ;
         i<reloadlump+lumpcount ;
         i++,lump_p++, fileinfo++)
    {
        if (lumpcache[i])
            Z_Free (lumpcache[i]);

        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);
    }

    close (handle);
}


//
// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
int W_InitMultipleFiles (char** filenames)
{
    int         rc=1;

    // open all the files, load headers, and count lumps
    numwadfiles = 0;

    // will be realloced as lumps are added
    for ( ; *filenames ; filenames++)
        rc &= (W_LoadWadFile (*filenames) != -1) ? 1 : 0;

    if (!numwadfiles)
        I_Error ("W_InitMultipleFiles: no files found");

    return rc;
}


// !!!NOT CHECKED WITH NEW WAD SYSTEM
//
// W_InitFile
// Just initialize from a single file.
//
/*
void W_InitFile (char* filename)
{
    char*       names[2];

    names[0] = filename;
    names[1] = NULL;
    W_InitMultipleFiles (names);
}*/


// !!!NOT CHECKED WITH NEW WAD SYSTEM
//
// W_NumLumps
//
/*
int W_NumLumps (void)
{
    return numlumps;
}*/



//
//  W_CheckNumForName
//  Returns -1 if name not found.
//

// this is normally always false, so external pwads take precedence,
//  this is set true temporary as W_GetNumForNameFirst() is called
static boolean scanforward = false;

int W_CheckNumForName (char* name)
{
    union {
                char    s[9];
                int             x[2];
    } name8;

    int         i,j;
    int         v1;
    int         v2;
    lumpinfo_t* lump_p;

    // make the name into two integers for easy compares
    strncpy (name8.s,name,8);

    // in case the name was a fill 8 chars
    name8.s[8] = 0;

    // case insensitive
    strupr (name8.s);

    v1 = name8.x[0];
    v2 = name8.x[1];

    if (!scanforward)
    {
        //
        // scan wad files backwards so patch lump files take precedence
        //
        for (i = numwadfiles-1 ; i>=0; i--)
        {
            lump_p = wadfiles[i]->lumpinfo;

            for (j = 0; j<wadfiles[i]->numlumps; j++,lump_p++)
            {
                if (    *(int *)lump_p->name == v1
                     && *(int *)&lump_p->name[4] == v2)
                {
                    // high word is the wad file number
                    return ((i<<16) + j);
                }
            }
        }
        // not found.
        return -1;
    }

    //
    // scan wad files forward, when original wad resources
    //  must take precedence
    //
    for (i = 0; i<numwadfiles; i++)
    {
        lump_p = wadfiles[i]->lumpinfo;
        for (j = 0; j<wadfiles[i]->numlumps; j++,lump_p++)
        {
            if (    *(int *)lump_p->name == v1
                 && *(int *)&lump_p->name[4] == v2)
            {
                return ((i<<16) + j);
            }
        }
    }
    // not found.
    return -1;
}


//
//  Same as the original, but checks in one pwad only
//  wadid is a wad number
//  (Used for sprites loading)
//
//  'startlump' is the lump number to start the search
//
int W_CheckNumForNamePwad (char* name, int wadid, int startlump)
{
    union {
        char  s[9];
        int   x[2];
    } name8;

    int         i;
    int         v1;
    int         v2;
    lumpinfo_t* lump_p;

    strncpy (name8.s,name,8);
    name8.s[8] = 0;
    strupr (name8.s);

    v1 = name8.x[0];
    v2 = name8.x[1];

    //
    // scan forward
    // start at 'startlump', useful parameter when there are multiple
    //                       resources with the same name
    //
    if (startlump < wadfiles[wadid]->numlumps)
    {
        lump_p = wadfiles[wadid]->lumpinfo + startlump;
        for (i = startlump; i<wadfiles[wadid]->numlumps; i++,lump_p++)
        {
            if ( *(int *)lump_p->name == v1
              && *(int *)&lump_p->name[4] == v2)
            {
                return ((wadid<<16)+i);
            }
        }
    }

    // not found.
    return -1;
}



//
// W_GetNumForName
//   Calls W_CheckNumForName, but bombs out if not found.
//
int W_GetNumForName (char* name)
{
    int i;

    i = W_CheckNumForName (name);

    if (i == -1)
    {
        if (!strcmp(name, "PLAYPAL"))
        {
        //Hurdler: I'm tired of that question ;)
        I_Error ("Main WAD file not found (do not use doom3.wad or legacy.dat)\n"
                 "You need either Doom.wad, Doom1.wad, Doom2.wad,\n"
                 "Tnt.wad, Plutonia.wad, Heretic.wad or Heretic1.wad\n"
                 "from any sharware or commercial version of Doom or Heretic!\n");
        }
        else
        I_Error ("W_GetNumForName: %s not found!\n", name);
    }

    return i;
}

int W_CheckNumForNameFirst (char* name)
{
    int i;

    // 3am coding.. force a scan of resource name forward, for one call
    scanforward = true;
    i = W_CheckNumForName (name);
    scanforward = false;

    return i;
}

//
//  W_GetNumForNameFirst : like W_GetNumForName, but scans FORWARD
//                         so it gets resources from the original wad first
//  (this is used only to get S_START for now, in r_data.c)
int W_GetNumForNameFirst (char* name)
{
    int i;

    i = W_CheckNumForNameFirst (name);
    if (i == -1)
        I_Error ("W_GetNumForNameFirst: %s not found!", name);

    return i;
}


//
//  W_LumpLength
//   Returns the buffer size needed to load the given lump.
//
int W_LumpLength (int lump)
{
#ifdef PARANOIA
    if (lump<0) I_Error("W_LumpLenght: lump not exist\n");

    if ((lump&0xFFFF) >= wadfiles[lump>>16]->numlumps)
        I_Error ("W_LumpLength: %i >= numlumps",lump);
#endif
    return wadfiles[lump>>16]->lumpinfo[lump&0xFFFF].size;
}



//
// W_ReadLumpHeader : read 'size' bytes of lump
//                    sometimes just the header is needed
//
//Fab:02-08-98: now returns the number of bytes read (should == size)
int  W_ReadLumpHeader ( int           lump,
                        void*         dest,
                        int           size )
{
    int         bytesread;
    lumpinfo_t* l;
    int         handle;
#ifdef PARANOIA
    if (lump<0) I_Error("W_ReadLumpHeader: lump not exist\n");

    if ((lump&0xFFFF) >= wadfiles[lump>>16]->numlumps)
        I_Error ("W_ReadLumpHeader: %i >= numlumps",lump);
#endif
    l = wadfiles[lump>>16]->lumpinfo + (lump&0xFFFF);

    // the good ole 'loading' disc icon TODO: restore it :)
    // ??? I_BeginRead ();

    // empty resource (usually markers like S_START, F_END ..)
    if (l->size==0)
        return 0;

/*    if (l->handle == -1)
    {
        // reloadable file, so use open / read / close
        if ( (handle = open (reloadname,O_RDONLY|O_BINARY,0666)) == -1)
            I_Error ("W_ReadLumpHeader: couldn't open %s",reloadname);
    }
    else*/
        handle = wadfiles[lump>>16]->handle; //l->handle;

    // 0 size means read all the lump
    if (!size || size>l->size)
        size = l->size;
    
    lseek (handle, l->position, SEEK_SET);
    bytesread = read (handle, dest, size);

    /*if (l->handle == -1)
        close (handle);*/

    // ??? I_EndRead ();
    return bytesread;
}


//
//  W_ReadLump
//  Loads the lump into the given buffer,
//   which must be >= W_LumpLength().
//
//added:06-02-98: now calls W_ReadLumpHeader() with full lump size.
//                0 size means the size of the lump, see W_ReadLumpHeader
void W_ReadLump ( int           lump,
                  void*         dest )
{
    W_ReadLumpHeader (lump, dest, 0);
}


// ==========================================================================
// W_CacheLumpNum
// ==========================================================================
void* W_CacheLumpNum ( int lump, int tag )
{
    byte*         ptr;
    lumpcache_t*  lumpcache;

    //SoM: 4/8/2000: Don't keep doing oporations to the lump variable!
    int           llump = lump & 0xffff;
    int           lfile = lump >> 16;

#ifdef PARANOIA
    // check return value of a previous W_CheckNumForName()
    //SoM: 4/8/2000: Do better checking. No more SIGSEGV's!
    if (lfile >= numwadfiles)
      I_Error("W_CacheLumpNum: %i >= numwadfiles(%i)\n", lfile, numwadfiles);
    if (llump >= wadfiles[lfile]->numlumps)
      I_Error ("W_CacheLumpNum: %i >= numlumps", llump);
    if(lump == -1)
      I_Error ("W_CacheLumpNum: -1 passed!\n");
    if(llump < 0)
      I_Error ("W_CacheLumpNum: %i < 0!\n", llump);
#endif

    lumpcache = wadfiles[lfile]->lumpcache;
    if (!lumpcache[llump]) {
        // read the lump in

        //CONS_Printf ("cache miss on lump %i\n",lump);
        //Hurdler: FIXME: that crashes inside Z_Malloc with MR.ROCKET's wad
        ptr = Z_Malloc (W_LumpLength (lump), tag, &lumpcache[llump]);
        W_ReadLumpHeader (lump, lumpcache[llump], 0);   // read full
    }
    else {
        //CONS_Printf ("cache hit on lump %i\n",lump);
        Z_ChangeTag (lumpcache[llump],tag);
    }

    return lumpcache[llump];
}


// ==========================================================================
// W_CacheLumpName
// ==========================================================================
void* W_CacheLumpName ( char* name, int tag )
{
    return W_CacheLumpNum (W_GetNumForName(name), tag);
}



// ==========================================================================
//                                         CACHING OF GRAPHIC PATCH RESOURCES
// ==========================================================================

// Graphic 'patches' are loaded, and if necessary, converted into the format
// the most useful for the current rendermode. For software renderer, the
// graphic patches are kept as is. For the hardware renderer, graphic patches
// are 'unpacked', and are kept into the cache in that unpacked format, the
// heap memory cache then act as a 'level 2' cache just after the graphics
// card memory.

//
// Cache a patch into heap memory, convert the patch format as necessary
//

// Software-only compile cache the data without conversion
#ifdef HWRENDER // not win32 only 19990829 by Kin

void* W_CachePatchNum ( int lump,int tag )
{
    GlidePatch_t*   grPatch;

    if( rendermode == render_soft )
        return W_CacheLumpNum(lump,tag);

// ------------------------------------------------------ accelereted RENDER

#ifdef PARANOIA
    // check the return value of a previous W_CheckNumForName()
    if ( ( lump==-1 ) ||
         ((lump&0xFFFF) >= wadfiles[lump>>16]->numlumps) )
        I_Error ("W_CachePatchNum: %i >= numlumps", lump&0xffff);
#endif

    grPatch = &(wadfiles[lump>>16]->hwrcache[lump & 0xffff]);

    if( grPatch->mipmap.grInfo.data ) 
    {   
        if( tag == PU_CACHE )
            tag = PU_HWRCACHE;
        Z_ChangeTag (grPatch->mipmap.grInfo.data, tag); 
    }
    else
    {   // first time init grPatch fields
        // we need patch w,h,offset,...
        // well this code will be executed latter in GetPatch, anyway 
        // do it now ...
        patch_t *ptr = W_CacheLumpNum(grPatch->patchlump, PU_STATIC);
        HWR_MakePatch ( ptr, grPatch, &grPatch->mipmap);
        Z_Free (ptr); 
        //Hurdler: why not do a Z_ChangeTag (grPatch->mipmap.grInfo.data, tag) here?
        //BP: mmm, yes we can...
    }

    // return GlidePatch_t, which can be casted to (patch_t) with valid patch header info
    return (void*)grPatch;
}

#endif // HWRENDER Glide version

//
//
//
void* W_CachePatchName ( char*   name,
                         int     tag )
{
    if( W_CheckNumForName( name )<0)
        return W_CachePatchNum (W_GetNumForName("BRDR_MM"), tag);
    return W_CachePatchNum (W_GetNumForName(name), tag);
}

// convert raw heretic picture to legacy pic_t format
void *W_CacheRawAsPic( int lump, int width, int height, int tag)
{
    lumpcache_t*  lumpcache;
    //SoM: 4/8/2000: Don't keep doing oporations to the lump variable!
    int           llump = lump & 0xffff;
    int           lfile = lump >> 16;
    pic_t         *pic;

#ifdef PARANOIA
    // check return value of a previous W_CheckNumForName()
    //SoM: 4/8/2000: Do better checking. No more SIGSEGV's!
    if (lfile >= numwadfiles)
      I_Error("W_CacheRawAsPic: %i >= numwadfiles(%i)\n", lfile, numwadfiles);
    if (llump >= wadfiles[lfile]->numlumps)
      I_Error ("W_CacheRawAsPic: %i >= numlumps", llump);
    if(lump == -1)
      I_Error ("W_CacheRawAsPic: -1 passed!\n");
    if(llump < 0)
      I_Error ("W_CacheRawAsPic: %i < 0!\n", llump);
#endif

    lumpcache = wadfiles[lfile]->lumpcache;
    if (!lumpcache[llump]) 
    {
        // read the lump in

        pic = Z_Malloc (W_LumpLength (lump)+sizeof(pic_t), tag, &lumpcache[llump]);
        W_ReadLumpHeader (lump, pic->data, 0);   // read full
        pic->width = SHORT(width);
        pic->height = SHORT(height);
        pic->mode = PALETTE;
    }
    else 
        Z_ChangeTag (lumpcache[llump],tag);

    return lumpcache[llump];
}


// search for all DEHACKED lump in all wads and load it
void W_LoadDehackedLumps( int wadnum )
{
    int clump = 0;
    
    while (1)
    { 
        clump = W_CheckNumForNamePwad("DEHACKED", wadnum, clump);
        if(clump == -1)
            break;
        CONS_Printf("Loading dehacked from %s\n",wadfiles[wadnum]->filename);
        DEH_LoadDehackedLump(clump);
        clump++;
    }
}



// --------------------------------------------------------------------------
// W_Profile
// --------------------------------------------------------------------------
//
/*     --------------------- UNUSED ------------------------
int             info[2500][10];
int             profilecount;

void W_Profile (void)
{
    int         i;
    memblock_t* block;
    void*       ptr;
    char        ch;
    FILE*       f;
    int         j;
    char        name[9];


    for (i=0 ; i<numlumps ; i++)
    {
        ptr = lumpcache[i];
        if (!ptr)
        {
            ch = ' ';
            continue;
        }
        else
        {
            block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
            if (block->tag < PU_PURGELEVEL)
                ch = 'S';
            else
                ch = 'P';
        }
        info[i][profilecount] = ch;
    }
    profilecount++;

    f = fopen ("waddump.txt","w");
    name[8] = 0;

    for (i=0 ; i<numlumps ; i++)
    {
        memcpy (name,lumpinfo[i].name,8);

        for (j=0 ; j<8 ; j++)
            if (!name[j])
                break;

        for ( ; j<8 ; j++)
            name[j] = ' ';

        fprintf (f,"%s ",name);

        for (j=0 ; j<profilecount ; j++)
            fprintf (f,"    %c",info[i][j]);

        fprintf (f,"\n");
    }
    fclose (f);
}

// --------------------------------------------------------------------------
// W_AddFile : the old code kept for reference
// --------------------------------------------------------------------------
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//

int filelen (int handle)
{
    struct stat fileinfo;

    if (fstat (handle,&fileinfo) == -1)
        I_Error ("Error fstating");

    return fileinfo.st_size;
}


int W_AddFile (char *filename)
{
    wadinfo_t           header;
    lumpinfo_t*         lump_p;
    unsigned            i;
    int                 handle;
    int                 length;
    int                 startlump;
    filelump_t*         fileinfo;
    filelump_t          singleinfo;
    int                 storehandle;

    // open the file and add to directory

    // handle reload indicator.
    if (filename[0] == '~')
    {
        filename++;
        reloadname = filename;
        reloadlump = numlumps;
    }

    if ( (handle = open (filename,O_RDONLY | O_BINARY)) == -1)
    {
        CONS_Printf (" couldn't open %s\n",filename);
        return 0;
    }

    CONS_Printf (" adding %s\n",filename);
    startlump = numlumps;

    if (stricmp (filename+strlen(filename)-3, "wad") )
    {
        // single lump file
        fileinfo = &singleinfo;
        singleinfo.filepos = 0;
        singleinfo.size = LONG(filelen(handle));
        FIL_ExtractFileBase (filename, singleinfo.name);
        numlumps++;
    }
    else
    {
        // WAD file
        read (handle, &header, sizeof(header));
        if (strncmp(header.identification,"IWAD",4))
        {
            // Homebrew levels?
            if (strncmp(header.identification,"PWAD",4))
            {
                I_Error ("Wad file %s doesn't have IWAD "
                         "or PWAD id\n", filename);
            }

            // ???modifiedgame = true;
        }
        header.numlumps = LONG(header.numlumps);
        header.infotableofs = LONG(header.infotableofs);
        length = header.numlumps*sizeof(filelump_t);
        fileinfo = alloca (length);
        lseek (handle, header.infotableofs, SEEK_SET);
        read (handle, fileinfo, length);
        numlumps += header.numlumps;
    }


    // Fill in lumpinfo
    lumpinfo = realloc (lumpinfo, numlumps*sizeof(lumpinfo_t));

    if (!lumpinfo)
        I_Error ("Couldn't realloc lumpinfo");

    lump_p = &lumpinfo[startlump];

    storehandle = reloadname ? -1 : handle;

    for (i=startlump ; i<numlumps ; i++,lump_p++, fileinfo++)
    {
        lump_p->handle = storehandle;
        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);
        strncpy (lump_p->name, fileinfo->name, 8);
    }

    if (reloadname)
        close (handle);

    return 1;
}
*/
