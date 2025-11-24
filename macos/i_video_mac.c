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
// DESCRIPTION:
//      DOOM graphics stuff for Mac
//
//-----------------------------------------------------------------------------

#include <AGL/agl.h>
#include <AGL/gl.h>
#include <AGL/glu.h>
#include <Carbon/Carbon.h>

#include "doomdef.h"
#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "m_menu.h"
#include "d_main.h"
#include "s_sound.h"
#include "g_input.h"
#include "st_stuff.h"
#include "g_game.h"
#include "i_video.h"
#include "z_zone.h"
#include "hw_main.h"
#include "hw_drv.h"
#include "console.h"
#include "command.h"
#include "hwsym_mac.h" // For dynamic referencing of HW rendering functions
#include "r_opengl.h"

void VID_PrepareModeList(void);
int VID_SetMode(int modeNum);

struct modeDescription
{
	int w;
	int h;
	int freq;
};

RGBA_t				gamma_correction = {0x7F7F7F7F};
extern consvar_t	cv_grgammared;
extern consvar_t	cv_grgammagreen;
extern consvar_t	cv_grgammablue;

extern consvar_t	cv_fullscreen; // for fullscreen support 

char			vidModeName[33][32]; // allow 33 different modes

rendermode_t		rendermode=render_opengl;
boolean				highcolor = false;
byte				graphics_started = 0; // Is used in console.c and screen.c
// To disable fullscreen at startup; is set in VID_PrepareModeList
boolean				allow_fullscreen = true;
int menu_height;
consvar_t       cv_vidwait = {"vid_wait","1",CV_SAVE,CV_OnOff};

WindowRef mainWindow = NULL;

struct modeDescription modeList[20];
int numModes;

#define MAXWINMODES 6
// windowed video modes from which to choose from.
static int windowedModes[MAXWINMODES][2] = {
    {320, 200},
    {400, 300},
    {512, 384},
    {640, 480},
    {800, 600},
    {1024, 768}};

void I_UpdateNoBlit(void){}
void I_ReadScreen(byte* scr){}

void OglMacSetPalette(RGBA_t *palette, RGBA_t *gamma)
{
    int i;
    
    for (i=0; i<256; i++) {
        myPaletteData[i].s.red   = MIN((palette->s.red   * gamma->s.red)  /127, 255);
        myPaletteData[i].s.green = MIN((palette->s.green * gamma->s.green)/127, 255);
        myPaletteData[i].s.blue  = MIN((palette->s.blue  * gamma->s.blue) /127, 255);
        myPaletteData[i].s.alpha = 0xff; // opaque
        palette++;
    }
    
    Flush();
}

//
// I_SetPalette
//
void I_SetPalette (RGBA_t* palette)
{
    int i;
    
    for (i=0; i<256; i++) {
        myPaletteData[i].s.red   = MIN((palette->s.red   * cv_grgammared.value)  /127, 255);
        myPaletteData[i].s.green = MIN((palette->s.green * cv_grgammagreen.value)/127, 255);
        myPaletteData[i].s.blue  = MIN((palette->s.blue  * cv_grgammablue.value) /127, 255);
        myPaletteData[i].s.alpha = 0xff; // opaque
        palette++;
    }

    Flush();
    
    return;
}

//------------------------------
//  VID_Pause
//  Used by macConfigureInput
//  Stops fullscreen mode to allow ISp dialog appear
//  newMode - new mode to switch to
//  returns - current mode (should switch back to this)
void VID_Pause(boolean pause)
{
    static int oldMode = -1;

    /*if (pause)
    {
        oldMode = vid.modenum;
        VID_SetMode (3);
    }
    else if (oldMode>0)
	{
        VID_SetMode(oldMode);
		oldMode = -1;
	}*/
}

int   VID_NumModes(void) 
{
	return numModes;
}

//------------------------------
// VID_GetModeName
// Used in the video mode menu
// Puts 'W' or 'F' before the size to
// indicate window/fullscreen
char  *VID_GetModeName(int modeNum) 
{
	sprintf(&vidModeName[modeNum][0], "%ix%i", modeList[modeNum].w, modeList[modeNum].h);

    return &vidModeName[modeNum][0];
}

int VID_GetModeForSize(int w, int h)
{
   int matchMode, i;
    
    matchMode=-1;
    for(i=0; i<MAXWINMODES; i++)
        if(modeList[i].w == w && modeList[i].h == h)
        {
            matchMode = i;
            break;
        }
    if(-1 == matchMode) // use smallest windowed mode (640x480)
        matchMode = 3;
	
    return matchMode;
}


void VID_PrepareModeList(void)
{
    int i;
	
	if(graphics_started)
            return;
    
    for (i=0;i<MAXWINMODES;i++)
    {
        modeList[i].w = windowedModes[i][0];
        modeList[i].h = windowedModes[i][1];
        modeList[i].freq = 0;
    }
    
    numModes = i;
}

void SetDSpMode(int w, int h, boolean enable)
{
	static int lastw, lasth, last_enable = -1;
	
	if (mainWindow)
	{
		DisposeWindow(mainWindow);
		mainWindow = NULL;
	}
	
	if (enable)
	{
		HideCursor();
		HideMenuBar();
	}
	else
	{
		ShowCursor();
		ShowMenuBar();
		DrawMenuBar();
	}
	
	lastw = w;
	lasth = h;
	last_enable = enable;
}

int VID_SetMode(int modeNum)
{
    CONS_Printf("VID_SetMode(%i)\n",modeNum);
    
    if (!graphics_started)
        cv_scr_depth.value = 16;            // quick hack as config hasn't been parsed
											// (don't want to assume 32 bit available)
	if (cv_scr_depth.value<16)
		CV_Set(&cv_scr_depth,"16");			// dont want 8-bit (?)
	
	vid.bpp = 4;
    highcolor = true;
	vid.width = modeList[modeNum].w;
	vid.height = modeList[modeNum].h;
	vid.rowbytes = vid.width * vid.bpp;
	vid.recalc = true;
	vid.modenum = modeNum;
	
	SetDSpMode(modeList[modeNum].w, modeList[modeNum].h, cv_fullscreen.value);
    
    OglMacSurface(&mainWindow, vid.width, vid.height, cv_fullscreen.value);
    
    CONS_Printf("    VID_SetMode done\n");
    
    return modeNum;
}

int GetTextureMemoryUsed(void)
{
    return 0;
}

void I_StartupGraphics(void)
{
    I_StartupMouse();
    
    if(graphics_started)
    	return;
    
    CONS_Printf("I_StartupGraphics...\n");
    
    VID_PrepareModeList();
    
	menu_height = GetMBarHeight();
    
    HWD.pfnInit             = hwSym("Init");
    HWD.pfnFinishUpdate     = hwSym("FinishUpdate");
    HWD.pfnDraw2DLine       = hwSym("Draw2DLine");
    HWD.pfnDrawPolygon      = hwSym("DrawPolygon");
    HWD.pfnSetBlend         = hwSym("SetBlend");
    HWD.pfnClearBuffer      = hwSym("ClearBuffer");
    HWD.pfnSetTexture       = hwSym("SetTexture");
    HWD.pfnReadRect         = hwSym("ReadRect");
    HWD.pfnGClipRect        = hwSym("GClipRect");
    HWD.pfnClearMipMapCache = hwSym("ClearMipMapCache");
    HWD.pfnSetSpecialState  = hwSym("SetSpecialState");
    HWD.pfnSetTransform     = hwSym("SetTransform");
    HWD.pfnDrawMD2			= hwSym("DrawMD2");
    HWD.pfnSetPalette		= OglMacSetPalette;
    HWD.pfnGetTextureUsed   = GetTextureMemoryUsed;
	
	VID_SetMode(3);
    
    textureformatGL = GL_RGBA;
    graphics_started = 1;
	rendermode = render_opengl;
    
    CV_RegisterVar (&cv_vidwait);
    CONS_Printf("\tI_StartupGraphics done\n");
    return;
}

void I_ShutdownGraphics(void)
{
	// was graphics initialized anyway?
    if (!graphics_started)
            return;
    
    CONS_Printf("I_ShutdownGraphics\n");
    OglMacShutdown();
    DisposeWindow(mainWindow);
    ShowCursor();
}