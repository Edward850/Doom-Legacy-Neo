// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: vid_x11.c,v 1.2 2000/02/27 00:42:12 hurdler Exp $
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
// $Log: vid_x11.c,v $
// Revision 1.2  2000/02/27 00:42:12  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      dummy X11 vid runtime for doom legacy
//      
//
//-----------------------------------------------------------------------------


#include<stdlib.h>

int   VID_NumModes(void) {
  return 1;
}

char  *VID_GetModeName(int modenum) {
  if(modenum!=0) {
    return NULL;
  }
  return "320x200";
}

int  VID_SetMode(int modenum) { return 0; } /* dummy */

int VID_GetModeForSize( int w, int h) {
  return 0;
}
