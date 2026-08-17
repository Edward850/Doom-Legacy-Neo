// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: p_inter.h,v 1.6 2002/08/24 22:42:03 hurdler Exp $
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
// $Log: p_inter.h,v $
// Revision 1.6  2002/08/24 22:42:03  hurdler
// Apply Robert Hogberg patches
//
// Revision 1.5  2001/11/02 21:39:45  judgecutor
// Added Frag's weapon falling
//
// Revision 1.4  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.3  2000/11/04 16:23:43  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//
//
//-----------------------------------------------------------------------------


#ifndef __P_INTER__
#define __P_INTER__


#ifdef __GNUG__
#pragma interface
#endif

extern consvar_t cv_fragsweaponfalling;

// Boris hack : preferred weapons order
void VerifFavoritWeapon (player_t *player);

int FindBestWeapon(player_t *player);

boolean P_GivePower(player_t*, int);
void P_CheckFragLimit(player_t *p);

void P_KillMobj ( mobj_t*       target,
                  mobj_t*       inflictor,
                  mobj_t*       source );
boolean P_GiveBody ( player_t*     player,
                     int           num );


//added:28-02-98: boooring handling of thing(s) on top of thing(s)
/* BUGGY CODE
void P_CheckSupportThings (mobj_t* mobj);
void P_MoveSupportThings (mobj_t* mobj, fixed_t xmove,
                                        fixed_t ymove,
                                        fixed_t zmove);
void P_LinkFloorThing(mobj_t* mobj);
void P_UnlinkFloorThing(mobj_t* mobj);
*/

#endif
