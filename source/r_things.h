// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_things.h,v 1.11 2003/01/19 21:24:26 bock Exp $
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
// $Log: r_things.h,v $
// Revision 1.11  2003/01/19 21:24:26  bock
// Make sources buildable on FreeBSD 5-CURRENT.
//
// Revision 1.10  2001/12/31 16:56:39  metzgermeister
// see Dec 31 log
// .
//
// Revision 1.9  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.8  2001/06/16 08:07:55  bpereira
// no message
//
// Revision 1.7  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.6  2000/11/09 17:56:20  stroggonmeth
// Hopefully fixed a few bugs and did a few optimizations.
//
// Revision 1.5  2000/11/03 02:37:36  stroggonmeth
// Fix a few warnings when compiling.
//
// Revision 1.4  2000/11/02 17:50:10  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.3  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Rendering of moving objects, sprites.
//
//-----------------------------------------------------------------------------


#ifndef __R_THINGS__
#define __R_THINGS__

#include "sounds.h"

// number of sprite lumps for spritewidth,offset,topoffset lookup tables
// Fab: this is a hack : should allocate the lookup tables per sprite
#define     MAXSPRITELUMPS     4096

#define MAXVISSPRITES   256 // added 2-2-98 was 128

// Constant arrays used for psprite clipping
//  and initializing clipping.
extern short            negonearray[MAXVIDWIDTH];
extern short            screenheightarray[MAXVIDWIDTH];

// vars for R_DrawMaskedColumn
extern short*           mfloorclip;
extern short*           mceilingclip;
extern fixed_t          spryscale;
extern fixed_t          sprtopscreen;
extern fixed_t          sprbotscreen;
extern fixed_t          windowtop;
extern fixed_t          windowbottom;

extern fixed_t          pspritescale;
extern fixed_t          pspriteiscale;
extern fixed_t          pspriteyscale;  //added:02-02-98:for aspect ratio

extern const int PSpriteSY[];

void R_DrawMaskedColumn (column_t* column);

void R_SortVisSprites (void);

//faB: find sprites in wadfile, replace existing, add new ones
//     (only sprites from namelist are added or replaced)
void R_AddSpriteDefs (char** namelist, int wadnum);

//SoM: 6/5/2000: Light sprites correctly!
void R_AddSprites (sector_t* sec, int lightlevel);
void R_AddPSprites (void);
void R_DrawSprite (vissprite_t* spr);
void R_InitSprites (char** namelist);
void R_ClearSprites (void);
void R_DrawSprites (void);  //draw all vissprites
void R_DrawMasked (void);

void
R_ClipVisSprite
( vissprite_t*          vis,
  int                   xl,
  int                   xh );


// -----------
// SKINS STUFF
// -----------
#define SKINNAMESIZE 16
#define DEFAULTSKIN  "marine"   // name of the standard doom marine as skin

typedef struct
{
    char        name[SKINNAMESIZE+1];   // short descriptive name of the skin
    spritedef_t spritedef;
    char        faceprefix[4];          // 3 chars+'\0', default is "STF"

    // specific sounds per skin
    short       soundsid[NUMSKINSOUNDS]; // sound # in S_sfx table

} skin_t;

extern int       numskins;
extern skin_t    skins[MAXSKINS+1];
//extern CV_PossibleValue_t skin_cons_t[MAXSKINS+1];
extern consvar_t cv_skin;

//void    R_InitSkins (void);
void    SetPlayerSkin(int playernum,char *skinname);
int     R_SkinAvailable (char* name);
void    R_AddSkins (int wadnum);

void    R_InitDrawNodes();

#endif /*__R_THINGS__*/
