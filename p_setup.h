// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_setup.h,v 1.6 2002/07/26 15:21:36 hurdler Exp $
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
// $Log: p_setup.h,v $
// Revision 1.6  2002/07/26 15:21:36  hurdler
// near RC release
//
// Revision 1.5  2002/07/24 19:03:06  ssntails
// Added support for things to retain spawned Z position.
//
// Revision 1.4  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.3  2000/11/02 17:50:08  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//   Setup a game, startup stuff.
//
//-----------------------------------------------------------------------------


#ifndef __P_SETUP__
#define __P_SETUP__

#include "doomdata.h"
#include "r_defs.h"

// Player spawn spots for deathmatch.
#define MAX_DM_STARTS   64
extern  mapthing_t      *deathmatchstarts[MAX_DM_STARTS];
extern  int             numdmstarts;
//extern  mapthing_t**    deathmatch_p;

extern int        lastloadedmaplumpnum; // for comparative savegame
//
// MAP used flats lookup table
//
typedef struct
{
    char        name[8];        // resource name from wad
    int         lumpnum;        // lump number of the flat

    // for flat animation
    int         baselumpnum;
    int         animseq;        // start pos. in the anim sequence
    int         numpics;
    int         speed;
} levelflat_t;

extern int             numlevelflats;
extern levelflat_t*    levelflats;
int P_AddLevelFlat (char* flatname, levelflat_t* levelflat);
char *P_FlatNameForNum(int num);

extern int             nummapthings;
extern mapthing_t*     mapthings;

// NOT called by W_Ticker. Fixme.
boolean P_SetupLevel( int           episode,
                      int           map,
                      skill_t       skill,
                      char*         mapname);

boolean P_AddWadFile (char* wadfilename,char **firstmapname);

subsector_t* R_PointInSubsector(fixed_t x, fixed_t y);


extern boolean  newlevel;
extern boolean  doom1level;
extern char     *levelmapname;

#endif
