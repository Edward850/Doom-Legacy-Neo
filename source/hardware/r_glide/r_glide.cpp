// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_glide.c,v 1.26 2001/08/26 15:27:30 bpereira Exp $
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
// $Log: r_glide.c,v $
// Revision 1.26  2001/08/26 15:27:30  bpereira
// added fov for glide and fixed newcoronas code
//
// Revision 1.25  2001/08/20 18:34:20  bpereira
// glide ligthing and map30 bug
//
// Revision 1.24  2001/08/19 15:40:07  bpereira
// added Treansform (and lighting) to glide
//
// Revision 1.23  2001/07/28 16:18:39  bpereira
// no message
//
// Revision 1.22  2001/02/24 13:35:22  bpereira
// no message
//
// Revision 1.21  2001/01/25 18:56:28  bpereira
// no message
//
// Revision 1.20  2001/01/05 18:19:13  hurdler
// add renderer version checking
//
// Revision 1.19  2000/11/04 16:23:45  bpereira
// no message
//
// Revision 1.18  2000/10/08 13:30:02  bpereira
// no message
//
// Revision 1.17  2000/10/04 16:29:57  hurdler
// Implement hardware texture memory stats (TODO in glide mode)
//
// Revision 1.16  2000/09/28 20:57:21  bpereira
// no message
//
// Revision 1.15  2000/08/31 14:30:57  bpereira
// no message
//
// Revision 1.14  2000/08/10 14:17:58  hurdler
// add waitvbl
//
// Revision 1.13  2000/08/03 17:57:42  bpereira
// no message
//
// Revision 1.12  2000/07/01 09:23:50  bpereira
// no message
//
// Revision 1.11  2000/05/09 20:50:57  hurdler
// remove warning
//
// Revision 1.10  2000/05/05 18:00:06  bpereira
// no message
//
// Revision 1.9  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.8  2000/04/23 16:19:52  bpereira
// no message
//
// Revision 1.7  2000/04/18 12:50:55  hurdler
// join with Boris' code
//
// Revision 1.5  2000/04/14 16:38:24  hurdler
// some nice changes for coronas
//
// Revision 1.4  2000/03/06 15:26:17  hurdler
// change version number
//
// Revision 1.3  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      3Dfx Glide Render driver
//
//-----------------------------------------------------------------------------


#include <windows.h>
#include <glide.h>
#include <math.h>

#define  _CREATE_DLL_
#include "../hw_drv.h"

#include "../../screen.h"
#include "3dmath.h"

#undef DEBUG_TO_FILE
#define DEBUG_TO_FILE         //output debugging msgs to r_glide.log


// **************************************************************************
//                                                                     PROTOS
// **************************************************************************

// output all debugging messages to this file
#ifdef DEBUG_TO_FILE
static HANDLE  logstream;
#endif

static void GR_ResetStates(viddef_t *lvid);
static void GR_InitMipmapCache(void);
static void GR_ClearMipmapCache(void);

static void MakeFogTable(void);
static void FreeFogTable(void);
static void ReSetSpecialState(void);

// **************************************************************************
//                                                                    GLOBALS
// **************************************************************************

//0 means glide mode not set yet, this is returned by grSstWinOpen()
static GrContext_t grPreviousContext = 0;

// align boundary for textures in texture cache, set at Init()
static FxU32 gr_alignboundary;

static FBITFIELD CurrentPolyFlags;
static FBITFIELD CurrentTextureFlags;

static FOutVector tmpVerts[MAXCLIPVERTS*2];
static FOutVector tmp2Verts[MAXCLIPVERTS*2];
static FOutVector tmp3Verts[MAXCLIPVERTS*2];

static FTransform grTransform;
static FTransform defaulttransform = {0,0,0,0,90,1,1,1,90,90};
static float szsinx, szcosx, siny, cosy, sxsiny, sxcosy, sysinx, sycosx;

static I_Error_t I_ErrorGr = NULL;
static glide_initialized = false;
static int glide_state[HWD_NUMSTATE];

// **************************************************************************
//                                                            DLL ENTRY POINT
// **************************************************************************
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
            logstream = CreateFile ("r_glide.log", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
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


// ----------
// DBG_Printf
// Output error messages to debug log if DEBUG_TO_FILE is defined,
// else do nothing
// ----------
void DBG_Printf (LPCTSTR lpFmt, ...)
{
#ifdef DEBUG_TO_FILE
    char    str[1999];
    va_list arglist;
    DWORD   bytesWritten;

    va_start  (arglist, lpFmt);
    vsprintf (str, lpFmt, arglist);
    va_end    (arglist);

    if ( logstream != INVALID_HANDLE_VALUE )
        WriteFile (logstream, str, lstrlen(str), &bytesWritten, NULL);
#endif
}


// error callback function
void GrErrorCallbackFnc (const char *string, FxBool fatal)
{
    DBG_Printf ("Glide error(%d) : %s\n",string,fatal);
    if( fatal && I_ErrorGr )
        I_ErrorGr ("Glide error : %s",string);
}

// ==========================================================================
// Initialise
// ==========================================================================
EXPORT BOOL HWRAPI( Init ) (I_Error_t FatalErrorFunction)
{
    FxU32 numboards;
    FxI32 fxret;

    I_ErrorGr = FatalErrorFunction;
    DBG_Printf ("HWRAPI Init(): 3Dfx Glide Render driver for Doom Legacy v1.31\n");

    // check for Voodoo card
    // - the ONLY possible call before GlideInit
    grGet (GR_NUM_BOARDS,4,&numboards);
    DBG_Printf ("Num 3Dfx boards : %d\n", numboards);
    if (!numboards) {
        I_ErrorGr ("3dfx chipset not detected\n");
        return FALSE;
    }

    // init
    grGlideInit();
    grErrorSetCallback(GrErrorCallbackFnc);

    // select subsystem
    grSstSelect( 0 );

    DBG_Printf( "GR_VENDOR: %s\n", grGetString(GR_VENDOR) );
    DBG_Printf( "GR_EXTENSION: %s\n", grGetString(GR_EXTENSION) );
    DBG_Printf( "GR_HARDWARE: %s\n", grGetString(GR_HARDWARE) );
    DBG_Printf( "GR_RENDERER: %s\n", grGetString(GR_RENDERER) );
    DBG_Printf( "GR_VERSION: %s\n", grGetString(GR_VERSION) );

    // info
    grGet (GR_MAX_TEXTURE_SIZE, 4, &fxret);
    DBG_Printf ( "Max texture size : %d\n", fxret);
    grGet (GR_NUM_TMU, 4, &fxret);
    DBG_Printf ( "Number of TMU's : %d\n", fxret);
    grGet (GR_TEXTURE_ALIGN, 4, &fxret);
    DBG_Printf ( "Align boundary for textures : %d\n", fxret);
    // save for later!
    gr_alignboundary = fxret;
    if (fxret==0)
        gr_alignboundary = 16;  //hack, need to be > 0

    glide_state[HWD_SET_FOG_MODE] = 1;
    glide_state[HWD_SET_FOG_COLOR] = 0x7f7f7f;
    glide_state[HWD_SET_FOG_DENSITY] = 500;
    glide_state[HWD_SET_POLYGON_SMOOTH] = false;
    glide_state[HWD_SET_TEXTUREFILTERMODE] = HWD_SET_TEXTUREFILTER_BILINEAR;

    SetTransform(NULL);

    return TRUE;
}


static viddef_t* viddef;
// ==========================================================================
//
// ==========================================================================
EXPORT void HWRAPI( Shutdown ) (void)
{
    DBG_Printf ("HWRAPI Shutdown()\n");
    grGlideShutdown();
    FreeFogTable();
    glide_initialized = false;
}


// **************************************************************************
//                                                  3DFX DISPLAY MODES DRIVER
// **************************************************************************

static int Set3DfxMode (viddef_t *lvid, vmode_t *pcurrentmode) ;


// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
#define MAX_VIDEO_MODES         30
static  vmode_t         video_modes[MAX_VIDEO_MODES] = {{NULL, NULL}};


EXPORT void HWRAPI( GetModeList ) (vmode_t** pvidmodes, int* numvidmodes)
{
    GrResolution query;
    GrResolution *list;
    GrResolution *grRes;
    char*   resTxt;
    int     listSize;
    int     listPos;
    int     iPrevWidth, iPrevHeight;    //skip duplicate modes
    int     iMode, iWidth, iHeight;

    DBG_Printf ("HWRAPI GetModeList()\n");

    // find all possible modes that include a z-buffer
    query.resolution = GR_QUERY_ANY;
    query.refresh    = GR_QUERY_ANY;
    query.numColorBuffers = 2;  //GR_QUERY_ANY;
    query.numAuxBuffers = 1;
    listSize = grQueryResolutions (&query, NULL);
    list = _alloca(listSize);
    grQueryResolutions (&query, list);

    iMode=0;
    grRes = list;
    iPrevWidth = 0; iPrevHeight = 0;
    for (listPos=0; listPos<listSize; listPos+=sizeof(GrResolution) , grRes++)
    {
        if (iMode>=MAX_VIDEO_MODES)
        {
            DBG_Printf ("Too many vidmode\n");
            break; // quit the for
        }

        switch (grRes->resolution)
        {
        case GR_RESOLUTION_320x200  : resTxt = "320x200";   iWidth = 320;  iHeight = 200;  break;
        case GR_RESOLUTION_320x240  : resTxt = "320x240";   iWidth = 320;  iHeight = 240;  break;
        case GR_RESOLUTION_400x256  : resTxt = "400x256";   iWidth = 400;  iHeight = 256;  break;
        case GR_RESOLUTION_512x384  : resTxt = "512x384";   iWidth = 512;  iHeight = 384;  break;
        case GR_RESOLUTION_640x200  : resTxt = "640x200";   iWidth = 640;  iHeight = 200;  break;
        case GR_RESOLUTION_640x350  : resTxt = "640x350";   iWidth = 640;  iHeight = 350;  break;
        case GR_RESOLUTION_640x400  : resTxt = "640x400";   iWidth = 640;  iHeight = 400;  break;
        case GR_RESOLUTION_640x480  : resTxt = "640x480";   iWidth = 640;  iHeight = 480;  break;
        case GR_RESOLUTION_800x600  : resTxt = "800x600";   iWidth = 800;  iHeight = 600;  break;
        case GR_RESOLUTION_960x720  : resTxt = "960x720";   iWidth = 960;  iHeight = 720;  break;
        case GR_RESOLUTION_856x480  : resTxt = "856x480";   iWidth = 856;  iHeight = 480;  break;
        case GR_RESOLUTION_512x256  : resTxt = "512x256";   iWidth = 512;  iHeight = 256;  break;
        case GR_RESOLUTION_1024x768 : resTxt = "1024x768";  iWidth = 1024; iHeight = 768;  break;
        case GR_RESOLUTION_1280x1024: resTxt = "1280x1024"; iWidth = 1280; iHeight = 1024; break;
        case GR_RESOLUTION_1600x1200: resTxt = "1600x1200"; iWidth = 1600; iHeight = 1200; break;
        case GR_RESOLUTION_400x300  : resTxt = "400x300";   iWidth = 400;  iHeight = 300;  break;
        case GR_RESOLUTION_NONE     : resTxt = "NONE";      iWidth = 0;    iHeight = 0;    break;
        default:
            iWidth = 0; iHeight = 0;
            resTxt = "unknown resolution"; break;
        }

        if( iWidth == 0 || iHeight == 0)
        {
            DBG_Printf ("Wrong Mode (%s)\n",resTxt);
            continue;
        }

        // skip duplicate resolutions where only the refresh rate changes
        if( iWidth==iPrevWidth && iHeight==iPrevHeight)
        {
            DBG_Printf ("Same mode (%s) %d\n",resTxt,grRes->refresh);
            continue;
        }

        // disable too big modes until we get it fixed
        if ( iWidth > MAXVIDWIDTH || iHeight > MAXVIDHEIGHT )
        {
            DBG_Printf ("Mode too big (%s)\n",resTxt);
            continue;
        }

        // save video mode information for video modes menu
        video_modes[iMode].pnext = &video_modes[iMode+1];
        video_modes[iMode].windowed = 0;
        video_modes[iMode].misc = grRes->resolution;
        video_modes[iMode].name = resTxt;
        video_modes[iMode].width = iWidth;
        video_modes[iMode].height = iHeight;
        video_modes[iMode].rowbytes = iWidth * 2;   //framebuffer 16bit, but we don't use this anyway..
        video_modes[iMode].bytesperpixel = 2;
        video_modes[iMode].pextradata = NULL;       // for VESA, unused here
        video_modes[iMode].setmode = Set3DfxMode;
        DBG_Printf ("Mode %d : %s (%dx%d) %dHz ires %d \n",
                     iMode,resTxt, iWidth, iHeight, grRes->refresh, grRes->resolution);
        iMode++;
        iPrevWidth = iWidth; iPrevHeight = iHeight;
    }

    // add video modes to the list
    if (iMode>0)
    {
        video_modes[iMode-1].pnext = NULL;
        (*(vmode_t**)pvidmodes) = &video_modes[0];
        (*numvidmodes) = iMode;
    }

    // VGA Init equivalent (add first defautlt mode)
    //glidevidmodes[NUMGLIDEVIDMODES-1].pnext = NULL;
    //*((vmode_t**)pvidmodes) = &glidevidmodes[0];
    //*numvidmodes = NUMGLIDEVIDMODES;
}

static int currentmode_width;
static int currentmode_height;

// Set3DfxMode
// switch to one of the video modes querried at initialization
static int Set3DfxMode (viddef_t *lvid, vmode_t *pcurrentmode)
{
    int     iResolution;

    // we stored the GR_RESOLUTION_XXX number into the 'misc' field
    iResolution = pcurrentmode->misc;

    DBG_Printf ("Set3DfxMode() iResolution(%d)\n", iResolution);

    // this is normally not used (but we are actually double-buffering with glide)
    lvid->u.numpages = 2;

    // Change window attributes
    SetWindowLong (lvid->WndParent, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(lvid->WndParent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE |
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);


    //cleanup previous mode
    if (grPreviousContext) {
        // close the current graphics subsystem
        DBG_Printf ("grSstWinClose()\n");
        grSstWinClose (grPreviousContext);
    }

    // create Glide window
    grPreviousContext = grSstWinOpen( (FxU32)lvid->WndParent,
                        iResolution,
                        GR_REFRESH_60Hz,      //note: we could always use the higher refresh?
                        GR_COLORFORMAT_ABGR,
                        GR_ORIGIN_UPPER_LEFT,
                        2, 1 );

    if ( !grPreviousContext ) {
        DBG_Printf ("grSstWinOpen() FAILED\n");
        return 0;
    }

    // for glide debug console
    //tlSetScreen (pcurrentmode->width, pcurrentmode->height);
    glide_initialized = true;
    GR_ResetStates (lvid);

    // force reload of patches because the memory is trashed while we change the screen
    GR_InitMipmapCache();

    lvid->buffer = NULL;    //unless we use the software view
    lvid->direct = NULL;    //direct access to video memory, old DOS crap

    currentmode_width = pcurrentmode->width;
    currentmode_height = pcurrentmode->height;

    // remember lvid here, to free the software buffer if we used it (software view)
    viddef = lvid;

    return 1;
}


// --------------------------------------------------------------------------
// Swap front and back buffers
// --------------------------------------------------------------------------
static int glide_console = 1;
EXPORT void HWRAPI( FinishUpdate ) ( int waitvbl )
{
    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI FinishUpdate() : display not set\n");
        return;
    }

    /*
static int frame=0;
static int lasttic=0;
    int tic,fps;

    // draw silly stuff here
    if (glide_console & 1)
        tlPrintNumber (frame++);

    if (glide_console & 1)
    {
        tic = dl.I_GetTime();
        fps = TICRATE - (tic - lasttic) + 1;
        lasttic = tic;
        tlPrintNumber (fps);
    }
*/

    //DBG_Printf ("HWRAPI FinishUpdate()\n");

    // flip screen
    grBufferSwap( waitvbl );      
}


// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
static unsigned long myPaletteData[256];  // 256 ARGB entries
//TODO: do the chroma key stuff out of here
EXPORT void HWRAPI( SetPalette ) (RGBA_t* pal, RGBA_t *gamma)
{
    int i;

    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI SetPalette() : display not set\n");
        return;
    }

    // create the palette in the format used for downloading to 3Dfx card
    for (i=0; i<256; i++)
        myPaletteData[i] = (pal[i].s.alpha << 24) |          
                           (pal[i].s.red<<16)     |
                           (pal[i].s.green<<8)    |
                            pal[i].s.blue;

    // make sure the chromakey color is always the same value
    myPaletteData[HWR_PATCHES_CHROMAKEY_COLORINDEX] = HWR_PATCHES_CHROMAKEY_COLORVALUE;

    grTexDownloadTable (GR_TEXTABLE_PALETTE, (void*)myPaletteData);
    guGammaCorrectionRGB(gamma->s.red/127.0f,gamma->s.green/127.0f,gamma->s.blue/127.0f);
}


// **************************************************************************
//
// **************************************************************************

// store min/max w depth buffer values here, used to clear buffer
static  FxU32  gr_wrange[2];


// --------------------------------------------------------------------------
// Do a full buffer clear including color / alpha / and Z buffers
// --------------------------------------------------------------------------
static void BufferClear (void)
{
    if (!grPreviousContext) {
        DBG_Printf ("BufferClear() : display not set\n");
        return;
    }
    grDepthMask (FXTRUE);
    grColorMask (FXTRUE,FXFALSE);
    grBufferClear(0x00000000, 0, gr_wrange[1]);
}


//
// set initial state of 3d card settings
//
static void GR_ResetStates (viddef_t *lvid)
{
    DBG_Printf ("ResetStates()\n");

    if (!grPreviousContext) {
        DBG_Printf ("ResetStates() : display not set\n");
        return;
    }
    //grSplash( 64, 64, SCREEN_WIDTH-64, SCREEN_HEIGHT-64, 0 ); //splash screen!

    // get min/max w buffer range values
    grGet (GR_WDEPTH_MIN_MAX, 8, gr_wrange);
    grCoordinateSpace(GR_CLIP_COORDS);
    grViewport((FxU32)0, (FxU32)0, (FxU32)lvid->width, (FxU32)lvid->height);

    // don't work ! the only function that support this is drawpoly
    // y=0 is in lower left corner, (not like in vga)
    //grSstOrigin( GR_ORIGIN_LOWER_LEFT );


    // initialize depth buffer type
    grDepthRange( 0.0, 1.0 );
    grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
    grDepthBufferFunction( GR_CMP_LEQUAL );
    grDepthMask( FXTRUE );
    grColorMask ( FXTRUE, FXFALSE);

    // my vertex format
    grVertexLayout(GR_PARAM_XY   , FIELD_OFFSET(FOutVector,x)   , GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_PARGB, FIELD_OFFSET(FOutVector,argb), GR_PARAM_ENABLE);
    //grVertexLayout(GR_PARAM_Q, 12, GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_W    , FIELD_OFFSET(FOutVector,z) , GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_ST0  , FIELD_OFFSET(FOutVector,sow) , GR_PARAM_ENABLE);  //s and t for tmu0

    grTexCombine (GR_TMU0, GR_COMBINE_FUNCTION_LOCAL,
                  GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_LOCAL,
                  GR_COMBINE_FACTOR_NONE,
                  FXFALSE, FXFALSE );

    // no mipmaps based on depth
    grTexMipMapMode (GR_TMU0, GR_MIPMAP_DISABLE, FXFALSE );

    grConstantColorValue (0xffffffff);
    
    grChromakeyValue(HWR_PATCHES_CHROMAKEY_COLORVALUE);

    grAlphaTestReferenceValue( 0 );

    // this set CurrentPolyFlags to the acctual configuration
    CurrentPolyFlags = 0xffffffff;
    SetBlend(0);

    BufferClear();
    MakeFogTable();
    ReSetSpecialState();
}


// **************************************************************************
//                                               3DFX MEMORY CACHE MANAGEMENT
// **************************************************************************

#define TEXMEM_2MB_EDGE         (1<<21)

static  FxU32           gr_cachemin;
static  FxU32           gr_cachemax;

static  FxU32           gr_cachepos;
static  GlideMipmap_t*  gr_cachetail = NULL;
static  GlideMipmap_t*  gr_cachehead;
static  GlideMipmap_t*  lastmipmapset;

// --------------------------------------------------------------------------
// This must be done once only for all program execution
// --------------------------------------------------------------------------
GlideMipmap_t fakemipmap;
static void GR_ClearMipmapCache (void)
{
    while (gr_cachetail)
    {
        gr_cachetail->downloaded = false;
        gr_cachehead = gr_cachetail;
        gr_cachetail = gr_cachetail->nextmipmap;
        gr_cachehead->nextmipmap = NULL;
    }

    // make a dummy first just for easy cache handling
    fakemipmap.cachepos = gr_cachemin;
    fakemipmap.mipmapSize = grTexCalcMemRequired(GR_LOD_LOG2_1,GR_LOD_LOG2_1,GR_ASPECT_LOG2_1x1,GR_TEXFMT_P_8);
    fakemipmap.downloaded = true;
    fakemipmap.nextmipmap = NULL;
    lastmipmapset = NULL;

    gr_cachetail = &fakemipmap;
    gr_cachehead = &fakemipmap;
    gr_cachepos = gr_cachetail->cachepos+fakemipmap.mipmapSize;

    DBG_Printf ("Cache cleared\n");
}

EXPORT void HWRAPI( ClearMipMapCache ) (void)
{
    GR_ClearMipmapCache ();
}

static void GR_InitMipmapCache (void)
{
    gr_cachemin = grTexMinAddress(GR_TMU0);
    gr_cachemax = grTexMaxAddress(GR_TMU0);

    // deboging...
    // reduise memory so there will use more the legacy heap
    //gr_cachemax = gr_cachemin + (256<<10);

    if( gr_cachemax-gr_cachemin < 64<<10 )
        I_ErrorGr("R_Glide : Only %d memory available for texture !\n",gr_cachemax-gr_cachemin);

    GR_ClearMipmapCache();

    DBG_Printf ("HWR_InitMipmapCache() : %d kb, from %x to %x\n"
                "tmu2 : from %x to %x\n", (gr_cachemax-gr_cachemin)>>10,gr_cachemin,gr_cachemax,
                                           grTexMinAddress(GR_TMU1),grTexMaxAddress(GR_TMU1));
}

static boolean possibleproblem=false;

static void GR_FlushMipmap ()
{
    if( !gr_cachetail)
        return;
    if(!gr_cachetail->downloaded)
        DBG_Printf ("flush not dowloaded !!\n");
    gr_cachetail->downloaded = false;
    // should never happen
    if (!gr_cachetail->nextmipmap)
    {
        if( possibleproblem )
            I_ErrorGr ("This just CAN'T HAPPEN!!! So you think you're different eh ?");
        possibleproblem=true;
        DBG_Printf ("Never happen happen !!\n");
        GR_ClearMipmapCache ();
    }
    else
    {
//        DBG_Printf ("fluching mipmap at position %d (%d bytes) tailpos=%d (%d byte)\n",gr_cachetail->cachepos,gr_cachetail->mipmapSize,gr_cachetail->nextmipmap->cachepos,gr_cachetail->nextmipmap->mipmapSize);
        gr_cachetail->cachepos = 0;
        gr_cachetail->mipmapSize = -1;
        gr_cachetail = gr_cachetail->nextmipmap;
    }
}

// --------------------------------------------------------------------------
// Download a 'surface' into the graphics card memory
// --------------------------------------------------------------------------
static void GR_DownloadMipmap (GlideMipmap_t* grMipmap)
{
    FxU32   mipmapSize;

    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI DownloadMipmap() : display not set\n");
        return;
    }

    if ( !grMipmap->grInfo.data ) {
        DBG_Printf ("HWRAPI DownloadMipmap() : No DATA !!!\n");
        return;
    }
        
    mipmapSize = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &grMipmap->grInfo);

    while (1)
    {
        // 3Dfx specs : a mipmap level can not straddle the 2MByte boundary
        if (gr_cachetail->cachepos >= gr_cachepos)
        {
            if (gr_cachetail->cachepos >= gr_cachepos + mipmapSize)
                if((gr_cachepos < TEXMEM_2MB_EDGE) && (gr_cachepos+mipmapSize > TEXMEM_2MB_EDGE))
                {
                    if( gr_cachetail->cachepos >= TEXMEM_2MB_EDGE )
                    {    
                        gr_cachepos = TEXMEM_2MB_EDGE;
                        continue;
                    }
                    // else FlushMipmap
                }
                else
                    break;
            GR_FlushMipmap ();
        }
        else
        {
            if (gr_cachemax >= gr_cachepos + mipmapSize)
            {
                if((gr_cachepos < TEXMEM_2MB_EDGE) && (gr_cachepos+mipmapSize > TEXMEM_2MB_EDGE) && (gr_cachetail->cachepos < TEXMEM_2MB_EDGE))
                    gr_cachepos = TEXMEM_2MB_EDGE;
                else
                    break;
            }
            else
                // not enough space in the end of the buffer
                gr_cachepos = gr_cachemin;
                //dbg_printf ("        cycle over\n");
        }
        //dbg_printf ("        tailpos: %7d pos: %7d size: %7d free: %7d\n",
        //            gr_cachetail->cachepos, gr_cachepos, mipmapsize, freespace);
    }
    gr_cachehead->nextmipmap = grMipmap;
   
    //DBG_Printf ("download %d byte at %d\n",mipmapSize,gr_cachepos);

    grTexDownloadMipMap (GR_TMU0, gr_cachepos, GR_MIPMAPLEVELMASK_BOTH, &grMipmap->grInfo);
    grMipmap->cachepos     = gr_cachepos;
    grMipmap->mipmapSize   = mipmapSize;
    grMipmap->downloaded   = true;
    grMipmap->nextmipmap    = NULL;     // the head don't have next

    gr_cachepos += mipmapSize;

    gr_cachehead = grMipmap;   // the head is the last loaded texture (FIFO)
    possibleproblem=false;
}


// ==========================================================================
// The mipmap becomes the current texture source
// ==========================================================================
EXPORT void HWRAPI( SetTexture ) (GlideMipmap_t* grMipmap)
{
    FBITFIELD xor;
    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI SetTexture() : display not set\n");
        return;
    }

    // don't set exactely the same mipmap 
    if( lastmipmapset == grMipmap )
        return;

    if (!grMipmap->downloaded)
        GR_DownloadMipmap (grMipmap);

    xor = grMipmap->flags ^ CurrentTextureFlags;
    if(xor)
    {
        if(xor&TF_WRAPXY)
        {
            switch(grMipmap->flags & TF_WRAPXY)
        {
            case 0 :
                grTexClampMode (GR_TMU0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);
                break;
            case TF_WRAPX :
                grTexClampMode (GR_TMU0, GR_TEXTURECLAMP_WRAP , GR_TEXTURECLAMP_CLAMP);
                break;
            case TF_WRAPY :
                grTexClampMode (GR_TMU0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_WRAP);
                break;
            case TF_WRAPXY :
                grTexClampMode (GR_TMU0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);
                break;
        }
        }
        if( xor & TF_CHROMAKEYED )
        {
            if(grMipmap->flags & TF_CHROMAKEYED )
                grChromakeyMode (GR_CHROMAKEY_ENABLE);
            else
                grChromakeyMode (GR_CHROMAKEY_DISABLE);
        }
        CurrentTextureFlags = grMipmap->flags;
    }
    grTexSource (GR_TMU0, grMipmap->cachepos, GR_MIPMAPLEVELMASK_BOTH, &grMipmap->grInfo);
    lastmipmapset = grMipmap;
}


// -----------------+
// SetBlend         : Set render mode
// -----------------+
// PF_Masked - we could use an ALPHA_TEST of GL_EQUAL, and alpha ref of 0,
//             is it faster when pixels are discarded ?
EXPORT void HWRAPI(     SetBlend ) ( FBITFIELD PolyFlags )
{
    FBITFIELD Xor;

    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI SetBlend() : display not set\n");
        return;
    }

    // Detect changes in the blending modes.
    Xor = CurrentPolyFlags^PolyFlags;
    if( !Xor )
        return;

    if( Xor&(PF_Blending) ) // if blending mode must be changed
    {
        switch(PolyFlags & PF_Blending) {
            case PF_Translucent & PF_Blending:
                grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                      GR_BLEND_ONE      , GR_BLEND_ZERO );
                break;
            case PF_Masked & PF_Blending:
                // no alpha blending
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ZERO, GR_BLEND_ONE, GR_BLEND_ZERO);
                break;
            case PF_Additive & PF_Blending:
                // blend destination for transparency, but no source for additive 
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE, GR_BLEND_ONE, GR_BLEND_ZERO);
                break;
            case PF_Environment & PF_Blending:
                grAlphaBlendFunction (GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                      GR_BLEND_ONE, GR_BLEND_ZERO );
                break;
            case PF_Substractive & PF_Blending:
                // not realy but what else ?
                grAlphaBlendFunction (GR_BLEND_ZERO, GR_BLEND_ONE_MINUS_SRC_COLOR,
                                      GR_BLEND_ONE , GR_BLEND_ZERO );
                break;
            default :
                grAlphaBlendFunction (GR_BLEND_ONE,GR_BLEND_ZERO,GR_BLEND_ONE,GR_BLEND_ZERO );
                break;
        }
    }
    if( Xor & PF_NoAlphaTest)
    {
        if( PolyFlags & PF_NoAlphaTest)
            // desable alpha testing 
            grAlphaTestFunction (GR_CMP_ALWAYS);
        else
            // discard 0 alpha pixels (holes in texture)
            grAlphaTestFunction (GR_CMP_GREATER); 
    }    
    if( Xor & PF_Decal )
    {
        // work a little but not like opengl one :(
        if( PolyFlags & PF_Decal )
            grDepthBiasLevel( -1 );
        else
            grDepthBiasLevel( 0 );
    }
    if( Xor&(PF_Modulated | PF_NoTexture))
    {
        switch (PolyFlags & (PF_Modulated | PF_NoTexture)) {
        case 0 :
            // colour from texture is unchanged before blending
            grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                            GR_COMBINE_FACTOR_ONE,
                            GR_COMBINE_LOCAL_NONE,
                            GR_COMBINE_OTHER_TEXTURE,
                            FXFALSE );
            // use alpha texture only
            grAlphaCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                            GR_COMBINE_FACTOR_ONE,
                            GR_COMBINE_LOCAL_NONE,
                            GR_COMBINE_OTHER_TEXTURE,
                            FXFALSE );
            break;
        case PF_Modulated :
            // mix texture colour with Surface->FlatColor (constant color)
            grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,        // factor * Color other
                            GR_COMBINE_FACTOR_LOCAL,                // or local_alpha ???
                            GR_COMBINE_LOCAL_CONSTANT,              // local is constant color
                            GR_COMBINE_OTHER_TEXTURE,               // color from texture map
                            FXFALSE );
            // use (alpha constant)*(alpha texture)
            grAlphaCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                            GR_COMBINE_FACTOR_LOCAL,
                            GR_COMBINE_LOCAL_CONSTANT,
                            GR_COMBINE_OTHER_TEXTURE,
                            FXFALSE );
            break;
        case PF_NoTexture :
            // no texture, no modulate what color use ?
            // in opengl fab use white texture
            grConstantColorValue(0xffFFffFF);
            // no need break
        case PF_Modulated | PF_NoTexture :
            grColorCombine( GR_COMBINE_FUNCTION_LOCAL,              // factor * Color other
                            GR_COMBINE_FACTOR_NONE,
                            GR_COMBINE_LOCAL_CONSTANT,              // local is constant color
                            GR_COMBINE_OTHER_NONE,                  // color from texture map
                            FXFALSE);
            // use (alpha constant)*(alpha texture)
            grAlphaCombine( GR_COMBINE_FUNCTION_LOCAL,
                            GR_COMBINE_FACTOR_NONE,
                            GR_COMBINE_LOCAL_CONSTANT,
                            GR_COMBINE_OTHER_NONE,
                            FXFALSE );
            break;
        }
    }
    if( Xor&PF_NoDepthTest )
    {
        if( PolyFlags & PF_NoDepthTest )
            grDepthBufferFunction(GR_CMP_ALWAYS);
        else
            grDepthBufferFunction(GR_CMP_LEQUAL);
    }
    if( Xor & PF_Occlude )
    {
        // depth is tested but no writed
        grDepthMask( (PolyFlags&PF_Occlude)!=0 );
    }
    if( Xor & PF_Invisible )
    {
        grColorMask( (PolyFlags&PF_Invisible)==0 , FXFALSE );
    }

    CurrentPolyFlags = PolyFlags;
}


// ==========================================================================
// Read a rectangle region of the truecolor framebuffer
// store pixels as 16bit 565 RGB
// ==========================================================================
EXPORT void HWRAPI( ReadRect ) (int x, int y, int width, int height,
                                int dst_stride, unsigned short * dst_data)
{
    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI ReadRect() : display not set\n");
        return;
    }
    grLfbReadRegion (GR_BUFFER_FRONTBUFFER,
                     x, y, width, height, dst_stride, dst_data);
}


// ==========================================================================
// Defines the 2D hardware clipping window
// ==========================================================================
EXPORT void HWRAPI( GClipRect ) (int minx, int miny, int maxx, int maxy, float nearclip)
{
    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI GClipRect() : display not set\n");
        return;
    }
    // BP: swap maxy and miny because wrong axe position
    grClipWindow ( (FxU32)minx, (FxU32)miny, (FxU32)maxx, (FxU32)maxy);
}


// -----------------+
// HWRAPI ClearBuffer
//                  : Clear the color/alpha/depth buffer(s)
// -----------------+
EXPORT void HWRAPI( ClearBuffer ) ( FBOOLEAN ColorMask,
                                    FBOOLEAN DepthMask,
                                    FRGBAFloat * ClearColor )
{
    FBITFIELD polyflags;

    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI ClearBuffer() : display not set\n");
        return;
    }

    grColorMask (ColorMask, FXFALSE);

    polyflags = CurrentPolyFlags;
    // enable or desable z-buffer
    if( DepthMask )
        polyflags |= PF_Occlude;
    else
        polyflags &= ~PF_Occlude;
    // enable disable colorbuffer
    if( ColorMask )
        polyflags &= ~PF_Invisible;
    else
        polyflags |= PF_Invisible;

    SetBlend( polyflags );

    if( ClearColor ) 
        grBufferClear ((int)(ClearColor->alpha*255)<<24|
                       (int)(ClearColor->red*255)<<16  |
                       (int)(ClearColor->green*255)<<8 |
                       (int)(ClearColor->blue)*255, 
                       0, gr_wrange[1]);
    else
        grBufferClear (0,0,gr_wrange[1]);
}


// -----------------+
// HWRAPI Draw2DLine: Render a 2D line
// -----------------+
EXPORT void HWRAPI( Draw2DLine ) ( F2DCoord * v1,
                                   F2DCoord * v2,
                                   RGBA_t Color )
{
    FOutVector a,b;

    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI DrawLine() : display not set\n");
        return;
    }

    a.x = v1->x;
    a.y = v1->y;
    a.z = 1.0f;

    b.x = v2->x;
    b.y = v2->y;
    b.z = 1.0f;
    
    SetBlend( PF_Modulated|PF_NoTexture );
    grConstantColorValue(Color.rgba);

    // damed, 3dfx have a bug in grDrawLine !
    //v1->y = -v1->y;
    //v2->y = -v2->y;

    grDrawLine (&a, &b);
}

// convert 4exp mantice 12 (w-buffer) to ieee float 
#define W16_TO_FLOAT(w) ((( ( ((int)w) & 0xF000) <<11) + 0x3F800000) | ((( ((int)w) & 0xFFF)<<11) + (1<<10)) )
// convert ieee float to 4exp mantice 12 (w-buffer)
#define FLOAT_TO_W16(f) ( ( ( ( (*(int*)&(f)) & 0x7F800000) - 0x3F800000)>>11 ) | ( ( *(int*)&(f) & 0x7FFFFF)>>11)  )
#define BYTEPERPIXEL 2

// test if center of corona is visible with the zbuffer (need raw acces to zbuffer)
static boolean ComputeCoronaAlpha( float *retscalef, FOutVector *projVerts )
{
#define NUMPIXELS 8
    unsigned short buf[NUMPIXELS][NUMPIXELS];
    float     cx, cy, cz;
    float     scalef = 0;
    int       x,y;
    int       i,j;
    unsigned short z;
    
    // negative z, so not drawed !
    if( projVerts[0].z < 0)
        return false;
    
    cx = (projVerts[0].x + projVerts[2].x) / 2.0f; // we should change the coronas' ...
    cy = (projVerts[0].y + projVerts[2].y) / 2.0f; // ... code so its only done once.
    cz = projVerts[0].z;
    //DBG_Printf("z : %f\n",cz);
    
    // project (note y is -y in glide)
    x = (int)(currentmode_width * (1 + cx / cz) /2);
    if(x>=currentmode_width || x<0) return false;
    y = (int)(currentmode_height * (1 - cy / cz) /2);
    if(y>=currentmode_height || y<0) return false;
    
    // get z buffer
    grLfbReadRegion( GR_BUFFER_AUXBUFFER,
                     x-NUMPIXELS/2,y-NUMPIXELS/2,NUMPIXELS,NUMPIXELS,
                     BYTEPERPIXEL*NUMPIXELS,buf);
    
    // comparaison in float are the same as in int so use int compare
    // anyway 1 comparaison is better than 64 !
    z = FLOAT_TO_W16(cz);
    //DBG_Printf("z (w16) : %x\n",z);
    for (i=0; i<NUMPIXELS; i++)
        if(i+x>=0 && i+x<currentmode_width)
            for (j=0; j<NUMPIXELS; j++)
            {
                // BP: it seam like not perfect :( not find why ...
#if 1
                if(z < buf[i][j] && j+y>=0 && j+y<currentmode_height)
                    scalef += 1;
                //DBG_Printf("buf[%d][%d]=%x (w16) : %x scale %f\n",i,j,buf[i][j],z,scalef);
#else
                float f;
                *(int *)&f = W16_TO_FLOAT(buf[i][j]);
                if(cz < f && j+y>=0 && j+y<currentmode_height)
                    scalef += 1;
                //DBG_Printf("buf[%d][%d] : %f scale %f\n",i,j,f,scalef);
#endif
                
            }
            
    scalef /= NUMPIXELS*NUMPIXELS;
            
#if 0 // see what pixel we are testing !
            // write white rectangle
    for(i=0;i<NUMPIXELS;i++)
        for(j=0;j<NUMPIXELS;j++)
            buf[i][j] = 0xffff;
    grLfbWriteRegion( GR_BUFFER_BACKBUFFER,
                      x-NUMPIXELS/2,y-NUMPIXELS/2,GR_LFB_SRC_FMT_565,
                      NUMPIXELS,NUMPIXELS,FXTRUE,
                      BYTEPERPIXEL*NUMPIXELS,buf);
        
    // can't be overwrited, z=0
    for(i=0;i<8;i++)
        for(j=0;j<8;j++)
            buf[i][j] = 0x0000;
    grLfbWriteRegion( GR_BUFFER_AUXBUFFER,
                      x-NUMPIXELS/2,y-NUMPIXELS/2,GR_LFB_SRC_FMT_ZA16,
                      NUMPIXELS,NUMPIXELS,FXTRUE,
                      BYTEPERPIXEL*NUMPIXELS,buf);
            
#endif
    if (scalef < 0.05) // alpha too small so don't draw
        return false;

    *retscalef = scalef;
    return true;
}

static FOutVector *doTransform(FOutVector *projVerts, 
                               FUINT       nClipVerts )
{
    float tx,ty,tz;
    FUINT i;
    FOutVector *p = tmp3Verts;

    for(i=0;i<nClipVerts;i++,p++,projVerts++)
    {
        
        *p = *projVerts; // copy texture coord too
        tx = projVerts->x - grTransform.x;
        ty = projVerts->y - grTransform.z;
        tz = projVerts->z - grTransform.y;
        
        p->x = tx*sxsiny - tz * sxcosy;
        tz   = tx*cosy + tz * siny;
        
        p->y =  ty * sycosx - tz * sysinx;
        p->z =  ty * szsinx + tz * szcosx;
        
    }

    return tmp3Verts;
}


// ==========================================================================
// Draw a triangulated polygon
// ==========================================================================
EXPORT void HWRAPI( DrawPolygon ) ( FSurfaceInfo  *pSurf,
                                    FOutVector    *projVerts,
                                    FUINT         nClipVerts,
                                    FBITFIELD     PolyFlags )
{
    int i;

    if (!grPreviousContext) {
        DBG_Printf ("HWRAPI DrawPolygon() : display not set\n");
        return;
    }

    if (nClipVerts < 3)
        return;

    SetBlend( PolyFlags );

    projVerts = doTransform(projVerts, nClipVerts);

    // this test is added for new coronas' code (without depth buffer)
    // I think I should do a separate function for drawing coronas, so it will be a little faster
    if (PolyFlags & PF_Corona) // check to see if we need to draw the corona
    {
        RGBA_t c;
        float  scalef;
        
        if( !ComputeCoronaAlpha(&scalef, projVerts) )
            return;
        
        c.rgba = pSurf->FlatColor.rgba;
        c.s.alpha *= scalef; // change the alpha value (it seems better than changing the size of the corona)
        grConstantColorValue(c.rgba);
    }
    else
    if( (CurrentPolyFlags & PF_Modulated) && pSurf )
        grConstantColorValue(pSurf->FlatColor.rgba);


    // cut polygone to the screen 
    if( CurrentPolyFlags & PF_Clip )
    {
        if( CurrentPolyFlags & PF_NoZClip )
        {
            nClipVerts = ClipToFrustum (projVerts, tmp2Verts, nClipVerts );
            
            if (nClipVerts<3) 
                return;
        }
        else
        {
            // clip to near z plane
            nClipVerts = ClipZ (projVerts, tmpVerts, nClipVerts );

            // -!!!- EXIT HERE if not enough points
            if (nClipVerts<3)
                return;
            nClipVerts = ClipToFrustum (tmpVerts, tmp2Verts, nClipVerts );
            
            if (nClipVerts<3) 
                return;
        }
    
        projVerts=tmp2Verts;
    }

    for(i=0; (FUINT)i < nClipVerts; i++)
        projVerts[i].y = -projVerts[i].y;

    grDrawVertexArrayContiguous(GR_POLYGON, nClipVerts, projVerts, sizeof(FOutVector));
}

// ==========================================================================
// Fog stuff
// ==========================================================================
static GrFog_t *fogtable =NULL;
static float fogdensity=500.0f/(6000.0f);
static int nFog=0;

static void ComputeFogTable(void)
{
    int     i,j;

    // happen because cvar execution of config.cfg
    if(!fogtable)
    {
        DBG_Printf ("Fog table size: nFog %d\n", nFog);
        return;
    }

    // the table is an exponential fog table. It computes q from i using guFogTableIndexToW()
    // and then computes the fog table entries as fog[i]=(1–e -kw )·255 where k is a user-defined
    // constant, FOG_DENSITY.
    for (i=0; i<nFog; i++) {
        // remove (float) to see original doom bands
        
        //j = (int) (exp((float)i*fogdensity)-1);
        //j = (int)((1 - exp((- fogdensity) * guFogTableIndexToW(i))) * 255);
        j = fogdensity * i*i;//guFogTableIndexToW(i);
        //DBG_Printf ("Fog[%d]= %d\n", i, j);
        // see something even if far
        //if (j > 128) j = 128;
        if (j > 200) j = 200;
        fogtable[i] = j;
        
    }
    grFogTable (fogtable);
}

static void MakeFogTable(void)
{
    FreeFogTable();

    grGet (GR_FOG_TABLE_ENTRIES, 4, &nFog);

    fogtable = (GrFog_t*) malloc(nFog * sizeof(GrFog_t) );

    if (!fogtable)
        I_ErrorGr ("could not allocate fog table\n");
    else
        DBG_Printf ("Fog table size: nFog %d\n", nFog);

    ComputeFogTable();
}

static void FreeFogTable(void)
{
    if(fogtable)
    {
        free(fogtable);
        fogtable = NULL;
    }
}



// ==========================================================================
//
// ==========================================================================
static Glide_SetSpecialState( hwdspecialstate_t IdState, int value )
{
    switch (IdState)
    {
        case HWD_SET_FOG_MODE:
            if( value ) 
                grFogMode (GR_FOG_WITH_TABLE_ON_Q);
            else
                grFogMode (GR_FOG_DISABLE);
            break;

        case HWD_SET_FOG_COLOR: 
            grFogColorValue (value);
            break;
        
        case HWD_SET_FOG_DENSITY:
            fogdensity = (float)value/(6000.0f);
            ComputeFogTable();
            break;

        case HWD_SET_FOV:
            //can't be implementd until we find a "Liang-Barsky" algo for non 90°
            break;

        case HWD_SET_POLYGON_SMOOTH:
            if( value )
                grEnable(GR_AA_ORDERED);
            else
                grDisable(GR_AA_ORDERED);
            break;
        case HWD_SET_TEXTUREFILTERMODE:
            switch (value) {
                case HWD_SET_TEXTUREFILTER_BILINEAR :
                case HWD_SET_TEXTUREFILTER_TRILINEAR:
                     grTexFilterMode (GR_TMU0, GR_TEXTUREFILTER_BILINEAR,
                                               GR_TEXTUREFILTER_BILINEAR);
                     break;
                case HWD_SET_TEXTUREFILTER_POINTSAMPLED :
                     grTexFilterMode (GR_TMU0, GR_TEXTUREFILTER_POINT_SAMPLED,
                                               GR_TEXTUREFILTER_POINT_SAMPLED);
                     break;
                case HWD_SET_TEXTUREFILTER_MIXED1 :
                     grTexFilterMode (GR_TMU0, GR_TEXTUREFILTER_BILINEAR,
                                               GR_TEXTUREFILTER_POINT_SAMPLED);
                     break;
                case HWD_SET_TEXTUREFILTER_MIXED2 :
                     grTexFilterMode (GR_TMU0, GR_TEXTUREFILTER_POINT_SAMPLED,
                                               GR_TEXTUREFILTER_BILINEAR);
                     break;
            }
            break;
    }
}

static void ReSetSpecialState(void)
{
    int i;
    for(i=0;i<HWD_NUMSTATE;i++)
        Glide_SetSpecialState(i, glide_state[i]);
}

EXPORT void HWRAPI( SetSpecialState ) (hwdspecialstate_t IdState, int value)
{
    glide_state[IdState]=value;
    if( glide_initialized )
        Glide_SetSpecialState(IdState, value);
}


// -----------------+
// HWRAPI DrawMD2   : Draw an MD2 model with glcommands
// -----------------+
EXPORT void HWRAPI( DrawMD2 ) (int *gl_cmd_buffer, md2_frame_t *frame, FTransform *pos, float scale)
{
}

EXPORT void HWRAPI( SetTransform ) (FTransform *transform_parm)
{
    float sinx, cosx;

    if( transform_parm ) 
        grTransform = *transform_parm;
    else
        grTransform = defaulttransform;

    sinx = sin( grTransform.anglex*2*PI/360);
    cosx = cos( grTransform.anglex*2*PI/360);
    siny = sin( grTransform.angley*2*PI/360);
    cosy = cos( grTransform.angley*2*PI/360);

    szsinx = sinx * grTransform.scalez;
    szcosx = cosx * grTransform.scalez;
    sxsiny = siny * grTransform.scalex * (1/tan(grTransform.fovxangle*PI/360));
    sxcosy = cosy * grTransform.scalex * (1/tan(grTransform.fovxangle*PI/360));
    sysinx = sinx * grTransform.scaley * (1/tan(grTransform.fovyangle*PI/360));
    sycosx = cosx * grTransform.scaley * (1/tan(grTransform.fovyangle*PI/360));
}


EXPORT int  HWRAPI( GetTextureUsed ) (void)
{
    FTextureInfo*   tmp = gr_cachehead;
    int             res = 0;

    for(tmp=gr_cachetail;tmp;tmp = tmp->nextmipmap)
        res += tmp->mipmapSize;
    return res;
}

EXPORT int  HWRAPI( GetRenderVersion ) (void)
{
    return VERSION;
}
