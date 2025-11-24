// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: g_state.h,v 1.3 2000/03/29 19:39:48 bpereira Exp $
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
// $Log: g_state.h,v $
// Revision 1.3  2000/03/29 19:39:48  bpereira
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
//      Doom/Hexen game states
//
//-----------------------------------------------------------------------------


#ifndef __G_STATE__
#define __G_STATE__

#include "doomtype.h"

// skill levels
typedef enum
{
    sk_baby,
    sk_easy,
    sk_medium,
    sk_hard,
    sk_nightmare
} skill_t;

// the current state of the game
typedef enum
{
    GS_NULL = 0,                // at begin
    GS_LEVEL,                   // we are playing
    GS_INTERMISSION,            // gazing at the intermission screen
    GS_FINALE,                  // game final animation
    GS_DEMOSCREEN,              // looking at a demo
    //legacy
    GS_DEDICATEDSERVER,         // added 27-4-98 : new state for dedicated server
    GS_WAITINGPLAYERS           // added 3-9-98 : waiting player in net game
} gamestate_t;

typedef enum
{
    ga_nothing,
    ga_completed,
    ga_worlddone,
    //HeXen
/*
    ga_initnew,
    ga_newgame,
    ga_loadgame,
    ga_savegame,
    ga_leavemap,
    ga_singlereborn
*/
} gameaction_t;



extern  gamestate_t     gamestate;
extern  gameaction_t    gameaction;
extern  skill_t         gameskill;

extern  boolean         demoplayback;

#endif //__G_STATE__
