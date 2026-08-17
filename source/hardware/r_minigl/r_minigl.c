// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_minigl.c,v 1.7 2000/10/22 14:18:00 hurdler Exp $
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
// $Log: r_minigl.c,v $
// Revision 1.7  2000/10/22 14:18:00  hurdler
// Adjust version string
//
// Revision 1.6  2000/05/09 21:13:14  hurdler
// update version
//
// Revision 1.5  2000/04/27 23:43:42  hurdler
// Change coronas' code for MiniGL compatibility
//
// Revision 1.4  2000/03/06 15:29:04  hurdler
// change version number
//
// Revision 1.3  2000/02/27 16:36:32  hurdler
// Update version number
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      MiniGL API for Doom Legacy
//
//-----------------------------------------------------------------------------


// tell r_opengl.cpp to compile for MiniGL Drivers
#define MINI_GL_COMPATIBILITY   

// tell r_opengl.cpp to compile for ATI Rage Pro OpenGL driver
//#define ATI_RAGE_PRO_COMPATIBILITY   

#define DRIVER_STRING "HWRAPI Init(): DooM Legacy MiniGL renderer"

// Include this at end
#include "../r_opengl/r_opengl.c"
#include "../r_opengl/ogl_win.c"

// That's all ;-) 
// Just, be sure to do the right changes in r_opengl.cpp
