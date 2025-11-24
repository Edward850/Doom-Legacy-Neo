// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: screen.c,v 1.13 2002/11/12 00:06:05 ssntails Exp $
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
// $Log: screen.c,v $
// Revision 1.13  2002/11/12 00:06:05  ssntails
// Support for translated translucent columns in software mode.
//
// Revision 1.12  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.11  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.10  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.9  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.8  2000/11/02 19:49:37  bpereira
// no message
//
// Revision 1.7  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.6  2000/08/11 19:10:13  metzgermeister
// *** empty log message ***
//
// Revision 1.5  2000/08/03 17:57:42  bpereira
// no message
//
// Revision 1.4  2000/04/22 20:27:35  metzgermeister
// support for immediate fullscreen switching
//
// Revision 1.3  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      handles multiple resolutions, 8bpp/16bpp(highcolor) modes
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "screen.h"
#include "console.h"
#include "am_map.h"
#include "i_system.h"
#include "i_video.h"
#include "r_local.h"
#include "r_sky.h"
#include "m_argv.h"
#include "v_video.h"
#include "st_stuff.h"
#include "hu_stuff.h"
#include "z_zone.h"
#include "d_main.h"

void VID_PrepareModeList(void); // from i_video_xshm.c

#ifdef DIRECTFULLSCREEN
// allow_fullscreen is set in VID_PrepareModeList
extern boolean allow_fullscreen;
#endif

// --------------------------------------------
// assembly or c drawer routines for 8bpp/16bpp
// --------------------------------------------
void (*skycolfunc) (void);       //new sky column drawer draw posts >128 high
void (*colfunc) (void);          // standard column upto 128 high posts

#ifdef HORIZONTALDRAW
//Fab 17-06-98
void (*hcolfunc) (void);         // horizontal column drawer optimisation
#endif

void (*basecolfunc) (void);
void (*fuzzcolfunc) (void);      // standard fuzzy effect column drawer
void (*transcolfunc) (void);     // translucent column drawer
void (*shadecolfunc) (void);     // smokie test..
void (*spanfunc) (void);         // span drawer, use a 64x64 tile
void (*basespanfunc) (void);     // default span func for color mode

// Tails 11-11-2002
void (*transtransfunc) (void);

// ------------------
// global video state
// ------------------
viddef_t  vid;
int       setmodeneeded;     //video mode change needed if > 0
                             // (the mode number to set + 1)

// TO DO!!! make it a console variable !!
boolean   fuzzymode=false;   // use original Doom fuzzy effect instead
                             // of translucency

CV_PossibleValue_t scr_depth_cons_t[]={{8,"8 bits"}, {16,"16 bits"}, {24,"24 bits"}, {32,"32 bits"}, {0,NULL}};

//added:03-02-98: default screen mode, as loaded/saved in config
consvar_t   cv_scr_width  = {"scr_width",  "320", CV_SAVE, CV_Unsigned};
consvar_t   cv_scr_height = {"scr_height", "200", CV_SAVE, CV_Unsigned};
consvar_t   cv_scr_depth =  {"scr_depth",  "8 bits",   CV_SAVE, scr_depth_cons_t};
consvar_t   cv_fullscreen = {"fullscreen",  "Yes",CV_SAVE | CV_CALL, CV_YesNo, SCR_ChangeFullscreen};

// =========================================================================
//                           SCREEN VARIABLES
// =========================================================================

int       scr_bpp;         // current video mode bytes per pixel
byte*     scr_borderpatch; // flat used to fill the reduced view borders
                           // set at ST_Init ()

// =========================================================================

//added:27-01-98: tell asm code the new rowbytes value.
void ASMCALL ASM_PatchRowBytes(int rowbytes);


//  Set the video mode right now,
//  the video mode change is delayed until the start of the next refresh
//  by setting the setmodeneeded to a value >0
//
int  VID_SetMode(int modenum);

//  Short and Tall sky drawer, for the current color mode
void (*skydrawerfunc[2]) (void);

void SCR_SetMode (void)
{
    if(dedicated)
	return;
    
    if (!setmodeneeded)
        return;                 //should never happen

    VID_SetMode(--setmodeneeded);

    V_SetPalette (0);
        //CONS_Printf ("SCR_SetMode : vid.bpp is %d\n", vid.bpp);

    //
    //  setup the right draw routines for either 8bpp or 16bpp
    //
    if (vid.bpp==1)
    {
        colfunc = basecolfunc = R_DrawColumn_8;
#ifdef HORIZONTALDRAW
        hcolfunc = R_DrawHColumn_8;
#endif

        fuzzcolfunc = (fuzzymode) ? R_DrawFuzzColumn_8 : R_DrawTranslucentColumn_8;
        transcolfunc = R_DrawTranslatedColumn_8;
        shadecolfunc = R_DrawShadeColumn_8;  //R_DrawColumn_8;
        spanfunc = basespanfunc = R_DrawSpan_8;

		// SSNTails 11-11-2002
		transtransfunc = R_DrawTranslatedTranslucentColumn_8;

        // FIXME: quick fix
        skydrawerfunc[0] = R_DrawColumn_8;      //old skies
        skydrawerfunc[1] = R_DrawSkyColumn_8;   //tall sky
    }
    else
    if (vid.bpp>1)
    {
        CONS_Printf ("using highcolor mode\n");

        colfunc = basecolfunc = R_DrawColumn_16;

        fuzzcolfunc = (fuzzymode) ? R_DrawFuzzColumn_16 : R_DrawTranslucentColumn_16;
        transcolfunc = R_DrawTranslatedColumn_16;
                shadecolfunc = NULL;      //detect error if used somewhere..
        spanfunc = basespanfunc = R_DrawSpan_16;

		// No 16bit operation for this function SSNTails 11-11-2002
		transtransfunc = R_DrawTranslucentColumn_16;

        // FIXME: quick fix to think more..
        skydrawerfunc[0] = R_DrawColumn_16;
        skydrawerfunc[1] = R_DrawSkyColumn_16;
    }
    else
        I_Error ("unknown bytes per pixel mode %d\n", vid.bpp);

    // set the apprpriate drawer for the sky (tall or short)

    setmodeneeded = 0;
}


//  do some initial settings for the game loading screen
//
void SCR_Startup (void)
{
    if(dedicated)
	return;
    
    vid.modenum = 0;
    
    vid.fdupx = (float)vid.width/BASEVIDWIDTH;//1.0;
    vid.fdupy = (float)vid.height/BASEVIDHEIGHT; //1.0f;
    vid.dupx = (int)vid.fdupx; //1;
    vid.dupy = (int)vid.fdupy; //1;

    vid.baseratio = FRACUNIT;

    scaledofs = 0;
    vid.centerofs = 0;

#ifdef USEASM
    ASM_PatchRowBytes(vid.width);
#endif

    V_Init();
    CV_RegisterVar (&cv_ticrate);

    V_SetPalette (0);
}


//added:24-01-98:
//
// Called at new frame, if the video mode has changed
//
void SCR_Recalc (void)
{
    if(dedicated)
	return;
    
    // bytes per pixel quick access
    scr_bpp = vid.bpp;

    //added:18-02-98: scale 1,2,3 times in x and y the patches for the
    //                menus and overlays... calculated once and for all
    //                used by routines in v_video.c
    //if ( rendermode == render_soft ) 
    {
        // leave it be 1 in hardware accelerated modes
        vid.dupx = vid.width / BASEVIDWIDTH;
        vid.dupy = vid.height / BASEVIDHEIGHT;
        vid.fdupx = (float)vid.width / BASEVIDWIDTH;
        vid.fdupy = (float)vid.height / BASEVIDHEIGHT;
        vid.baseratio = FixedDiv(vid.height << FRACBITS, BASEVIDHEIGHT << FRACBITS);
    }

    //added:18-02-98: calculate centering offset for the scaled menu
    scaledofs = 0;  //see v_video.c
    vid.centerofs = (((vid.height%BASEVIDHEIGHT)/2) * vid.width) +
                    (vid.width%BASEVIDWIDTH)/2;

    // patch the asm code depending on vid buffer rowbytes
#ifdef USEASM
    ASM_PatchRowBytes(vid.width);
#endif

    // toggle off automap because some screensize-dependent values will
    // be calculated next time the automap is activated.
    if (automapactive)
        AM_Stop();

    // fuzzoffsets for the 'spectre' effect,... this is a quick hack for
    // compatibility, because I don't use it anymore since transparency
    // looks much better.
    R_RecalcFuzzOffsets();

        // r_plane stuff : visplanes, openings, floorclip, ceilingclip, spanstart,
        //                 spanstop, yslope, distscale, cachedheight, cacheddistance,
        //                 cachedxstep, cachedystep
        //              -> allocated at the maximum vidsize, static.

    // r_main : xtoviewangle, allocated at the maximum size.
    // r_things : negonearray, screenheightarray allocated max. size.

    // set the screen[x] ptrs on the new vidbuffers
    V_Init();

    // scr_viewsize doesn't change, neither detailLevel, but the pixels
    // per screenblock is different now, since we've changed resolution.
    R_SetViewSize ();   //just set setsizeneeded true now ..

    // vid.recalc lasts only for the next refresh...
    con_recalc = true;
//    CON_ToggleOff ();  // make sure con height is right for new screen height

    st_recalc = true;
    am_recalc = true;
}


// Check for screen cmd-line parms : to force a resolution.
//
// Set the video mode to set at the 1st display loop (setmodeneeded)
//
int VID_GetModeForSize( int w, int h);  //vid_vesa.c

void SCR_CheckDefaultMode (void)
{
    int p;
    int scr_forcex;     // resolution asked from the cmd-line
    int scr_forcey;

    if(dedicated)
	return;
    
    // 0 means not set at the cmd-line
    scr_forcex = 0;
    scr_forcey = 0;

    p = M_CheckParm("-width");
    if (p && p < myargc-1)
        scr_forcex = atoi(myargv[p+1]);

    p = M_CheckParm("-height");
    if (p && p < myargc-1)
        scr_forcey = atoi(myargv[p+1]);

    if (scr_forcex && scr_forcey)
    {
        CONS_Printf("Using resolution: %d x %d\n",scr_forcex,scr_forcey);
        // returns -1 if not found, thus will be 0 (no mode change) if not found
        setmodeneeded = VID_GetModeForSize(scr_forcex,scr_forcey) + 1;
        //if (scr_forcex!=BASEVIDWIDTH || scr_forcey!=BASEVIDHEIGHT)
    }
    else
    {
        CONS_Printf("Default resolution: %d x %d (%d bits)\n",cv_scr_width.value,cv_scr_height.value,cv_scr_depth.value);
        // see note above
        setmodeneeded = VID_GetModeForSize(cv_scr_width.value,cv_scr_height.value) + 1;
    }
}


//added:03-02-98: sets the modenum as the new default video mode to be saved
//                in the config file
void SCR_SetDefaultMode (void)
{
    // remember the default screen size
    CV_SetValue (&cv_scr_width,  vid.width);
    CV_SetValue (&cv_scr_height, vid.height);
    CV_SetValue (&cv_scr_depth,  vid.bpp*8);
    //    CV_SetValue (&cv_fullscreen, !vid.u.windowed); metzgermeister: unnecessary?
}

// Change fullscreen on/off according to cv_fullscreen

void SCR_ChangeFullscreen (void)
{
#ifdef DIRECTFULLSCREEN
    int modenum;

    // allow_fullscreen is set by VID_PrepareModeList
    // it is used to prevent switching to fullscreen during startup
    if(!allow_fullscreen) return;

    if(graphics_started) {
        VID_PrepareModeList();
        modenum = VID_GetModeForSize(cv_scr_width.value,cv_scr_height.value);
        setmodeneeded = ++modenum;
    }
    return;
#endif
}
