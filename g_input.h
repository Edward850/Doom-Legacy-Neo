// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: g_input.h,v 1.8 2002/08/24 22:42:03 hurdler Exp $
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
// $Log: g_input.h,v $
// Revision 1.8  2002/08/24 22:42:03  hurdler
// Apply Robert Hogberg patches
//
// Revision 1.7  2002/07/01 19:59:58  metzgermeister
// *** empty log message ***
//
// Revision 1.6  2001/04/17 22:26:07  calumr
// Initial Mac add
//
// Revision 1.5  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.4  2001/01/25 22:15:42  bpereira
// added heretic support
//
// Revision 1.3  2000/04/04 00:32:45  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      handle mouse/keyboard/joystick inputs,
//      maps inputs to game controls (forward,use,open...)
//
//-----------------------------------------------------------------------------


#ifndef __G_INPUT__
#define __G_INPUT__

#include "d_event.h"
#include "keys.h"
#include "command.h"

#define MAXMOUSESENSITIVITY   40        // sensitivity steps

// number of total 'button' inputs, include keyboard keys, plus virtual
// keys (mousebuttons and joybuttons becomes keys)
#define NUMKEYS         256

#define MOUSEBUTTONS    8
#define JOYBUTTONS      14  // 10 bases + 4 hat

//
// mouse and joystick buttons are handled as 'virtual' keys
//
typedef enum {
    KEY_MOUSE1        = NUMKEYS,                  
    KEY_JOY1          = KEY_MOUSE1+MOUSEBUTTONS,  
    KEY_DBLMOUSE1     = KEY_JOY1+JOYBUTTONS,        // double clicks
    KEY_DBLJOY1       = KEY_DBLMOUSE1+MOUSEBUTTONS,
    KEY_2MOUSE1       = KEY_DBLJOY1+JOYBUTTONS,
    KEY_DBL2MOUSE1    = KEY_2MOUSE1+MOUSEBUTTONS,
    KEY_MOUSEWHEELUP  = KEY_DBL2MOUSE1+MOUSEBUTTONS,
    KEY_MOUSEWHEELDOWN,
    KEY_2MOUSEWHEELUP,
    KEY_2MOUSEWHEELDOWN,
    NUMINPUTS
} key_input_e;

typedef enum
{
    gc_null = 0,        //a key/button mapped to gc_null has no effect
    gc_forward,
    gc_backward,
    gc_strafe,
    gc_straferight,
    gc_strafeleft,
    gc_speed,
    gc_turnleft,
    gc_turnright,
    gc_fire,
    gc_use,
    gc_lookup,
    gc_lookdown,
    gc_centerview,
    gc_mouseaiming,     // mouse aiming is momentary (toggleable in the menu)
    gc_weapon1,
    gc_weapon2,
    gc_weapon3,
    gc_weapon4,
    gc_weapon5,
    gc_weapon6,
    gc_weapon7,
    gc_weapon8,
    gc_talkkey,
    gc_scores,
    gc_jump,
    gc_console,
    gc_nextweapon,
    gc_prevweapon,
    gc_bestweapon,
    gc_invnext,
    gc_invprev,
    gc_invuse,
    gc_flydown,     // flyup is jump !
    num_gamecontrols
} gamecontrols_e;


// mouse values are used once
extern consvar_t       cv_mousesens;
extern consvar_t       cv_mlooksens;
extern consvar_t       cv_allowjump;
extern consvar_t       cv_allowrocketjump;
extern consvar_t       cv_allowautoaim;
extern int             mousex;
extern int             mousey;
extern int             mlooky;  //mousey with mlookSensitivity
extern int             mouse2x;
extern int             mouse2y;
extern int             mlook2y;

extern int             dclicktime;
extern int             dclickstate;
extern int             dclicks;
extern int             dclicktime2;
extern int             dclickstate2;
extern int             dclicks2;

extern int             joyxmove;
extern int             joyymove;

// current state of the keys : true if pushed
extern  byte    gamekeydown[NUMINPUTS];

// two key codes (or virtual key) per game control
extern  int     gamecontrol[num_gamecontrols][2];
extern  int     gamecontrolbis[num_gamecontrols][2];    // secondary splitscreen player

// peace to my little coder fingers!
// check a gamecontrol being active or not

// remaps the input event to a game control.
void  G_MapEventsToControls (event_t *ev);

// returns the name of a key
char* G_KeynumToString (int keynum);
int   G_KeyStringtoNum(char *keystr);

// detach any keys associated to the given game control
void  G_ClearControlKeys (int (*setupcontrols)[2], int control);
void  Command_Setcontrol_f(void);
void  Command_Setcontrol2_f(void);
void  G_Controldefault(void);
void  G_SaveKeySetting(FILE *f);
void  G_CheckDoubleUsage(int keynum);


#endif
