// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_opengl.h,v 1.16 2001/03/09 21:53:56 metzgermeister Exp $
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
// $Log: r_opengl.h,v $
// Revision 1.16  2001/03/09 21:53:56  metzgermeister
// *** empty log message ***
//
// Revision 1.15  2001/02/19 17:45:20  hurdler
// Fix the problem of fullbright with Matrox's drivers under Linux
//
// Revision 1.14  2000/11/02 19:49:40  bpereira
// no message
//
// Revision 1.13  2000/10/22 14:17:17  hurdler
// Adjust version string
//
// Revision 1.12  2000/09/25 19:29:24  hurdler
// Maintenance modifications
//
// Revision 1.11  2000/08/11 12:28:08  hurdler
// latest changes for v1.30
//
// Revision 1.10  2000/08/10 19:58:05  bpereira
// no message
//
// Revision 1.9  2000/08/03 17:57:42  bpereira
// no message
//
// Revision 1.8  2000/05/09 21:10:04  hurdler
// update version
//
// Revision 1.7  2000/04/18 12:45:09  hurdler
// change a little coronas' code
//
// Revision 1.6  2000/04/07 23:10:15  metzgermeister
// fullscreen support under X in Linux
//
// Revision 1.5  2000/03/07 03:31:14  hurdler
// fix linux compilation
//
// Revision 1.4  2000/03/06 15:29:32  hurdler
// change version number
//
// Revision 1.3  2000/02/27 16:37:14  hurdler
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
//      
//
//-----------------------------------------------------------------------------


#ifndef _R_OPENGL_H_
#define _R_OPENGL_H_

#include <GL/gl.h>
#include <GL/glu.h>

#define  _CREATE_DLL_  // necessary for Unix AND Windows
#include "../../doomdef.h"
#include "../hw_drv.h"

// ==========================================================================
//                                                                DEFINITIONS
// ==========================================================================

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )

#undef DEBUG_TO_FILE            // maybe defined in previous *.h
#define DEBUG_TO_FILE           // output debugging msgs to ogllog.txt

#ifndef MINI_GL_COMPATIBILITY
//    #define USE_PALETTED_TEXTURE
#define DRIVER_STRING "HWRAPI Init(): DooM Legacy OpenGL renderer"
#endif

#ifndef __WIN32__
typedef unsigned int    DWORD;
typedef char*           LPCTSTR;
typedef int             HANDLE;
#endif

// ==========================================================================
//                                                                     PROTOS
// ==========================================================================

void DBG_Printf(LPCTSTR lpFmt, ...);                                          
void Flush(void);
int  isExtAvailable(char *extension);
int  SetRes(viddef_t *lvid, vmode_t *pcurrentmode);
void UnSetRes(void);
boolean SetupPixelFormat(int WantColorBits, int WantStencilBits, int WantDepthBits);
void SetModelView(GLint w, GLint h);
void SetStates(void);

// ==========================================================================
//                                                                     GLOBAL
// ==========================================================================

extern const GLubyte    *gl_extensions;
extern RGBA_t           myPaletteData[];
extern HANDLE           logstream;
extern GLint            screen_width;
extern GLint            screen_height;
extern GLbyte           screen_depth;
extern int              oglflags;
extern GLint            textureformatGL;

typedef enum {
    GLF_NOZBUFREAD = 0x01,
    GLF_NOTEXENV   = 0x02,
} oglflags_t;

#endif
