// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: p_spec.c,v 1.45 2003/07/23 17:26:36 darkwolf95 Exp $
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
// $Log: p_spec.c,v $
// Revision 1.45  2003/07/23 17:26:36  darkwolf95
// SetLineTexture function for Fraggle Script
//
// Revision 1.44  2003/06/14 04:29:43  ssntails
// Added linedef #s for vertical wind currents. 294 is up, 295 is down.
//
// Revision 1.43  2003/06/11 03:02:12  ssntails
// Vertical wind currents and friction capability on 3d floors
//
// Revision 1.42  2003/05/04 07:34:38  sburke
// Removed LONG swap of scroll_t->affectee, as it broke single-player mode.
//
// Revision 1.41  2003/05/04 02:39:34  sburke
// Need to use LONG(s->affectee) for big-endian machines.
//
// Revision 1.40  2002/09/26 22:54:15  ssntails
// Fixed bugs with new current/wind handling
//
// Revision 1.39  2002/09/25 18:25:21  ssntails
// Better wind/current support for 3d floors
//
// Revision 1.38  2002/09/25 16:38:35  ssntails
// Alpha support for trans 3d floors in software
//
// Revision 1.37  2002/07/26 15:21:36  hurdler
// near RC release
//
// Revision 1.36  2002/07/18 20:47:41  ssntails
// Fix for vertical currents
//
// Revision 1.35  2002/06/30 13:57:30  ssntails
// Added vertical water currents.
//
// Revision 1.34  2002/06/23 17:14:31  ssntails
// Fixed bug with animated flats
//
// Revision 1.33  2002/06/14 02:43:43  ssntails
// Instant-lower and instant-raise capability for sectors added.
//
// Revision 1.32  2002/01/12 02:21:36  stroggonmeth
// Big commit
//
// Revision 1.31  2001/08/19 20:41:04  hurdler
// small changes
//
// Revision 1.30  2001/08/13 22:53:40  stroggonmeth
// Small commit
//
// Revision 1.29  2001/08/11 15:18:02  hurdler
// Add sector colormap in hw mode (first attempt)
//
// Revision 1.28  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.27  2001/06/16 08:07:55  bpereira
// no message
//
// Revision 1.26  2001/05/27 13:42:48  bpereira
// no message
//
// Revision 1.25  2001/05/03 18:23:30  crashrl
// corrected wrong comment concerning teamstartsec
//
// Revision 1.24  2001/04/04 20:24:21  judgecutor
// Added support for the 3D Sound
//
// Revision 1.23  2001/03/21 18:24:38  stroggonmeth
// Misc changes and fixes. Code cleanup
//
// Revision 1.22  2001/03/13 22:14:19  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.21  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.20  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.19  2000/11/21 21:13:17  stroggonmeth
// Optimised 3D floors and fixed crashing bug in high resolutions.
//
// Revision 1.18  2000/11/11 13:59:45  bpereira
// no message
//
// Revision 1.17  2000/11/04 16:23:43  bpereira
// no message
//
// Revision 1.16  2000/11/02 17:50:08  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.15  2000/10/21 08:43:30  bpereira
// no message
//
// Revision 1.14  2000/10/07 20:36:13  crashrl
// Added deathmatch team-start-sectors via sector/line-tag and linedef-type 1000-1031
//
// Revision 1.13  2000/05/23 15:22:34  stroggonmeth
// Not much. A graphic bug fixed.
//
// Revision 1.12  2000/05/03 23:51:00  stroggonmeth
// A few, quick, changes.
//
// Revision 1.11  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.10  2000/04/15 22:12:57  stroggonmeth
// Minor bug fixes
//
// Revision 1.9  2000/04/13 23:47:47  stroggonmeth
// See logs
//
// Revision 1.8  2000/04/11 19:07:24  stroggonmeth
// Finished my logs, fixed a crashing bug.
//
// Revision 1.7  2000/04/08 17:29:25  stroggonmeth
// no message
//
// Revision 1.6  2000/04/08 11:27:29  hurdler
// fix some boom stuffs
//
// Revision 1.5  2000/04/06 20:54:28  hurdler
// Mostly remove warnings under windows
//
// Revision 1.4  2000/04/04 19:28:43  stroggonmeth
// Global colormaps working. Added a new linedef type 272.
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
//      Implements special effects:
//      Texture animation, height or lighting changes
//       according to adjacent sectors, respective
//       utility functions, etc.
//      Line Tag handling. Line and Sector triggers.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "g_game.h"
#include "p_local.h"
#include "p_setup.h"    //levelflats for flat animation
#include "r_data.h"
#include "m_random.h"

#include "s_sound.h"
#include "w_wad.h"
#include "z_zone.h"
#include "dstrings.h" //SoM: 3/10/2000
#include "r_main.h"   //Two extra includes.
#include "t_script.h"
//#include "r_sky.h" // Portals

#include "hardware/hw3sound.h"

//SoM: Enable Boom features?
int boomsupport = 1;
int variable_friction = 1;
int allow_pushers = 1;



//SoM: 3/7/2000
static void P_SpawnScrollers(void);

static void P_SpawnFriction(void);
static void P_SpawnPushers(void);
static void Add_Pusher(int type, int x_mag, int y_mag, mobj_t* source, int affectee); //SoM: 3/9/2000
void P_FindAnimatedFlat (int i);



//
// Animating textures and planes
// There is another anim_t used in wi_stuff, unrelated.
//
typedef struct
{
    boolean     istexture;
    int         picnum;
    int         basepic;
    int         numpics;
    int         speed;
} anim_t;


//
//      source animation definition
//
#pragma pack(1) //Hurdler: 04/04/2000: I think pragma is more portable
typedef struct
{
    char        istexture;      // if false, it is a flat
    char        endname[9];
    char        startname[9];
    int         speed;
} animdef_t; 
#pragma pack()



#define MAXANIMS     32


//SoM: 3/7/2000: New sturcture without limits.
static anim_t*   lastanim;
static anim_t*   anims;
static size_t    maxanims;

//
// P_InitPicAnims
//

// Floor/ceiling animation sequences,
//  defined by first and last frame,
//  i.e. the flat (64x64 tile) name to
//  be used.
// The full animation sequence is given
//  using all the flats between the start
//  and end entry, in the order found in
//  the WAD file.
//
animdef_t               harddefs[] =
{
    // DOOM II flat animations.
    {false,     "NUKAGE3",      "NUKAGE1",      8},
    {false,     "FWATER4",      "FWATER1",      8},
    {false,     "SWATER4",      "SWATER1",      8},
    {false,     "LAVA4",        "LAVA1",        8},
    {false,     "BLOOD3",       "BLOOD1",       8},

    {false,     "RROCK08",      "RROCK05",      8},
    {false,     "SLIME04",      "SLIME01",      8},
    {false,     "SLIME08",      "SLIME05",      8},
    {false,     "SLIME12",      "SLIME09",      8},

    // animated textures
    {true,      "BLODGR4",      "BLODGR1",      8},
    {true,      "SLADRIP3",     "SLADRIP1",     8},

    {true,      "BLODRIP4",     "BLODRIP1",     8},
    {true,      "FIREWALL",     "FIREWALA",     8},
    {true,      "GSTFONT3",     "GSTFONT1",     8},
    {true,      "FIRELAVA",     "FIRELAV3",     8},
    {true,      "FIREMAG3",     "FIREMAG1",     8},
    {true,      "FIREBLU2",     "FIREBLU1",     8},
    {true,      "ROCKRED3",     "ROCKRED1",     8},

    {true,      "BFALL4",       "BFALL1",       8},
    {true,      "SFALL4",       "SFALL1",       8},
    {true,      "WFALL4",       "WFALL1",       8},
    {true,      "DBRAIN4",      "DBRAIN1",      8},

    // heretic 
    {false,     "FLTWAWA3",     "FLTWAWA1",     8}, // Water
    {false,     "FLTSLUD3",     "FLTSLUD1",     8}, // Sludge
    {false,     "FLTTELE4",     "FLTTELE1",     6}, // Teleport
    {false,     "FLTFLWW3",     "FLTFLWW1",     9}, // River - West
    {false,     "FLTLAVA4",     "FLTLAVA1",     8}, // Lava
    {false,     "FLATHUH4",     "FLATHUH1",     8}, // Super Lava
    {true,      "LAVAFL3",      "LAVAFL1",      6}, // Texture: Lavaflow
    {true,      "WATRWAL3",     "WATRWAL1",     4}, // Texture: Waterfall

    {-1}
};


//
//      Animating line specials
//

//
// Init animated textures
// - now called at level loading P_SetupLevel()
//

static animdef_t   *animdefs;

//SoM: 3/7/2000: Use new boom method of reading lump from wad file.
void P_InitPicAnims (void)
{
  //  Init animation
  int         i;

  if(W_CheckNumForName("ANIMATED") != -1)
    animdefs = (animdef_t *)W_CacheLumpName("ANIMATED",PU_STATIC);
  else
    animdefs = harddefs;

  for (i = 0; animdefs[i].istexture != -1; i++, maxanims++);
  anims = (anim_t *)malloc(sizeof(anim_t) * (maxanims + 1));

  lastanim = anims;
  for (i = 0; animdefs[i].istexture != -1; i++)
  {

    if (animdefs[i].istexture)
    {
      // different episode ?
      if (R_CheckTextureNumForName(animdefs[i].startname) == -1)
          continue;

      lastanim->picnum = R_TextureNumForName (animdefs[i].endname);
      lastanim->basepic = R_TextureNumForName (animdefs[i].startname);
    }
    else
    {
      if ((W_CheckNumForName(animdefs[i].startname)) == -1)
          continue;

      lastanim->picnum = R_FlatNumForName (animdefs[i].endname);
      lastanim->basepic = R_FlatNumForName (animdefs[i].startname);
    }


    lastanim->istexture = (boolean)animdefs[i].istexture;
    lastanim->numpics = lastanim->picnum - lastanim->basepic + 1;

    if (lastanim->numpics < 2)
        I_Error ("P_InitPicAnims: bad cycle from %s to %s",
                  animdefs[i].startname,
                  animdefs[i].endname);

    lastanim->speed = LONG(animdefs[i].speed) * NEWTICRATERATIO;
    lastanim++;
  }
  lastanim->istexture = -1;

  if(animdefs != harddefs)
    Z_ChangeTag (animdefs,PU_CACHE);
}

//  Check for flats in levelflats, that are part
//  of a flat anim sequence, if so, set them up for animation
//
//SoM: 3/16/2000: Changed parameter from pointer to "anims" entry number
void P_FindAnimatedFlat (int animnum)
{
    int            i;
    int            startflatnum,endflatnum;
    levelflat_t*   foundflats = levelflats;

    startflatnum = anims[animnum].basepic;
    endflatnum   = anims[animnum].picnum;

    // note: high word of lumpnum is the wad number
    if ( (startflatnum>>16) != (endflatnum>>16) )
       I_Error ("AnimatedFlat start %s not in same wad as end %s\n",
                animdefs[animnum].startname, animdefs[animnum].endname);

    //
    // now search through the levelflats if this anim flat sequence is used
    //
    for (i = 0; i<numlevelflats; i++, foundflats++)
    {
        // is that levelflat from the flat anim sequence ?
        if (foundflats->lumpnum >= startflatnum &&
            foundflats->lumpnum <= endflatnum)
        {
            foundflats->baselumpnum = startflatnum;
            foundflats->animseq = foundflats->lumpnum - startflatnum;
            foundflats->numpics = endflatnum - startflatnum + 1;
            foundflats->speed = anims[animnum].speed;

            if (devparm)
                CONS_Printf("animflat: %#03d name:%.8s animseq:%d numpics:%d speed:%d\n",
                            i, foundflats->name, foundflats->animseq,
                            foundflats->numpics,foundflats->speed);
        }
    }

}


//
//  Called by P_LoadSectors
//
void P_SetupLevelFlatAnims (void)
{
    int    i;

    // the original game flat anim sequences
    for (i=0 ; anims[i].istexture != -1; i++)
    {
        if (!anims[i].istexture)
        {
            P_FindAnimatedFlat (i);
        }
    }
}


//
// UTILITIES
//


//
// getSide()
// Will return a side_t*
//  given the number of the current sector,
//  the line number, and the side (0/1) that you want.
//
side_t*
getSide
( int           currentSector,
  int           line,
  int           side )
{
    return &sides[ (sectors[currentSector].lines[line])->sidenum[side] ];
}


//
// getSector()
// Will return a sector_t*
//  given the number of the current sector,
//  the line number and the side (0/1) that you want.
//
sector_t*
getSector
( int           currentSector,
  int           line,
  int           side )
{
    return sides[ (sectors[currentSector].lines[line])->sidenum[side] ].sector;
}


//
// twoSided()
// Given the sector number and the line number,
//  it will tell you whether the line is two-sided or not.
//
//SoM: 3/7/2000: Use the boom method
int
twoSided
( int   sector,
  int   line )
{
  return boomsupport?
    ((sectors[sector].lines[line])->sidenum[1] != -1)
    :
    ((sectors[sector].lines[line])->flags & ML_TWOSIDED);
}




//
// getNextSector()
// Return sector_t * of sector next to current.
// NULL if not two-sided line
//
//SoM: 3/7/2000: Use boom method.
sector_t*
getNextSector
( line_t*       line,
  sector_t*     sec )
{
  if (!boomsupport)
  {
    if (!(line->flags & ML_TWOSIDED))
      return NULL;
  }

  if (line->frontsector == sec)
  {
    if (!boomsupport || line->backsector!=sec)
      return line->backsector;
    else
      return NULL;
  }
  return line->frontsector;
}




//
// P_FindLowestFloorSurrounding()
// FIND LOWEST FLOOR HEIGHT IN SURROUNDING SECTORS
//
fixed_t P_FindLowestFloorSurrounding(sector_t* sec)
{
    int                 i;
    line_t*             check;
    sector_t*           other;
    fixed_t             floor = sec->floorheight;

    for (i=0 ;i < sec->linecount ; i++)
    {
        check = sec->lines[i];
        other = getNextSector(check,sec);

        if (!other)
            continue;

        if (other->floorheight < floor)
            floor = other->floorheight;
    }
    return floor;
}




//
// P_FindHighestFloorSurrounding()
// FIND HIGHEST FLOOR HEIGHT IN SURROUNDING SECTORS
//
fixed_t P_FindHighestFloorSurrounding(sector_t *sec)
{
    int                 i;
    line_t*             check;
    sector_t*           other;
    fixed_t             floor = -500*FRACUNIT;
    int                 foundsector = 0;


    for (i=0 ;i < sec->linecount ; i++)
    {
        check = sec->lines[i];
        other = getNextSector(check,sec);

        if (!other)
            continue;

        if (other->floorheight > floor || !foundsector)
            floor = other->floorheight;

        if(!foundsector)
          foundsector = 1;
    }
    return floor;
}



//
// P_FindNextHighestFloor
// FIND NEXT HIGHEST FLOOR IN SURROUNDING SECTORS
// SoM: 3/7/2000: Use Lee Killough's version insted.
// Rewritten by Lee Killough to avoid fixed array and to be faster
//
fixed_t P_FindNextHighestFloor(sector_t *sec, int currentheight)
{
  sector_t *other;
  int i;

  for (i=0 ;i < sec->linecount ; i++)
    if ((other = getNextSector(sec->lines[i],sec)) &&
         other->floorheight > currentheight)
    {
      int height = other->floorheight;
      while (++i < sec->linecount)
        if ((other = getNextSector(sec->lines[i],sec)) &&
            other->floorheight < height &&
            other->floorheight > currentheight)
          height = other->floorheight;
      return height;
    }
  return currentheight;
}


////////////////////////////////////////////////////
// SoM: Start new Boom functions
////////////////////////////////////////////////////

// P_FindNextLowestFloor()
//
// Passed a sector and a floor height, returns the fixed point value
// of the largest floor height in a surrounding sector smaller than
// the floor height passed. If no such height exists the floorheight
// passed is returned.
//
//
fixed_t P_FindNextLowestFloor(sector_t *sec, int currentheight)
{
  sector_t *other;
  int i;

  for (i=0 ;i < sec->linecount ; i++)
    if ((other = getNextSector(sec->lines[i],sec)) &&
         other->floorheight < currentheight)
    {
      int height = other->floorheight;
      while (++i < sec->linecount)
        if ((other = getNextSector(sec->lines[i],sec)) &&
            other->floorheight > height &&
            other->floorheight < currentheight)
          height = other->floorheight;
      return height;
    }
  return currentheight;
}


//
// P_FindNextLowestCeiling()
//
// Passed a sector and a ceiling height, returns the fixed point value
// of the largest ceiling height in a surrounding sector smaller than
// the ceiling height passed. If no such height exists the ceiling height
// passed is returned.
//
//
fixed_t P_FindNextLowestCeiling(sector_t *sec, int currentheight)
{
  sector_t *other;
  int i;

  for (i=0 ;i < sec->linecount ; i++)
    if ((other = getNextSector(sec->lines[i],sec)) &&
        other->ceilingheight < currentheight)
    {
      int height = other->ceilingheight;
      while (++i < sec->linecount)
        if ((other = getNextSector(sec->lines[i],sec)) &&
            other->ceilingheight > height &&
            other->ceilingheight < currentheight)
          height = other->ceilingheight;
      return height;
    }
  return currentheight;
}




//
// P_FindNextHighestCeiling()
//
// Passed a sector and a ceiling height, returns the fixed point value
// of the smallest ceiling height in a surrounding sector larger than
// the ceiling height passed. If no such height exists the ceiling height
// passed is returned.
//
//
fixed_t P_FindNextHighestCeiling(sector_t *sec, int currentheight)
{
  sector_t *other;
  int i;

  for (i=0 ;i < sec->linecount ; i++)
    if ((other = getNextSector(sec->lines[i],sec)) &&
         other->ceilingheight > currentheight)
    {
      int height = other->ceilingheight;
      while (++i < sec->linecount)
        if ((other = getNextSector(sec->lines[i],sec)) &&
            other->ceilingheight < height &&
            other->ceilingheight > currentheight)
          height = other->ceilingheight;
      return height;
    }
  return currentheight;
}

////////////////////////////
// End New Boom functions
////////////////////////////



//
// FIND LOWEST CEILING IN THE SURROUNDING SECTORS
//
fixed_t
P_FindLowestCeilingSurrounding(sector_t* sec)
{
    int                 i;
    line_t*             check;
    sector_t*           other;
    fixed_t             height = MAXINT;
    int                 foundsector = 0;

    if (boomsupport) height = 32000*FRACUNIT; //SoM: 3/7/2000: Remove ovf
                                              
    for (i=0 ;i < sec->linecount ; i++)
    {
        check = sec->lines[i];
        other = getNextSector(check,sec);

        if (!other)
            continue;

        if (other->ceilingheight < height || !foundsector)
            height = other->ceilingheight;

        if(!foundsector)
          foundsector = 1;
    }
    return height;
}


//
// FIND HIGHEST CEILING IN THE SURROUNDING SECTORS
//
fixed_t P_FindHighestCeilingSurrounding(sector_t* sec)
{
    int         i;
    line_t*     check;
    sector_t*   other;
    fixed_t     height = 0;
    int         foundsector = 0;

    for (i=0 ;i < sec->linecount ; i++)
    {
        check = sec->lines[i];
        other = getNextSector(check,sec);

        if (!other)
            continue;

        if (other->ceilingheight > height || !foundsector)
            height = other->ceilingheight;

        if(!foundsector)
          foundsector = 1;
    }
    return height;
}



//SoM: 3/7/2000: UTILS.....
//
// P_FindShortestTextureAround()
//
// Passed a sector number, returns the shortest lower texture on a
// linedef bounding the sector.
//
//
fixed_t P_FindShortestTextureAround(int secnum)
{
  int minsize = MAXINT;
  side_t*     side;
  int i;
  sector_t *sec = &sectors[secnum];

  if (boomsupport)
    minsize = 32000<<FRACBITS;

  for (i = 0; i < sec->linecount; i++)
  {
    if (twoSided(secnum, i))
    {
      side = getSide(secnum,i,0);
      if (side->bottomtexture > 0)
        if (textureheight[side->bottomtexture] < minsize)
          minsize = textureheight[side->bottomtexture];
      side = getSide(secnum,i,1);
      if (side->bottomtexture > 0)
        if (textureheight[side->bottomtexture] < minsize)
          minsize = textureheight[side->bottomtexture];
    }
  }
  return minsize;
}



//SoM: 3/7/2000: Stuff.... (can you tell I'm getting tired? It's 12:30!)
//
// P_FindShortestUpperAround()
//
// Passed a sector number, returns the shortest upper texture on a
// linedef bounding the sector.
//
//
fixed_t P_FindShortestUpperAround(int secnum)
{
  int minsize = MAXINT;
  side_t*     side;
  int i;
  sector_t *sec = &sectors[secnum];

  if (boomsupport)
    minsize = 32000<<FRACBITS;

  for (i = 0; i < sec->linecount; i++)
  {
    if (twoSided(secnum, i))
    {
      side = getSide(secnum,i,0);
      if (side->toptexture > 0)
        if (textureheight[side->toptexture] < minsize)
          minsize = textureheight[side->toptexture];
      side = getSide(secnum,i,1);
      if (side->toptexture > 0)
        if (textureheight[side->toptexture] < minsize)
          minsize = textureheight[side->toptexture];
    }
  }
  return minsize;
}




//SoM: 3/7/2000
//
// P_FindModelFloorSector()
//
// Passed a floor height and a sector number, return a pointer to a
// a sector with that floor height across the lowest numbered two sided
// line surrounding the sector.
//
// Note: If no sector at that height bounds the sector passed, return NULL
//
//
sector_t *P_FindModelFloorSector(fixed_t floordestheight,int secnum)
{
  int i;
  sector_t *sec=NULL;
  int linecount;

  sec = &sectors[secnum];
  linecount = sec->linecount;
  for (i = 0; i < (!boomsupport && sec->linecount<linecount?
                   sec->linecount : linecount); i++)
  {
    if ( twoSided(secnum, i) )
    {
      if (getSide(secnum,i,0)->sector-sectors == secnum)
          sec = getSector(secnum,i,1);
      else
          sec = getSector(secnum,i,0);

      if (sec->floorheight == floordestheight)
        return sec;
    }
  }
  return NULL;
}



//SoM: 3/7/2000: Last one...
//
// P_FindModelCeilingSector()
//
// Passed a ceiling height and a sector number, return a pointer to a
// a sector with that ceiling height across the lowest numbered two sided
// line surrounding the sector.
//
// Note: If no sector at that height bounds the sector passed, return NULL
//
//
sector_t *P_FindModelCeilingSector(fixed_t ceildestheight,int secnum)
{
  int i;
  sector_t *sec=NULL;
  int linecount;

  sec = &sectors[secnum];
  linecount = sec->linecount;
  for (i = 0; i < (!boomsupport && sec->linecount<linecount?
                   sec->linecount : linecount); i++)
  {
    if ( twoSided(secnum, i) )
    {
      if (getSide(secnum,i,0)->sector-sectors == secnum)
          sec = getSector(secnum,i,1);
      else
          sec = getSector(secnum,i,0);

      if (sec->ceilingheight == ceildestheight)
        return sec;
    }
  }
  return NULL;
}



//
// RETURN NEXT SECTOR # THAT LINE TAG REFERS TO
//
//SoM: 3/7/2000: Killough wrote this to improve the process.
int
P_FindSectorFromLineTag
( line_t*       line,
  int           start )
{
  start = start >= 0 ? sectors[start].nexttag :
    sectors[(unsigned) line->tag % (unsigned) numsectors].firsttag;
  while (start >= 0 && sectors[start].tag != line->tag)
    start = sectors[start].nexttag;
  return start;
}



//
// P_FindSectorFromTag
// Used by FraggleScript
int
P_FindSectorFromTag
( int           tag,
  int           start )
{
  start = start >= 0 ? sectors[start].nexttag :
    sectors[(unsigned) tag % (unsigned) numsectors].firsttag;
  while (start >= 0 && sectors[start].tag != tag)
    start = sectors[start].nexttag;
  return start;
}

//DarkWolf95:July 23, 2003: Needed for SF_SetLineTexture
int P_FindLineFromTag(int tag, int start)
{
  start = start >= 0 ? lines[start].nexttag :
    lines[(unsigned) tag % (unsigned) numlines].firsttag;
  while (start >= 0 && lines[start].tag != tag)
    start = lines[start].nexttag;
  return start;
}

//SoM: 3/7/2000: More boom specific stuff...
// killough 4/16/98: Same thing, only for linedefs

int P_FindLineFromLineTag(const line_t *line, int start)
{
  start = start >= 0 ? lines[start].nexttag :
    lines[(unsigned) line->tag % (unsigned) numlines].firsttag;
  while (start >= 0 && lines[start].tag != line->tag)
    start = lines[start].nexttag;
  return start;
}



//SoM: 3/7/2000: Oh joy!
// Hash the sector tags across the sectors and linedefs.
static void P_InitTagLists(void)
{
  register int i;

  for (i=numsectors; --i>=0; )
    sectors[i].firsttag = -1;
  for (i=numsectors; --i>=0; )
    {
      int j = (unsigned) sectors[i].tag % (unsigned) numsectors;
      sectors[i].nexttag = sectors[j].firsttag;
      sectors[j].firsttag = i;
    }

  for (i=numlines; --i>=0; )
    lines[i].firsttag = -1;
  for (i=numlines; --i>=0; )
    {
      int j = (unsigned) lines[i].tag % (unsigned) numlines;
      lines[i].nexttag = lines[j].firsttag;
      lines[j].firsttag = i;
    }
}




//
// Find minimum light from an adjacent sector
//
int
P_FindMinSurroundingLight
( sector_t*     sector,
  int           max )
{
    int         i;
    int         min;
    line_t*     line;
    sector_t*   check;

    min = max;
    for (i=0 ; i < sector->linecount ; i++)
    {
        line = sector->lines[i];
        check = getNextSector(line,sector);

        if (!check)
            continue;

        if (check->lightlevel < min)
            min = check->lightlevel;
    }
    return min;
}



//SoM: 3/7/2000
//
// P_CanUnlockGenDoor()
//
// Passed a generalized locked door linedef and a player, returns whether
// the player has the keys necessary to unlock that door.
//
// Note: The linedef passed MUST be a generalized locked door type
//       or results are undefined.
//
//
boolean P_CanUnlockGenDoor( line_t* line, player_t* player)
{
  // does this line special distinguish between skulls and keys?
  int skulliscard = (line->special & LockedNKeys)>>LockedNKeysShift;

  // determine for each case of lock type if player's keys are adequate
  switch((line->special & LockedKey)>>LockedKeyShift)
  {
    case AnyKey_:
      if
      (
        !(player->cards & it_redcard) &&
        !(player->cards & it_redskull) &&
        !(player->cards & it_bluecard) &&
        !(player->cards & it_blueskull) &&
        !(player->cards & it_yellowcard) &&
        !(player->cards & it_yellowskull)
      )
      {
        player->message = PD_ANY;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case RCard:
      if
      (
        !(player->cards & it_redcard) &&
        (!skulliscard || !(player->cards & it_redskull))
      )
      {
        player->message = skulliscard? PD_REDK : PD_REDC;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case BCard:
      if
      (
        !(player->cards & it_bluecard) &&
        (!skulliscard || !(player->cards & it_blueskull))
      )
      {
        player->message = skulliscard? PD_BLUEK : PD_BLUEC;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case YCard:
      if
      (
        !(player->cards & it_yellowcard) &&
        (!skulliscard || !(player->cards & it_yellowskull))
      )
      {
        player->message = skulliscard? PD_YELLOWK : PD_YELLOWC;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case RSkull:
      if
      (
        !(player->cards & it_redskull) &&
        (!skulliscard || !(player->cards & it_redcard))
      )
      {
        player->message = skulliscard? PD_REDK : PD_REDS;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case BSkull:
      if
      (
        !(player->cards & it_blueskull) &&
        (!skulliscard || !(player->cards & it_bluecard))
      )
      {
        player->message = skulliscard? PD_BLUEK : PD_BLUES;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case YSkull:
      if
      (
        !(player->cards & it_yellowskull) &&
        (!skulliscard || !(player->cards & it_yellowcard))
      )
      {
        player->message = skulliscard? PD_YELLOWK : PD_YELLOWS;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
    case AllKeys:
      if
      (
        !skulliscard &&
        (
          !(player->cards & it_redcard) ||
          !(player->cards & it_redskull) ||
          !(player->cards & it_bluecard) ||
          !(player->cards & it_blueskull) ||
          !(player->cards & it_yellowcard) ||
          !(player->cards & it_yellowskull)
        )
      )
      {
        player->message = PD_ALL6;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      if
      (
        skulliscard &&
        (
          (!(player->cards & it_redcard) &&
            !(player->cards & it_redskull)) ||
          (!(player->cards & it_bluecard) &&
            !(player->cards & it_blueskull)) ||
          (!(player->cards & it_yellowcard) &&
            !(player->cards & it_yellowskull))
        )
      )
      {
        player->message = PD_ALL3;
        S_StartScreamSound(player->mo, sfx_oof);
        return false;
      }
      break;
  }
  return true;
}





//
// P_SectorActive()
//
// Passed a linedef special class (floor, ceiling, lighting) and a sector
// returns whether the sector is already busy with a linedef special of the
// same class. If old demo compatibility true, all linedef special classes
// are the same.
//
//
int P_SectorActive(special_e t,sector_t *sec)
{
  if (!boomsupport)
    return sec->floordata || sec->ceilingdata || sec->lightingdata;
  else
    switch (t)
    {
      case floor_special:
        return (int)sec->floordata;
      case ceiling_special:
        return (int)sec->ceilingdata;
      case lighting_special:
        return (int)sec->lightingdata;
    }
  return 1;
}


//SoM: 3/7/2000
//
// P_CheckTag()
//
// Passed a line, returns true if the tag is non-zero or the line special
// allows no tag without harm. If compatibility, all linedef specials are
// allowed to have zero tag.
//
// Note: Only line specials activated by walkover, pushing, or shooting are
//       checked by this routine.
//
//
int P_CheckTag(line_t *line)
{
  if (!boomsupport)
    return 1;

  if (line->tag)
    return 1;

  switch(line->special)
  {
    case 1:                 // Manual door specials
    case 26:
    case 27:
    case 28:
    case 31:
    case 32:
    case 33:
    case 34:
    case 117:
    case 118:

    case 139:               // Lighting specials
    case 170:
    case 79:
    case 35:
    case 138:
    case 171:
    case 81:
    case 13:
    case 192:
    case 169:
    case 80:
    case 12:
    case 194:
    case 173:
    case 157:
    case 104:
    case 193:
    case 172:
    case 156:
    case 17:

    case 195:               // Thing teleporters
    case 174:
    case 97:
    case 39:
    case 126:
    case 125:
    case 210:
    case 209:
    case 208:
    case 207:

    case 11:                // Exits
    case 52:
    case 197:
    case 51:
    case 124:
    case 198:

    case 48:                // Scrolling walls
    case 85:
    // FraggleScript types!
    case 272:   // WR
    case 273:
    case 274:   // W1
    case 275:
    case 276:   // SR
    case 277:   // S1
    case 278:   // GR
    case 279:   // G1
      return 1;   // zero tag allowed

    case 105: if( gamemode == heretic )
                  return 1;

    default:
      break;
  }
  return 0;       // zero tag not allowed
}



//SoM: 3/7/2000: Is/WasSecret.
//
// P_IsSecret()
//
// Passed a sector, returns if the sector secret type is still active, i.e.
// secret type is set and the secret has not yet been obtained.
//
boolean P_IsSecret(sector_t *sec)
{
  return (sec->special==9 || (sec->special&SECRET_MASK));
}


//
// P_WasSecret()
//
// Passed a sector, returns if the sector secret type is was active, i.e.
// secret type was set and the secret has been obtained already.
//
boolean P_WasSecret(sector_t *sec)
{
  return (sec->oldspecial==9 || (sec->oldspecial&SECRET_MASK));
}


//
// EVENTS
// Events are operations triggered by using, crossing,
// or shooting special lines, or by timed thinkers.
//

//
// P_CrossSpecialLine - TRIGGER
// Called every time a thing origin is about
//  to cross a line with a non 0 special.
//
void
P_CrossSpecialLine
( int           linenum,
  int           side,
  mobj_t*       thing )
{
    line_t*     line;
    line = &lines[linenum];

    P_ActivateCrossedLine(line, side, thing);
}


void
P_ActivateCrossedLine
( line_t*       line,
  int           side,
  mobj_t*       thing)
{
    int         ok;
    int         forceuse; //SoM: 4/26/2000: ALLTRIGGER should allow monsters to use generalized types too!

    forceuse = line->flags & ML_ALLTRIGGER && thing->type != MT_BLOOD;

    //  Triggers that other things can activate
    if (!thing->player && gamemode != heretic)
    {
        // Things that should NOT trigger specials...
        switch(thing->type)
        {
          case MT_ROCKET:
          case MT_PLASMA:
          case MT_BFG:
          case MT_TROOPSHOT:
          case MT_HEADSHOT:
          case MT_BRUISERSHOT:
            return;
            break;

          default: break;
        }
    }

    //SoM: 3/7/2000: Check for generalized line types/
    if (boomsupport)
    {
      // pointer to line function is NULL by default, set non-null if
      // line special is walkover generalized linedef type
      int (*linefunc)(line_t *line)=NULL;
  
      // check each range of generalized linedefs
      if ((unsigned)line->special >= GenFloorBase)
      {
        if (!thing->player)
          if (((line->special & FloorChange) || !(line->special & FloorModel)) && !forceuse)
            return;     // FloorModel is "Allow Monsters" if FloorChange is 0
        if (!line->tag)
          return;
        linefunc = EV_DoGenFloor;
      }
      else if ((unsigned)line->special >= GenCeilingBase)
      {
        if (!thing->player)
          if (((line->special & CeilingChange) || !(line->special & CeilingModel)) && !forceuse)
            return;     // CeilingModel is "Allow Monsters" if CeilingChange is 0
        if (!line->tag)
          return;
        linefunc = EV_DoGenCeiling;
      }
      else if ((unsigned)line->special >= GenDoorBase)
      {
        if (!thing->player)
        {
          if (!(line->special & DoorMonster) && !forceuse)
            return;                    // monsters disallowed from this door
          if (line->flags & ML_SECRET) // they can't open secret doors either
            return;
        }
        if (!line->tag)
          return;
        linefunc = EV_DoGenDoor;
      }
      else if ((unsigned)line->special >= GenLockedBase)
      {
        if (!thing->player)
          return;                     // monsters disallowed from unlocking doors
        if (((line->special&TriggerType)==WalkOnce) || ((line->special&TriggerType)==WalkMany))
        {
          if (!P_CanUnlockGenDoor(line,thing->player))
            return;
        }
        else
          return;
        linefunc = EV_DoGenLockedDoor;
      }
      else if ((unsigned)line->special >= GenLiftBase)
      {
        if (!thing->player)
          if (!(line->special & LiftMonster) && !forceuse)
            return; // monsters disallowed
        if (!line->tag)
          return;
        linefunc = EV_DoGenLift;
      }
      else if ((unsigned)line->special >= GenStairsBase)
      {
        if (!thing->player)
          if (!(line->special & StairMonster) && !forceuse)
            return; // monsters disallowed
        if (!line->tag)
          return;
        linefunc = EV_DoGenStairs;
      }
      else if ((unsigned)line->special >= GenCrusherBase)
      {
        if (!thing->player)
          if (!(line->special & StairMonster) && !forceuse)
            return; // monsters disallowed
        if (!line->tag)
          return;
        linefunc = EV_DoGenCrusher;
      }
  
      if (linefunc) // if it was a valid generalized type
        switch((line->special & TriggerType) >> TriggerTypeShift)
        {
          case WalkOnce:
            if (linefunc(line))
              line->special = 0;    // clear special if a walk once type
            return;
          case WalkMany:
            linefunc(line);
            return;
          default:                  // if not a walk type, do nothing here
            return;
        }
      }


      if(!thing->player)
      {
        ok = 0;
        if( gamemode == heretic && (line->special == 4 || line->special==39 || line->special == 97) )
            ok = 1;
        else
        switch(line->special)
        {
          case 39:      // TELEPORT TRIGGER
          case 97:      // TELEPORT RETRIGGER
          case 125:     // TELEPORT MONSTERONLY TRIGGER
          case 126:     // TELEPORT MONSTERONLY RETRIGGER
          case 4:       // RAISE DOOR
          case 10:      // PLAT DOWN-WAIT-UP-STAY TRIGGER
          case 88:      // PLAT DOWN-WAIT-UP-STAY RETRIGGER
            ok = 1;
            break; 
          // SoM: 3/4/2000: Add boom compatibility for extra monster usable
          // linedef types.
          case 208:     //SoM: Silent thing teleporters
          case 207:
          case 243:     //Silent line to line teleporter
          case 244:     //Same as above but trigger once.
          case 262:     //Same as 243 but reversed
          case 263:     //Same as 244 but reversed
          case 264:     //Monster only, silent, trigger once, reversed
          case 265:     //Same as 264 but repeatable
          case 266:     //Monster only, silent, trigger once
          case 267:     //Same as 266 bot repeatable
          case 268:     //Monster only, silent, trigger once, set pos to thing
          case 269:     //Monster only, silent, repeatable, set pos to thing
            if(boomsupport)
              ok = 1;
            break;
        }
        //SoM: Anything can trigger this line!
        if(line->flags & ML_ALLTRIGGER)
          ok = 1;

        if (!ok)
            return;
    }

    if (!P_CheckTag(line) && boomsupport)
      return;

    // Note: could use some const's here.
    switch (line->special)
    {
        // TRIGGERS.
        // All from here to RETRIGGERS.
      case 2:
        // Open Door
        if(EV_DoDoor(line,dooropen,VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 3:
        // Close Door
        if(EV_DoDoor(line,doorclose,VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 4:
        // Raise Door
        if(EV_DoDoor(line,normalDoor,VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 5:
        // Raise Floor
        if(EV_DoFloor(line,raiseFloor) || !boomsupport)
          line->special = 0;
        break;

      case 6:
        // Fast Ceiling Crush & Raise
        if(EV_DoCeiling(line,fastCrushAndRaise) || !boomsupport)
          line->special = 0;
        break;

      case 8:
        // Build Stairs
        if(EV_BuildStairs(line,gamemode == heretic ? 8*FRACUNIT : build8) || !boomsupport)
          line->special = 0;
        break;

      case 10:
        // PlatDownWaitUp
        if(EV_DoPlat(line,downWaitUpStay,0) || !boomsupport)
          line->special = 0;
        break;

      case 12:
        // Light Turn On - brightest near
        if(EV_LightTurnOn(line,0) || !boomsupport)
          line->special = 0;
        break;

      case 13:
        // Light Turn On 255
        if(EV_LightTurnOn(line,255) || !boomsupport)
          line->special = 0;
        break;

      case 16:
        // Close Door 30
        if(EV_DoDoor(line,close30ThenOpen,VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 17:
        // Start Light Strobing
        if(EV_StartLightStrobing(line) || !boomsupport)
          line->special = 0;
        break;

      case 19:
        // Lower Floor
        if(EV_DoFloor(line,lowerFloor) || !boomsupport)
          line->special = 0;
        break;

      case 22:
        // Raise floor to nearest height and change texture
        if(EV_DoPlat(line,raiseToNearestAndChange,0) || !boomsupport)
          line->special = 0;
        break;

      case 25:
        // Ceiling Crush and Raise
        if(EV_DoCeiling(line,crushAndRaise) || !boomsupport)
          line->special = 0;
        break;

      case 30:
        // Raise floor to shortest texture height
        //  on either side of lines.
        if(EV_DoFloor(line,raiseToTexture) || !boomsupport)
          line->special = 0;
        break;

      case 35:
        // Lights Very Dark
        if(EV_LightTurnOn(line,35) || !boomsupport)
          line->special = 0;
        break;

      case 36:
        // Lower Floor (TURBO)
        if(EV_DoFloor(line,turboLower) || !boomsupport)
          line->special = 0;
        break;

      case 37:
        // LowerAndChange
        if(EV_DoFloor(line,lowerAndChange) || !boomsupport)
          line->special = 0;
        break;

      case 38:
        // Lower Floor To Lowest
        if(EV_DoFloor( line, lowerFloorToLowest ) || !boomsupport)
          line->special = 0;
        break;

      case 39:
        // TELEPORT!
        if(EV_Teleport( line, side, thing ) || !boomsupport)
          line->special = 0;
        break;

      case 40:
        // RaiseCeilingLowerFloor
        if(EV_DoCeiling( line, raiseToHighest ) || EV_DoFloor( line, lowerFloorToLowest ) ||
           !boomsupport)
          line->special = 0;
        break;

      case 44:
        // Ceiling Crush
        if(EV_DoCeiling( line, lowerAndCrush ) || !boomsupport)
          line->special = 0;
        break;

      case 52:
        // EXIT!
        if( cv_allowexitlevel.value )
        {
            G_ExitLevel ();
            line->special = 0;  // heretic have right
        }
        break;

      case 53:
        // Perpetual Platform Raise
        if(EV_DoPlat(line,perpetualRaise,0) || !boomsupport)
          line->special = 0;
        break;

      case 54:
        // Platform Stop
        if(EV_StopPlat(line) || !boomsupport)
          line->special = 0;
        break;

      case 56:
        // Raise Floor Crush
        if(EV_DoFloor(line,raiseFloorCrush) || !boomsupport)
          line->special = 0;
        break;

      case 57:
        // Ceiling Crush Stop
        if(EV_CeilingCrushStop(line) || !boomsupport)
          line->special = 0;
        break;

      case 58:
        // Raise Floor 24
        if(EV_DoFloor(line,raiseFloor24) || !boomsupport)
          line->special = 0;
        break;

      case 59:
        // Raise Floor 24 And Change
        if(EV_DoFloor(line,raiseFloor24AndChange) || !boomsupport)
          line->special = 0;
        break;

      case 104:
        // Turn lights off in sector(tag)
        if(EV_TurnTagLightsOff(line) || !boomsupport)
          line->special = 0;
        break;

      case 108:
        // Blazing Door Raise (faster than TURBO!)
        if(EV_DoDoor (line,blazeRaise,4*VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 109:
        // Blazing Door Open (faster than TURBO!)
        if(EV_DoDoor (line,blazeOpen,4*VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 100:
        if( gamemode == heretic )
          EV_DoDoor (line, normalDoor, VDOORSPEED * 3);
        else
        {
          // Build Stairs Turbo 16
          if(EV_BuildStairs(line,turbo16) || !boomsupport)
            line->special = 0;
        }
        break;

      case 110:
        // Blazing Door Close (faster than TURBO!)
        if(EV_DoDoor (line,blazeClose,4*VDOORSPEED) || !boomsupport)
          line->special = 0;
        break;

      case 119:
        // Raise floor to nearest surr. floor
        if(EV_DoFloor(line,raiseFloorToNearest) || !boomsupport)
          line->special = 0;
        break;

      case 121:
        // Blazing PlatDownWaitUpStay
        if(EV_DoPlat(line,blazeDWUS,0) || !boomsupport)
          line->special = 0;
        break;

      case 124:
        // Secret EXIT
        if( cv_allowexitlevel.value )
            G_SecretExitLevel ();
        break;

      case 125:
        // TELEPORT MonsterONLY
        if (!thing->player)
        {
            if(EV_Teleport( line, side, thing ) || !boomsupport)
              line->special = 0;
        }
        break;

      case 130:
        // Raise Floor Turbo
        if(EV_DoFloor(line,raiseFloorTurbo) || !boomsupport)
          line->special = 0;
        break;

      case 141:
        // Silent Ceiling Crush & Raise
        if(EV_DoCeiling(line,silentCrushAndRaise) || !boomsupport)
          line->special = 0;
        break;

      //SoM: FraggleScript
      case 273: //(1sided)
        if(side) break;

      case 272: //(2sided)
        t_trigger = thing;
        T_RunScript(line->tag);
        break;

      // once-only triggers
      case 275: //(1sided)
        if(side) break;

      case 274: //(2sided)
        t_trigger = thing;
        T_RunScript(line->tag);
        line->special = 0;        // clear trigger
        break;


      // RETRIGGERS.  All from here till end.
      case 72:
        // Ceiling Crush
        EV_DoCeiling( line, lowerAndCrush );
        break;

      case 73:
        // Ceiling Crush and Raise
        EV_DoCeiling(line,crushAndRaise);
        break;

      case 74:
        // Ceiling Crush Stop
        EV_CeilingCrushStop(line);
        break;

      case 75:
        // Close Door
        EV_DoDoor(line,doorclose,VDOORSPEED);
        break;

      case 76:
        // Close Door 30
        EV_DoDoor(line,close30ThenOpen,VDOORSPEED);
        break;

      case 77:
        // Fast Ceiling Crush & Raise
        EV_DoCeiling(line,fastCrushAndRaise);
        break;

      case 79:
        // Lights Very Dark
        EV_LightTurnOn(line,35);
        break;

      case 80:
        // Light Turn On - brightest near
        EV_LightTurnOn(line,0);
        break;

      case 81:
        // Light Turn On 255
        EV_LightTurnOn(line,255);
        break;

      case 82:
        // Lower Floor To Lowest
        EV_DoFloor( line, lowerFloorToLowest );
        break;

      case 83:
        // Lower Floor
        EV_DoFloor(line,lowerFloor);
        break;

      case 84:
        // LowerAndChange
        EV_DoFloor(line,lowerAndChange);
        break;

      case 86:
        // Open Door
        EV_DoDoor(line,dooropen,VDOORSPEED);
        break;

      case 87:
        // Perpetual Platform Raise
        EV_DoPlat(line,perpetualRaise,0);
        break;

      case 88:
        // PlatDownWaitUp
        EV_DoPlat(line,downWaitUpStay,0);
        break;

      case 89:
        // Platform Stop
        EV_StopPlat(line);
        break;

      case 90:
        // Raise Door
        EV_DoDoor(line,normalDoor,VDOORSPEED);
        break;

      case 91:
        // Raise Floor
        EV_DoFloor(line,raiseFloor);
        break;

      case 92:
        // Raise Floor 24
        EV_DoFloor(line,raiseFloor24);
        break;

      case 93:
        // Raise Floor 24 And Change
        EV_DoFloor(line,raiseFloor24AndChange);
        break;

      case 94:
        // Raise Floor Crush
        EV_DoFloor(line,raiseFloorCrush);
        break;

      case 95:
        // Raise floor to nearest height
        // and change texture.
        EV_DoPlat(line,raiseToNearestAndChange,0);
        break;

      case 96:
        // Raise floor to shortest texture height
        // on either side of lines.
        EV_DoFloor(line,raiseToTexture);
        break;

      case 97:
        // TELEPORT!
        EV_Teleport( line, side, thing );
        break;

      case 98:
        // Lower Floor (TURBO)
        EV_DoFloor(line,turboLower);
        break;

      case 105:
        if( gamemode == heretic )
        {
            if( cv_allowexitlevel.value )
            {
                G_SecretExitLevel ();
                line->special = 0;
            }
        }
        else
            // Blazing Door Raise (faster than TURBO!)
            EV_DoDoor (line,blazeRaise,4*VDOORSPEED);
        break;

      case 106:
        if( gamemode == heretic )
        {
          if(EV_BuildStairs (line, 16 * FRACUNIT) || !boomsupport)
             line->special = 0;
        }
        else
            // Blazing Door Open (faster than TURBO!)
            EV_DoDoor (line,blazeOpen,4*VDOORSPEED);
        break;

      case 107:
        if( gamemode != heretic ) // used for a switch !
            // Blazing Door Close (faster than TURBO!)
            EV_DoDoor (line,blazeClose,4*VDOORSPEED);
        break;

      case 120:
        // Blazing PlatDownWaitUpStay.
        EV_DoPlat(line,blazeDWUS,0);
        break;

      case 126:
        // TELEPORT MonsterONLY.
        if (!thing->player)
            EV_Teleport( line, side, thing );
        break;

      case 128:
        // Raise To Nearest Floor
        EV_DoFloor(line,raiseFloorToNearest);
        break;

      case 129:
        // Raise Floor Turbo
        EV_DoFloor(line,raiseFloorTurbo);
        break;

      // SoM:3/4/2000: Extended Boom W* triggers.
      default:
        if(boomsupport) {
          switch(line->special) {
            //SoM: 3/4/2000:Boom Walk once triggers.
            //SoM: 3/4/2000:Yes this is "copied" code! I just cleaned it up. Did you think I was going to retype all this?!
            case 142:
              // Raise Floor 512
              if (EV_DoFloor(line,raiseFloor512))
                line->special = 0;
              break;
  
            case 143:
              // Raise Floor 24 and change
              if (EV_DoPlat(line,raiseAndChange,24))
                line->special = 0;
              break;

            case 144:
              // Raise Floor 32 and change
              if (EV_DoPlat(line,raiseAndChange,32))
                line->special = 0;
              break;

            case 145:
              // Lower Ceiling to Floor
              if (EV_DoCeiling( line, lowerToFloor ))
                line->special = 0;
              break;

            case 146:
              // Lower Pillar, Raise Donut
              if (EV_DoDonut(line))
                line->special = 0;
              break;

            case 199:
              // Lower ceiling to lowest surrounding ceiling
              if (EV_DoCeiling(line,lowerToLowest))
                line->special = 0;
              break;

            case 200:
              // Lower ceiling to highest surrounding floor
              if (EV_DoCeiling(line,lowerToMaxFloor))
                line->special = 0;
              break;

            case 207:
              // W1 silent teleporter (normal kind)
              if (EV_SilentTeleport(line, side, thing))
                line->special = 0;
              break;

            case 153: 
              // Texture/Type Change Only (Trig)
              if (EV_DoChange(line,trigChangeOnly))
                line->special = 0;
              break;
  
            case 239: 
              // Texture/Type Change Only (Numeric)
              if (EV_DoChange(line,numChangeOnly))
                line->special = 0;
              break;
 
            case 219:
              // Lower floor to next lower neighbor
              if (EV_DoFloor(line,lowerFloorToNearest))
                line->special = 0;
              break;

            case 227:
              // Raise elevator next floor
              if (EV_DoElevator(line,elevateUp))
                line->special = 0;
              break;

            case 231:
              // Lower elevator next floor
              if (EV_DoElevator(line,elevateDown))
                line->special = 0;
              break;

            case 235:
              // Elevator to current floor
              if (EV_DoElevator(line,elevateCurrent))
                line->special = 0;
              break;

            case 243: 
              // W1 silent teleporter (linedef-linedef kind)
              if (EV_SilentLineTeleport(line, side, thing, false))
                line->special = 0;
              break;

            case 262: 
              if (EV_SilentLineTeleport(line, side, thing, true))
                line->special = 0;
              break;
 
            case 264: 
              if (!thing->player &&
              EV_SilentLineTeleport(line, side, thing, true))
                line->special = 0;
              break;

            case 266: 
              if (!thing->player &&
                  EV_SilentLineTeleport(line, side, thing, false))
                line->special = 0;
              break;

            case 268: 
              if (!thing->player && EV_SilentTeleport(line, side, thing))
                line->special = 0;
              break;

            // Extended walk many retriggerable
 
            //Boom added lots of linedefs to fill in the gaps in trigger types

            case 147:
              // Raise Floor 512
              EV_DoFloor(line,raiseFloor512);
              break;

            case 148:
              // Raise Floor 24 and Change
              EV_DoPlat(line,raiseAndChange,24);
              break;

            case 149:
              // Raise Floor 32 and Change
              EV_DoPlat(line,raiseAndChange,32);
              break;

            case 150:
              // Start slow silent crusher
              EV_DoCeiling(line,silentCrushAndRaise);
              break;

            case 151:
              // RaiseCeilingLowerFloor
              EV_DoCeiling( line, raiseToHighest );
              EV_DoFloor( line, lowerFloorToLowest );
              break;

            case 152:
              // Lower Ceiling to Floor
              EV_DoCeiling( line, lowerToFloor );
              break;

            case 256:
              // Build stairs, step 8
              EV_BuildStairs(line,build8);
              break;

            case 257:
              // Build stairs, step 16
              EV_BuildStairs(line,turbo16);
              break;

            case 155:
              // Lower Pillar, Raise Donut
              EV_DoDonut(line);
              break;

            case 156:
              // Start lights strobing
              EV_StartLightStrobing(line);
              break;

            case 157:
              // Lights to dimmest near
              EV_TurnTagLightsOff(line);
              break;

            case 201:
              // Lower ceiling to lowest surrounding ceiling
              EV_DoCeiling(line,lowerToLowest);
              break;

            case 202:
              // Lower ceiling to highest surrounding floor
              EV_DoCeiling(line,lowerToMaxFloor);
              break;

            case 208:
              // WR silent teleporter (normal kind)
              EV_SilentTeleport(line, side, thing);
              break;

            case 212:
              // Toggle floor between C and F instantly
              EV_DoPlat(line,toggleUpDn,0);
              break;

            case 154:
              // Texture/Type Change Only (Trigger)
              EV_DoChange(line,trigChangeOnly);
              break;

            case 240: 
              // Texture/Type Change Only (Numeric)
              EV_DoChange(line,numChangeOnly);
              break;

            case 220:
              // Lower floor to next lower neighbor
              EV_DoFloor(line,lowerFloorToNearest);
              break;

            case 228:
              // Raise elevator next floor
              EV_DoElevator(line,elevateUp);
              break;

            case 232:
              // Lower elevator next floor
              EV_DoElevator(line,elevateDown);
              break;

            case 236:
              // Elevator to current floor
              EV_DoElevator(line,elevateCurrent);
              break;

            case 244: 
              // WR silent teleporter (linedef-linedef kind)
              EV_SilentLineTeleport(line, side, thing, false);
              break;

            case 263: 
              //Silent line-line reversed
              EV_SilentLineTeleport(line, side, thing, true);
              break;

            case 265: 
              //Monster-only silent line-line reversed
              if (!thing->player)
                EV_SilentLineTeleport(line, side, thing, true);
              break;

            case 267: 
              //Monster-only silent line-line
              if (!thing->player)
                EV_SilentLineTeleport(line, side, thing, false);
              break;

            case 269: 
              //Monster-only silent
              if (!thing->player)
                EV_SilentTeleport(line, side, thing);
              break;
              }
            }
    }
}



//
// P_ShootSpecialLine - IMPACT SPECIALS
// Called when a thing shoots a special line.
//
void P_ShootSpecialLine ( mobj_t*       thing,
                          line_t*       line )
{
    int         ok;


    //SoM: 3/7/2000: Another General type check
    if (boomsupport)
    {
      // pointer to line function is NULL by default, set non-null if
      // line special is gun triggered generalized linedef type
      int (*linefunc)(line_t *line)=NULL;

      // check each range of generalized linedefs
      if ((unsigned)line->special >= GenFloorBase)
      {
        if (!thing->player)
          if ((line->special & FloorChange) || !(line->special & FloorModel))
            return;   // FloorModel is "Allow Monsters" if FloorChange is 0
        if (!line->tag) //jff 2/27/98 all gun generalized types require tag
          return;

        linefunc = EV_DoGenFloor;
      }
      else if ((unsigned)line->special >= GenCeilingBase)
      {
        if (!thing->player)
          if ((line->special & CeilingChange) || !(line->special & CeilingModel))
            return;   // CeilingModel is "Allow Monsters" if CeilingChange is 0
        if (!line->tag) //jff 2/27/98 all gun generalized types require tag
          return;
        linefunc = EV_DoGenCeiling;
      }
      else if ((unsigned)line->special >= GenDoorBase)
      {
        if (!thing->player)
        {
          if (!(line->special & DoorMonster))
            return;   // monsters disallowed from this door
          if (line->flags & ML_SECRET) // they can't open secret doors either
            return;
        }
        if (!line->tag) //jff 3/2/98 all gun generalized types require tag
          return;
        linefunc = EV_DoGenDoor;
      }
      else if ((unsigned)line->special >= GenLockedBase)
      {
        if (!thing->player)
          return;   // monsters disallowed from unlocking doors
        if (((line->special&TriggerType)==GunOnce) || ((line->special&TriggerType)==GunMany))
        { //jff 4/1/98 check for being a gun type before reporting door type
          if (!P_CanUnlockGenDoor(line,thing->player))
            return;
        }
        else
          return;
        if (!line->tag) //jff 2/27/98 all gun generalized types require tag
          return;

        linefunc = EV_DoGenLockedDoor;
      }
      else if ((unsigned)line->special >= GenLiftBase)
      {
        if (!thing->player)
          if (!(line->special & LiftMonster))
            return; // monsters disallowed
        linefunc = EV_DoGenLift;
      }
      else if ((unsigned)line->special >= GenStairsBase)
      {
        if (!thing->player)
          if (!(line->special & StairMonster))
            return; // monsters disallowed
        if (!line->tag) //jff 2/27/98 all gun generalized types require tag
          return;
        linefunc = EV_DoGenStairs;
      }
      else if ((unsigned)line->special >= GenCrusherBase)
      {
        if (!thing->player)
          if (!(line->special & StairMonster))
            return; // monsters disallowed
        if (!line->tag) //jff 2/27/98 all gun generalized types require tag
          return;
        linefunc = EV_DoGenCrusher;
      }

      if (linefunc)
        switch((line->special & TriggerType) >> TriggerTypeShift)
        {
          case GunOnce:
            if (linefunc(line))
              P_ChangeSwitchTexture(line,0);
            return;
          case GunMany:
            if (linefunc(line))
              P_ChangeSwitchTexture(line,1);
            return;
          default:  // if not a gun type, do nothing here
            return;
        }
    }


    //  Impacts that other things can activate.
    if (!thing->player)
    {
        ok = 0;
        switch(line->special)
        {
          case 46:
            // OPEN DOOR IMPACT
            ok = 1;
            break;
        }
        if (!ok)
            return;
    }

    if(!P_CheckTag(line))
      return;

    switch(line->special)
    {
      case 24:
        // RAISE FLOOR
        if(EV_DoFloor(line,raiseFloor) || !boomsupport)
          P_ChangeSwitchTexture(line,0);
        break;

      case 46:
        // OPEN DOOR
        if(EV_DoDoor(line,dooropen,VDOORSPEED) || !boomsupport)
          P_ChangeSwitchTexture(line,1);
        break;

      case 47:
        // RAISE FLOOR NEAR AND CHANGE
        if(EV_DoPlat(line,raiseToNearestAndChange,0) || !boomsupport)
          P_ChangeSwitchTexture(line,0);
        break;

      //SoM: FraggleScript
      case 278:
      case 279:
        t_trigger = thing;
        T_RunScript(line->tag);
        if(line->special == 279) line->special = 0;       // clear if G1
        break;

      default:
        if (boomsupport)
          switch (line->special)
          {
            case 197:
              // Exit to next level
              if( cv_allowexitlevel.value )
              {
                  P_ChangeSwitchTexture(line,0);
                  G_ExitLevel();
              }
              break;

            case 198:
              // Exit to secret level
              if( cv_allowexitlevel.value )
              {
                  P_ChangeSwitchTexture(line,0);
                  G_SecretExitLevel();
              }
              break;
              //jff end addition of new gun linedefs
          }
        break;
    }
}


//
// P_ProcessSpecialSector
// Function that actually applies the sector special to the player.
void P_ProcessSpecialSector(player_t* player, sector_t* sector, boolean instantdamage)
{
    if(sector->special < 32)
    {
      // Has hitten ground.
      switch (sector->special)
      {
        case 5:
          // HELLSLIME DAMAGE
          if (!player->powers[pw_ironfeet])
              if (instantdamage)
              {
                  P_DamageMobj (player->mo, NULL, NULL, 10);

                  // spawn a puff of smoke
                  //CONS_Printf ("damage!\n"); //debug
                  if( demoversion>=125 )
                      P_SpawnSmoke (player->mo->x, player->mo->y, player->mo->z);
              }
          break;

        case 7:
          // NUKAGE DAMAGE
          if (!player->powers[pw_ironfeet])
              if (instantdamage)
                  P_DamageMobj (player->mo, NULL, NULL, 5);
          break;

        case 16:
          // SUPER HELLSLIME DAMAGE
        case 4:
          // STROBE HURT
          if (!player->powers[pw_ironfeet]
              || (P_Random()<5) )
          {
              if (instantdamage)
                  P_DamageMobj (player->mo, NULL, NULL, 20);
          }
          break;

        case 9:
          // SECRET SECTOR
          player->secretcount++;
          sector->special = 0;

          //faB: useful only in single & coop.
          if (!cv_deathmatch.value && players-player == displayplayer)
              CONS_Printf ("\2You found a secret area!\n");

          break;

        case 11:
          // EXIT SUPER DAMAGE! (for E1M8 finale)
          player->cheats &= ~CF_GODMODE;

          if (instantdamage)
              P_DamageMobj (player->mo, NULL, NULL, 20);

          if ((player->health <= 10) && cv_allowexitlevel.value)
              G_ExitLevel();
          break;

        default:
          //SoM: 3/8/2000: Just ignore.
          //CONS_Printf ("P_PlayerInSpecialSector: unknown special %i",
          //             sector->special);
          break;
      };
   }
   else //SoM: Extended sector types for secrets and damage
   {
     switch ((sector->special&DAMAGE_MASK)>>DAMAGE_SHIFT)
     {
       case 0: // no damage
         break;
       case 1: // 2/5 damage per 31 ticks
         if (!player->powers[pw_ironfeet] && instantdamage)
             P_DamageMobj (player->mo, NULL, NULL, 5);
         break;
       case 2: // 5/10 damage per 31 ticks
         if (!player->powers[pw_ironfeet] && instantdamage)
             P_DamageMobj (player->mo, NULL, NULL, 10);
         break;
       case 3: // 10/20 damage per 31 ticks
         if ((!player->powers[pw_ironfeet]
              || P_Random()<5) && instantdamage)  // take damage even with suit
         {
             P_DamageMobj (player->mo, NULL, NULL, 20);
         }
         break;
     }
     if (sector->special&SECRET_MASK)
     {
       player->secretcount++;
       sector->special &= ~SECRET_MASK;

       if (!cv_deathmatch.value && players-player == displayplayer)
          CONS_Printf ("\2You found a secret area!\n");

       if (sector->special<32)
         sector->special=0;
     }
   }
}



//
// P_PlayerOnSpecial3DFloor
// Checks to see if a player is standing on or is inside a 3D floor (water)
// and applies any speicials..
void P_PlayerOnSpecial3DFloor(player_t* player)
{
  sector_t* sector;
  boolean   instantdamage = false;
  ffloor_t* rover;

  sector = player->mo->subsector->sector;
  if(!sector->ffloors)
    return;

  for(rover = sector->ffloors; rover; rover = rover->next)
  {
    if(!rover->master->frontsector->special)
      continue;

    // Check the 3D floor's type...
    if(rover->flags & FF_SOLID)
    {
      // Player must be on top of the floor to be affected...
      if(player->mo->z != *rover->topheight)
        continue;

      if (demoversion >= 125 &&
        (player->mo->eflags & MF_JUSTHITFLOOR) &&
        sector->heightsec == -1 && (leveltime % (2*NEWTICRATERATIO))) //SoM: penalize jumping less.
        instantdamage = true;
      else
        instantdamage = !(leveltime % (32*NEWTICRATERATIO));
    }
    else
    {
      //Water and DEATH FOG!!! heh
      if(player->mo->z > *rover->topheight || (player->mo->z + player->mo->height) < *rover->bottomheight)
        continue;
      instantdamage = !(leveltime % (32*NEWTICRATERATIO));
    }

    P_ProcessSpecialSector(player, rover->master->frontsector, instantdamage);
  }
}


//
// P_PlayerInSpecialSector
// Called every tic frame
//  that the player origin is in a special sector
//
void P_PlayerInSpecialSector (player_t* player)
{
    sector_t*   sector;
    boolean     instantdamage=false;

    // SoM: Check 3D floors...
    P_PlayerOnSpecial3DFloor(player);

    sector = player->mo->subsector->sector;

    //Fab: keep track of what sector type the player's currently in
    player->specialsector = sector->special;

#ifdef OLDWATER
    //Fab: VERY NASTY hack for water QUICK TEST !!!!!!!!!!!!!!!!!!!!!!!
    if (sector->tag < 0)
    {
        player->specialsector = 888;    // no particular value
        return;
    }
    else
    // old water (flat texture)
    if (levelflats[sector->floorpic].iswater)
    {
        player->specialsector = 887;
        return;
    }
#endif

    if (!player->specialsector)     // nothing special, exit
        return;

    if( gamemode == heretic )
    {
        P_HerePlayerInSpecialSector(player);
        return;
    }

    // Falling, not all the way down yet?
    //SoM: 3/17/2000: Damage if in slimey water!
    if (sector->heightsec != -1)
    {
        if(player->mo->z > sectors[sector->heightsec].floorheight)
            return;
    }
    else if (player->mo->z != sector->floorheight)
            return;

    //Fab: jumping in lava/slime does instant damage (no jump cheat)
    if (demoversion >= 125 &&
        (player->mo->eflags & MF_JUSTHITFLOOR) &&
        sector->heightsec == -1 && (leveltime % (2*NEWTICRATERATIO))) //SoM: penalize jumping less.
        instantdamage = true;
    else
        instantdamage = !(leveltime % (32*NEWTICRATERATIO));

    P_ProcessSpecialSector(player, sector, instantdamage);
}




//
// P_UpdateSpecials
// Animate planes, scroll walls, etc.
//

void P_UpdateSpecials (void)
{
    anim_t*     anim;
    int         i;
    int         pic; //SoM: 3/8/2000

    levelflat_t*     foundflats;        // for flat animation

    //  LEVEL TIMER
    if (cv_timelimit.value && (ULONG)cv_timelimit.value < leveltime)
        G_ExitLevel();

    //  ANIMATE TEXTURES
    for (anim = anims ; anim < lastanim ; anim++)
    {
      for (i=anim->basepic ; i<anim->basepic+anim->numpics ; i++)
      {
        pic = anim->basepic + ( (leveltime/anim->speed + i)%anim->numpics );
        if (anim->istexture)
          texturetranslation[i] = pic;
      }
    }


    //  ANIMATE FLATS
    //Fab:FIXME: do not check the non-animate flat.. link the animated ones?
    // note: its faster than the original anywaysince it animates only
    //    flats used in the level, and there's usually very few of them
    foundflats = levelflats;
    for (i = 0; i<numlevelflats; i++,foundflats++)
    {
         if (foundflats->speed) // it is an animated flat
         {
             // update the levelflat lump number
             foundflats->lumpnum = foundflats->baselumpnum +
                                   ( (leveltime/foundflats->speed + foundflats->animseq) % foundflats->numpics);
         }
    }

    //  DO BUTTONS
    for (i = 0; i < MAXBUTTONS; i++)
        if (buttonlist[i].btimer)
        {
            buttonlist[i].btimer--;
            if (!buttonlist[i].btimer)
            {
                switch(buttonlist[i].where)
                {
                  case top:
                    sides[buttonlist[i].line->sidenum[0]].toptexture =
                        buttonlist[i].btexture;
                    break;

                  case middle:
                    sides[buttonlist[i].line->sidenum[0]].midtexture =
                        buttonlist[i].btexture;
                    break;

                  case bottom:
                    sides[buttonlist[i].line->sidenum[0]].bottomtexture =
                        buttonlist[i].btexture;
                    break;
                }
                S_StartSound((mobj_t *)&buttonlist[i].soundorg,sfx_swtchn);
                memset(&buttonlist[i],0,sizeof(button_t));
            }
        }

}


//SoM: 3/8/2000: EV_DoDonut moved to p_floor.c

//SoM: 3/23/2000: Adds a sectors floor and ceiling to a sector's ffloor list
void P_AddFakeFloor(sector_t* sec, sector_t* sec2, line_t* master, int flags);
void P_AddFFloor(sector_t* sec, ffloor_t* ffloor);


void P_AddFakeFloor(sector_t* sec, sector_t* sec2, line_t* master, int flags)
{
  ffloor_t*      ffloor;

  if(sec2->numattached == 0)
  {
    sec2->attached = malloc(sizeof(int));
    sec2->attached[0] = sec - sectors;
    sec2->numattached = 1;
  }
  else
  {
    int  i;

    for(i = 0; i < sec2->numattached; i++)
      if(sec2->attached[i] == sec - sectors)
        return;

    sec2->attached = realloc(sec2->attached, sizeof(int) * (sec2->numattached + 1));
    sec2->attached[sec2->numattached] = sec - sectors;
    sec2->numattached ++;
  }

  //Add the floor
  ffloor = Z_Malloc(sizeof(ffloor_t), PU_LEVEL, NULL);
  ffloor->secnum = sec2 - sectors;
  ffloor->target = sec;
  ffloor->bottomheight     = &sec2->floorheight;
  ffloor->bottompic        = &sec2->floorpic;
  //ffloor->bottomlightlevel = &sec2->lightlevel;
  ffloor->bottomxoffs      = &sec2->floor_xoffs;
  ffloor->bottomyoffs      = &sec2->floor_yoffs;

  //Add the ceiling
  ffloor->topheight     = &sec2->ceilingheight;
  ffloor->toppic        = &sec2->ceilingpic;
  ffloor->toplightlevel = &sec2->lightlevel;
  ffloor->topxoffs      = &sec2->ceiling_xoffs;
  ffloor->topyoffs      = &sec2->ceiling_yoffs;

  ffloor->flags = flags;
  ffloor->master = master;

  if(flags & FF_TRANSLUCENT)
  {
    if(sides[master->sidenum[0]].toptexture > 0)
      ffloor->alpha = sides[master->sidenum[0]].toptexture;
    else
      ffloor->alpha = 0x80; // 127
  }

  P_AddFFloor(sec, ffloor);
}



void P_AddFFloor(sector_t* sec, ffloor_t* ffloor)
{
  ffloor_t* rover;

  if(!sec->ffloors)
  {
    sec->ffloors = ffloor;
    ffloor->next = 0;
    ffloor->prev = 0;
    return;
  }

  for(rover = sec->ffloors; rover->next; rover = rover->next);

  rover->next = ffloor;
  ffloor->prev = rover;
  ffloor->next = 0;
}

//
// SPECIAL SPAWNING
//

//
// P_SpawnSpecials
// After the map has been loaded, scan for specials
//  that spawn thinkers
//

// Parses command line parameters.
void P_SpawnSpecials (void)
{
    sector_t*   sector;
    int         i;
    int         episode;

    episode = 1;
    if (W_CheckNumForName("texture2") >= 0)
        episode = 2;

    //  Init special SECTORs.
    sector = sectors;
    for (i=0 ; i<numsectors ; i++, sector++)
    {
        if (!sector->special)
            continue;

        if (sector->special&SECRET_MASK) //SoM: 3/8/2000: count secret flags
          totalsecret++;

        switch (raven ? sector->special : sector->special&31)
        {
          case 1:
            // FLICKERING LIGHTS
            P_SpawnLightFlash (sector);
            break;

          case 2:
            // STROBE FAST
            P_SpawnStrobeFlash(sector,FASTDARK,0);
            break;

          case 3:
            // STROBE SLOW
            P_SpawnStrobeFlash(sector,SLOWDARK,0);
            break;

          case 4:
            if( raven )
                break;
            // STROBE FAST/DEATH SLIME
            P_SpawnStrobeFlash(sector,FASTDARK,0);
            sector->special |= 3<<DAMAGE_SHIFT; //SoM: 3/8/2000: put damage bits in
            break;

          case 8:
            // GLOWING LIGHT
            P_SpawnGlowingLight(sector);
            break;

          case 9:
            // SECRET SECTOR
            if(sector->special<32)
              totalsecret++;
            break;

          case 10:
            // DOOR CLOSE IN 30 SECONDS
            P_SpawnDoorCloseIn30 (sector);
            break;

          case 12:
            // SYNC STROBE SLOW
            P_SpawnStrobeFlash (sector, SLOWDARK, 1);
            break;

          case 13:
            // SYNC STROBE FAST
            P_SpawnStrobeFlash (sector, FASTDARK, 1);
            break;

          case 14:
            // DOOR RAISE IN 5 MINUTES
            P_SpawnDoorRaiseIn5Mins (sector, i);
            break;

          case 17:
            P_SpawnFireFlicker(sector);
            break;
        }
    }

    //SoM: 3/8/2000: Boom level init functions
    P_RemoveAllActiveCeilings();
    P_RemoveAllActivePlats();
    for (i = 0;i < MAXBUTTONS;i++)
      memset(&buttonlist[i],0,sizeof(button_t));

    P_InitTagLists();   //Create xref tables for tags
    P_SpawnScrollers(); //Add generalized scrollers
    P_SpawnFriction();  //New friction model using linedefs
    P_SpawnPushers();   //New pusher model using linedefs

    //  Init line EFFECTs
    for (i = 0;i < numlines; i++)
    {
        switch(lines[i].special)
        {
          int s, sec;

          // support for drawn heights coming from different sector
          case 242:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              sectors[s].heightsec = sec;
            break;

          //SoM: 3/20/2000: support for drawn heights coming from different sector
          case 280:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
            {
              sectors[s].heightsec = sec;
              sectors[s].altheightsec = 1;
            }
            break;

          //SoM: 4/4/2000: HACK! Copy colormaps. Just plain colormaps.
          case 282:
            for(s = -1; (s = P_FindSectorFromLineTag(lines + i, s)) >= 0;)
            {
              sectors[s].midmap = lines[i].frontsector->midmap;
              sectors[s].altheightsec = 2;
            }
            break;

          case 281:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_SOLID|FF_RENDERALL|FF_CUTLEVEL);
            break;

          case 289:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_SOLID|FF_RENDERALL|FF_NOSHADE|FF_CUTLEVEL);
            break;

          // TL block
          case 300:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_SOLID|FF_RENDERALL|FF_NOSHADE|FF_TRANSLUCENT|FF_EXTRA|FF_CUTEXTRA);
            break;

          // TL water
          case 301:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_RENDERALL|FF_TRANSLUCENT|FF_SWIMMABLE|FF_BOTHPLANES|FF_ALLSIDES|FF_CUTEXTRA|FF_EXTRA|FF_DOUBLESHADOW|FF_CUTSPRITES);
            break;

          // Fog
          case 302:
            sec = sides[*lines[i].sidenum].sector-sectors;
            // SoM: Because it's fog, check for an extra colormap and set
            // the fog flag...
            if(sectors[sec].extra_colormap)
              sectors[sec].extra_colormap->fog = 1;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_RENDERALL|FF_FOG|FF_BOTHPLANES|FF_INVERTPLANES|FF_ALLSIDES|FF_INVERTSIDES|FF_CUTEXTRA|FF_EXTRA|FF_DOUBLESHADOW|FF_CUTSPRITES);
            break;

          // Light effect
          case 303:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_CUTSPRITES);
            break;

          // Opaque water
          case 304:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_RENDERALL|FF_SWIMMABLE|FF_BOTHPLANES|FF_ALLSIDES|FF_CUTEXTRA|FF_EXTRA|FF_DOUBLESHADOW|FF_CUTSPRITES);
            break;

          // Double light effect
          case 305:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_CUTSPRITES|FF_DOUBLESHADOW);
            break;

          // Invisible barrior
          case 306:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              P_AddFakeFloor(&sectors[s], &sectors[sec], lines+i, FF_EXISTS|FF_SOLID);
            break;

          // floor lighting independently (e.g. lava)
          case 213:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              sectors[s].floorlightsec = sec;
            break;

          // ceiling lighting independently
          case 261:
            sec = sides[*lines[i].sidenum].sector-sectors;
            for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
              sectors[s].ceilinglightsec = sec;
            break;
		  
          // Instant lower for floor SSNTails 06-13-2002
          case 290:
            EV_DoFloor(&lines[i], instantLower);
            break;

          // Instant raise for ceilings SSNTails 06-13-2002
          case 291:
            EV_DoCeiling(&lines[i], instantRaise);
            break;

          default:
            if(lines[i].special>=1000 && lines[i].special<1032)
            {
                for (s = -1; (s = P_FindSectorFromLineTag(lines+i,s)) >= 0;)
                {
                  sectors[s].teamstartsec = lines[i].special-999; // only 999 so we know when it is set (it's != 0)
                }
                break;
            }
        }
    }
}





/*
  SoM: 3/8/2000: General scrolling functions.
  T_Scroll,
  Add_Scroller,
  Add_WallScroller,
  P_SpawnScrollers
*/
//
// This function, with the help of r_plane.c and r_bsp.c, supports generalized
// scrolling floors and walls, with optional mobj-carrying properties, e.g.
// conveyor belts, rivers, etc. A linedef with a special type affects all
// tagged sectors the same way, by creating scrolling and/or object-carrying
// properties. Multiple linedefs may be used on the same sector and are
// cumulative, although the special case of scrolling a floor and carrying
// things on it, requires only one linedef. The linedef's direction determines
// the scrolling direction, and the linedef's length determines the scrolling
// speed. This was designed so that an edge around the sector could be used to
// control the direction of the sector's scrolling, which is usually what is
// desired.
//
// Process the active scrollers.

void T_Scroll(scroll_t *s)
{
  fixed_t  dx = s->dx, dy = s->dy;
  int	   affectee = s->affectee;

  if (s->control != -1)
    {   // compute scroll amounts based on a sector's height changes
      fixed_t height = sectors[s->control].floorheight +
        sectors[s->control].ceilingheight;
      fixed_t delta = height - s->last_height;
      s->last_height = height;
      dx = FixedMul(dx, delta);
      dy = FixedMul(dy, delta);
    }

  if (s->accel)
    {
      s->vdx = dx += s->vdx;
      s->vdy = dy += s->vdy;
    }

  if (!(dx | dy))                   // no-op if both (x,y) offsets 0
    return;

  switch (s->type)
    {
      side_t *side;
      sector_t *sec;
      fixed_t height, waterheight;
      msecnode_t *node;
      mobj_t *thing;

    case sc_side:                   //Scroll wall texture
	side = sides + affectee;
        side->textureoffset += dx;
        side->rowoffset += dy;
        break;

    case sc_floor:                  //Scroll floor texture
        sec = sectors + affectee;
        sec->floor_xoffs += dx;
        sec->floor_yoffs += dy;
        break;

    case sc_ceiling:               //Scroll ceiling texture
        sec = sectors + affectee;
        sec->ceiling_xoffs += dx;
        sec->ceiling_yoffs += dy;
        break;

    case sc_carry:

      sec = sectors + affectee;
      height = sec->floorheight;
      waterheight = sec->heightsec != -1 &&
        sectors[sec->heightsec].floorheight > height ?
        sectors[sec->heightsec].floorheight : MININT;

      for (node = sec->touching_thinglist; node; node = node->m_snext)
        if (!((thing = node->m_thing)->flags & MF_NOCLIP) &&
            (!(thing->flags & MF_NOGRAVITY || thing->z > height) ||
             thing->z < waterheight))
          {
            // Move objects only if on floor or underwater,
            // non-floating, and clipped.
            thing->momx += dx;
            thing->momy += dy;
          }
      break;

    case sc_carry_ceiling:       // to be added later
      break;
    }
}

//
// Add_Scroller()
//
// Add a generalized scroller to the thinker list.
//
// type: the enumerated type of scrolling: floor, ceiling, floor carrier,
//   wall, floor carrier & scroller
//
// (dx,dy): the direction and speed of the scrolling or its acceleration
//
// control: the sector whose heights control this scroller's effect
//   remotely, or -1 if no control sector
//
// affectee: the index of the affected object (sector or sidedef)
//
// accel: non-zero if this is an accelerative effect
//

static void Add_Scroller(int type, fixed_t dx, fixed_t dy,
                         int control, int affectee, int accel)
{
  scroll_t *s = Z_Malloc(sizeof *s, PU_LEVSPEC, 0);
  s->thinker.function.acp1 = (actionf_p1) T_Scroll;
  s->type = type;
  s->dx = dx;
  s->dy = dy;
  s->accel = accel;
  s->vdx = s->vdy = 0;
  if ((s->control = control) != -1)
    s->last_height =
      sectors[control].floorheight + sectors[control].ceilingheight;
  s->affectee = affectee;
  P_AddThinker(&s->thinker);
}

// Adds wall scroller. Scroll amount is rotated with respect to wall's
// linedef first, so that scrolling towards the wall in a perpendicular
// direction is translated into vertical motion, while scrolling along
// the wall in a parallel direction is translated into horizontal motion.

static void Add_WallScroller(fixed_t dx, fixed_t dy, const line_t *l,
                             int control, int accel)
{
  fixed_t x = abs(l->dx), y = abs(l->dy), d;
  if (y > x)
    d = x, x = y, y = d;
  d = FixedDiv(x, finesine[(tantoangle[FixedDiv(y,x) >> DBITS] + ANG90)
                          >> ANGLETOFINESHIFT]);
  x = -FixedDiv(FixedMul(dy, l->dy) + FixedMul(dx, l->dx), d);
  y = -FixedDiv(FixedMul(dx, l->dy) - FixedMul(dy, l->dx), d);
  Add_Scroller(sc_side, x, y, control, *l->sidenum, accel);
}

// Amount (dx,dy) vector linedef is shifted right to get scroll amount
#define SCROLL_SHIFT 5

// Factor to scale scrolling effect into mobj-carrying properties = 3/32.
// (This is so scrolling floors and objects on them can move at same speed.)
#define CARRYFACTOR ((fixed_t)(FRACUNIT*.09375))

// Initialize the scrollers
static void P_SpawnScrollers(void)
{
  int i;
  line_t *l = lines;

  for (i=0;i<numlines;i++,l++)
    {
      fixed_t dx = l->dx >> SCROLL_SHIFT;  // direction and speed of scrolling
      fixed_t dy = l->dy >> SCROLL_SHIFT;
      int control = -1, accel = 0;         // no control sector or acceleration
      int special = l->special;

      // Types 245-249 are same as 250-254 except that the
      // first side's sector's heights cause scrolling when they change, and
      // this linedef controls the direction and speed of the scrolling. The
      // most complicated linedef since donuts, but powerful :)

      if (special >= 245 && special <= 249)         // displacement scrollers
        {
          special += 250-245;
          control = sides[*l->sidenum].sector - sectors;
        }
      else
        if (special >= 214 && special <= 218)       // accelerative scrollers
          {
            accel = 1;
            special += 250-214;
            control = sides[*l->sidenum].sector - sectors;
          }

      switch (special)
        {
          register int s;

        case 250:   // scroll effect ceiling
          for (s=-1; (s = P_FindSectorFromLineTag(l,s)) >= 0;)
            Add_Scroller(sc_ceiling, -dx, dy, control, s, accel);
          break;

        case 251:   // scroll effect floor
        case 253:   // scroll and carry objects on floor
          for (s=-1; (s = P_FindSectorFromLineTag(l,s)) >= 0;)
            Add_Scroller(sc_floor, -dx, dy, control, s, accel);
          if (special != 253)
            break;

        case 252: // carry objects on floor
          dx = FixedMul(dx,CARRYFACTOR);
          dy = FixedMul(dy,CARRYFACTOR);
          for (s=-1; (s = P_FindSectorFromLineTag(l,s)) >= 0;)
            Add_Scroller(sc_carry, dx, dy, control, s, accel);
          break;

          // scroll wall according to linedef
          // (same direction and speed as scrolling floors)
        case 254:
          for (s=-1; (s = P_FindLineFromLineTag(l,s)) >= 0;)
            if (s != i)
              Add_WallScroller(dx, dy, lines+s, control, accel);
          break;

        case 255:
          s = lines[i].sidenum[0];
          Add_Scroller(sc_side, -sides[s].textureoffset,
                       sides[s].rowoffset, -1, s, accel);
          break;

        case 48:                  // scroll first side
          Add_Scroller(sc_side,  FRACUNIT, 0, -1, lines[i].sidenum[0], accel);
          break;

        case 99: // heretic right scrolling
          if(gamemode != heretic)
              break; // doom use it as bluekeydoor
        case 85:                  // jff 1/30/98 2-way scroll
          Add_Scroller(sc_side, -FRACUNIT, 0, -1, lines[i].sidenum[0], accel);
          break;
        }
    }
}



/*
  SoM: 3/8/2000: Friction functions start.
  Add_Friction,
  T_Friction,
  P_SpawnFriction
*/

// Adds friction thinker.
static void Add_Friction(int friction, int movefactor, int affectee)
{
    friction_t *f = Z_Malloc(sizeof *f, PU_LEVSPEC, 0);

    f->thinker.function.acp1 = (actionf_p1) T_Friction;
    f->friction = friction;
    f->movefactor = movefactor;
    f->affectee = affectee;
    P_AddThinker(&f->thinker);
}



//Function to apply friction to all the things in a sector.
void T_Friction(friction_t *f)
    {
    sector_t *sec;
    mobj_t   *thing;
    msecnode_t* node;
	boolean foundfloor = false;

    if (!boomsupport || !variable_friction)
        return;

    sec = sectors + f->affectee;

    // Be sure the special sector type is still turned on. If so, proceed.
    // Else, bail out; the sector type has been changed on us.

    if (!(sec->special & FRICTION_MASK))
	{
		if(sec->ffloors)
		{
		  ffloor_t*  rover;

		  for(rover = sec->ffloors; rover; rover = rover->next)
		  {
			// Do some small extra checks here to possibly save unneeded work.
			if(!(rover->master->frontsector->special & FRICTION_MASK))
			  continue;
			foundfloor = true;
		  }
		}

		if(foundfloor == false) // Not even a 3d floor has the FRICTION_MASK.
			return;
	}

    // Assign the friction value to players on the floor, non-floating,
    // and clipped. Normally the object's friction value is kept at
    // ORIG_FRICTION and this thinker changes it for icy or muddy floors.

    // In Phase II, you can apply friction to Things other than players.

    // When the object is straddling sectors with the same
    // floorheight that have different frictions, use the lowest
    // friction value (muddy has precedence over icy).

    node = sec->touching_thinglist; // things touching this sector
    while (node)
        {
        thing = node->m_thing;
        if (thing->player &&
            !(thing->flags & (MF_NOGRAVITY | MF_NOCLIP)) &&
            thing->z == thing->floorz)
            {
			if(foundfloor && thing->z == sec->floorheight); // Skip

            else if ((thing->friction == ORIG_FRICTION) ||     // normal friction?
                (f->friction < thing->friction))
                {
                thing->friction   = f->friction;
                thing->movefactor = f->movefactor;
                }
            }
        node = node->m_snext;
        }
    }


//Spawn all friction.
static void P_SpawnFriction(void)
    {
    int i;
    line_t *l = lines;
    register int s;
    int length;     // line length controls magnitude
    int friction;   // friction value to be applied during movement
    int movefactor; // applied to each player move to simulate inertia

    for (i = 0 ; i < numlines ; i++,l++)
        if (l->special == 223)
            {
            length = P_AproxDistance(l->dx,l->dy)>>FRACBITS;
            friction = (0x1EB8*length)/0x80 + 0xD000;

            if(friction > FRACUNIT)
              friction = FRACUNIT;
            if(friction < 0)
              friction = 0;

            // The following check might seem odd. At the time of movement,
            // the move distance is multiplied by 'friction/0x10000', so a
            // higher friction value actually means 'less friction'.

            if (friction > ORIG_FRICTION)       // ice
                movefactor = ((0x10092 - friction)*(0x70))/0x158;
            else
                movefactor = ((friction - 0xDB34)*(0xA))/0x80;

            // killough 8/28/98: prevent odd situations
            if (movefactor < 32)
              movefactor = 32;

            for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Friction(friction,movefactor,s);
            }
    }





/*
  SoM: 3/8/2000: Push/Pull/Wind/Current functions.
  Add_Pusher,
  PIT_PushThing,
  T_Pusher,
  P_GetPushThing,
  P_SpawnPushers
*/


#define PUSH_FACTOR 7

// Adds a pusher
static void Add_Pusher(int type, int x_mag, int y_mag, mobj_t* source, int affectee)
    {
    pusher_t *p = Z_Malloc(sizeof *p, PU_LEVSPEC, 0);

    p->thinker.function.acp1 = (actionf_p1) T_Pusher;
    p->source = source;
    p->type = type;
    p->x_mag = x_mag>>FRACBITS;
    p->y_mag = y_mag>>FRACBITS;

    // Magnitude is used for something else for vertical currents
    // SSNTails 06-14-2002
    if(type == p_downcurrent || type == p_upcurrent || type == p_upwind || type == p_downwind)
        p->magnitude = P_AproxDistance(p->x_mag,p->y_mag)<<(FRACBITS-PUSH_FACTOR);
    else
        p->magnitude = P_AproxDistance(p->x_mag,p->y_mag);

    if (source) // point source exist?
        {
        p->radius = (p->magnitude)<<(FRACBITS+1); // where force goes to zero
        p->x = p->source->x;
        p->y = p->source->y;
        }
    p->affectee = affectee;
    P_AddThinker(&p->thinker);
    }


// PIT_PushThing determines the angle and magnitude of the effect.
// The object's x and y momentum values are changed.

pusher_t* tmpusher; // pusher structure for blockmap searches

boolean PIT_PushThing(mobj_t* thing)
    {
    if (thing->player &&
        !(thing->flags & (MF_NOGRAVITY | MF_NOCLIP)))
        {
        angle_t pushangle;
        int dist;
        int speed;
        int sx,sy;

        sx = tmpusher->x;
        sy = tmpusher->y;
        dist = P_AproxDistance(thing->x - sx,thing->y - sy);
        speed = (tmpusher->magnitude -
                 ((dist>>FRACBITS)>>1))<<(FRACBITS-PUSH_FACTOR-1);

        // If speed <= 0, you're outside the effective radius. You also have
        // to be able to see the push/pull source point.

        if ((speed > 0) && (P_CheckSight(thing,tmpusher->source)))
            {
            pushangle = R_PointToAngle2(thing->x,thing->y,sx,sy);
            if (tmpusher->source->type == MT_PUSH)
                pushangle += ANG180;    // away
            pushangle >>= ANGLETOFINESHIFT;
            thing->momx += FixedMul(speed,finecosine[pushangle]);
            thing->momy += FixedMul(speed,finesine[pushangle]);
            }
        }
    return true;
    }



// T_Pusher looks for all objects that are inside the radius of
// the effect.

void T_Pusher(pusher_t *p)
    {
    sector_t *sec;
    mobj_t   *thing;
    msecnode_t* node;
    int xspeed = 0, yspeed = 0;
    int xl,xh,yl,yh,bx,by;
    int radius;
    int ht = 0;
	boolean inwater;
	boolean touching;
	fixed_t z;

	inwater = touching = false;

    if (!allow_pushers)
        return;

    sec = sectors + p->affectee;

    // Be sure the special sector type is still turned on. If so, proceed.
    // Else, bail out; the sector type has been changed on us.

	if(demoversion <= 140)
	{
        if (!(sec->special & PUSH_MASK))
            return;
	}
	else // Now you can have a PUSH_MASK for individual 3d floors. SSNTails 09-25-2002
	{
		if (!(sec->special & PUSH_MASK)) // Main sector doesn't have one, so let's check the rovers.
		{
			boolean foundfloor;
			foundfloor = false;

			if(sec->ffloors)
			{
			  ffloor_t*  rover;

			  for(rover = sec->ffloors; rover; rover = rover->next)
			  {
				// Do some small extra checks here to possibly save unneeded work.
				if(!(rover->master->frontsector->special & PUSH_MASK))
				  continue;
				foundfloor = true;
			  }
			}

			if(foundfloor == false) // Not even a 3d floor has the PUSH_MASK.
				return;
		}
	}

    // For constant pushers (wind/current) there are 3 situations:
    //
    // 1) Affected Thing is above the floor.
    //
    //    Apply the full force if wind, no force if current.
    //
    // 2) Affected Thing is on the ground.
    //
    //    Apply half force if wind, full force if current.
    //
    // 3) Affected Thing is below the ground (underwater effect).
    //
    //    Apply no force if wind, full force if current.
    //
    // Apply the effect to clipped players only for now.
    //
    // In Phase II, you can apply these effects to Things other than players.

    if (p->type == p_push)
        {

        // Seek out all pushable things within the force radius of this
        // point pusher. Crosses sectors, so use blockmap.

        tmpusher = p; // MT_PUSH/MT_PULL point source
        radius = p->radius; // where force goes to zero
        tmbbox[BOXTOP]    = p->y + radius;
        tmbbox[BOXBOTTOM] = p->y - radius;
        tmbbox[BOXRIGHT]  = p->x + radius;
        tmbbox[BOXLEFT]   = p->x - radius;

        xl = (tmbbox[BOXLEFT] - bmaporgx - MAXRADIUS)>>MAPBLOCKSHIFT;
        xh = (tmbbox[BOXRIGHT] - bmaporgx + MAXRADIUS)>>MAPBLOCKSHIFT;
        yl = (tmbbox[BOXBOTTOM] - bmaporgy - MAXRADIUS)>>MAPBLOCKSHIFT;
        yh = (tmbbox[BOXTOP] - bmaporgy + MAXRADIUS)>>MAPBLOCKSHIFT;
        for (bx=xl ; bx<=xh ; bx++)
            for (by=yl ; by<=yh ; by++)
                P_BlockThingsIterator(bx,by,PIT_PushThing);
        return;
        }

    // constant pushers p_wind and p_current

	if(demoversion <= 140)
	{
		if (sec->heightsec != -1) // special water sector?
			ht = sectors[sec->heightsec].floorheight;

		node = sec->touching_thinglist; // things touching this sector
		for ( ; node ; node = node->m_snext)
			{
			thing = node->m_thing;
			if (!thing->player || (thing->flags & (MF_NOGRAVITY | MF_NOCLIP)))
				continue;
			if (p->type == p_wind)
				{
				if (sec->heightsec == -1) // NOT special water sector
					if (thing->z > thing->floorz) // above ground
						{
						xspeed = p->x_mag; // full force
						yspeed = p->y_mag;
						}
					else // on ground
						{
						xspeed = (p->x_mag)>>1; // half force
						yspeed = (p->y_mag)>>1;
						}
				else // special water sector
					{
					if (thing->z > ht) // above ground
						{
						xspeed = p->x_mag; // full force
						yspeed = p->y_mag;
						}
					else if (thing->player->viewz < ht) // underwater
						xspeed = yspeed = 0; // no force
					else // wading in water
						{
						xspeed = (p->x_mag)>>1; // half force
						yspeed = (p->y_mag)>>1;
						}
					}
				}
			else // p_current
			{
				// Added Z currents SSNTails 06-10-2002
				if (sec->heightsec == -1) // NOT special water sector
					if (thing->z > sec->floorheight) // above ground
						xspeed = yspeed = 0; // no force
					else // on ground
					{
						if(p->type == p_upcurrent)
							thing->momz += p->magnitude;
						else if(p->type == p_downcurrent)
							thing->momz -= p->magnitude;
						else
						{
							xspeed = p->x_mag; // full force
							yspeed = p->y_mag;
						}
					}
				else // special water sector
					if (thing->z > ht) // above ground
						xspeed = yspeed = 0; // no force
					else // underwater
					{
						if(p->type == p_upcurrent)
							thing->momz += p->magnitude;
						else if(p->type == p_downcurrent)
							thing->momz -= p->magnitude;
						else
						{
							xspeed = p->x_mag; // full force
							yspeed = p->y_mag;
						}
					}
			}

			if(p->type != p_downcurrent && p->type != p_upcurrent)
			{
				thing->momx += xspeed<<(FRACBITS-PUSH_FACTOR);
				thing->momy += yspeed<<(FRACBITS-PUSH_FACTOR);
			}

		}
	}
	else // New support
	{
		node = sec->touching_thinglist; // things touching this sector
		for ( ; node ; node = node->m_snext)
			{
			thing = node->m_thing;
			if (!thing->player || (thing->flags & (MF_NOGRAVITY | MF_NOCLIP)))
				continue;

			// Find the area that the 'thing' is in
			// Kudos to P_MobjCheckWater().
			// SSNTails 09-25-2002
			if (sec->heightsec > -1 && sec->altheightsec == 1)
			{
				boolean water;
				water = false;

				if (sec->heightsec > -1)  //water hack
				{
					z = (sectors[sec->heightsec].floorheight);
					water = true;
				}
				else if (sec->floortype == FLOOR_WATER || sec->floortype == FLOOR_LAVA) // Lava support
				{
					z = sec->floorheight + (FRACUNIT/4); // water texture
					water = true;
				}

				if(water == true) // Sector has water
				{
					if (thing->z<=z &&thing->z+thing->height > z)
						touching = true;

					if (thing->z+(thing->height>>1) <= z)
					{
						inwater = true;
					}
				}
			}
			
			// Not "else"! Check ALL possibilities!
			if((inwater == false || touching == false) // Only if both aren't true
				&& sec->ffloors)
			{
			  ffloor_t*  rover;

			  for(rover = sec->ffloors; rover; rover = rover->next)
			  {
				if(*rover->topheight < thing->z || *rover->bottomheight > (thing->z + (thing->height >> 1)))
				  continue;

				if(!(rover->master->frontsector->special & PUSH_MASK))
					continue;

				if(thing->z + thing->height > *rover->topheight)
					touching = true;

				if(thing->z + (thing->height >> 1) < *rover->topheight)
				{
					inwater = true;
				}
			  }
			}

			if (thing->z == sec->floorheight)
				touching = true;
			
			if (p->type == p_wind)
			{
				if (!touching && !inwater) // above ground
				{
					xspeed = p->x_mag; // full force
					yspeed = p->y_mag;
				}
				else if (touching) // on ground
				{
					xspeed = (p->x_mag)>>1; // half force
					yspeed = (p->y_mag)>>1;
				}
				else if (inwater) // underwater
					xspeed = yspeed = 0; // no force
				else
					xspeed = yspeed = 0;
			}
			else if (p->type == p_upwind)
            {
                if (!touching && !inwater) // above ground
                    {
                    thing->momz += p->magnitude;
                    }
                else if (touching) // on ground
                    {
                    thing->momz += (p->magnitude)>>1;
                    }
                else if (inwater) // underwater
                    xspeed = yspeed = 0; // no force
                else
					xspeed = yspeed = 0;
            }
			else if (p->type == p_downwind)
            {
                if (!touching && !inwater) // above ground
                    {
                    thing->momz -= p->magnitude;
                    }
                else if (touching) // on ground
                    {
                    thing->momz -= (p->magnitude)>>1;
                    }
                else if (inwater) // underwater
                    xspeed = yspeed = 0; // no force
                else
					xspeed = yspeed = 0;
            }
			else // p_current
			{
				// Added Z currents SSNTails 06-10-2002
				if(!touching && !inwater) // Not in water at all
					xspeed = yspeed = 0; // no force
				else // underwater / touching water
				{
					if(p->type == p_upcurrent)
						thing->momz += p->magnitude;
					else if(p->type == p_downcurrent)
						thing->momz -= p->magnitude;
					else
					{
						xspeed = p->x_mag; // full force
						yspeed = p->y_mag;
					}
				}
			}

			if(p->type != p_downcurrent && p->type != p_upcurrent
				&& p->type != p_upwind && p->type != p_downwind)
			{
				thing->momx += xspeed<<(FRACBITS-PUSH_FACTOR);
				thing->momy += yspeed<<(FRACBITS-PUSH_FACTOR);
			}
		}
	}
}


// Get pusher object.
mobj_t* P_GetPushThing(int s)
    {
    mobj_t* thing;
    sector_t* sec;

    sec = sectors + s;
    thing = sec->thinglist;
    while (thing)
        {
        switch(thing->type)
            {
          case MT_PUSH:
          case MT_PULL:
            return thing;
          default:
            break;
            }
        thing = thing->snext;
        }
    return NULL;
    }


// Spawn pushers.
static void P_SpawnPushers(void)
    {
    int i;
    line_t *l = lines;
    register int s;
    mobj_t* thing;

    for (i = 0 ; i < numlines ; i++,l++)
        switch(l->special)
            {
          case 224: // wind
            for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Pusher(p_wind,l->dx,l->dy,NULL,s);
            break;
          case 225: // current
            for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Pusher(p_current,l->dx,l->dy,NULL,s);
            break;
          case 226: // push/pull
            for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                {
                thing = P_GetPushThing(s);
                if (thing) // No MT_P* means no effect
                    Add_Pusher(p_push,l->dx,l->dy,thing,s);
                }
            break;
          case 292: // current up SSNTails 06-10-2002
            for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Pusher(p_upcurrent,l->dx,l->dy,NULL,s);
            break;
		  case 293: // current down SSNTails 06-10-2002
            for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Pusher(p_downcurrent,l->dx,l->dy,NULL,s);
            break;
		  case 294: // wind up SSNTails 06-14-2003
			  for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Pusher(p_upwind,l->dx,l->dy,NULL,s);
			  break;
		  case 295: // wind down SSNTails 06-14-2003
			  for (s = -1; (s = P_FindSectorFromLineTag(l,s)) >= 0 ; )
                Add_Pusher(p_downwind,l->dx,l->dy,NULL,s);
			  break;
            }
    }
