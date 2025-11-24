// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: b_bot.h,v 1.3 2002/09/28 06:53:11 tonyd Exp $
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
// $Log: b_bot.h,v $
// Revision 1.3  2002/09/28 06:53:11  tonyd
// fixed CR problem, fixed game options crash
//
// Revision 1.2  2002/09/27 16:40:07  tonyd
// First commit of acbot
//

#include "tables.h"
#include "p_mobj.h"
#include "b_search.h"

#ifndef __BOT_H__
#define __BOT_H__

typedef struct
{
	boolean			straferight;
	byte			lastNumWeapons;		//used to check if got a new weapon

	int				blockedcount,
					avoidtimer,		//used so if blocked by something, like a barrel will reverse, and try to get around it
					strafetimer,
					weaponchangetimer;

	fixed_t			lastMobjX,			//where last enemy was seen
					lastMobjY;

	mobj_t			*bestSeenItem,		//best item seen
					*bestItem,			//best item on map, not neccessarily seen
					*closestEnemy,
					*closestMissile,
					*closestUnseenEnemy,//goes towards this enemy if have nothing else todo
					*closestUnseenTeammate,//goes towards this teammate if have nothing else todo
					*lastMobj,			//last enemy
					*teammate;

	LinkedList_t	*path;				//path to the best item on the map
	SearchNode_t	*destNode;			//the closest node to where wants to go

} bot_t;

#endif
