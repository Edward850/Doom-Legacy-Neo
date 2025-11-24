// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_fab.h,v 1.2 2000/02/27 00:42:10 hurdler Exp $
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
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
// $Log: p_fab.h,v $
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


#ifndef __P_FAB__
#define __P_FAB__

#include "doomtype.h"
#include "command.h"

extern consvar_t cv_solidcorpse;        //p_enemy
extern consvar_t cv_bloodtime;

// spawn smoke trails behind rockets and skull head attacks
void A_SmokeTrailer (mobj_t* actor);

// hack the states table to set Doom Legacy's default translucency on sprites
void P_SetTranslucencies (void);

// add commands for deathmatch rules and style (like more blood) :)
void D_AddDeathmatchCommands (void);

#endif
