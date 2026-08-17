// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw_light.h,v 1.16 2001/08/27 19:59:35 hurdler Exp $
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
// $Log: hw_light.h,v $
// Revision 1.16  2001/08/27 19:59:35  hurdler
// Fix colormap in heretic + opengl, fixedcolormap and NEWCORONA
//
// Revision 1.15  2001/08/08 20:34:43  hurdler
// Big TANDL update
//
// Revision 1.14  2001/05/01 20:38:34  hurdler
// some fix/hack for the beta release
//
// Revision 1.13  2001/04/28 15:18:46  hurdler
// newcoronas defined again
//
// Revision 1.12  2001/04/16 21:41:39  hurdler
// do not define NEWCORONA by default
//
// Revision 1.11  2001/02/24 13:35:22  bpereira
// no message
//
// Revision 1.10  2001/01/25 18:56:27  bpereira
// no message
//
// Revision 1.9  2000/11/18 15:51:25  bpereira
// no message
//
// Revision 1.8  2000/08/31 14:30:57  bpereira
// no message
//
// Revision 1.7  2000/08/03 17:57:42  bpereira
// no message
//
// Revision 1.6  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.5  2000/04/14 16:34:26  hurdler
// some nice changes for coronas
//
// Revision 1.4  2000/04/12 16:03:51  hurdler
// ready for T&L code and true static lighting
//
// Revision 1.3  2000/03/29 19:39:49  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Dynamic lighting & coronas add on by Hurdler 
//
//-----------------------------------------------------------------------------


#ifndef _HW_LIGHTS_
#define _HW_LIGHTS_

#include "hw_glob.h"
#include "hw_drv.h"
#include "hw_defs.h"

//#define DO_MIRROR
#define NEWCORONAS

#define DL_MAX_LIGHT    256  // maximum number of light (extra light are ignored)

void HWR_InitLight( void );
void HWR_DL_AddLight(gr_vissprite_t *spr, GlidePatch_t *patch);
void HWR_DynamicShadowing(FOutVector *clVerts, int nrClipVerts, player_t *p);
void HWR_PlaneLighting(FOutVector *clVerts, int nrClipVerts);
void HWR_WallLighting(FOutVector *wlVerts);
void HWR_ResetLights(void);
void HWR_SetLights(int viewnumber);

#ifdef NEWCORONAS
void HWR_DrawCoronas( void );
#else
void HWR_DoCoronasLighting(FOutVector *outVerts, gr_vissprite_t *spr);
#endif

typedef struct {
    int         nb;
    light_t    *p_lspr[DL_MAX_LIGHT];
    FVector    position[DL_MAX_LIGHT]; // actually maximum DL_MAX_LIGHT lights
    mobj_t     *mo[DL_MAX_LIGHT];
} dynlights_t;

#endif
