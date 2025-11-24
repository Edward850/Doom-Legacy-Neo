// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: r_data.c,v 1.31 2003/05/04 04:12:54 sburke Exp $
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
// $Log: r_data.c,v $
// Revision 1.31  2003/05/04 04:12:54  sburke
// Replace memcpy with memmove, to prevent a misaligned access fault on sparc.
//
// Revision 1.30  2002/12/13 19:22:12  ssntails
// Fix for the Z_CheckHeap and random crashes! (I hope!)
//
// Revision 1.29  2002/01/12 12:41:05  hurdler
// very small commit
//
// Revision 1.28  2002/01/12 02:21:36  stroggonmeth
// Big commit
//
// Revision 1.27  2001/12/27 22:50:25  hurdler
// fix a colormap bug, add scrolling floor/ceiling in hw mode
//
// Revision 1.26  2001/08/13 22:53:40  stroggonmeth
// Small commit
//
// Revision 1.25  2001/03/21 18:24:39  stroggonmeth
// Misc changes and fixes. Code cleanup
//
// Revision 1.24  2001/03/19 18:52:01  hurdler
// lil fix
//
// Revision 1.23  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.22  2000/11/04 16:23:43  bpereira
// no message
//
// Revision 1.21  2000/11/02 17:50:09  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.20  2000/10/04 16:19:23  hurdler
// Change all those "3dfx names" to more appropriate names
//
// Revision 1.19  2000/09/28 20:57:17  bpereira
// no message
//
// Revision 1.18  2000/08/11 12:25:23  hurdler
// latest changes for v1.30
//
// Revision 1.17  2000/07/01 09:23:49  bpereira
// no message
//
// Revision 1.16  2000/05/03 23:51:01  stroggonmeth
// A few, quick, changes.
//
// Revision 1.15  2000/04/23 16:19:52  bpereira
// no message
//
// Revision 1.14  2000/04/18 17:39:39  stroggonmeth
// Bug fixes and performance tuning.
//
// Revision 1.13  2000/04/18 12:54:58  hurdler
// software mode bug fixed
//
// Revision 1.12  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.11  2000/04/15 22:12:58  stroggonmeth
// Minor bug fixes
//
// Revision 1.10  2000/04/13 23:47:47  stroggonmeth
// See logs
//
// Revision 1.9  2000/04/08 17:45:11  hurdler
// fix some boom stuffs
//
// Revision 1.8  2000/04/08 17:29:25  stroggonmeth
// no message
//
// Revision 1.7  2000/04/08 11:27:29  hurdler
// fix some boom stuffs
//
// Revision 1.6  2000/04/07 01:39:53  stroggonmeth
// Fixed crashing bug in Linux.
// Made W_ColormapNumForName search in the other direction to find newer colormaps.
//
// Revision 1.5  2000/04/06 21:06:19  stroggonmeth
// Optimized extra_colormap code...
// Added #ifdefs for older water code.
//
// Revision 1.4  2000/04/06 20:40:22  hurdler
// Mostly remove warnings under windows
//
// Revision 1.3  2000/04/04 00:32:47  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Preparation of data for rendering,
//      generation of lookups, caching, retrieval by name.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "g_game.h"
#include "i_video.h"
#include "r_local.h"
#include "r_sky.h"
#include "p_local.h"
#include "r_data.h"
#include "w_wad.h"
#include "z_zone.h"
#include "p_setup.h" //levelflats
#include "v_video.h" //pLoaclPalette

#ifdef __WIN32__
#include "malloc.h"
#endif

//
// Graphics.
// DOOM graphics for walls and sprites
// is stored in vertical runs of opaque pixels (posts).
// A column is composed of zero or more posts,
// a patch or sprite is composed of zero or more columns.
//

#ifdef OLDWATER
int             firstwaterflat; //added:18-02-98:WATER!
#endif

int             firstflat, lastflat, numflats;
int             firstpatch, lastpatch, numpatches;
int             firstspritelump, lastspritelump, numspritelumps;



// textures
int             numtextures=0;      // total number of textures found,
// size of following tables

texture_t**     textures=NULL;
unsigned int**  texturecolumnofs;   // column offset lookup table for each texture
byte**          texturecache;       // graphics data for each generated full-size texture
int*            texturewidthmask;   // texture width is a power of 2, so it
                                    // can easily repeat along sidedefs using
                                    // a simple mask
fixed_t*        textureheight;      // needed for texture pegging

int       *flattranslation;             // for global animation
int       *texturetranslation;

// needed for pre rendering
fixed_t*        spritewidth;
fixed_t*        spriteoffset;
fixed_t*        spritetopoffset;
fixed_t*        spriteheight; //SoM

lighttable_t    *colormaps;


//faB: for debugging/info purpose
int             flatmemory;
int             spritememory;
int             texturememory;


//faB: highcolor stuff
short    color8to16[256];       //remap color index to highcolor rgb value
short*   hicolormaps;           // test a 32k colormap remaps high -> high


//
// MAPTEXTURE_T CACHING
// When a texture is first needed,
//  it counts the number of composite columns
//  required in the texture and allocates space
//  for a column directory and any new columns.
// The directory will simply point inside other patches
//  if there is only one patch in a given column,
//  but any columns with multiple patches
//  will have new column_ts generated.
//



//
// R_DrawColumnInCache
// Clip and draw a column
//  from a patch into a cached post.
//

void R_DrawColumnInCache ( column_t*     patch,
                           byte*         cache,
                           int           originy,
                           int           cacheheight )
{
    int         count;
    int         position;
    byte*       source;
    byte*       dest;

    dest = (byte *)cache;// + 3;

    while (patch->topdelta != 0xff)
    {
        source = (byte *)patch + 3;
        count = patch->length;
        position = originy + patch->topdelta;

        if (position < 0)
        {
            count += position;
            position = 0;
        }

        if (position + count > cacheheight)
            count = cacheheight - position;

        if (count > 0)
            memcpy (cache + position, source, count);

        patch = (column_t *)(  (byte *)patch + patch->length + 4);
    }
}



//
// R_GenerateTexture
//
//   Allocate space for full size texture, either single patch or 'composite'
//   Build the full textures from patches.
//   The texture caching system is a little more hungry of memory, but has
//   been simplified for the sake of highcolor, dynamic ligthing, & speed.
//
//   This is not optimised, but it's supposed to be executed only once
//   per level, when enough memory is available.
//
byte* R_GenerateTexture (int texnum)
{
    byte*               block;
    byte*               blocktex;
    texture_t*          texture;
    texpatch_t*         patch;
    patch_t*            realpatch;
    int                 x;
    int                 x1;
    int                 x2;
    int                 i;
    column_t*           patchcol;
    unsigned int*       colofs;
    int                 blocksize;

    texture = textures[texnum];

    // allocate texture column offset lookup

    // single-patch textures can have holes in it and may be used on
    // 2sided lines so they need to be kept in 'packed' format
    if (texture->patchcount==1)
    {
        patch = texture->patches;
        blocksize = W_LumpLength (patch->patch);
#if 1
        realpatch = W_CacheLumpNum (patch->patch, PU_CACHE);

        block = Z_Malloc (blocksize,
                          PU_STATIC,         // will change tag at end of this function
                          &texturecache[texnum]);
        memcpy (block, realpatch, blocksize);
#else
        // FIXME: this version don't put the user z_block
        texturecache[texnum] = block = W_CacheLumpNum (patch->patch, PU_STATIC);
#endif
        //CONS_Printf ("R_GenTex SINGLE %.8s size: %d\n",texture->name,blocksize);
        texturememory+=blocksize;

        // use the patch's column lookup
        colofs = (unsigned int*)(block + 8);
        texturecolumnofs[texnum] = colofs;
                blocktex = block;
        for (i=0; i<texture->width; i++)
             colofs[i] = LONG(colofs[i]) + 3;
        goto done;
    }

    //
    // multi-patch textures (or 'composite')
    //
    blocksize = (texture->width * 4) + (texture->width * texture->height);

    //CONS_Printf ("R_GenTex MULTI  %.8s size: %d\n",texture->name,blocksize);
    texturememory+=blocksize;

    block = Z_Malloc (blocksize,
                      PU_STATIC,
                      &texturecache[texnum]);

    // columns lookup table
    colofs = (unsigned int*)block;
    texturecolumnofs[texnum] = colofs;

    // texture data before the lookup table
    blocktex = block + (texture->width*4);

    // Composite the columns together.
    patch = texture->patches;

    for (i=0 , patch = texture->patches;
         i<texture->patchcount;
         i++, patch++)
    {
        realpatch = W_CacheLumpNum (patch->patch, PU_CACHE);
        x1 = patch->originx;
        x2 = x1 + SHORT(realpatch->width);

        if (x1<0)
            x = 0;
        else
            x = x1;

        if (x2 > texture->width)
            x2 = texture->width;

        for ( ; x<x2 ; x++)
        {
            patchcol = (column_t *)((byte *)realpatch
                                    + LONG(realpatch->columnofs[x-x1]));

            // generate column ofset lookup
            colofs[x] = (x * texture->height) + (texture->width*4);

            R_DrawColumnInCache (patchcol,
                                 block + colofs[x],
                                 patch->originy,
                                 texture->height);
        }
    }

done:
    // Now that the texture has been built in column cache,
    //  it is purgable from zone memory.
    Z_ChangeTag (block, PU_CACHE);

    return blocktex;
}





//
// R_GetColumn
//


//
// new test version, short!
//
byte* R_GetColumn ( int           tex,
                    int           col )
{
    byte*       data;

    col &= texturewidthmask[tex];
    data = texturecache[tex];

    if (!data)
        data = R_GenerateTexture (tex);

    return data + texturecolumnofs[tex][col];
}


//  convert flat to hicolor as they are requested
//
//byte**  flatcache;

byte* R_GetFlat (int  flatlumpnum)
{
    return W_CacheLumpNum (flatlumpnum, PU_CACHE);

/*  // this code work but is useless
    byte*    data;
    short*   wput;
    int      i,j;

    //FIXME: work with run time pwads, flats may be added
    // lumpnum to flatnum in flatcache
    if ((data = flatcache[flatlumpnum-firstflat])!=0)
                return data;

    data = W_CacheLumpNum (flatlumpnum, PU_CACHE);
    i=W_LumpLength(flatlumpnum);

    Z_Malloc (i,PU_STATIC,&flatcache[flatlumpnum-firstflat]);
    memcpy (flatcache[flatlumpnum-firstflat], data, i);

    return flatcache[flatlumpnum-firstflat];
*/

/*  // this code don't work because it don't put a proper user in the z_block
    if ((data = flatcache[flatlumpnum-firstflat])!=0)
       return data;

    data = (byte *) W_CacheLumpNum(flatlumpnum,PU_LEVEL);
    flatcache[flatlumpnum-firstflat] = data;
    return data;

    flatlumpnum -= firstflat;

    if (scr_bpp==1)
    {
                flatcache[flatlumpnum] = data;
                return data;
    }

    // allocate and convert to high color

    wput = (short*) Z_Malloc (64*64*2,PU_STATIC,&flatcache[flatlumpnum]);
    //flatcache[flatlumpnum] =(byte*) wput;

    for (i=0; i<64; i++)
       for (j=0; j<64; j++)
                        wput[i*64+j] = ((color8to16[*data++]&0x7bde) + ((i<<9|j<<4)&0x7bde))>>1;

                //Z_ChangeTag (data, PU_CACHE);

                return (byte*) wput;
*/
}

//
// Empty the texture cache (used for load wad at runtime)
//
void R_FlushTextureCache (void)
{
    int i;

    if (numtextures>0)
        for (i=0; i<numtextures; i++)
        {
            if (texturecache[i])
                Z_Free (texturecache[i]);
        }
}

//
// R_InitTextures
// Initializes the texture list with the textures from the world map.
//
void R_LoadTextures (void)
{
    maptexture_t*       mtexture;
    texture_t*          texture;
    mappatch_t*         mpatch;
    texpatch_t*         patch;
    char*               pnames;

    int                 i;
    int                 j;

    int*                maptex;
    int*                maptex2;
    int*                maptex1;

    char                name[9];
    char*               name_p;

    int*                patchlookup;

    int                 nummappatches;
    int                 offset;
    int                 maxoff;
    int                 maxoff2;
    int                 numtextures1;
    int                 numtextures2;

    int*                directory;


    // free previous memory before numtextures change

    if (numtextures>0)
        for (i=0; i<numtextures; i++)
        {
            if (textures[i])
                Z_Free (textures[i]);
            if (texturecache[i])
                Z_Free (texturecache[i]);
        }

    // Load the patch names from pnames.lmp.
    name[8] = 0;
    pnames = W_CacheLumpName ("PNAMES", PU_STATIC);
    nummappatches = LONG ( *((int *)pnames) );
    name_p = pnames+4;
    patchlookup = alloca (nummappatches*sizeof(*patchlookup));

    for (i=0 ; i<nummappatches ; i++)
    {
        strncpy (name,name_p+i*8, 8);
        patchlookup[i] = W_CheckNumForName (name);
    }
    Z_Free (pnames);

    // Load the map texture definitions from textures.lmp.
    // The data is contained in one or two lumps,
    //  TEXTURE1 for shareware, plus TEXTURE2 for commercial.
    maptex = maptex1 = W_CacheLumpName ("TEXTURE1", PU_STATIC);
    numtextures1 = LONG(*maptex);
    maxoff = W_LumpLength (W_GetNumForName ("TEXTURE1"));
    directory = maptex+1;

    if (W_CheckNumForName ("TEXTURE2") != -1)
    {
        maptex2 = W_CacheLumpName ("TEXTURE2", PU_STATIC);
        numtextures2 = LONG(*maptex2);
        maxoff2 = W_LumpLength (W_GetNumForName ("TEXTURE2"));
    }
    else
    {
        maptex2 = NULL;
        numtextures2 = 0;
        maxoff2 = 0;
    }
    numtextures = numtextures1 + numtextures2;


    //faB : there is actually 5 buffers allocated in one for convenience
    if (textures)
        Z_Free (textures);

    textures         = Z_Malloc (numtextures*4*5, PU_STATIC, 0);

    texturecolumnofs = (void*)((int*)textures + numtextures);
    texturecache     = (void*)((int*)textures + numtextures*2);
    texturewidthmask = (void*)((int*)textures + numtextures*3);
    textureheight    = (void*)((int*)textures + numtextures*4);

    for (i=0 ; i<numtextures ; i++, directory++)
    {
        //only during game startup
        //if (!(i&63))
        //    CONS_Printf (".");

        if (i == numtextures1)
        {
            // Start looking in second texture file.
            maptex = maptex2;
            maxoff = maxoff2;
            directory = maptex+1;
        }

        // offset to the current texture in TEXTURESn lump
        offset = LONG(*directory);

        if (offset > maxoff)
            I_Error ("R_LoadTextures: bad texture directory");

        // maptexture describes texture name, size, and
        // used patches in z order from bottom to top
        mtexture = (maptexture_t *) ( (byte *)maptex + offset);

        texture = textures[i] =
            Z_Malloc (sizeof(texture_t)
                      + sizeof(texpatch_t)*(SHORT(mtexture->patchcount)-1),
                      PU_STATIC, 0);

        texture->width  = SHORT(mtexture->width);
        texture->height = SHORT(mtexture->height);
        texture->patchcount = SHORT(mtexture->patchcount);

	// Sparc requires memmove, becuz gcc doesn't know mtexture is not aligned.
	// gcc will replace memcpy with two 4-byte read/writes, which will bus error.
        memmove(texture->name, mtexture->name, sizeof(texture->name));	
        mpatch = &mtexture->patches[0];
        patch = &texture->patches[0];

        for (j=0 ; j<texture->patchcount ; j++, mpatch++, patch++)
        {
            patch->originx = SHORT(mpatch->originx);
            patch->originy = SHORT(mpatch->originy);
            patch->patch = patchlookup[SHORT(mpatch->patch)];
            if (patch->patch == -1)
            {
                I_Error ("R_InitTextures: Missing patch in texture %s",
                         texture->name);
            }
        }

        j = 1;
        while (j*2 <= texture->width)
            j<<=1;

        texturewidthmask[i] = j-1;
        textureheight[i] = texture->height<<FRACBITS;
    }

    Z_Free (maptex1);
    if (maptex2)
        Z_Free (maptex2);


    //added:01-04-98: this takes 90% of texture loading time..
    // Precalculate whatever possible.
    for (i=0 ; i<numtextures ; i++)
        texturecache[i] = NULL;

    // Create translation table for global animation.
    if (texturetranslation)
        Z_Free (texturetranslation);

    texturetranslation = Z_Malloc ((numtextures+1)*4, PU_STATIC, 0);

    for (i=0 ; i<numtextures ; i++)
        texturetranslation[i] = i;
}


int R_CheckNumForNameList(char *name, lumplist_t* list, int listsize)
{
  int   i;
  int   lump;
  for(i = listsize - 1; i > -1; i--)
  {
    lump = W_CheckNumForNamePwad(name, list[i].wadfile, list[i].firstlump);
    if((lump & 0xffff) > (list[i].firstlump + list[i].numlumps) || lump == -1)
      continue;
    else
      return lump;
  }
  return -1;
}


lumplist_t*  colormaplumps;
int          numcolormaplumps;

void R_InitExtraColormaps()
{
    int       startnum;
    int       endnum;
    int       cfile;
    int       clump;

    numcolormaplumps = 0;
    colormaplumps = NULL;
    cfile = clump = 0;

    for(;cfile < numwadfiles;cfile ++, clump = 0)
    {
        startnum = W_CheckNumForNamePwad("C_START", cfile, clump);
        if(startnum == -1)
            continue;

        endnum = W_CheckNumForNamePwad("C_END", cfile, clump);

        if(endnum == -1)
            I_Error("R_InitColormaps: C_START without C_END\n");

        if((startnum >> 16) != (endnum >> 16))
            I_Error("R_InitColormaps: C_START and C_END in different wad files!\n");

        colormaplumps = (lumplist_t *)realloc(colormaplumps, sizeof(lumplist_t) * (numcolormaplumps + 1));
        colormaplumps[numcolormaplumps].wadfile = startnum >> 16;
        colormaplumps[numcolormaplumps].firstlump = (startnum&0xFFFF) + 1;
        colormaplumps[numcolormaplumps].numlumps = endnum - (startnum + 1);
        numcolormaplumps++;
    }
}



lumplist_t*  flats;
int          numflatlists;

extern int   numwadfiles;


void R_InitFlats ()
{
  int       startnum;
  int       endnum;
  int       cfile;
  int       clump;

  numflatlists = 0;
  flats = NULL;
  cfile = clump = 0;

#ifdef OLDWATER
  //added:18-02-98: WATER! flatnum of the first waterflat
  firstwaterflat = W_GetNumForName ("WATER0");
#endif

  for(;cfile < numwadfiles;cfile ++, clump = 0)
  {
    startnum = W_CheckNumForNamePwad("F_START", cfile, clump);
    if(startnum == -1)
    {
      clump = 0;
      startnum = W_CheckNumForNamePwad("FF_START", cfile, clump);

      if(startnum == -1) //If STILL -1, search the whole file!
      {
        flats = (lumplist_t *)realloc(flats, sizeof(lumplist_t) * (numflatlists + 1));
        flats[numflatlists].wadfile = cfile;
        flats[numflatlists].firstlump = 0;
        flats[numflatlists].numlumps = 0xffff; //Search the entire file!
        numflatlists ++;
        continue;
      }
    }

    endnum = W_CheckNumForNamePwad("F_END", cfile, clump);
    if(endnum == -1)
      endnum = W_CheckNumForNamePwad("FF_END", cfile, clump);

    if(endnum == -1 || (startnum &0xFFFF) > (endnum & 0xFFFF))
    {
      flats = (lumplist_t *)realloc(flats, sizeof(lumplist_t) * (numflatlists + 1));
      flats[numflatlists].wadfile = cfile;
      flats[numflatlists].firstlump = 0;
      flats[numflatlists].numlumps = 0xffff; //Search the entire file!
      numflatlists ++;
      continue;
    }

    flats = (lumplist_t *)realloc(flats, sizeof(lumplist_t) * (numflatlists + 1));
    flats[numflatlists].wadfile = startnum >> 16;
    flats[numflatlists].firstlump = (startnum&0xFFFF) + 1;
    flats[numflatlists].numlumps = endnum - (startnum + 1);
    numflatlists++;
    continue;
  }

  if(!numflatlists)
    I_Error("R_InitFlats: No flats found!\n");
}



int R_GetFlatNumForName(char *name)
{
    // BP: don't work with gothic2.wad
  //return R_CheckNumForNameList(name, flats, numflatlists);

  int lump = W_CheckNumForName(name); 
  if(lump == -1)
    I_Error("R_GetFlatNumForName: Could not find flat %.8s\n", name);

  return lump;
}


//
// R_InitSpriteLumps
// Finds the width and hoffset of all sprites in the wad,
//  so the sprite does not need to be cached completely
//  just for having the header info ready during rendering.
//

//
//   allocate sprite lookup tables
//
void R_InitSpriteLumps (void)
{
    // the original Doom used to set numspritelumps from S_END-S_START+1

    //Fab:FIXME: find a better solution for adding new sprites dynamically
    numspritelumps = 0;

    spritewidth = Z_Malloc (MAXSPRITELUMPS*4, PU_STATIC, 0);
    spriteoffset = Z_Malloc (MAXSPRITELUMPS*4, PU_STATIC, 0);
    spritetopoffset = Z_Malloc (MAXSPRITELUMPS*4, PU_STATIC, 0);
    spriteheight = Z_Malloc (MAXSPRITELUMPS*4, PU_STATIC, 0);
}


void R_InitExtraColormaps();
void R_ClearColormaps();

//
// R_InitColormaps
//
void R_InitColormaps (void)
{
    int lump;

    // Load in the light tables,
    // now 64k aligned for smokie...
    lump = W_GetNumForName("COLORMAP");
    colormaps = Z_MallocAlign (W_LumpLength (lump), PU_STATIC, 0, 16);
    W_ReadLump (lump,colormaps);

    //SoM: 3/30/2000: Init Boom colormaps.
    {
      R_ClearColormaps();
      R_InitExtraColormaps();
    }
}


int    foundcolormaps[MAXCOLORMAPS];

//SoM: Clears out extra colormaps between levels.
void R_ClearColormaps()
{
  int   i;

  num_extra_colormaps = 0;
  for(i = 0; i < MAXCOLORMAPS; i++)
    foundcolormaps[i] = -1;
  memset(extra_colormaps, 0, sizeof(extra_colormaps));
}



int R_ColormapNumForName(char *name)
{
  int lump, i;

  if(num_extra_colormaps == MAXCOLORMAPS)
    I_Error("R_ColormapNumForName: Too many colormaps!\n");

  lump = R_CheckNumForNameList(name, colormaplumps, numcolormaplumps);
  if(lump == -1)
    I_Error("R_ColormapNumForName: Cannot find colormap lump %s\n", name);

  for(i = 0; i < num_extra_colormaps; i++)
    if(lump == foundcolormaps[i])
      return i;

  foundcolormaps[num_extra_colormaps] = lump;

  // aligned on 8 bit for asm code
  extra_colormaps[num_extra_colormaps].colormap = Z_MallocAlign (W_LumpLength (lump), PU_LEVEL, 0, 8);
  W_ReadLump (lump,extra_colormaps[num_extra_colormaps].colormap);

  // SoM: Added, we set all params of the colormap to normal because there
  // is no real way to tell how GL should handle a colormap lump anyway..
  extra_colormaps[num_extra_colormaps].maskcolor = 0xffff;
  extra_colormaps[num_extra_colormaps].fadecolor = 0x0;
  extra_colormaps[num_extra_colormaps].maskamt = 0x0;
  extra_colormaps[num_extra_colormaps].fadestart = 0;
  extra_colormaps[num_extra_colormaps].fadeend = 33;
  extra_colormaps[num_extra_colormaps].fog = 0;

  num_extra_colormaps++;
  return num_extra_colormaps - 1;
}



// SoM:
//
// R_CreateColormap
// This is a more GL friendly way of doing colormaps: Specify colormap
// data in a special linedef's texture areas and use that to generate
// custom colormaps at runtime. NOTE: For GL mode, we only need to color
// data and not the colormap data. 
double  deltas[256][3], map[256][3];

unsigned char  NearestColor(unsigned char r, unsigned char g, unsigned char b);
int            RoundUp(double number);

int R_CreateColormap(char *p1, char *p2, char *p3)
{
  double cmaskr, cmaskg, cmaskb, cdestr, cdestg, cdestb;
  double r, g, b;
  double cbrightness;
  double maskamt = 0, othermask = 0;
  int    mask;
  int    i, p;
  char   *colormap_p;
  unsigned int  cr, cg, cb;
  unsigned int  maskcolor, fadecolor;
  unsigned int  fadestart = 0, fadeend = 33, fadedist = 33;
  int           fog = 0;
  int           mapnum = num_extra_colormaps;

  #define HEX2INT(x) (x >= '0' && x <= '9' ? x - '0' : x >= 'a' && x <= 'f' ? x - 'a' + 10 : x >= 'A' && x <= 'F' ? x - 'A' + 10 : 0)
  if(p1[0] == '#')
  {
    cr = cmaskr = ((HEX2INT(p1[1]) * 16) + HEX2INT(p1[2]));
    cg = cmaskg = ((HEX2INT(p1[3]) * 16) + HEX2INT(p1[4]));
    cb = cmaskb = ((HEX2INT(p1[5]) * 16) + HEX2INT(p1[6]));
    // Create a rough approximation of the color (a 16 bit color)
    maskcolor = ((cb) >> 3) + (((cg) >> 2) << 5) + (((cr) >> 3) << 11);
    if(p1[7] >= 'a' && p1[7] <= 'z')
      mask = (p1[7] - 'a');
    else if(p1[7] >= 'A' && p1[7] <= 'Z')
      mask = (p1[7] - 'A');
    else
      mask = 24;


    maskamt = (double)mask / (double)24;

    othermask = 1 - maskamt;
    maskamt /= 0xff;
    cmaskr *= maskamt;
    cmaskg *= maskamt;
    cmaskb *= maskamt;
  }
  else
  {
    cmaskr = 0xff;
    cmaskg = 0xff;
    cmaskb = 0xff;
    maskamt = 0;
    maskcolor = ((0xff) >> 3) + (((0xff) >> 2) << 5) + (((0xff) >> 3) << 11);
  }


  #define NUMFROMCHAR(c)  (c >= '0' && c <= '9' ? c - '0' : 0)
  if(p2[0] == '#')
  {
    // SoM: Get parameters like, fadestart, fadeend, and the fogflag...
    fadestart = NUMFROMCHAR(p2[3]) + (NUMFROMCHAR(p2[2]) * 10);
    fadeend = NUMFROMCHAR(p2[5]) + (NUMFROMCHAR(p2[4]) * 10);
    if(fadestart > 32 || fadestart < 0)
      fadestart = 0;
    if(fadeend > 33 || fadeend < 1)
      fadeend = 33;
    fadedist = fadeend - fadestart;
    fog = NUMFROMCHAR(p2[1]) ? 1 : 0;
  }
  #undef getnum


  if(p3[0] == '#')
  {
    cdestr = cr = ((HEX2INT(p3[1]) * 16) + HEX2INT(p3[2]));
    cdestg = cg = ((HEX2INT(p3[3]) * 16) + HEX2INT(p3[4]));
    cdestb = cb = ((HEX2INT(p3[5]) * 16) + HEX2INT(p3[6]));
    fadecolor = (((cb) >> 3) + (((cg) >> 2) << 5) + (((cr) >> 3) << 11));
  }
  else
  {
    cdestr = 0;
    cdestg = 0;
    cdestb = 0;
    fadecolor = 0;
  }
  #undef HEX2INT

  for(i = 0; i < num_extra_colormaps; i++)
  {
    if(foundcolormaps[i] != -1)
      continue;
    if(maskcolor == extra_colormaps[i].maskcolor &&
       fadecolor == extra_colormaps[i].fadecolor &&
       maskamt == extra_colormaps[i].maskamt &&
       fadestart == extra_colormaps[i].fadestart &&
       fadeend == extra_colormaps[i].fadeend &&
       fog == extra_colormaps[i].fog)
      return i;
  }

  if(num_extra_colormaps == MAXCOLORMAPS)
    I_Error("R_CreateColormap: Too many colormaps!\n");
  num_extra_colormaps++;

#ifdef HWRENDER
  if(rendermode == render_soft)
#endif
  {
    for(i = 0; i < 256; i++)
    {
      r = pLocalPalette[i].s.red;
      g = pLocalPalette[i].s.green;
      b = pLocalPalette[i].s.blue;
      cbrightness = sqrt((r*r) + (g*g) + (b*b));


      map[i][0] = (cbrightness * cmaskr) + (r * othermask);
      if(map[i][0] > 255.0)
        map[i][0] = 255.0;
      deltas[i][0] = (map[i][0] - cdestr) / (double)fadedist;

      map[i][1] = (cbrightness * cmaskg) + (g * othermask);
      if(map[i][1] > 255.0)
        map[i][1] = 255.0;
      deltas[i][1] = (map[i][1] - cdestg) / (double)fadedist;

      map[i][2] = (cbrightness * cmaskb) + (b * othermask);
      if(map[i][2] > 255.0)
        map[i][2] = 255.0;
      deltas[i][2] = (map[i][2] - cdestb) / (double)fadedist;
    }
  }

  foundcolormaps[mapnum] = -1;

  // aligned on 8 bit for asm code
  extra_colormaps[mapnum].colormap = NULL;
  extra_colormaps[mapnum].maskcolor = maskcolor;
  extra_colormaps[mapnum].fadecolor = fadecolor;
  extra_colormaps[mapnum].maskamt = maskamt;
  extra_colormaps[mapnum].fadestart = fadestart;
  extra_colormaps[mapnum].fadeend = fadeend;
  extra_colormaps[mapnum].fog = fog;

#define ABS2(x) (x) < 0 ? -(x) : (x)
#ifdef HWRENDER
  if(rendermode == render_soft)
#endif
  {
    extra_colormaps[mapnum].colormap = colormap_p = Z_MallocAlign((256 * 34) + 10, PU_LEVEL, 0, 16); // Aligning on 16 bits, NOT 8, keeps it from crashing! SSNTails 12-13-2002

    for(p = 0; p < 34; p++)
    {
      for(i = 0; i < 256; i++)
      {
        *colormap_p = NearestColor(RoundUp(map[i][0]), RoundUp(map[i][1]), RoundUp(map[i][2]));
        colormap_p++;
  
        if((unsigned int)p < fadestart)
          continue;
  
        if(ABS2(map[i][0] - cdestr) > ABS2(deltas[i][0]))
          map[i][0] -= deltas[i][0];
        else
          map[i][0] = cdestr;

        if(ABS2(map[i][1] - cdestg) > ABS2(deltas[i][1]))
          map[i][1] -= deltas[i][1];
        else
          map[i][1] = cdestg;

        if(ABS2(map[i][2] - cdestb) > ABS2(deltas[i][1]))
          map[i][2] -= deltas[i][2];
        else
          map[i][2] = cdestb;
      }
    }
  }
#undef ABS2

  return mapnum;
}


//Thanks to quake2 source!
//utils3/qdata/images.c
unsigned char NearestColor(unsigned char r, unsigned char g, unsigned char b) {
  int dr, dg, db;
  int distortion;
  int bestdistortion = 256 * 256 * 4;
  int bestcolor = 0;
  int i;

  for(i = 0; i < 256; i++) {
    dr = r - pLocalPalette[i].s.red;
    dg = g - pLocalPalette[i].s.green;
    db = b - pLocalPalette[i].s.blue;
    distortion = dr*dr + dg*dg + db*db;
    if(distortion < bestdistortion) {

      if(!distortion)
        return i;

      bestdistortion = distortion;
      bestcolor = i;
      }
    }

  return bestcolor;
  }


// Rounds off floating numbers and checks for 0 - 255 bounds
int RoundUp(double number) {
  if(number > 255.0)
    return 255.0;
  if(number < 0)
    return 0;

  if((int)number <= (int)(number -0.5))
    return (int)number + 1;

  return (int)number;
  }




char *R_ColormapNameForNum(int num)
{
  if(num == -1)
    return "NONE";

  if(num < 0 || num > MAXCOLORMAPS)
    I_Error("R_ColormapNameForNum: num is invalid!\n");

  if(foundcolormaps[num] == -1)
    return "INLEVEL";

  return wadfiles[foundcolormaps[num] >> 16]->lumpinfo[foundcolormaps[num] & 0xffff].name;
}


//
//  build a table for quick conversion from 8bpp to 15bpp
//
int makecol15(int r, int g, int b)
{
   return (((r >> 3) << 10) | ((g >> 3) << 5) | ((b >> 3)));
}

void R_Init8to16 (void)
{
    byte*       palette;
    int         i;

    palette = W_CacheLumpName ("PLAYPAL",PU_CACHE);

    for (i=0;i<256;i++)
    {
                // doom PLAYPAL are 8 bit values
        color8to16[i] = makecol15 (palette[0],palette[1],palette[2]);
        palette += 3;
    }

    // test a big colormap
    hicolormaps = Z_Malloc (32768 /**34*/, PU_STATIC, 0);
    for (i=0;i<16384;i++)
         hicolormaps[i] = i<<1;
}


//
// R_InitData
// Locates all the lumps
//  that will be used by all views
// Must be called after W_Init.
//
void R_InitData (void)
{
    //fab highcolor
    if (highcolor)
    {
        CONS_Printf ("\nInitHighColor...");
        R_Init8to16 ();
    }

    CONS_Printf ("\nInitTextures...");
    R_LoadTextures ();
    CONS_Printf ("\nInitFlats...");
    R_InitFlats ();

    CONS_Printf ("\nInitSprites...\n");
    R_InitSpriteLumps ();
    R_InitSprites (sprnames);

    CONS_Printf ("\nInitColormaps...\n");
    R_InitColormaps ();
}


//SoM: REmoved R_FlatNumForName


//
// R_CheckTextureNumForName
// Check whether texture is available.
// Filter out NoTexture indicator.
//
int     R_CheckTextureNumForName (char *name)
{
    int         i;

    // "NoTexture" marker.
    if (name[0] == '-')
        return 0;

    for (i=0 ; i<numtextures ; i++)
        if (!strncasecmp (textures[i]->name, name, 8) )
            return i;

    return -1;
}



//
// R_TextureNumForName
// Calls R_CheckTextureNumForName,
//  aborts with error message.
//
int R_TextureNumForName (char* name)
{
    int i;

    i = R_CheckTextureNumForName (name);

    if (i==-1)
    {
        //I_Error ("R_TextureNumForName: %.8s not found", name);
        CONS_Printf("WARNING: R_TextureNumForName: %.8s not found\n", name);
        return 1;
    }
    return i;
}




//
// R_PrecacheLevel
// Preloads all relevant graphics for the level.
//

// BP: rules : no extern in c !!!
//     slution put a new function in p_setup.c or put this in global (not recommended)
// SoM: Ok.. Here it goes. This function is in p_setup.c and caches the flats.
int P_PrecacheLevelFlats();

void R_PrecacheLevel (void)
{
//  char*               flatpresent; //SoM: 4/18/2000: No longer used
    char*               texturepresent;
    char*               spritepresent;

    int                 i;
    int                 j;
    int                 k;
    int                 lump;

    thinker_t*          th;
    spriteframe_t*      sf;

    //int numgenerated;  //faB:debug

    if (demoplayback)
        return;

    // do not flush the memory, Z_Malloc twice with same user
    // will cause error in Z_CheckHeap(), 19991022 by Kin
    if (rendermode != render_soft)
        return;

    // Precache flats.
    /*flatpresent = alloca(numflats);
    memset (flatpresent,0,numflats);

    // Check for used flats
    for (i=0 ; i<numsectors ; i++)
    {
#ifdef PARANOIA
        if( sectors[i].floorpic<0 || sectors[i].floorpic>numflats )
            I_Error("sectors[%d].floorpic=%d out of range [0..%d]\n",i,sectors[i].floorpic,numflats);
        if( sectors[i].ceilingpic<0 || sectors[i].ceilingpic>numflats )
            I_Error("sectors[%d].ceilingpic=%d out of range [0..%d]\n",i,sectors[i].ceilingpic,numflats);
#endif
        flatpresent[sectors[i].floorpic] = 1;
        flatpresent[sectors[i].ceilingpic] = 1;
    }

    flatmemory = 0;

    for (i=0 ; i<numflats ; i++)
    {
        if (flatpresent[i])
        {
            lump = firstflat + i;
            if(devparm)
               flatmemory += W_LumpLength(lump);
            R_GetFlat (lump);
//            W_CacheLumpNum(lump, PU_CACHE);
        }
    }*/
    flatmemory = P_PrecacheLevelFlats();

    //
    // Precache textures.
    //
    // no need to precache all software textures in 3D mode
    // (note they are still used with the reference software view)
    texturepresent = alloca(numtextures);
    memset (texturepresent,0, numtextures);

    for (i=0 ; i<numsides ; i++)
    {
        //Hurdler: huh, a potential bug here????
        if (sides[i].toptexture < numtextures)
            texturepresent[sides[i].toptexture] = 1;
        if (sides[i].midtexture < numtextures)
            texturepresent[sides[i].midtexture] = 1;
        if (sides[i].bottomtexture < numtextures)
            texturepresent[sides[i].bottomtexture] = 1;
    }

    // Sky texture is always present.
    // Note that F_SKY1 is the name used to
    //  indicate a sky floor/ceiling as a flat,
    //  while the sky texture is stored like
    //  a wall texture, with an episode dependend
    //  name.
    texturepresent[skytexture] = 1;

    //if (devparm)
    //    CONS_Printf("Generating textures..\n");

    texturememory = 0;
    for (i=0 ; i<numtextures ; i++)
    {
        if (!texturepresent[i])
            continue;

        //texture = textures[i];
        if( texturecache[i]==NULL )
            R_GenerateTexture (i);
        //numgenerated++;

        // note: pre-caching individual patches that compose textures became
        //       obsolete since we now cache entire composite textures

        //for (j=0 ; j<texture->patchcount ; j++)
        //{
        //    lump = texture->patches[j].patch;
        //    texturememory += W_LumpLength(lump);
        //    W_CacheLumpNum(lump , PU_CACHE);
        //}
    }
    //CONS_Printf ("total mem for %d textures: %d k\n",numgenerated,texturememory>>10);

    //
    // Precache sprites.
    //
    spritepresent = alloca(numsprites);
    memset (spritepresent,0, numsprites);

    for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    {
        if (th->function.acp1 == (actionf_p1)P_MobjThinker)
            spritepresent[((mobj_t *)th)->sprite] = 1;
    }

    spritememory = 0;
    for (i=0 ; i<numsprites ; i++)
    {
        if (!spritepresent[i])
            continue;

        for (j=0 ; j<sprites[i].numframes ; j++)
        {
            sf = &sprites[i].spriteframes[j];
            for (k=0 ; k<8 ; k++)
            {
                //Fab: see R_InitSprites for more about lumppat,lumpid
                lump = /*firstspritelump +*/ sf->lumppat[k];
                if(devparm)
                   spritememory += W_LumpLength(lump);
                W_CachePatchNum(lump , PU_CACHE);
            }
        }
    }

    //FIXME: this is no more correct with glide render mode
    if (devparm)
    {
        CONS_Printf("Precache level done:\n"
                    "flatmemory:    %ld k\n"
                    "texturememory: %ld k\n"
                    "spritememory:  %ld k\n", flatmemory>>10, texturememory>>10, spritememory>>10 );
    }
}
