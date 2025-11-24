// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: console.h,v 1.2 2000/02/27 00:42:10 hurdler Exp $
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
// $Log: console.h,v $
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


#include "d_event.h"


// for debugging shopuld be replaced by nothing later.. so debug is inactive
#define LOG(x) CONS_Printf(x)

void CON_Init (void);

boolean CON_Responder (event_t *ev);

// set true when screen size has changed, to adapt console
extern boolean con_recalc;

extern boolean con_startup;

// top clip value for view render: do not draw part of view hidden by console
extern int     con_clipviewtop;

// 0 means console if off, or moving out
extern int     con_destlines;

extern int     con_clearlines;  // lines of top of screen to refresh
extern boolean con_hudupdate;   // hud messages have changed, need refresh

extern int     con_keymap;      //0 english, 1 french

extern byte*   whitemap;
extern byte*   greenmap;
extern byte*   graymap;

void CON_ClearHUD (void);       // clear heads up messages

void CON_Ticker (void);
void CON_Drawer (void);
void CONS_Error (char *msg);       // print out error msg, and wait a key

// force console to move out
void CON_ToggleOff (void);
