// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: ogl_win.c,v 1.24 2002/09/21 11:10:27 hurdler Exp $
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
// $Log: ogl_win.c,v $
// Revision 1.24  2002/09/21 11:10:27  hurdler
// no message
//
// Revision 1.23  2001/04/18 19:32:27  hurdler
// no message
//
// Revision 1.22  2001/04/18 15:02:25  hurdler
// fix bis
//
// Revision 1.21  2001/04/18 14:35:10  hurdler
// fix pixel format
//
// Revision 1.20  2001/04/08 15:11:06  hurdler
// Boris, are you happy with that :) ?
//
// Revision 1.19  2001/04/01 17:35:07  bpereira
// no message
//
// Revision 1.18  2001/03/09 21:53:56  metzgermeister
// *** empty log message ***
//
// Revision 1.17  2001/02/19 17:41:27  hurdler
// It's better like that
//
// Revision 1.16  2001/02/14 20:59:27  hurdler
// fix texture bug under Linux
//
// Revision 1.15  2000/11/27 17:22:07  hurdler
// fix a small bug with GeForce based cards
//
// Revision 1.14  2000/11/04 16:23:45  bpereira
// no message
//
// Revision 1.13  2000/11/02 19:49:39  bpereira
// no message
//
// Revision 1.12  2000/10/04 16:29:10  hurdler
// Windowed mode looks better now. Still need some work, though
//
// Revision 1.11  2000/09/28 20:57:21  bpereira
// no message
//
// Revision 1.10  2000/09/25 19:29:24  hurdler
// Maintenance modifications
//
// Revision 1.9  2000/08/10 19:58:04  bpereira
// no message
//
// Revision 1.8  2000/08/10 14:19:19  hurdler
// add waitvbl, fix sky problem
//
// Revision 1.7  2000/08/03 17:57:42  bpereira
// no message
//
// Revision 1.6  2000/05/30 18:01:07  kegetys
// Removed the chromakey code from here
//
// Revision 1.5  2000/05/10 17:43:48  kegetys
// Sprites are drawn using PF_Environment
//
// Revision 1.4  2000/04/19 10:54:43  hurdler
// no message
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
//      Windows specific part of the OpenGL API for Doom Legacy
// TODO:
// - check if windowed mode works
// - support different pixel formats
//
//-----------------------------------------------------------------------------


#ifdef __WIN32__

#include <windows.h>
#include <time.h>
#include "r_opengl.h"


// **************************************************************************
//                                                                    GLOBALS
// **************************************************************************

#ifdef DEBUG_TO_FILE
static unsigned long nb_frames=0;
static clock_t my_clock;
HANDLE logstream;
#endif

static  HDC     hDC   = NULL;       // the window's device context
static  HGLRC   hGLRC = NULL;       // the OpenGL rendering context
static  HWND    hWnd  = NULL;
static  BOOL    WasFullScreen = FALSE;

#define MAX_VIDEO_MODES   32
static  vmode_t     video_modes[MAX_VIDEO_MODES];
int     oglflags = 0;

// **************************************************************************
//                                                                  FUNCTIONS
// **************************************************************************

// -----------------+
// APIENTRY DllMain : DLL Entry Point,
//                  : open/close debug log
// Returns          :
// -----------------+
BOOL APIENTRY DllMain( HANDLE hModule,      // handle to DLL module
                       DWORD fdwReason,     // reason for calling function
                       LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
#ifdef DEBUG_TO_FILE
            logstream = INVALID_HANDLE_VALUE;
            logstream = CreateFile ("ogllog.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_WRITE_THROUGH*/, NULL);
            if (logstream == INVALID_HANDLE_VALUE)
                return FALSE;
#endif
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
#ifdef DEBUG_TO_FILE
            if ( logstream != INVALID_HANDLE_VALUE ) {
                CloseHandle ( logstream );
                logstream  = INVALID_HANDLE_VALUE;
            }
#endif
            break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}


// -----------------+
// SetupPixelFormat : Set the device context's pixel format
// Note             : Because we currently use only the desktop's BPP format, all the
//                  : video modes in Doom Legacy OpenGL are of the same BPP, thus the
//                  : PixelFormat is set only once.
//                  : Setting the pixel format more than once on the same window
//                  : doesn't work. (ultimately for different pixel formats, we
//                  : should close the window, and re-create it)
// -----------------+
int SetupPixelFormat( int WantColorBits, int WantStencilBits, int WantDepthBits )
{
    static DWORD iLastPFD = 0;
    int nPixelFormat;
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),  // size
        1,                              // version
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,                  // color type
        32 /*WantColorBits*/,           // cColorBits : prefered color depth
        0, 0,                           // cRedBits, cRedShift
        0, 0,                           // cGreenBits, cGreenShift
        0, 0,                           // cBlueBits, cBlueShift
        0, 0,                           // cAlphaBits, cAlphaShift
        0,                              // cAccumBits
        0, 0, 0, 0,                     // cAccum Red/Green/Blue/Alpha Bits
        WantDepthBits,                  // cDepthBits (0,16,24,32)
        WantStencilBits,                // cStencilBits
        0,                              // cAuxBuffers
        PFD_MAIN_PLANE,                 // iLayerType
        0,                              // reserved, must be zero
        0, 0, 0,                        // dwLayerMask, dwVisibleMask, dwDamageMask
    };

    DWORD iPFD = (WantColorBits<<16) | (WantStencilBits<<8) | WantDepthBits;

    if( iLastPFD )
    {
        DBG_Printf( "WARNING : SetPixelFormat() called twise not supported by all drivers !\n" );
    }

    // set the pixel format only if different than the current
    if( iPFD == iLastPFD )
        return 2;
    else
        iLastPFD = iPFD;

    DBG_Printf( "SetupPixelFormat() - %d ColorBits - %d StencilBits - %d DepthBits\n",
                WantColorBits, WantStencilBits, WantDepthBits );


    nPixelFormat = ChoosePixelFormat( hDC, &pfd );

    if( nPixelFormat==0 )
        DBG_Printf( "ChoosePixelFormat() FAILED\n" );

    if( SetPixelFormat( hDC, nPixelFormat, &pfd ) == 0 )
    {
        DBG_Printf( "SetPixelFormat() FAILED\n" );
        return 0;
    }

    return 1;
}


// -----------------+
// SetRes           : Set a display mode
// Notes            : pcurrentmode is actually not used
// -----------------+
int SetRes( viddef_t *lvid, vmode_t *pcurrentmode )
{
    char *renderer;
    BOOL WantFullScreen = !(lvid->u.windowed);  //(lvid->u.windowed ? 0 : CDS_FULLSCREEN );

    DBG_Printf ("SetMode(): %dx%d %d bits (%s)\n",
                lvid->width, lvid->height, lvid->bpp*8,
                WantFullScreen ? "fullscreen" : "windowed");

    hWnd = lvid->WndParent;

    // BP : why flush texture ?
    //      if important flush also the first one (white texture) and restore it !
    Flush();    // Flush textures.

// TODO: if not fullscreen, skip display stuff and just resize viewport stuff ...

    // Exit previous mode
    //if( hGLRC ) //Hurdler: TODO: check if this is valid
    //    UnSetRes();

        // Change display settings.
    if( WantFullScreen )
    {
        DEVMODE dm;
        ZeroMemory( &dm, sizeof(dm) );
        dm.dmSize       = sizeof(dm);
        dm.dmPelsWidth  = lvid->width;
        dm.dmPelsHeight = lvid->height;
        dm.dmBitsPerPel = lvid->bpp*8;
        dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
        if( ChangeDisplaySettings( &dm, CDS_TEST ) != DISP_CHANGE_SUCCESSFUL )
            return 0;
        if( ChangeDisplaySettings( &dm, CDS_FULLSCREEN ) !=DISP_CHANGE_SUCCESSFUL )
            return 0;

        SetWindowLong( hWnd, GWL_STYLE, WS_POPUP|WS_VISIBLE );
        // faB : book says to put these, surely for windowed mode
        //WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
        SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, lvid->width, lvid->height,
                      SWP_NOACTIVATE | SWP_NOZORDER );
    }
    else // TODO: get right titlebar height / window border size
        SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, lvid->width+4, lvid->height+24,
                      SWP_NOACTIVATE | SWP_NOZORDER );

    if( !hDC )
        hDC = GetDC( hWnd );
    if( !hDC )
    {
        DBG_Printf("GetDC() FAILED\n");
        return 0;
    }

    {
        int res;
    
        // Set res.
        res = SetupPixelFormat( lvid->bpp*8, 0, 16 );
        if( res==0 )
           return 0;
        else if ( res==1 ) 
        {
            // Exit previous mode
            if( hGLRC )
                UnSetRes();
            hGLRC = wglCreateContext( hDC );
            if( !hGLRC ) 
            {  
                DBG_Printf("wglCreateContext() FAILED\n"); 
                return 0;
            }
            if( !wglMakeCurrent( hDC, hGLRC ) )
            {
                DBG_Printf("wglMakeCurrent() FAILED\n");
                return 0;
            }
        }
    }

    gl_extensions = glGetString(GL_EXTENSIONS);
    // Get info and extensions.
    //BP: why don't we make it earlier ?
    //Hurdler: we cannot do that before intialising gl context
    renderer = strdup(glGetString(GL_RENDERER));
    DBG_Printf("Vendor     : %s\n", glGetString(GL_VENDOR) );
    DBG_Printf("Renderer   : %s\n", renderer );
    DBG_Printf("Version    : %s\n", glGetString(GL_VERSION) );
    DBG_Printf("Extensions : %s\n", gl_extensions );

    // BP: disable advenced feature that don't work on somes hardware
    // Hurdler: Now works on G400 with bios 1.6 and certified drivers 6.04
    if( strstr(renderer, "810" ) )   oglflags |= GLF_NOZBUFREAD;
    free(renderer);
    DBG_Printf("oglflags   : 0x%X\n", oglflags );

#ifdef USE_PALETTED_TEXTURE
    usePalettedTexture = isExtAvailable("GL_EXT_paletted_texture");
    if( usePalettedTexture )
    {
        glColorTableEXT=(PFNGLCOLORTABLEEXTPROC)wglGetProcAddress("glColorTableEXT");
        if (glColorTableEXT==NULL)
            usePalettedTexture = 0;
    }
#endif

    screen_depth = lvid->bpp*8;
    if( screen_depth > 16)
        textureformatGL = GL_RGBA;
    else
        textureformatGL = GL_RGB5_A1;

    SetModelView( lvid->width, lvid->height );
    SetStates();
    // we need to clear the depth buffer. Very important!!!
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    lvid->buffer = NULL;    // unless we use the software view
    lvid->direct = NULL;    // direct access to video memory, old DOS crap

    WasFullScreen = WantFullScreen;

    return 1;               // on renvoie une valeur pour dire que cela s'est bien passé
}


// -----------------+
// UnSetRes         : Restore the original display mode
// -----------------+
void UnSetRes( void )
{
    DBG_Printf( "UnSetRes()\n" );

    wglMakeCurrent( hDC, NULL );
    wglDeleteContext( hGLRC );
    hGLRC = NULL;
    if( WasFullScreen )
        ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
}


// -----------------+
// GetModeList      : Return the list of available display modes.
// Returns          : pvidmodes   - points to list of detected OpenGL video modes
//                  : numvidmodes - number of detected OpenGL video modes
// -----------------+
EXPORT void HWRAPI( GetModeList ) ( vmode_t** pvidmodes, int* numvidmodes )
{
    int  i;

/*
    faB test code

    Commented out because there might be a problem with combo (Voodoo2 + other card),
    we need to get the 3D card's display modes only.

    (*pvidmodes) = &video_modes[0];

    // Get list of device modes
    for( i=0,iMode=0; iMode<MAX_VIDEO_MODES; i++ )
        {
                DEVMODE Tmp;
                memset( &Tmp, 0, sizeof(Tmp) );
                Tmp.dmSize = sizeof( Tmp );
                if( !EnumDisplaySettings( NULL, i, &Tmp ) )
                        break;

        // add video mode
        if( Tmp.dmBitsPerPel==16 )
        {
            video_modes[iMode].pnext = &video_modes[iMode+1];
            video_modes[iMode].windowed = 0;                    // fullscreen is the default
            video_modes[iMode].misc = 0;
            video_modes[iMode].name = (char *)malloc(12);
            sprintf(video_modes[iMode].name, "%dx%d", Tmp.dmPelsWidth, Tmp.dmPelsHeight);
            video_modes[iMode].width = Tmp.dmPelsWidth;
            video_modes[iMode].height = Tmp.dmPelsHeight;
            video_modes[iMode].bytesperpixel = Tmp.dmBitsPerPel/8;
            video_modes[iMode].rowbytes = Tmp.dmPelsWidth * video_modes[iMode].bytesperpixel;
            video_modes[iMode].pextradata = NULL;
            video_modes[iMode].setmode = SetMode;
            iMode++;
        }

        DBG_Printf ("Mode %d : %s %dBPP Freq: %d\n", i, video_modes[i].name, Tmp.dmBitsPerPel, Tmp.dmDisplayFrequency);
        }
    (*numvidmodes) = iMode;
*/

    // classic video modes (fullscreen/windowed)
    int res[][2] = {
                    {320, 200}, {320, 240}, {400, 300}, {512, 384},
                    {640, 400}, {640, 480}, {800, 600}, {1024, 768} };
                   // {1280, 1024} ARGH.. doesnt work in Legacy
                   //};

    HDC bpphdc;
    int iBitsPerPel;

    DBG_Printf ("HWRAPI GetModeList()\n");

    bpphdc = GetDC(NULL); // on obtient le bpp actuel
    iBitsPerPel = GetDeviceCaps( bpphdc, BITSPIXEL );

    ReleaseDC( NULL, bpphdc );

    (*pvidmodes) = &video_modes[0];
    (*numvidmodes) = sizeof(res) / sizeof(res[0]);
    for( i=0; i<(*numvidmodes); i++ )
    {
        video_modes[i].pnext = &video_modes[i+1];
        video_modes[i].windowed = 0; // fullscreen is the default
        video_modes[i].misc = 0;
        video_modes[i].name = (char *)malloc(12);
        sprintf(video_modes[i].name, "%dx%d", res[i][0], res[i][1]);
        DBG_Printf ("Mode: %s\n", video_modes[i].name);
        video_modes[i].width = res[i][0];
        video_modes[i].height = res[i][1];
        video_modes[i].bytesperpixel = iBitsPerPel/8;
        video_modes[i].rowbytes = res[i][0] * video_modes[i].bytesperpixel;
        video_modes[i].pextradata = NULL;
        video_modes[i].setmode = SetRes;
    }

    video_modes[(*numvidmodes)-1].pnext = NULL;
}


// -----------------+
// Shutdown         : Shutdown OpenGL, restore the display mode
// -----------------+
EXPORT void HWRAPI( Shutdown ) ( void )
{
#ifdef DEBUG_TO_FILE
    long nb_centiemes;

    DBG_Printf ("HWRAPI Shutdown()\n");
    nb_centiemes = ((clock()-my_clock)*100)/CLOCKS_PER_SEC;
    DBG_Printf("Nb frames: %li ;  Nb sec: %2.2f  ->  %2.1f fps\n",
                    nb_frames, nb_centiemes/100.0f, (100*nb_frames)/(double)nb_centiemes);
#endif

    Flush();

    // Exit previous mode
    if( hGLRC )
        UnSetRes();

    if( hDC )
    {
        ReleaseDC( hWnd, hDC );
        hDC = NULL;
    }

    DBG_Printf ("HWRAPI Shutdown(DONE)\n");
}

//extern int num_drawn_poly;

// -----------------+
// FinishUpdate     : Swap front and back buffers
// -----------------+
EXPORT void HWRAPI( FinishUpdate ) ( int waitvbl )
{
    // DBG_Printf ("FinishUpdate()\n");
#ifdef DEBUG_TO_FILE
    if( (++nb_frames)==2 )  // on ne commence pas à la première frame
        my_clock = clock();
#endif
    // TODO: implement waitvbl
    //DBG_Printf ("num_drawn_poly: %d\n", num_drawn_poly);
    //num_drawn_poly=0;

    SwapBuffers( hDC );
}


// -----------------+
// SetPalette       : Set the color lookup table for paletted textures
//                  : in OpenGL, we store values for conversion of paletted graphics when
//                  : they are downloaded to the 3D card.
// -----------------+
EXPORT void HWRAPI( SetPalette ) ( RGBA_t* pal, RGBA_t *gamma )
{
    int i;

    for (i=0; i<256; i++) {
        myPaletteData[i].s.red   = MIN((pal[i].s.red*gamma->s.red)/127,     255);
        myPaletteData[i].s.green = MIN((pal[i].s.green*gamma->s.green)/127, 255);
        myPaletteData[i].s.blue  = MIN((pal[i].s.blue*gamma->s.blue)/127,   255);
        myPaletteData[i].s.alpha = pal[i].s.alpha; 
    }
#ifdef USE_PALETTED_TEXTURE
    if (usePalettedTexture)
    {
        for (i=0; i<256; i++)
        {
            palette_tex[3*i+0] = pal[i].s.red;
            palette_tex[3*i+1] = pal[i].s.green;
            palette_tex[3*i+2] = pal[i].s.blue;
        }
        glColorTableEXT(GL_TEXTURE_2D, GL_RGB8, 256, GL_RGB, GL_UNSIGNED_BYTE, palette_tex);
    }
#endif
    // on a changé de palette, il faut recharger toutes les textures
    Flush();
}

#endif
