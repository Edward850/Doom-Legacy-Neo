// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: R_d3d_2009.cpp,v 1.2 2001/08/08 20:34:44 hurdler Exp $
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
// $Log: R_d3d_2009.cpp,v $
// Revision 1.2  2001/08/08 20:34:44  hurdler
// Big TANDL update
//
// Revision 1.1  2000/10/01 15:14:32  hurdler
// Completely rewritten d3d driver... absolutely not finished at all
//
//
//
// DESCRIPTION:
//      DirectX 7 (and higher) API for Doom Legacy
//
//-----------------------------------------------------------------------------


#include <windows.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#define D3D_OVERLOADS
#include <d3dx.h>
#include "r_d3d.h"
#pragma warning (disable : 4244)

// ==========================================================================
//                                                                  CONSTANTS
// ==========================================================================

#define NOTEXTURE_NUM     1     // small white texture
#define FIRST_TEX_AVAIL   (NOTEXTURE_NUM + 1)

#define N_PI_DEMI  (1.5707963268f)                  // PI/2

#define ASPECT_RATIO            (1.0f)  //(320.0f/200.0f)
#define FAR_CLIPPING_PLANE      6000.0f
float   NEAR_CLIPPING_PLANE =   0.9f;

#define MIPMAP_MASK     0x0100

// **************************************************************************
//                                                                    GLOBALS
// **************************************************************************

LPDIRECT3DDEVICE7   pD3DDev = NULL;
LPDIRECTDRAW7       pDD     = NULL;
ID3DXContext*       pD3DX   = NULL;

unsigned int    NextTexAvail    = FIRST_TEX_AVAIL;
unsigned int    tex_downloaded  = 0;
float           fov             = 90.0;
unsigned int    pal_col         = 0;
FRGBAFloat      const_pal_col;
FBITFIELD       CurrentPolyFlags;

FTextureInfo*   gr_cachetail = NULL;
FTextureInfo*   gr_cachehead = NULL;

RGBA_t          myPaletteData[256];
int             screen_width;               // used by Draw2DLine()
int             screen_height;
char            screen_depth;

//TODO: use d3d LINEAR constant
#define LINEAR 1

int             min_filter = LINEAR;
int             mag_filter = LINEAR;

static double   modelMatrix[16];
static double   projMatrix[16];
static int      viewport[4]; 

static const float    int2float[256] = {
    0.000000f, 0.003922f, 0.007843f, 0.011765f, 0.015686f, 0.019608f, 0.023529f, 0.027451f,
    0.031373f, 0.035294f, 0.039216f, 0.043137f, 0.047059f, 0.050980f, 0.054902f, 0.058824f,
    0.062745f, 0.066667f, 0.070588f, 0.074510f, 0.078431f, 0.082353f, 0.086275f, 0.090196f,
    0.094118f, 0.098039f, 0.101961f, 0.105882f, 0.109804f, 0.113725f, 0.117647f, 0.121569f,
    0.125490f, 0.129412f, 0.133333f, 0.137255f, 0.141176f, 0.145098f, 0.149020f, 0.152941f,
    0.156863f, 0.160784f, 0.164706f, 0.168627f, 0.172549f, 0.176471f, 0.180392f, 0.184314f,
    0.188235f, 0.192157f, 0.196078f, 0.200000f, 0.203922f, 0.207843f, 0.211765f, 0.215686f,
    0.219608f, 0.223529f, 0.227451f, 0.231373f, 0.235294f, 0.239216f, 0.243137f, 0.247059f,
    0.250980f, 0.254902f, 0.258824f, 0.262745f, 0.266667f, 0.270588f, 0.274510f, 0.278431f,
    0.282353f, 0.286275f, 0.290196f, 0.294118f, 0.298039f, 0.301961f, 0.305882f, 0.309804f,
    0.313726f, 0.317647f, 0.321569f, 0.325490f, 0.329412f, 0.333333f, 0.337255f, 0.341176f,
    0.345098f, 0.349020f, 0.352941f, 0.356863f, 0.360784f, 0.364706f, 0.368627f, 0.372549f,
    0.376471f, 0.380392f, 0.384314f, 0.388235f, 0.392157f, 0.396078f, 0.400000f, 0.403922f,
    0.407843f, 0.411765f, 0.415686f, 0.419608f, 0.423529f, 0.427451f, 0.431373f, 0.435294f,
    0.439216f, 0.443137f, 0.447059f, 0.450980f, 0.454902f, 0.458824f, 0.462745f, 0.466667f,
    0.470588f, 0.474510f, 0.478431f, 0.482353f, 0.486275f, 0.490196f, 0.494118f, 0.498039f,
    0.501961f, 0.505882f, 0.509804f, 0.513726f, 0.517647f, 0.521569f, 0.525490f, 0.529412f,
    0.533333f, 0.537255f, 0.541177f, 0.545098f, 0.549020f, 0.552941f, 0.556863f, 0.560784f,
    0.564706f, 0.568627f, 0.572549f, 0.576471f, 0.580392f, 0.584314f, 0.588235f, 0.592157f,
    0.596078f, 0.600000f, 0.603922f, 0.607843f, 0.611765f, 0.615686f, 0.619608f, 0.623529f,
    0.627451f, 0.631373f, 0.635294f, 0.639216f, 0.643137f, 0.647059f, 0.650980f, 0.654902f,
    0.658824f, 0.662745f, 0.666667f, 0.670588f, 0.674510f, 0.678431f, 0.682353f, 0.686275f,
    0.690196f, 0.694118f, 0.698039f, 0.701961f, 0.705882f, 0.709804f, 0.713726f, 0.717647f,
    0.721569f, 0.725490f, 0.729412f, 0.733333f, 0.737255f, 0.741177f, 0.745098f, 0.749020f,
    0.752941f, 0.756863f, 0.760784f, 0.764706f, 0.768627f, 0.772549f, 0.776471f, 0.780392f,
    0.784314f, 0.788235f, 0.792157f, 0.796078f, 0.800000f, 0.803922f, 0.807843f, 0.811765f,
    0.815686f, 0.819608f, 0.823529f, 0.827451f, 0.831373f, 0.835294f, 0.839216f, 0.843137f,
    0.847059f, 0.850980f, 0.854902f, 0.858824f, 0.862745f, 0.866667f, 0.870588f, 0.874510f,
    0.878431f, 0.882353f, 0.886275f, 0.890196f, 0.894118f, 0.898039f, 0.901961f, 0.905882f,
    0.909804f, 0.913726f, 0.917647f, 0.921569f, 0.925490f, 0.929412f, 0.933333f, 0.937255f,
    0.941177f, 0.945098f, 0.949020f, 0.952941f, 0.956863f, 0.960784f, 0.964706f, 0.968628f,
    0.972549f, 0.976471f, 0.980392f, 0.984314f, 0.988235f, 0.992157f, 0.996078f, 1.000000
};


static I_Error_t I_Error_GL = NULL;

// !!! TEST !!!
D3DLVERTEX vTriangle[256];


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
            logstream = CreateFile ("d3dlog.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
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
// SetRes           : Set a display mode
// Notes            : pcurrentmode is actually not used
// -----------------+
int SetRes( viddef_t *lvid, vmode_t *pcurrentmode )
{
/*
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
    if( hGLRC )
        UnSetRes();

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
    }
    SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, lvid->width, lvid->height,
                  SWP_NOACTIVATE | SWP_NOZORDER );

    if( !hDC )
        hDC = GetDC( hWnd );
    if( !hDC )
    {
        DBG_Printf("GetDC() FAILED\n");
        return 0;
    }

    // Set res.
    if( !SetupPixelFormat( lvid->bpp*8, 0, 16 ) )
        return 0;

    hGLRC = wglCreateContext( hDC );
    if( !hGLRC ) {  DBG_Printf("wglCreateContext() FAILED\n");        return 0;    }

    if( !wglMakeCurrent( hDC, hGLRC ) )
    {
        DBG_Printf("wglMakeCurrent() FAILED\n");
        return 0;
    }


    screen_depth = lvid->bpp*8;
    SetModelView( lvid->width, lvid->height );
    SetStates();
    // we need to clear the depth buffer. Very important!!!
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    lvid->buffer = NULL;    // unless we use the software view
    lvid->direct = NULL;    // direct access to video memory, old DOS crap

    WasFullScreen = WantFullScreen;
*/
    BOOL WantFullScreen = !(lvid->u.windowed);  //(lvid->u.windowed ? 0 : CDS_FULLSCREEN );

    DBG_Printf ("SetMode(): %dx%d %d bits (%s)\n",
                lvid->width, lvid->height, lvid->bpp*8,
                WantFullScreen ? "fullscreen" : "windowed");

    hWnd = lvid->WndParent;

    // BP : why flush texture ?
    //      if important flush also the first one (white texture) and restore it !
    Flush();    // Flush textures.

    D3DXCreateContextEx( D3DX_DEFAULT,       // D3DX handle
                            D3DX_CONTEXT_FULLSCREEN,   // flags
                            hWnd,
                            hWnd,               // hwndFocus
                            D3DX_DEFAULT,       // colorbits
                            D3DX_DEFAULT,       // alphabits
                            D3DX_DEFAULT,       // numdepthbits
                            D3DX_DEFAULT,       // stencilbits
                            D3DX_DEFAULT,       // backbuffers
                            lvid->width,
                            lvid->height,
                            D3DX_DEFAULT,       // refreshrate
                            &pD3DX              // returned D3DX interface
                            );

    pD3DDev = pD3DX->GetD3DDevice();
    pDD = pD3DX->GetDD();

    SetStates();

    pD3DX->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);

    return 1;               // on renvoie une valeur pour dire que cela s'est bien passé
}


// -----------------+
// UnSetRes         : Restore the original display mode
// -----------------+
void UnSetRes( void )
{
/*
    DBG_Printf( "UnSetRes()\n" );

    wglMakeCurrent( hDC, NULL );
    wglDeleteContext( hGLRC );
    hGLRC = NULL;
    if( WasFullScreen )
        ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
*/
}


// -----------------+
// GetModeList      : Return the list of available display modes.
// Returns          : pvidmodes   - points to list of detected OpenGL video modes
//                  : numvidmodes - number of detected OpenGL video modes
// -----------------+
EXPORT void HWRAPI( GetModeList ) ( vmode_t** pvidmodes, int* numvidmodes )
{
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

    int  i;

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

    D3DXUninitialize();

    DBG_Printf ("HWRAPI Shutdown(DONE)\n");
}


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
    pD3DX->UpdateFrame(0);
    pD3DX->Clear(D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET);
}


// -----------------+
// SetPalette       : Set the color lookup table for paletted textures
//                  : in OpenGL, we store values for conversion of paletted graphics when
//                  : they are downloaded to the 3D card.
// -----------------+
EXPORT void HWRAPI( SetPalette ) ( PALETTEENTRY* pal, RGBA_t *gamma )
{
    // DBG_Printf ("SetPalette()\n");
    int i;

    for (i=0; i<256; i++) {
            myPaletteData[i].s.red   = MIN((pal[i].peRed*gamma->s.red)/127,     255);
            myPaletteData[i].s.green = MIN((pal[i].peGreen*gamma->s.green)/127, 255);
            myPaletteData[i].s.blue  = MIN((pal[i].peBlue*gamma->s.blue)/127,   255);
            myPaletteData[i].s.alpha = 0xff; // opaque
    }
    // on a changé de palette, il faut recharger toutes les textures
    Flush();
}


// -----------------+
// DBG_Printf       : Output error messages to debug log if DEBUG_TO_FILE is defined,
//                  : else do nothing
// Returns          :
// -----------------+
void DBG_Printf( LPCTSTR lpFmt, ... )
{
#ifdef DEBUG_TO_FILE
    char    str[1024];
    va_list arglist;
    DWORD   bytesWritten;

    va_start (arglist, lpFmt);
    vsprintf (str, lpFmt, arglist);
    va_end   (arglist);
    if( logstream != INVALID_HANDLE_VALUE )
        WriteFile( logstream, str, lstrlen(str), &bytesWritten, NULL );
#endif
}


// -----------------+
// SetNoTexture     : Disable texture
// -----------------+
static void SetNoTexture( void )
{
/*
    // Set small white texture.
    if( tex_downloaded != NOTEXTURE_NUM )
    {
        glBindTexture( GL_TEXTURE_2D, NOTEXTURE_NUM );
        tex_downloaded = NOTEXTURE_NUM;
    }
*/
}


// -----------------+
// SetModelView     :
// -----------------+
void SetModelView( int w, int h )
{
/*
    DBG_Printf( "SetModelView(): %dx%d\n", w, h );

    screen_width = w;
    screen_height = h;

    glViewport( 0, 0, w, h );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( fov, ASPECT_RATIO, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    //glScalef(1.0f, 320.0f/200.0f, 1.0f);  // gr_scalefrustum (ORIGINAL_ASPECT)

    // added for new coronas' code (without depth buffer)
    glGetIntegerv(GL_VIEWPORT, viewport); 
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
*/
}


// -----------------+
// SetStates        : Set permanent states
// -----------------+
void SetStates( void )
{
    DBG_Printf( "SetStates()\n" );

    pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);
    pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);
    pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
    pD3DDev->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
    pD3DDev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
    pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    pD3DDev->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE );
    pD3DDev->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
    pD3DDev->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);

    pD3DX->SetClearColor(D3DRGBA(0.5f,0.7f,0.5f,0.3f));
/*
    // Bind little white RGBA texture to ID NOTEXTURE_NUM.
    FUINT Data[8*8];
    int i;

    DBG_Printf( "SetStates()\n" );

    // Hurdler: not necessary, is it?
    //glShadeModel( GL_SMOOTH );      // iterate vertice colors
    glShadeModel( GL_FLAT );

    glEnable( GL_TEXTURE_2D );      // two-dimensional texturing
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); //MODULATE );

    glAlphaFunc( GL_NOTEQUAL, 0 );
    glDisable( GL_ALPHA_TEST );     // enable alpha testing
    glBlendFunc( GL_ONE, GL_ZERO ); // copy pixel to frame buffer (opaque)
    glEnable( GL_BLEND );           // enable color blending

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

//    glDisable(GL_DITHER);         // faB: ??? (undocumented in OpenGL 1.1)
                                  // Hurdler: yes, it is!
    glEnable( GL_DEPTH_TEST );    // check the depth buffer
    glDepthMask( 1 );             // enable writing to depth buffer
    glClearDepth( 1.0 );
    glDepthRange( 0.0, 1.0 );
    glDepthFunc(GL_LEQUAL);

    CurrentPolyFlags = PF_Occlude;

    for(i=0; i<64; i++ )
        Data[i] = 0xffFFffFF;       // white pixel

    glBindTexture( GL_TEXTURE_2D, NOTEXTURE_NUM );
    tex_downloaded = NOTEXTURE_NUM;
    glTexImage2D( GL_TEXTURE_2D, 0, 4, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data );

    glPolygonOffset(-1.0, -1.0);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT, GL_LINE);

    //glFogi(GL_FOG_MODE, GL_EXP);
    //glHint(GL_FOG_HINT, GL_NICEST);
    //glFogfv(GL_FOG_COLOR, fogcolor);
    //glFogf(GL_FOG_DENSITY, 0.0005);

    // bp : when no t&l :)
    glLoadIdentity();
    glScalef(1.0, 1.0f, -1.0f);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix); // added for new coronas' code (without depth buffer)
*/
}


// -----------------+
// Flush            : flush OpenGL textures
//                  : Clear list of downloaded mipmaps
// -----------------+
void Flush( void )
{
/*
    //DBG_Printf ("HWR_Flush()\n");

    while( gr_cachehead )
    {
        // ceci n'est pas du tout necessaire vu que tu les a charger normalement et
        // donc il sont dans ta liste !
#if 0
        //Hurdler: 25/04/2000: now support colormap in hardware mode
        FTextureInfo    *tmp = gr_cachehead->nextskin;

        // The memory should be freed in the main code
        while (tmp)
        {
            glDeleteTextures( 1, &tmp->downloaded );
            tmp->downloaded = 0;
            tmp = tmp->nextcolormap;
        }
#endif
        glDeleteTextures( 1, &gr_cachehead->downloaded );
        gr_cachehead->downloaded = 0;
        gr_cachehead = gr_cachehead->nextmipmap;
    }
    gr_cachetail = gr_cachehead = NULL; //Hurdler: well, gr_cachehead is already NULL
    NextTexAvail = FIRST_TEX_AVAIL;
    tex_downloaded = 0;
*/
}


// -----------------+
// Init             : Initialise the OpenGL interface API
// Returns          :
// -----------------+
EXPORT boolean HWRAPI( Init ) (I_Error_t FatalErrorFunction)
{
    I_Error_GL = FatalErrorFunction;
    DBG_Printf (DRIVER_STRING);
    D3DXInitialize();
    return 1;
}


// -----------------+
// ClearMipMapCache : Flush OpenGL textures from memory
// -----------------+
EXPORT void HWRAPI( ClearMipMapCache ) ( void )
{
    // DBG_Printf ("HWR_Flush(exe)\n");
    Flush();
}


// -----------------+
// ReadRect         : Read a rectangle region of the truecolor framebuffer
//                  : store pixels as 16bit 565 RGB
// Returns          : 16bit 565 RGB pixel array stored in dst_data
// -----------------+
EXPORT void HWRAPI( ReadRect ) (int x, int y, int width, int height,
                                int dst_stride, unsigned short * dst_data)
{
/*
    // DBG_Printf ("ReadRect()\n");
    char *image;
    int i, j;

    image = (char *) malloc(width*height*3*sizeof(GLubyte));
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
    for (i=height-1; i>=0; i--)
        for (j=0; j<width; j++)
            dst_data[(height-1-i)*width+j] =
                                  ((image[(i*width+j)*3]>>3)<<11) |
                                  ((image[(i*width+j)*3+1]>>2)<<5) |
                                  ((image[(i*width+j)*3+2]>>3));
    free(image);
*/
}


// -----------------+
// ClipRect         : Defines the 2D hardware clipping window
// -----------------+
EXPORT void HWRAPI( ClipRect ) (int minx, int miny, int maxx, int maxy, float nearclip)
{
/*
    // DBG_Printf ("ClipRect(%d, %d, %d, %d)\n", minx, miny, maxx, maxy);

    glViewport( minx, screen_height-maxy, maxx-minx, maxy-miny );
    NEAR_CLIPPING_PLANE = nearclip;

    //glScissor(minx, screen_height-maxy, maxx-minx, maxy-miny);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // pas ok pour le mode "small window/status bar"
    if ((maxx/(maxy-miny)) >= 2)                // splitscreen
        gluPerspective( 53.13, 2*ASPECT_RATIO,  // 53.13 = 2*atan(0.5)
                        NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);
    else                                        // single
        gluPerspective( fov, ASPECT_RATIO, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);

    glMatrixMode(GL_MODELVIEW);

    // added for new coronas' code (without depth buffer)
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
*/
}


// -----------------+
// ClearBuffer      : Clear the color/alpha/depth buffer(s)
// -----------------+
EXPORT void HWRAPI( ClearBuffer ) ( FBOOLEAN ColorMask,
                                    FBOOLEAN DepthMask,
                                    FRGBAFloat * ClearColor )
{
/*
    // DBG_Printf ("ClearBuffer(%d)\n", alpha);
    FUINT   ClearMask = 0;

    if( ColorMask ) {
        if( ClearColor )
        //glClearColor( ClearColor->red,
        //              ClearColor->green,
        //              ClearColor->blue,
        //              ClearColor->alpha );
        ClearMask |= D3DCLEAR_TARGET;
    }
    if( DepthMask ) {
        //glClearDepth( 1.0 );     //Hurdler: all that are permanen states
        //glDepthRange( 0.0, 1.0 );
        //glDepthFunc( GL_LEQUAL );
        ClearMask |= D3DCLEAR_ZBUFFER;
    }

    SetBlend( DepthMask ? PF_Occlude | CurrentPolyFlags : CurrentPolyFlags&~PF_Occlude );

    //test
    ClearMask |= D3DCLEAR_ZBUFFER;
    ClearMask |= D3DCLEAR_TARGET;
    pD3DX->Clear(ClearMask);
*/
}


// -----------------+
// HWRAPI Draw2DLine: Render a 2D line
// -----------------+
EXPORT void HWRAPI( Draw2DLine ) ( F2DCoord * v1,
                                   F2DCoord * v2,
                                   RGBA_t Color )
{
    // DBG_Printf ("DrawLine() (%f %f %f) %d\n", v1->x, -v1->y, -v1->oow, v1->argb);

    // BP: we should reflect the new state in our variable
    //SetBlend( PF_Modulated|PF_NoTexture );

/*
    glDisable( GL_TEXTURE_2D );
    glColor4fv( (float *)&c );    // is in RGBA float format
    glBegin(GL_LINES);
        glVertex3f(v1->x, -v1->y, 1);
        glVertex3f(v2->x, -v2->y, 1);
    glEnd();
    glEnable( GL_TEXTURE_2D );
*/

    if (pD3DDev->BeginScene()==D3D_OK)
    {
        vTriangle[0].x = v1->x;
        vTriangle[0].y = -v1->y;
        vTriangle[0].z = 1;
        vTriangle[0].color = RGBA_MAKE(Color.s.red, Color.s.green, Color.s.blue, Color.s.alpha);
        vTriangle[0].specular = 0;
        vTriangle[0].tu = 0;
        vTriangle[0].tv = 0;
        vTriangle[1].x = v2->x;
        vTriangle[1].y = -v2->y;
        vTriangle[1].z = 1;
        vTriangle[1].color = RGBA_MAKE(Color.s.red, Color.s.green, Color.s.blue, Color.s.alpha);
        vTriangle[1].specular = 0;
        vTriangle[1].tu = 0;
        vTriangle[1].tv = 0;

        pD3DDev->DrawPrimitive(D3DPT_LINELIST, D3DFVF_LVERTEX, vTriangle, 2, 0);
        pD3DDev->EndScene();
    }
}


// -----------------+
// SetBlend         : Set render mode
// -----------------+
// PF_Masked - we could use an ALPHA_TEST of GL_EQUAL, and alpha ref of 0,
//             is it faster when pixels are discarded ?
EXPORT void HWRAPI( SetBlend ) ( FBITFIELD PolyFlags )
{
/*
    FBITFIELD   Xor = CurrentPolyFlags^PolyFlags;
    if( Xor & ( PF_Blending|PF_Occlude|PF_NoTexture|PF_Modulated|PF_NoDepthTest|PF_Decal|PF_Invisible ) )
    {
        if( Xor&(PF_Blending) ) // if blending mode must be changed
        {
            switch(PolyFlags & PF_Blending) {
                case PF_Translucent :
                     glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); // alpha = level of transparency
                     break;
                case PF_Masked :
                     // Hurdler: does that mean lighting is only made by alpha src?
                     // it sounds ok, but not for polygonsmooth
                     glBlendFunc( GL_SRC_ALPHA, GL_ZERO );                // 0 alpha = holes in texture
                     break;
                case PF_Additive :
                     glBlendFunc( GL_SRC_ALPHA, GL_ONE );                 // src * alpha + dest
                     break;
                case PF_Environment :
                     glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
                     break;
                case PF_Substractive :
                     // not realy but what else ?
                     glBlendFunc( GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
                     break;
                default :
                     // No blending
                     glBlendFunc( GL_ONE, GL_ZERO );   // the same as no blending
                     break;
            }
            if( Xor & PF_AlphaTest)
            {
                if( PolyFlags & PF_AlphaTest)
                    glEnable( GL_ALPHA_TEST );      // discard 0 alpha pixels (holes in texture)
                else
                    glDisable( GL_ALPHA_TEST );
            }
        }
        if( Xor & PF_Decal )
        {
            if( PolyFlags & PF_Decal )
                glEnable(GL_POLYGON_OFFSET_FILL);
            else
                glDisable(GL_POLYGON_OFFSET_FILL);
        }
        if( Xor&PF_NoDepthTest )
        {
            if( PolyFlags & PF_NoDepthTest )
                glDisable( GL_DEPTH_TEST );
            else
                glEnable( GL_DEPTH_TEST );
        }
        if( Xor&PF_Modulated )
        {
            if( PolyFlags & PF_Modulated )
            {   // mix texture colour with Surface->FlatColor
                glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
            }
            else
            {   // colour from texture is unchanged before blending
                glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            }
        }
        if( Xor & PF_Occlude ) // depth test but (no) depth write
        {
            if (PolyFlags&PF_Occlude)
                glDepthMask( GL_TRUE );
            else
                glDepthMask( GL_FALSE );
        }
        if( Xor & PF_Invisible )
        {                     
//            glColorMask( (PolyFlags&PF_Invisible)==0, (PolyFlags&PF_Invisible)==0,
//                         (PolyFlags&PF_Invisible)==0, (PolyFlags&PF_Invisible)==0 );
            
            if (PolyFlags&PF_Invisible)
                glBlendFunc( GL_ZERO, GL_ONE );         // transparent blending
            else
            {   // big hack: (TODO: manage that better)
                // be sure we are in PF_Masked mode which was overwrited
                if (PolyFlags&PF_Masked)
                    glBlendFunc( GL_SRC_ALPHA, GL_ZERO );  
            }
        }
        if( PolyFlags & PF_NoTexture )
        {
            SetNoTexture();
        }
    }
    CurrentPolyFlags = PolyFlags;
*/
}


// -----------------+
// SetTexture       : The mipmap becomes the current texture source
// -----------------+
EXPORT void HWRAPI( SetTexture ) ( FTextureInfo *pTexInfo )
{
/*
    if( pTexInfo->downloaded )
    {
        if (pTexInfo->downloaded != tex_downloaded)
        {
            glBindTexture(GL_TEXTURE_2D, pTexInfo->downloaded);
            tex_downloaded = pTexInfo->downloaded;
        }
    }
    else
    {
        // Download a mipmap
        static RGBA_t   tex[256*256];
        RGBA_t          *ptex = tex;
        int             w, h;

        // DBG_Printf ("DownloadMipmap()\n");
        w = pTexInfo->width;
        h = pTexInfo->height;

        if( (pTexInfo->grInfo.format==GR_TEXFMT_P_8) ||
            (pTexInfo->grInfo.format==GR_TEXFMT_AP_88) )
        {
            char *pImgData;
            int i, j;

            pImgData = (char *)pTexInfo->grInfo.data;
            for( j=0; j<h; j++ )
            {
                for( i=0; i<w; i++)
                {
                    if ( (*pImgData==HWR_PATCHES_CHROMAKEY_COLORINDEX) &&
                         (pTexInfo->flags & TF_CHROMAKEYED) )
                    {
                        tex[w*j+i].s.red   = 0;
                        tex[w*j+i].s.green = 0;
                        tex[w*j+i].s.blue  = 0;
                        tex[w*j+i].s.alpha = 0;
                    }
                    else
                    {
                        tex[w*j+i].s.red   = myPaletteData[*pImgData].s.red;
                        tex[w*j+i].s.green = myPaletteData[*pImgData].s.green;
                        tex[w*j+i].s.blue  = myPaletteData[*pImgData].s.blue;
                        tex[w*j+i].s.alpha = myPaletteData[*pImgData].s.alpha;
                    }

                    pImgData++;

                    if( pTexInfo->grInfo.format == GR_TEXFMT_AP_88 )
                    {
                        if( !(pTexInfo->flags & TF_CHROMAKEYED) )
                            tex[w*j+i].s.alpha = *pImgData;
                        pImgData++;
                    }

                }
            }
        }
        else if (pTexInfo->grInfo.format==46)              //DEF46 HACK
        {
            // corona test : passed as ARGB 8888, which is not in glide formats
            // Hurdler: not used for coronas anymore, just for dynamic lighting
            ptex = (RGBA_t *) pTexInfo->grInfo.data;
        }
        else if (pTexInfo->grInfo.format==GR_TEXFMT_ALPHA_INTENSITY_88)
        {
            char *pImgData;
            int i, j;

            pImgData = (char *)pTexInfo->grInfo.data;
            for( j=0; j<h; j++ )
            {
                for( i=0; i<w; i++)
                {
                    tex[w*j+i].s.red   = *pImgData;
                    tex[w*j+i].s.green = *pImgData;
                    tex[w*j+i].s.blue  = *pImgData;
                    pImgData++;
                    tex[w*j+i].s.alpha = *pImgData;
                    pImgData++;
                }
            }
        }
        else
            DBG_Printf ("SetTexture(bad format) %d\n", pTexInfo->grInfo.format);

        pTexInfo->downloaded = NextTexAvail++;
        tex_downloaded = pTexInfo->downloaded;
        glBindTexture( GL_TEXTURE_2D, pTexInfo->downloaded );

        if (pTexInfo->grInfo.format==46)
        {
            if (min_filter & MIPMAP_MASK)
                gluBuild2DMipmaps( GL_TEXTURE_2D, GL_ALPHA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
            else
                glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
        }
        else if (pTexInfo->grInfo.format==GR_TEXFMT_ALPHA_INTENSITY_88)
        {
            int i, j;

            // hack software pour les bords de la corona
            for (i=0; i<h; i++)
                for (j=0; j<w; j++)
                    if (((i-h/2)*(i-h/2))+((j-w/2)*(j-w/2)) > h*w/4) 
                        tex[w*j+i].s.alpha = 0;

            //glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
            if (min_filter & MIPMAP_MASK)
                gluBuild2DMipmaps( GL_TEXTURE_2D, GL_LUMINANCE_ALPHA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
            else
                glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
        }
        else if (screen_depth > 16)
        {
            if (min_filter & MIPMAP_MASK)
                gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
            else
                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
        }
        else // this is the mode for 16 bits 3dfx's cards
        {
            if (min_filter & MIPMAP_MASK)
                gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB5_A1, w, h, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
            else
                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB5_A1, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptex );
        }
        if( pTexInfo->flags & TF_WRAPX )
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        else
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

        if( pTexInfo->flags & TF_WRAPY )
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        else
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

        pTexInfo->nextmipmap = NULL;
        if (gr_cachetail) { // insertion en fin de liste
            gr_cachetail->nextmipmap = pTexInfo;
            gr_cachetail = pTexInfo;
        }
        else // initialisation de la liste
            gr_cachetail = gr_cachehead =  pTexInfo;
    }
*/
}


// -----------------+
// DrawPolygon      : Render a polygon, set the texture, set render mode
// -----------------+
EXPORT void HWRAPI( DrawPolygon ) ( FSurfaceInfo  *pSurf,
                                    //FTextureInfo  *pTexInfo,
                                    FOutVector    *pOutVerts,
                                    FUINT         iNumPts,
                                    FBITFIELD     PolyFlags )
{
    FUINT i, j;
    FRGBAFloat c;

    if (PolyFlags & PF_Corona) 
        PolyFlags &= ~(PF_NoDepthTest|PF_Corona);

    SetBlend( PolyFlags );    //TODO: inline (#pragma..)

    // If Modulated, mix the surface colour to the texture
    if( (CurrentPolyFlags & PF_Modulated) && pSurf)
    {
        if (pal_col) { // hack for non-palettized mode
            c.red   = (const_pal_col.red  +int2float[pSurf->FlatColor.s.red])  /2.0f;
            c.green = (const_pal_col.green+int2float[pSurf->FlatColor.s.green])/2.0f;
            c.blue  = (const_pal_col.blue +int2float[pSurf->FlatColor.s.blue]) /2.0f;
            c.alpha = int2float[pSurf->FlatColor.s.alpha];
        }
        else
        {
            c.red   = int2float[pSurf->FlatColor.s.red];
            c.green = int2float[pSurf->FlatColor.s.green];
            c.blue  = int2float[pSurf->FlatColor.s.blue];
            c.alpha = int2float[pSurf->FlatColor.s.alpha];
        }
//        glColor4fv( (float *)&c );    // is in RGBA float format
    }

    // this test is added for new coronas' code (without depth buffer)
    // I think I should do a separate function for drawing coronas, so it will be a little faster
/*    if (PolyFlags & PF_Corona) // check to see if we need to draw the corona
    {
        //rem: all 8 (or 8.0f) values are hard coded: it can be changed to a higher value
        float     buf[8][8];
        double    cx, cy, cz;
        double    px, py, pz;
        float     scalef = 0;

        cx = (pOutVerts[0].x + pOutVerts[2].x) / 2.0f; // we should change the coronas' ...
        cy = (pOutVerts[0].y + pOutVerts[2].y) / 2.0f; // ... code so its only done once.
        cz = pOutVerts[0].oow;

        // I dont know if this is slow or not
        gluProject(cx, cy, cz, modelMatrix, projMatrix, viewport, &px, &py, &pz);
        //DBG_Printf("Projection: (%f, %f, %f)\n", px, py, pz);

        if ( (pz <  0.0) ||
             (px < -8.0) ||
             (py < viewport[1]-8.0) ||
             (px > viewport[2]+8.0) ||
             (py > viewport[1]+viewport[3]+8.0))
            return;

        // the damned slow glReadPixels functions :(
        glReadPixels( (int)px-4, (int)py, 8, 8, GL_DEPTH_COMPONENT, GL_FLOAT, buf );
        //DBG_Printf("DepthBuffer: %f %f\n", buf[0][0], buf[3][3]);

        for (i=0; i<8; i++)
            for (j=0; j<8; j++)
                scalef += (pz > buf[i][j]) ? 0 : 1;

        // quick test for screen border (not 100% correct, but looks ok)
        if (px < 4) scalef -= 8*(4-px);
        if (py < viewport[1]+4) scalef -= 8*(viewport[1]+4-py);
        if (px > viewport[2]-4) scalef -= 8*(4-(viewport[2]-px));
        if (py > viewport[1]+viewport[3]-4) scalef -= 8*(4-(viewport[1]+viewport[3]-py));

        scalef /= 64;
        //DBG_Printf("Scale factor: %f\n", scalef);

        if (scalef < 0.05f) // ça sert à rien de tracer la light
            return;

        c.alpha *= scalef; // change the alpha value (it seems better than changing the size of the corona)
        glColor4fv( (float *)&c );
    }*/

    if (pD3DDev->BeginScene()==D3D_OK)
    {
        for (i=0; i<iNumPts; i++)
        {
            vTriangle[i].x = pOutVerts[i].x;
            vTriangle[i].y = pOutVerts[i].y;
            vTriangle[i].z = pOutVerts[i].oow;
            if (pSurf)
                vTriangle[i].color = RGBA_MAKE(pSurf->FlatColor.s.red, pSurf->FlatColor.s.green, pSurf->FlatColor.s.blue, pSurf->FlatColor.s.alpha);
            else
                vTriangle[i].color = 0x66FF0066;
            vTriangle[i].specular = 0;
            vTriangle[i].tu = pOutVerts[i].sow;
            vTriangle[i].tv = pOutVerts[i].tow;
        }
        pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_LVERTEX, vTriangle, iNumPts, 0);
        pD3DDev->EndScene();
    }
}


// ==========================================================================
//
// ==========================================================================
EXPORT void HWRAPI( SetSpecialState ) (hwdspecialstate_t IdState, int Value)
{
    switch (IdState)
    {
        case 77: {
            //08/01/00: Hurdler this is a test for mirror
            if (!Value)
                ClearBuffer( false, true, 0 ); // clear depth buffer
            break;
        }

        case HWD_SET_PALETTECOLOR: {
            pal_col = Value;
            const_pal_col.blue  = int2float[((Value>>16)&0xff)];
            const_pal_col.green = int2float[((Value>>8)&0xff)];
            const_pal_col.red   = int2float[((Value)&0xff)];
            break;
        }

        case HWD_SET_FOG_COLOR: {
            float fogcolor[4];

            fogcolor[0] = int2float[((Value>>16)&0xff)];
            fogcolor[1] = int2float[((Value>>8)&0xff)];
            fogcolor[2] = int2float[((Value)&0xff)];
            fogcolor[3] = 0x0;
//            glFogfv(GL_FOG_COLOR, fogcolor);
            break;
        }
        case HWD_SET_FOG_DENSITY:
//            glFogf(GL_FOG_DENSITY, Value/1000000.0f);
            break;

        case HWD_SET_FOG_MODE:
            if (Value)
            {
//                glEnable(GL_FOG);
            }
            else
//                glDisable(GL_FOG);
            break;

        case HWD_SET_FOV:
/*
            fov = (float)Value;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective( fov, ASPECT_RATIO, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);
            glGetDoublev(GL_PROJECTION_MATRIX, projMatrix); // added for new coronas' code (without depth buffer)
            glMatrixMode(GL_MODELVIEW);
*/
            break;

        case HWD_SET_POLYGON_SMOOTH:
/*
            if (Value)
                glEnable(GL_POLYGON_SMOOTH);
            else
                glDisable(GL_POLYGON_SMOOTH);
*/
            break;

        case HWD_SET_TEXTUREFILTERMODE:
            switch (Value) 
            {
                case HWD_SET_TEXTUREFILTER_TRILINEAR:
//                    min_filter = mag_filter = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                case HWD_SET_TEXTUREFILTER_BILINEAR :
                    min_filter = mag_filter = D3DTFG_LINEAR;
                    break;
                case HWD_SET_TEXTUREFILTER_POINTSAMPLED :
//                    min_filter = mag_filter = GL_NEAREST;
                    break;
                case HWD_SET_TEXTUREFILTER_MIXED1 :
//                    mag_filter = GL_LINEAR;
//                    min_filter = GL_NEAREST;
                    break;
                case HWD_SET_TEXTUREFILTER_MIXED2 :
//                    mag_filter = GL_NEAREST;
//                    min_filter = GL_LINEAR;
                    break;
            }
            Flush(); //??? if we want to change filter mode by texture, remove this

         default:
            break;
    }
}

FTransform  md2_transform;

// -----------------+
// HWRAPI DrawMD2   : Draw an MD2 model with glcommands
// -----------------+
EXPORT void HWRAPI( DrawMD2 ) (int *gl_cmd_buffer, md2_frame_t *frame, FTransform *pos)
{
/*
    int     val, count, index;
    float s, t;

    //TODO: Maybe we can put all this in a display list the first time it's
    //      called and after, use this display list: faster (how much?) but
    //      require more memory (how much?)

    DrawPolygon( NULL, NULL, 0, PF_Masked|PF_Modulated|PF_Occlude|PF_Clip);

    glPushMatrix(); // should be the same as glLoadIdentity
    glLoadIdentity();
    glScalef(1.0, 1.6f, 1.0f);
    glRotatef(md2_transform.anglex, -1.0f, 0.0f, 0.0f);
    glTranslatef(pos->x, pos->y, pos->z);
    glRotatef(pos->angley, 0.0f, 1.0f, 0.0f);

    val = *gl_cmd_buffer++;

    while (val != 0)
    {
        if (val < 0)
        {
            glBegin (GL_TRIANGLE_FAN);
            count = -val;
        }
        else
        {
            glBegin (GL_TRIANGLE_STRIP);
            count = val;
        }

        while (count--)
        {
            s = *(float *) gl_cmd_buffer++;
            t = *(float *) gl_cmd_buffer++;
            index = *gl_cmd_buffer++;

            glTexCoord2f (s, t);
            glVertex3f (frame->vertices[index].vertex[0]/2.0f,
                        frame->vertices[index].vertex[1]/2.0f,
                        frame->vertices[index].vertex[2]/2.0f);
        }

        glEnd ();

        val = *gl_cmd_buffer++;
    }
    glPopMatrix(); // should be the same as glLoadIdentity
*/
}

// -----------------+
// SetTransform     : 
// -----------------+
EXPORT void HWRAPI( SetTransform ) (FTransform *transform)
{
/*
    glLoadIdentity();
    if (transform)
    {
        // keep a trace of the transformation for md2
        memcpy(&md2_transform, transform, sizeof(md2_transform));
        glScalef(1.0, 1.6f, -1.0f);
        glRotatef(transform->anglex       , 1.0, 0.0, 0.0);
        glRotatef(transform->angley+270.0f, 0.0, 1.0, 0.0);
        glTranslatef(-transform->x, -transform->z, -transform->y);
    }
    else
        glScalef(1.0, 1.0f, -1.0f);

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix); // added for new coronas' code (without depth buffer)
*/
}
