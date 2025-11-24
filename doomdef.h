// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: doomdef.h,v 1.44 2004/04/18 20:40:31 hurdler Exp $
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
// $Log: doomdef.h,v $
// Revision 1.44  2004/04/18 20:40:31  hurdler
// go 1.42
//
// Revision 1.43  2003/08/11 13:50:02  hurdler
// go final + translucent HUD + fix spawn in net game
//
// Revision 1.42  2003/07/13 13:16:15  hurdler
// go RC1
//
// Revision 1.41  2002/09/10 19:29:46  hurdler
// Add log file under Linux
//
// Revision 1.40  2002/08/25 14:59:32  hurdler
// Go RC2
//
// Revision 1.39  2002/07/28 17:11:32  hurdler
// Change version number to reflect the big changes since v.30
//
// Revision 1.38  2002/06/30 21:37:48  hurdler
// Ready for 1.32 beta 5 release
//
// Revision 1.37  2002/06/14 02:49:46  ssntails
// Fix for 3dfloor bug with objects on them.
//
// Revision 1.36  2001/11/17 22:12:53  hurdler
// Ready to work on beta 4 ;)
//
// Revision 1.35  2001/08/08 20:34:43  hurdler
// Big TANDL update
//
// Revision 1.34  2001/07/16 22:35:40  bpereira
// - fixed crash of e3m8 in heretic
// - fixed crosshair not drawed bug
//
// Revision 1.33  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.32  2001/05/01 20:39:28  hurdler
// Ready for 1.32 beta 1
//
// Revision 1.31  2001/04/04 20:26:11  judgecutor
// Added definition for testing 3D Sound code (now Win32 only)
//
// Revision 1.30  2001/03/13 22:14:19  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.29  2001/03/03 19:43:58  ydario
// Check for PARANOIA
//
// Revision 1.28  2001/03/03 06:17:33  bpereira
// no message
//
// Revision 1.27  2001/02/24 13:35:19  bpereira
// no message
//
// Revision 1.26  2001/01/25 22:15:41  bpereira
// added heretic support
//
// Revision 1.25  2000/11/02 17:50:06  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.24  2000/10/21 08:43:28  bpereira
// no message
//
// Revision 1.23  2000/10/08 13:30:00  bpereira
// no message
//
// Revision 1.22  2000/10/02 18:25:44  bpereira
// no message
//
// Revision 1.21  2000/09/28 20:57:14  bpereira
// no message
//
// Revision 1.20  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.19  2000/08/21 11:44:42  hurdler
// Re-put the log messages
//
// Revision 1.18  2000/08/03 17:57:41  bpereira
// no message
//
// Revision 1.17  2000/06/09 00:49:37  hurdler
// change version number to 1.31 beta 1
//
// Revision 1.16  2000/05/09 20:50:19  hurdler
// change version
//
// Revision 1.15  2000/04/27 17:43:19  hurdler
// colormap code in hardware mode is now the default
//
// Revision 1.14  2000/04/23 16:19:52  bpereira
// no message
//
// Revision 1.13  2000/04/18 12:53:28  hurdler
// change version number
//
// Revision 1.12  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.11  2000/04/15 22:12:57  stroggonmeth
// Minor bug fixes
//
// Revision 1.10  2000/04/12 16:01:59  hurdler
// ready for T&L code and true static lighting
//
// Revision 1.9  2000/04/06 21:06:19  stroggonmeth
// Optimized extra_colormap code...
// Added #ifdefs for older water code.
//
// Revision 1.8  2000/04/06 20:40:22  hurdler
// Mostly remove warnings under windows
//
// Revision 1.7  2000/04/04 19:28:42  stroggonmeth
// Global colormaps working. Added a new linedef type 272.
//
// Revision 1.6  2000/04/04 00:32:45  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.5  2000/03/23 22:54:00  metzgermeister
// added support for HOME/.legacy under Linux
//
// Revision 1.4  2000/03/06 15:49:28  hurdler
// change version number
//
// Revision 1.3  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
//
// DESCRIPTION:
//      Internally used data structures for virtually everything,
//      key definitions, lots of other stuff.
//
//-----------------------------------------------------------------------------

#ifndef __DOOMDEF__
#define __DOOMDEF__

#ifdef __WIN32__
#define HWRENDER
// judgecutor: 3D sound support
#define HW3SOUND
#define ASMCALL __cdecl
#pragma warning (disable :  4244 4146 4761 4152 4115 4100 4201 4200) // 4244 4146 4761 4018
// warning C4146: unary minus operator applied to unsigned type, result still unsigned
// warning C4761: integral size mismatch in argument; conversion supplied
// warning C4244: 'initializing' : conversion from 'const double ' to 'int ', possible loss of data
// warning C4244: '=' : conversion from 'double ' to 'int ', possible loss of data
// warning C4018: '<' : signed/unsigned mismatch

// warning level 4
// warning C4152: nonstandard extension, function/data pointer conversion in expression
// warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses
// warning C4100: 'pitch' : unreferenced formal parameter
// warning C4201: nonstandard extension used : nameless struct/union
// warning C4200: nonstandard extension used : zero-sized array in struct/union
#else
#define ASMCALL
#define min(x,y) ( ((x)<(y)) ? (x) : (y) )
#define max(x,y) ( ((x)>(y)) ? (x) : (y) )
#endif

#include "doomtype.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#if defined( WIN32) || defined( __DJGPP__ )
#include <io.h>
#endif

#ifdef PC_DOS
#include <conio.h>
#endif

// Uncheck this to compile debugging code
//#define RANGECHECK
#ifndef PARANOIA
#define PARANOIA                // do some test that never happens but maybe
#endif
#if defined(__WIN32__) || defined(LINUX)
//#define LOGMESSAGES             // write message in log.txt (win32 and Linux only for the moment)
#endif
#define VERSION        142      // Game version
#define VERSIONSTRING  " Birthday version"

#if defined (LOGMESSAGES) && defined(LINUX)
#define INVALID_HANDLE_VALUE -1
extern int logstream;
#endif

// some tests, enable or desable it if it run or not
//#define HORIZONTALDRAW        // abandoned : too slow
//#define TILTVIEW              // not finished
//#define PERSPCORRECT          // not finished
#define SPLITSCREEN
#define ABSOLUTEANGLE           // work fine, soon #ifdef and old code remove
//#define CLIENTPREDICTION2     // differant methode
#define NEWLIGHT                // compute lighting with bsp (in construction)
//#define OLDWATER                // SoM: Allow old legacy water.
#define FRAGGLESCRIPT           // SoM: Activate FraggleScript
#define FIXROVERBUGS // Fix some 3dfloor bugs. SSNTails 06-13-2002

// =========================================================================


// demo version when playback demo, or the current VERSION
// used to enable/disable selected features for backward compatibility
// (where possible)
extern byte     demoversion;


// The maximum number of players, multiplayer/networking.
// NOTE: it needs more than this to increase the number of players...

#define MAXPLAYERS              32      // TODO: ... more!!!
#define MAXSKINS                MAXPLAYERS
#define PLAYERSMASK             (MAXPLAYERS-1)
#define MAXPLAYERNAME           21
#define MAXSKINCOLORS           11

#define SAVESTRINGSIZE          24

// State updates, number of tics / second.
// NOTE: used to setup the timer rate, see I_StartupTimer().
#define OLDTICRATE       35
#define NEWTICRATERATIO   1  // try 4 for 140 fps :)
#define TICRATE         (OLDTICRATE*NEWTICRATERATIO)


// Name of local directory for config files and savegames
#ifdef LINUX
#define DEFAULTDIR ".legacy"
#else
#define DEFAULTDIR "legacy"
#endif

#include "g_state.h"

// commonly used routines - moved here for include convenience

// i_system.h
void I_Error (char *error, ...);

// console.h
void    CONS_Printf (char *fmt, ...);

#include "m_swap.h"

// m_misc.h
char *va(char *format, ...);
char *Z_StrDup (const char *in);

// g_game.h
extern  boolean devparm;                // development mode (-devparm)

// =======================
// Misc stuff for later...
// =======================

// if we ever make our alloc stuff...
#define ZZ_Alloc(x) Z_Malloc(x,PU_STATIC,NULL)

// debug me in color (v_video.c)
void IO_Color( unsigned char color, unsigned char r, unsigned char g, unsigned char b );

// i_system.c, replace getchar() once the keyboard has been appropriated
int I_GetKey (void);

#endif          // __DOOMDEF__
