// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: d_event.h,v 1.3 2001/03/03 06:17:33 bpereira Exp $
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
// $Log: d_event.h,v $
// Revision 1.3  2001/03/03 06:17:33  bpereira
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
//      event handling.
//    
//-----------------------------------------------------------------------------

#ifndef __D_EVENT__
#define __D_EVENT__

#include "doomtype.h"
#include "g_state.h"

// Input event types.
typedef enum
{
    ev_keydown,
    ev_keyup,
    ev_mouse,
    ev_joystick,
    ev_mouse2
} evtype_t;

// Event structure.
typedef struct
{
    evtype_t    type;
    int         data1;          // keys / mouse/joystick buttons
    int         data2;          // mouse/joystick x move
    int         data3;          // mouse/joystick y move
} event_t;


//
// GLOBAL VARIABLES
//
#define MAXEVENTS               64

extern  event_t         events[MAXEVENTS];
extern  int             eventhead;
extern  int             eventtail;

extern  gameaction_t    gameaction;


#endif
