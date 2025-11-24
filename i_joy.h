// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_joy.h,v 1.2 2000/02/27 00:42:10 hurdler Exp $
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
// $Log: i_joy.h,v $
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      share joystick information with game control code
//
//-----------------------------------------------------------------------------


#ifndef __I_JOY_H__
#define __I_JOY_H__

#include "g_input.h"

#define JOYAXISRANGE     1023   //faB: (1024-1) so we can do a right shift instead of division
                                //     (doesnt matter anyway, just give enough precision)
                                // a gamepad will return -1, 0, or 1 in the event data
                                // an analog type joystick will return a value
                                // from -JOYAXISRANGE to +JOYAXISRANGE for each axis

// detect a bug if we increase JOYBUTTONS above DIJOYSTATE's number of buttons
#if (JOYBUTTONS > 32)
#error "JOYBUTTONS is greater than DIJOYSTATE number of buttons"
#endif

// share some joystick information (maybe 2 for splitscreen), to the game input code,
// actually, we need to know if it is a gamepad or analog controls

struct JoyType_s {
    int     bJoyNeedPoll;       // if true, we MUST Poll() to get new joystick data,
                                // that is: we NEED the DIRECTINPUTDEVICE2 ! (watchout NT compatibility)
    int     bGamepadStyle;      // this joystick is a gamepad, read: digital axes
                                // if FALSE, interpret the joystick event data as JOYAXISRANGE
                                // (see above)
};
typedef struct JoyType_s JoyType_t;

extern JoyType_t   Joystick;    //faB: may become an array (2 for splitscreen), I said: MAY BE...

#endif // __I_JOY_H__
