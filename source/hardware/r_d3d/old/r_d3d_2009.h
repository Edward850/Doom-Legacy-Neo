// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_d3d_2009.h,v 1.1 2000/10/01 15:14:32 hurdler Exp $
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
// $Log: r_d3d_2009.h,v $
// Revision 1.1  2000/10/01 15:14:32  hurdler
// Completely rewritten d3d driver... absolutely not finished at all
//
//
//
// DESCRIPTION:
//      
//
//-----------------------------------------------------------------------------


#ifndef _R_D3D_H_
#define _R_D3D_H_


#define  _CREATE_DLL_  // necessary for Unix AND Windows
#include "../hw_drv.h"

// ==========================================================================
//                                                                DEFINITIONS
// ==========================================================================

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )

#undef DEBUG_TO_FILE            // maybe defined in previous *.h
#define DEBUG_TO_FILE           // output debugging msgs to ogllog.txt

#define DRIVER_STRING "HWRAPI Init(): DooM Legacy Direct3D renderer v1.31\n"


// ==========================================================================
//                                                                     PROTOS
// ==========================================================================

void DBG_Printf(LPCTSTR lpFmt, ...);                                          
void Flush(void);
int  isExtAvailable(char *extension);
int  SetRes(viddef_t *lvid, vmode_t *pcurrentmode);
void UnSetRes(void);
boolean SetupPixelFormat(int WantColorBits, int WantStencilBits, int WantDepthBits);
void SetModelView(int w, int h);
void SetStates(void);


#endif
