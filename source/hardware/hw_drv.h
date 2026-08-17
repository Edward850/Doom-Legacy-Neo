// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw_drv.h,v 1.14 2003/01/19 21:24:26 bock Exp $
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
// $Log: hw_drv.h,v $
// Revision 1.14  2003/01/19 21:24:26  bock
// Make sources buildable on FreeBSD 5-CURRENT.
//
// Revision 1.13  2001/08/07 00:44:05  hurdler
// MD2 implementation is getting better but still need lots of work
//
// Revision 1.12  2001/02/24 13:35:22  bpereira
// no message
//
// Revision 1.11  2001/02/13 20:37:27  metzgermeister
// *** empty log message ***
//
// Revision 1.10  2001/01/05 18:18:39  hurdler
// add renderer version checking
//
// Revision 1.9  2000/11/04 16:23:44  bpereira
// no message
//
// Revision 1.8  2000/10/04 16:21:57  hurdler
// small clean-up
//
// Revision 1.7  2000/08/21 21:13:26  metzgermeister
// SDL support
//
// Revision 1.6  2000/08/10 14:16:25  hurdler
// no message
//
// Revision 1.5  2000/04/14 16:34:26  hurdler
// some nice changes for coronas
//
// Revision 1.4  2000/04/12 19:32:28  metzgermeister
// added GetRenderer function
//
// Revision 1.3  2000/04/07 23:10:15  metzgermeister
// fullscreen support under X in Linux
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      imports/exports for the 3D hardware low-level interface API
//
//-----------------------------------------------------------------------------


#ifndef __HWR_DRV_H__
#define __HWR_DRV_H__

// this must be here 19991024 by Kin
#include "../screen.h"
#include "hw_data.h"
#include "hw_defs.h"
#include "hw_md2.h"

// Function declaration for exports from the DLL :
// EXPORT <return-type> HWRAPI(<function-name>) ( <arguments> ) ;
// If _CREATE_DLL_ is defined the above declaration translates to :
// __declspec(dllexport) <return-type> WINAPI <function-name> ( <arguments> ) ;
// If _CREATE_DLL_ is NOT DEFINED the above declaration translates to :
// __declspec(dllexport) <return->type> (WINAPI *<function-name>) ( <arguments> ) ;

#ifdef _CREATE_DLL_
#ifdef __WIN32__
    #ifdef __cplusplus
    #define EXPORT  extern "C" __declspec( dllexport )
    #else
    #define EXPORT  __declspec( dllexport )
    #endif
    #define HWRAPI(fn)  WINAPI fn
#else
    #ifdef __cplusplus
    #define EXPORT  extern "C"
    #else
    #define EXPORT
    #endif
    #define HWRAPI(fn)  fn
#endif   
#else // _CREATE_DLL_
    #define EXPORT      typedef
#ifdef __WIN32__
    #define HWRAPI(fn)  (WINAPI *fn)
#else
    #define HWRAPI(fn)  (*fn)
#endif
#endif


// ==========================================================================
//                                                       STANDARD DLL EXPORTS
// ==========================================================================

typedef void (*I_Error_t) (char *error, ...);

EXPORT boolean HWRAPI( Init ) (I_Error_t ErrorFunction) ;
EXPORT void HWRAPI( Shutdown ) (void) ;
#ifdef __WIN32__
EXPORT void    HWRAPI( GetModeList ) (vmode_t** pvidmodes, int* numvidmodes) ;
EXPORT void    HWRAPI( SetPalette ) (RGBA_t* pal, RGBA_t *gamma) ;
#endif
#ifdef VID_X11
EXPORT Window  HWRAPI( HookXwin ) (Display*,int,int,boolean) ;
EXPORT void    HWRAPI( SetPalette ) (RGBA_t* pal, RGBA_t *gamma) ;
#endif
#if defined( PURESDL) || defined(__MACOS__)
EXPORT void HWRAPI( SetPalette ) (int*, RGBA_t *gamma);
#endif
EXPORT void HWRAPI( FinishUpdate ) ( int waitvbl ) ;

EXPORT void HWRAPI( Draw2DLine ) ( F2DCoord * v1, F2DCoord * v2, RGBA_t Color );
EXPORT void HWRAPI( DrawPolygon ) ( FSurfaceInfo  *pSurf,
                                    //FTextureInfo  *pTexInfo,
                                    FOutVector    *pOutVerts,
                                    FUINT         iNumPts,
                                    FBITFIELD     PolyFlags );

EXPORT void HWRAPI( SetBlend ) ( FBITFIELD PolyFlags );

EXPORT void HWRAPI( ClearBuffer ) ( FBOOLEAN ColorMask,
                                    FBOOLEAN DepthMask,
                                    FRGBAFloat *ClearColor );
EXPORT void HWRAPI( SetTexture ) ( FTextureInfo *TexInfo );
EXPORT void HWRAPI( ReadRect ) (int x, int y, int width, int height,
                                int dst_stride, unsigned short * dst_data) ;
EXPORT void HWRAPI( GClipRect ) (int minx, int miny, int maxx, int maxy, float nearclip) ;
EXPORT void HWRAPI( ClearMipMapCache ) (void) ;

//Hurdler: added for backward compatibility
EXPORT void HWRAPI( SetSpecialState ) (hwdspecialstate_t IdState, int Value) ;

//Hurdler: added for new development
EXPORT void HWRAPI( DrawMD2 ) (int *gl_cmd_buffer, md2_frame_t *frame, FTransform *pos, float scale);
EXPORT void HWRAPI( SetTransform ) (FTransform *transform);
EXPORT int  HWRAPI( GetTextureUsed ) (void);
EXPORT int  HWRAPI( GetRenderVersion ) (void);

#ifdef VID_X11 // ifdef to be removed as soon as windoze supports that as well
// metzgermeister: added for Voodoo detection
EXPORT char *HWRAPI( GetRenderer ) (void);
#endif

// ==========================================================================
//                                      HWR DRIVER OBJECT, FOR CLIENT PROGRAM
// ==========================================================================

#if !defined(_CREATE_DLL_)

struct hwdriver_s {
    Init                pfnInit;
    Shutdown            pfnShutdown;
#ifdef __WIN32__
    GetModeList         pfnGetModeList;
#endif
#ifdef VID_X11
    HookXwin            pfnHookXwin;
#endif
    SetPalette          pfnSetPalette;
    FinishUpdate        pfnFinishUpdate;
    Draw2DLine          pfnDraw2DLine;
    DrawPolygon         pfnDrawPolygon;
    SetBlend            pfnSetBlend;
    ClearBuffer         pfnClearBuffer;
    SetTexture          pfnSetTexture;
    ReadRect            pfnReadRect;
    GClipRect           pfnGClipRect;
    ClearMipMapCache    pfnClearMipMapCache;
    SetSpecialState     pfnSetSpecialState;//Hurdler: added for backward compatibility
    DrawMD2             pfnDrawMD2;
    SetTransform        pfnSetTransform;
    GetTextureUsed      pfnGetTextureUsed;
    GetRenderVersion    pfnGetRenderVersion;
#ifdef VID_X11
    GetRenderer         pfnGetRenderer;
#endif
};

extern struct hwdriver_s hwdriver;

//Hurdler: 16/10/99: added for OpenGL gamma correction
//extern RGBA_t  gamma_correction;

#define HWD hwdriver

#endif //not defined _CREATE_DLL_

#endif //__HWR_DRV_H__
