// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: d_main.h,v 1.6 2001/08/20 20:40:39 metzgermeister Exp $
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
// $Log: d_main.h,v $
// Revision 1.6  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.5  2000/10/21 08:43:28  bpereira
// no message
//
// Revision 1.4  2000/04/23 16:19:52  bpereira
// no message
//
// Revision 1.3  2000/04/16 18:38:07  bpereira
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
//      game startup, and main loop code, system specific interface stuff.
//
//-----------------------------------------------------------------------------


#ifndef __D_MAIN__
#define __D_MAIN__

#include "d_event.h"
#include "w_wad.h"   // for MAX_WADFILES


//void D_AddFile (char *file);

// make sure not to write back the config until it's been correctly loaded
extern tic_t      rendergametic;

// for dedicated server
extern boolean dedicated;

// the infinite loop of D_DoomLoop() called from win_main for windows version
void D_DoomLoop (void);

//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
// If not overrided by user input, calls N_AdvanceDemo.
//
void D_DoomMain (void);

// Called by IO functions when input is detected.
void D_PostEvent (const event_t* ev);
void D_PostEvent_end (void);    // delimiter for locking memory

void D_ProcessEvents (void);
void D_DoAdvanceDemo (void);

//
// BASE LEVEL
//
void D_PageTicker (void);
// pagename is lumpname of a 320x200 patch to fill the screen
void D_PageDrawer (char* pagename);
void D_AdvanceDemo (void);
void D_StartTitle (void);

#endif //__D_MAIN__
