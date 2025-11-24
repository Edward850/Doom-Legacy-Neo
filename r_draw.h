// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_draw.h,v 1.9 2003/06/10 23:36:09 ssntails Exp $
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
// $Log: r_draw.h,v $
// Revision 1.9  2003/06/10 23:36:09  ssntails
// Variable flat support (32x32 to 2048x2048)
//
// Revision 1.8  2002/11/12 00:06:05  ssntails
// Support for translated translucent columns in software mode.
//
// Revision 1.7  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.6  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.5  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.4  2000/11/09 17:56:20  stroggonmeth
// Hopefully fixed a few bugs and did a few optimizations.
//
// Revision 1.3  2000/11/02 17:50:09  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      low level span/column drawer functions.
//
//-----------------------------------------------------------------------------


#ifndef __R_DRAW__
#define __R_DRAW__

#include "r_defs.h"

// -------------------------------
// COMMON STUFF FOR 8bpp AND 16bpp
// -------------------------------
extern byte*            ylookup[MAXVIDHEIGHT];
extern byte*            ylookup1[MAXVIDHEIGHT];
extern byte*            ylookup2[MAXVIDHEIGHT];
extern int              columnofs[MAXVIDWIDTH];

#ifdef HORIZONTALDRAW
//Fab 17-06-98
extern byte*            yhlookup[MAXVIDWIDTH];
extern int              hcolumnofs[MAXVIDHEIGHT];
#endif

// -------------------------
// COLUMN DRAWING CODE STUFF
// -------------------------

extern lighttable_t*    dc_colormap;
extern lighttable_t*    dc_wcolormap;   //added:24-02-98:WATER!
extern int              dc_x;
extern int              dc_yl;
extern int              dc_yh;
extern int              dc_yw;          //added:24-02-98:WATER!
extern fixed_t          dc_iscale;
extern fixed_t          dc_texturemid;

extern byte*            dc_source;      // first pixel in a column

// translucency stuff here
extern byte*            transtables;    // translucency tables, should be (*transtables)[5][256][256]
extern byte*            dc_transmap;

// Variable flat sizes SSNTails 06-10-2003
extern int flatsize;
extern int flatmask;
extern int flatsubtract;

// translation stuff here

extern byte*            translationtables;
extern byte*            dc_translation;

extern struct r_lightlist_s*      dc_lightlist;
extern int                        dc_numlights;
extern int                        dc_maxlights;

//Fix TUTIFRUTI
extern int      dc_texheight;


// -----------------------
// SPAN DRAWING CODE STUFF
// -----------------------

extern int              ds_y;
extern int              ds_x1;
extern int              ds_x2;

extern lighttable_t*    ds_colormap;

extern fixed_t          ds_xfrac;
extern fixed_t          ds_yfrac;
extern fixed_t          ds_xstep;
extern fixed_t          ds_ystep;

extern byte*            ds_source;      // start of a 64*64 tile image
extern byte*            ds_transmap;


// viewborder patches lump numbers
#define BRDR_T      0
#define BRDR_B      1
#define BRDR_L      2
#define BRDR_R      3
#define BRDR_TL     4
#define BRDR_TR     5
#define BRDR_BL     6
#define BRDR_BR     7

extern int viewborderlump[8];

// ------------------------------------------------
// r_draw.c COMMON ROUTINES FOR BOTH 8bpp and 16bpp
// ------------------------------------------------

//added:26-01-98: called by SCR_Recalc() when video mode changes
void    R_RecalcFuzzOffsets (void);
// Initialize color translation tables, for player rendering etc.
void    R_InitTranslationTables (void);

void    R_InitViewBuffer ( int   width,
                           int   height );

void    R_InitViewBorder (void);

void    R_VideoErase ( unsigned      ofs,
                       int           count );

// Rendering function.
void    R_FillBackScreen (void);

// If the view size is not full screen, draws a border around it.
void    R_DrawViewBorder (void);


// -----------------
// 8bpp DRAWING CODE
// -----------------

#ifdef HORIZONTALDRAW
//Fab 17-06-98
void    R_DrawHColumn_8 (void);
#endif

extern void    ASMCALL R_DrawColumn_8 (void);
extern void    ASMCALL R_DrawSkyColumn_8 (void);
extern void    ASMCALL R_DrawShadeColumn_8 (void);             //smokie test..
extern void    ASMCALL R_DrawFuzzColumn_8 (void);
extern void    ASMCALL R_DrawTranslucentColumn_8 (void);
extern void    ASMCALL R_DrawTranslatedColumn_8 (void);
extern void    ASMCALL R_DrawSpan_8 (void);

// SSNTails 11-11-2002
void    R_DrawTranslatedTranslucentColumn_8 (void);

void    R_DrawTranslucentSpan_8 (void);
void    R_DrawFogSpan_8 (void);
void    R_DrawFogColumn_8 (void); //SoM: Test
void    R_DrawColumnShadowed_8 (void);
void    R_DrawPortalColumn_8 (void);

// ------------------
// 16bpp DRAWING CODE
// ------------------

extern void    ASMCALL R_DrawColumn_16 (void);
extern void    ASMCALL R_DrawSkyColumn_16 (void);
extern void    ASMCALL R_DrawFuzzColumn_16 (void);
extern void    ASMCALL R_DrawTranslucentColumn_16 (void);
extern void    ASMCALL R_DrawTranslatedColumn_16 (void);
extern void    ASMCALL R_DrawSpan_16 (void);


// =========================================================================
#endif  // __R_DRAW__
