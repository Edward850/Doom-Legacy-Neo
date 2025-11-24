// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_video.c,v 1.12 2002/07/01 19:59:59 metzgermeister Exp $
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
// $Log: i_video.c,v $
// Revision 1.12  2002/07/01 19:59:59  metzgermeister
// *** empty log message ***
//
// Revision 1.11  2001/12/31 16:56:39  metzgermeister
// see Dec 31 log
// .
//
// Revision 1.10  2001/08/20 20:40:42  metzgermeister
// *** empty log message ***
//
// Revision 1.9  2001/05/16 22:33:35  bock
// Initial FreeBSD support.
//
// Revision 1.8  2001/04/28 14:25:03  metzgermeister
// fixed mouse and menu bug
//
// Revision 1.7  2001/04/27 13:32:14  bpereira
// no message
//
// Revision 1.6  2001/03/12 21:03:10  metzgermeister
//   * new symbols for rendererlib added in SDL
//   * console printout fixed for Linux&SDL
//   * Crash fixed in Linux SW renderer initialization
//
// Revision 1.5  2001/03/09 21:53:56  metzgermeister
// *** empty log message ***
//
// Revision 1.4  2001/02/24 13:35:23  bpereira
// no message
//
// Revision 1.3  2001/01/25 22:15:45  bpereira
// added heretic support
//
// Revision 1.2  2000/11/02 19:49:40  bpereira
// no message
//
// Revision 1.1  2000/09/10 10:56:00  metzgermeister
// clean up & made it work again
//
// Revision 1.1  2000/08/21 21:17:32  metzgermeister
// Initial import to CVS
//
//
//
// DESCRIPTION:
//      DOOM graphics stuff for SDL
//
//-----------------------------------------------------------------------------

#include <stdlib.h>

#ifdef FREEBSD
#include <SDL.h>
#else
#include <SDL3/SDL.h>
#endif

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
#include "hardware/hw_main.h"
#include "hardware/hw_drv.h"
#include "console.h"
#include "command.h"
#include "hwsym_sdl.h" // For dynamic referencing of HW rendering functions
//#include "ogl_sdl.h"

void VID_PrepareModeList(void);

// maximum number of windowed modes (see windowedModes[][])
#define MAXWINMODES (6) 

//Hudler: 16/10/99: added for OpenGL gamma correction
RGBA_t  gamma_correction = { 0x7F7F7F7F };
extern consvar_t cv_grgammared;
extern consvar_t cv_grgammagreen;
extern consvar_t cv_grgammablue;

extern consvar_t cv_fullscreen; // for fullscreen support 

static int numVidModes = 0;

static char vidModeName[33][32]; // allow 33 different modes

rendermode_t    rendermode = render_soft;
boolean highcolor = false;

// synchronize page flipping with screen refresh
// unused and for compatibilityy reason 
consvar_t       cv_vidwait = { "vid_wait","1",CV_SAVE,CV_OnOff };

byte graphics_started = 0; // Is used in console.c and screen.c

// To disable fullscreen at startup; is set in VID_PrepareModeList
boolean allow_fullscreen = false;

event_t event;

// SDL vars
static SDL_Window* sdlWindow = NULL;
static SDL_Texture* sdlTexture = NULL;
static SDL_Renderer* sdlRenderer = NULL;
static SDL_Palette* sdlPalette = NULL;
static SDL_Surface* sdlSurface[2] = { NULL, NULL };
static       SDL_Color    localPalette[256];
static       SDL_Rect** modeList = NULL;
static       Uint8        BitsPerPixel;
//const static Uint32       surfaceFlags = SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF;

// first entry in the modelist which is not bigger than 1024x768
static int firstEntry = 0;

// windowed video modes from which to choose from.

static int windowedModes[MAXWINMODES][2] = {
    {1024, 768},
    {800, 600},
    {640, 480},
    {512, 384},
    {400, 300},
    {320, 200} };
//
//  Translates the SDL key into Doom key
//

static int xlatekey(SDL_Keycode sym)
{
    int rc = 0;

    switch (sym)
    {
    case SDLK_LEFT:  rc = KEY_LEFTARROW;     break;
    case SDLK_RIGHT: rc = KEY_RIGHTARROW;    break;
    case SDLK_DOWN:  rc = KEY_DOWNARROW;     break;
    case SDLK_UP:    rc = KEY_UPARROW;       break;

    case SDLK_ESCAPE:   rc = KEY_ESCAPE;        break;
    case SDLK_RETURN:   rc = KEY_ENTER;         break;
    case SDLK_TAB:      rc = KEY_TAB;           break;
    case SDLK_F1:       rc = KEY_F1;            break;
    case SDLK_F2:       rc = KEY_F2;            break;
    case SDLK_F3:       rc = KEY_F3;            break;
    case SDLK_F4:       rc = KEY_F4;            break;
    case SDLK_F5:       rc = KEY_F5;            break;
    case SDLK_F6:       rc = KEY_F6;            break;
    case SDLK_F7:       rc = KEY_F7;            break;
    case SDLK_F8:       rc = KEY_F8;            break;
    case SDLK_F9:       rc = KEY_F9;            break;
    case SDLK_F10:      rc = KEY_F10;           break;
    case SDLK_F11:      rc = KEY_F11;           break;
    case SDLK_F12:      rc = KEY_F12;           break;

    case SDLK_BACKSPACE: rc = KEY_BACKSPACE;    break;
    case SDLK_DELETE:    rc = KEY_DEL;          break;

    case SDLK_PAUSE:     rc = KEY_PAUSE;        break;

    case SDLK_EQUALS:
    case SDLK_PLUS:      rc = KEY_EQUALS;       break;

    case SDLK_MINUS:     rc = KEY_MINUS;        break;

    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        rc = KEY_SHIFT;
        break;

        //case SDLK_XK_Caps_Lock:
        //rc = KEY_CAPSLOCK;
        //break;

    case SDLK_LCTRL:
    case SDLK_RCTRL:
        rc = KEY_CTRL;
        break;

    case SDLK_LALT:
    case SDLK_RALT:
        rc = KEY_ALT;
        break;

    case SDLK_PAGEUP:   rc = KEY_PGUP; break;
    case SDLK_PAGEDOWN: rc = KEY_PGDN; break;
    case SDLK_END:      rc = KEY_END;  break;
    case SDLK_HOME:     rc = KEY_HOME; break;
    case SDLK_INSERT:   rc = KEY_INS;  break;

        //case SDLK_KP0: rc = KEY_KEYPAD0;  break;
        //case SDLK_KP1: rc = KEY_KEYPAD1;  break;
        //case SDLK_KP2: rc = KEY_KEYPAD2;  break;
        //case SDLK_KP3: rc = KEY_KEYPAD3;  break;
        //case SDLK_KP4: rc = KEY_KEYPAD4;  break;
        //case SDLK_KP5: rc = KEY_KEYPAD5;  break;
        //case SDLK_KP6: rc = KEY_KEYPAD6;  break;
        //case SDLK_KP7: rc = KEY_KEYPAD7;  break;
        //case SDLK_KP8: rc = KEY_KEYPAD8;  break;
        //case SDLK_KP9: rc = KEY_KEYPAD9;  break;

    case SDLK_KP_MINUS:  rc = KEY_KPADDEL;  break;
    case SDLK_KP_DIVIDE: rc = KEY_KPADSLASH; break;
    case SDLK_KP_ENTER:  rc = KEY_ENTER;    break;

    default:
        if (sym >= SDLK_SPACE && sym <= SDLK_DELETE)
            rc = sym - SDLK_SPACE + ' ';
        if (sym >= 'A' && sym <= 'Z')
            rc = sym - 'A' + 'a';
        break;
    }

    return rc;
}



//
// I_StartFrame
//
void I_StartFrame(void)
{
    /*if (render_soft == rendermode)
    {
        if (SDL_MUSTLOCK(vidSurface))
        {
            if (SDL_LockSurface(vidSurface) < 0)
                return;
        }
    }*/

    return;
}

#ifdef LJOYSTICK
extern void I_GetJoyEvent();
#endif
#ifdef LMOUSE2
extern void I_GetMouse2Event();
#endif


void I_GetEvent(void)
{
    SDL_Event inputEvent;

#ifdef LJOYSTICK
    I_GetJoyEvent();
#endif
#ifdef LMOUSE2
    I_GetMouse2Event();
#endif

    SDL_PumpEvents(); // FIXME: do we need this?

    while (SDL_PollEvent(&inputEvent))
    {
        switch (inputEvent.type)
        {
        case SDL_EVENT_KEY_DOWN:
            event.type = ev_keydown;
            event.data1 = xlatekey(inputEvent.key.key);
            D_PostEvent(&event);
            break;
        case SDL_EVENT_KEY_UP:
            event.type = ev_keyup;
            event.data1 = xlatekey(inputEvent.key.key);
            D_PostEvent(&event);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (cv_usemouse.value)
            {
				float mouseXRel = inputEvent.motion.xrel * 8.f;
				float mouseYRel = -inputEvent.motion.yrel;
				event.data2 = mouseXRel;
				event.data3 = mouseYRel;
                event.type = ev_mouse;
                event.data1 = 0;

                D_PostEvent(&event);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (cv_usemouse.value)
            {
                event.type = ev_keydown;
                if (inputEvent.button.button == 4)
                {
                    event.data1 = KEY_MOUSEWHEELUP;
                }
                else if (inputEvent.button.button == 5)
                {
                    event.data1 = KEY_MOUSEWHEELDOWN;
                }
                else
                {
                    event.data1 = KEY_MOUSE1 + inputEvent.button.button - 1; // FIXME!
                }
                D_PostEvent(&event);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (cv_usemouse.value)
            {
                event.type = ev_keyup;
                if (inputEvent.button.button == 4 || inputEvent.button.button == 5)
                {
                    //ignore wheel
                }
                else
                {
                    event.data1 = KEY_MOUSE1 + inputEvent.button.button - 1; // FIXME!
                    D_PostEvent(&event);
                }
            }
            break;

        case SDL_EVENT_TERMINATING:
        case SDL_EVENT_QUIT:
            M_QuitResponse('y');
            break;
        default:
            break;

        }
    }
}

static void doGrabMouse(void)
{
    SDL_SetWindowRelativeMouseMode(sdlWindow, true);
}

static void doUngrabMouse(void)
{
    SDL_SetWindowRelativeMouseMode(sdlWindow, false);
}

void I_StartupMouse(void)
{
    SDL_Event inputEvent;

    // remove the mouse event by reading the queue
    SDL_PollEvent(&inputEvent);

    if (cv_usemouse.value)
    {
        doGrabMouse();
    }
    else
    {
        doUngrabMouse();
    }

    return;
}

//
// I_OsPolling
//
void I_OsPolling(void)
{
    if (!graphics_started)
        return;

    I_GetEvent();

    //reset wheel like in win32, I don't understand it but works
    gamekeydown[KEY_MOUSEWHEELUP] = 0;
    gamekeydown[KEY_MOUSEWHEELDOWN] = 0;

    return;
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void)
{
    /* this function intentionally left empty */
}

//
// I_FinishUpdate
//
void I_FinishUpdate(void)
{
    if (render_soft == rendermode)
    {
        if (screens[0] != vid.direct)
        {
            memcpy(vid.direct, screens[0], vid.width * vid.height * vid.bpp);
            //screens[0] = vid.direct;
        }
        SDL_RenderClear(sdlRenderer);
        SDL_BlitSurface(sdlSurface[0], NULL, sdlSurface[1], NULL);
        SDL_UpdateTexture(sdlTexture, NULL, sdlSurface[1]->pixels, sdlSurface[1]->pitch);

		int screenW, screenH;
        SDL_GetWindowSize(sdlWindow, &screenW, &screenH);

		// Adjust aspect ratio of the view to match the video mode
		double origAspect = (double)vid.width / (double)vid.height;
		double newWidth = origAspect * (double)screenH;

        SDL_FRect viewRect = {0,0,0,0};
		viewRect.x = ((double)screenW < newWidth) ? 0 : ((double)screenW - newWidth) / 2;
		viewRect.w = newWidth;
		viewRect.h = (float)screenH;

        SDL_RenderTexture(sdlRenderer, sdlTexture, NULL, &viewRect);
        SDL_RenderPresent(sdlRenderer);
    }
    else
    {
        //OglSdlFinishUpdate(cv_vidwait.value);
    }

    I_GetEvent();

    return;
}


//
// I_ReadScreen
//
void I_ReadScreen(byte* scr)
{
    if (rendermode != render_soft)
        I_Error("I_ReadScreen: called while in non-software mode");

    memcpy(scr, screens[0], vid.width * vid.height * vid.bpp);
}



//
// I_SetPalette
//
void I_SetPalette(RGBA_t* palette)
{
    int i;

    for (i = 0; i < 256; i++)
    {
        localPalette[i].r = palette[i].s.red;
        localPalette[i].g = palette[i].s.green;
        localPalette[i].b = palette[i].s.blue;
    }

    SDL_SetPaletteColors(sdlPalette, localPalette, 0, 256);
    if (!SDL_SetSurfacePalette(sdlSurface[0], sdlPalette))
    {
        CONS_Printf("SDL_SetSurfacePalette failed: %s\n", SDL_GetError());
    }

    return;
}


// return number of fullscreen + X11 modes
int   VID_NumModes(void)
{
    if (cv_fullscreen.value)
        return numVidModes - firstEntry;
    else
        return MAXWINMODES;
}

char* VID_GetModeName(int modeNum)
{
    if (cv_fullscreen.value) { // fullscreen modes
        modeNum += firstEntry;
        if (modeNum >= numVidModes)
            return NULL;

        sprintf(&vidModeName[modeNum][0], "%dx%d",
            modeList[modeNum]->w,
            modeList[modeNum]->h);
    }
    else { // windowed modes
        if (modeNum >= MAXWINMODES)
            return NULL;

        sprintf(&vidModeName[modeNum][0], "win %dx%d",
            windowedModes[modeNum][0],
            windowedModes[modeNum][1]);
    }
    return &vidModeName[modeNum][0];
}

int VID_GetModeForSize(int w, int h) {
    int matchMode, i;

    if (cv_fullscreen.value)
    {
        matchMode = -1;

        for (i = firstEntry; i < numVidModes; i++)
        {
            if (modeList[i]->w == w &&
                modeList[i]->h == h)
            {
                matchMode = i;
                break;
            }
        }
        if (-1 == matchMode) // use smallest mode
        {
            matchMode = numVidModes - 1;
        }
        matchMode -= firstEntry;
    }
    else
    {
        matchMode = -1;

        for (i = 0; i < MAXWINMODES; i++)
        {
            if (windowedModes[i][0] == w &&
                windowedModes[i][1] == h)
            {
                matchMode = i;
                break;
            }
        }

        if (-1 == matchMode) // use smallest mode
        {
            matchMode = MAXWINMODES - 1;
        }
    }

    return matchMode;
}


void VID_PrepareModeList(void)
{
    int i;

    if (cv_fullscreen.value) // only fullscreen needs preparation
    {
        if (-1 != numVidModes)
        {
            for (i = 0; i < numVidModes; i++)
            {
                if (modeList[i]->w <= MAXVIDWIDTH &&
                    modeList[i]->h <= MAXVIDHEIGHT)
                {
                    firstEntry = i;
                    break;
                }
            }
        }
    }

    allow_fullscreen = true;
    return;
}

int VID_SetMode(int modeNum) 
{
    doUngrabMouse();

    SDL_SetWindowFullscreen(sdlWindow, cv_fullscreen.value);

    if (cv_fullscreen.value)
    {
        modeNum += firstEntry;
        vid.width = modeList[modeNum]->w;
        vid.height = modeList[modeNum]->h;
        vid.modenum = modeNum - firstEntry;
    }
    else //(cv_fullscreen.value)
    {
        vid.width = windowedModes[modeNum][0];
        vid.height = windowedModes[modeNum][1];
        vid.modenum = modeNum;
    }

    // Adjust aspect ratio of the view to match 4:3
    double origAspect = 1.3333333333333333;
    double newWidth = origAspect * (double)vid.height;
    vid.width = (int)newWidth;

    if(vid.width < 320)
		vid.width = 320;
	if (vid.height < 200)
		vid.height = 200;

    SDL_SetWindowSize(sdlWindow, vid.width, vid.height);

    if (sdlSurface[0])
    {
        SDL_DestroySurface(sdlSurface[0]);
        SDL_DestroySurface(sdlSurface[1]);
        SDL_DestroyTexture(sdlTexture);
        free(vid.buffer);
    }

    highcolor = false;
    vid.bpp = 1;
    vid.rowbytes = vid.width * vid.bpp;
    vid.recalc = true;
    vid.buffer = malloc(vid.width * vid.height * vid.bpp * NUMSCREENS);
    SDL_PixelFormat format = SDL_GetWindowPixelFormat(sdlWindow);
    sdlSurface[0] = SDL_CreateSurface(vid.width, vid.height, SDL_PIXELFORMAT_INDEX8);
    sdlSurface[1] = SDL_CreateSurface(vid.width, vid.height, format);
    vid.direct = sdlSurface[0]->pixels;
    sdlTexture = SDL_CreateTexture(sdlRenderer, format, SDL_TEXTUREACCESS_STREAMING, vid.width, vid.height);

    I_StartupMouse();
    return 1;
}

static bool checkModeExists(int w, int h)
{
    for (int i = 0; i < numVidModes; i++)
    {
        if(w == modeList[i]->w && h == modeList[i]->h)
			return true;
    }

	return false;
}

void I_StartupGraphics(void)
{
    if (graphics_started)
        return;

    CV_RegisterVar(&cv_vidwait);

    // Initialize Audio as well, otherwise DirectX can not use audio
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
        CONS_Printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return;
    }

	int numDisplays = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&numDisplays);

    int displayCount = 0;
    SDL_DisplayMode** displayModes = SDL_GetFullscreenDisplayModes(displays[0], &displayCount);

    if (displayCount < 0)
    {
		displayCount = 0;
    }

	modeList = malloc(sizeof(SDL_Rect*) * (displayCount + 1));
    if (modeList == NULL)
    {
        CONS_Printf("Couldn't allocate modelist");
        return;
    }
	memset(modeList, 0, sizeof(SDL_Rect*) * (displayCount + 1));
    numVidModes = 0;
    for(int i = 0; i < displayCount; i++) 
    {
        if(checkModeExists(displayModes[i]->w, displayModes[i]->h))
			continue;
        modeList[numVidModes] = malloc(sizeof(SDL_Rect));
        if (modeList[numVidModes] == NULL)
        {
            CONS_Printf("Couldn't allocate modelist");
            return;
        }
        modeList[numVidModes]->w = displayModes[i]->w;
		modeList[numVidModes]->h = displayModes[i]->h;
		numVidModes++;
    }

    BitsPerPixel = 8;

    SDL_CreateWindowAndRenderer("Doom Legacy Neo", 0, 0, cv_fullscreen.value ? SDL_WINDOW_FULLSCREEN : 0, &sdlWindow, &sdlRenderer);
    VID_SetMode(0);

    sdlPalette = SDL_CreatePalette(256);

#if 0
    // Get video info for screen resolutions
    //videoInfo = SDL_GetVideoInfo();
    // even if I set vid.bpp and highscreen properly it does seem to
    // support only 8 bit  ...  strange
    // so lets force 8 bit
    BitsPerPixel = 8;

    // Set color depth; either 1=256pseudocolor or 2=hicolor
    vid.bpp = 1 /*videoInfo->vfmt->BytesPerPixel*/;
    highcolor = (vid.bpp == 2) ? true : false;

    modeList = SDL_ListModes(NULL, SDL_FULLSCREEN | surfaceFlags);

    if (NULL == modeList)
    {
        CONS_Printf("No video modes present\n");
        return;
    }

    numVidModes = 0;
    if (NULL != modeList)
    {
        while (NULL != modeList[numVidModes])
            numVidModes++;
    }
    else
        // should not happen with fullscreen modes
        numVidModes = -1;

    //CONS_Printf("Found %d Video Modes\n", numVidModes);

    // default size for startup


    // Window title
    SDL_WM_SetCaption("Legacy", "Legacy");

    if (M_CheckParm("-opengl"))
    {
        rendermode = render_opengl;
        HWD.pfnInit = hwSym("Init");
        HWD.pfnFinishUpdate = hwSym("FinishUpdate");
        HWD.pfnDraw2DLine = hwSym("Draw2DLine");
        HWD.pfnDrawPolygon = hwSym("DrawPolygon");
        HWD.pfnSetBlend = hwSym("SetBlend");
        HWD.pfnClearBuffer = hwSym("ClearBuffer");
        HWD.pfnSetTexture = hwSym("SetTexture");
        HWD.pfnReadRect = hwSym("ReadRect");
        HWD.pfnGClipRect = hwSym("GClipRect");
        HWD.pfnClearMipMapCache = hwSym("ClearMipMapCache");
        HWD.pfnSetSpecialState = hwSym("SetSpecialState");
        HWD.pfnSetPalette = hwSym("SetPalette");
        HWD.pfnGetTextureUsed = hwSym("GetTextureUsed");

        HWD.pfnDrawMD2 = hwSym("DrawMD2");
        HWD.pfnSetTransform = hwSym("SetTransform");
        HWD.pfnGetRenderVersion = hwSym("GetRenderVersion");

        // check gl renderer lib
        if (HWD.pfnGetRenderVersion() != VERSION)
        {
            I_Error("The version of the renderer doesn't match the version of the executable\nBe sure you have installed Doom Legacy properly.\n");
        }

        vid.width = 640; // hack to make voodoo cards work in 640x480
        vid.height = 480;

        if (!OglSdlSurface(vid.width, vid.height, cv_fullscreen.value))
            rendermode = render_soft;
    }

    if (render_soft == rendermode)
    {
        vidSurface = SDL_SetVideoMode(vid.width, vid.height, BitsPerPixel, surfaceFlags);

        if (NULL == vidSurface)
        {
            CONS_Printf("Could not set vidmode\n");
            return;
        }
        vid.buffer = malloc(vid.width * vid.height * vid.bpp * NUMSCREENS);
        vid.direct = vidSurface->pixels; // FIXME
    }

    SDL_ShowCursor(0);
    doUngrabMouse();
#endif

    graphics_started = 1;

    return;
}

void I_ShutdownGraphics(void)
{
    // was graphics initialized anyway?
    if (!graphics_started)
        return;

    if (render_soft == rendermode)
    {
        //if (NULL != vidSurface)
        {
            //SDL_FreeSurface(vidSurface);
            //vidSurface = NULL;
        }
    }
    else
    {
        //OglSdlShutdown();
    }

    SDL_Quit();
}
