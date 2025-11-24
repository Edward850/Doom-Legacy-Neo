// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: screen.h,v 1.12 2003/05/30 22:44:08 hurdler Exp $
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
// $Log: screen.h,v $
// Revision 1.12  2003/05/30 22:44:08  hurdler
// add checkcvar function to FS
//
// Revision 1.11  2003/05/04 04:16:25  sburke
// Allow 1280x1024 for full-screen video on Solaris.
//
// Revision 1.10  2002/11/12 00:06:05  ssntails
// Support for translated translucent columns in software mode.
//
// Revision 1.9  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.8  2001/04/17 22:26:07  calumr
// Initial Mac add
//
// Revision 1.7  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.6  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.5  2000/11/02 19:49:37  bpereira
// no message
//
// Revision 1.4  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.3  2000/04/22 20:27:35  metzgermeister
// support for immediate fullscreen switching
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------


#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "command.h"

#ifdef __WIN32__
#include <windows.h>
#else
#define HWND    void*   //unused in DOS version
#endif

//added:26-01-98: quickhack for V_Init()... to be cleaned up
#define NUMSCREENS    4

// Size of statusbar.
#define ST_HEIGHT    32
#define ST_WIDTH     320
// heretic one's
#define SBARHEIGHT      42                      // status bar height at bottom of screen


extern int stbarheight;     // when scaled

//added:20-01-98: used now as a maximum video mode size for extra vesa modes.

// we try to re-allocate a minimum of buffers for stability of the memory,
// so all the small-enough tables based on screen size, are allocated once
// and for all at the maximum size.
//#ifdef SOLARIS
#define MAXVIDWIDTH    65535  // Needed for fullscreen mode on Sun monitors.
#define MAXVIDHEIGHT   65535  // 
//#else
//#define MAXVIDWIDTH    1024  // dont set this too high because actually
//#define MAXVIDHEIGHT    768  // lots of tables are allocated with the MAX size.
//#endif

#define BASEVIDWIDTH    320   //NEVER CHANGE THIS! this is the original
#define BASEVIDHEIGHT   200  // resolution of the graphics.

// global video state
typedef struct viddef_s
{
    int         modenum;         // vidmode num indexes videomodes list

    byte        *buffer;         // invisible screens buffer
    unsigned    rowbytes;        // bytes per scanline of the VIDEO mode
    int         width;           // PIXELS per scanline
    int         height;
    union { // hurdler: don't need numpages for OpenGL, so we can
            // 15/10/99 use it for fullscreen / windowed mode
    int         numpages;        // always 1, PAGE FLIPPING TODO!!!
    int         windowed;        // windowed or fullscren mode ?
    } u; //BP: name it please soo it work with gcc
    int         recalc;          // if true, recalc vid-based stuff
    byte        *direct;         // linear frame buffer, or vga base mem.
    int         dupx,dupy;       // scale 1,2,3 value for menus & overlays
    float       fdupx,fdupy;     // same as dupx,dupy but exact value when aspect ratio isn't 320/200
    int         centerofs;       // centering for the scaled menu gfx
    int         bpp;             // BYTES per pixel: 1=256color, 2=highcolor

    int         baseratio;       // SoM: Used to get the correct value for lighting walls

    // for Win32 version
    HWND        WndParent;       // handle of the application's window
} viddef_t;
#define VIDWIDTH    vid.width
#define VIDHEIGHT   vid.height


// internal additional info for vesa modes only
typedef struct {
    int         vesamode;         // vesa mode number plus LINEAR_MODE bit
    void        *plinearmem;      // linear address of start of frame buffer
} vesa_extra_t;
// a video modes from the video modes list,
// note: video mode 0 is always standard VGA320x200.
typedef struct vmode_s {

    struct vmode_s  *pnext;
    char         *name;
    unsigned int width;
    unsigned int height;
    unsigned int rowbytes;          //bytes per scanline
    unsigned int bytesperpixel;     // 1 for 256c, 2 for highcolor
    int          windowed;          // if true this is a windowed mode
    int          numpages;
    vesa_extra_t *pextradata;       //vesa mode extra data
    int          (*setmode)(viddef_t *lvid, struct vmode_s *pcurrentmode);
    int          misc;              //misc for display driver (r_glide.dll etc)
} vmode_t;

// ---------------------------------------------
// color mode dependent drawer function pointers
// ---------------------------------------------

extern void     (*skycolfunc) (void);
extern void     (*colfunc) (void);
#ifdef HORIZONTALDRAW
extern void     (*hcolfunc) (void);    //Fab 17-06-98
#endif
extern void     (*basecolfunc) (void);
extern void     (*fuzzcolfunc) (void);
extern void     (*transcolfunc) (void);
extern void     (*shadecolfunc) (void);
extern void     (*spanfunc) (void);
extern void     (*basespanfunc) (void);
extern void     (*transtransfunc) (void); // SSNTails 11-11-2002


// ----------------
// screen variables
// ----------------
extern viddef_t vid;
extern int      setmodeneeded;     // mode number to set if needed, or 0

extern boolean  fuzzymode;


extern int      scr_bpp;
extern byte*    scr_borderpatch;   // patch used to fill the view borders

extern consvar_t cv_scr_width;
extern consvar_t cv_scr_height;
extern consvar_t cv_scr_depth;
extern consvar_t cv_fullscreen;
// wait for page flipping to end or not
extern consvar_t cv_vidwait;

// quick fix for tall/short skies, depending on bytesperpixel
extern void (*skydrawerfunc[2]) (void);

// from vid_vesa.c : user config video mode decided at VID_Init ();
extern int      vid_modenum;

// Change video mode, only at the start of a refresh.
void SCR_SetMode (void);
// Recalc screen size dependent stuff
void SCR_Recalc (void);
// Check parms once at startup
void SCR_CheckDefaultMode (void);
// Set the mode number which is saved in the config
void SCR_SetDefaultMode (void);

void SCR_Startup (void);

void SCR_ChangeFullscreen (void);

#endif //__SCREEN_H__
