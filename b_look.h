// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: b_look.h,v 1.3 2002/09/28 06:53:11 tonyd Exp $
//
// Copyright (C) 2002 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
//
// $Log: b_look.h,v $
// Revision 1.3  2002/09/28 06:53:11  tonyd
// fixed CR problem, fixed game options crash
//
// Revision 1.2  2002/09/27 16:40:08  tonyd
// First commit of acbot
//

#include "d_player.h"
#include "r_defs.h"

#ifndef __BOTLOOK_H__
#define __BOTLOOK_H__

boolean B_ReachablePoint (player_t *p, sector_t* destSector, fixed_t x, fixed_t y);
boolean B_LookForSpecialLine (player_t* p, fixed_t* x, fixed_t* y);
void B_LookForThings (player_t* p);

#endif
