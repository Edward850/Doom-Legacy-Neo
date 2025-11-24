// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: hw_draw.c,v 1.26 2004/04/18 20:40:31 hurdler Exp $
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
// $Log: hw_draw.c,v $
// Revision 1.26  2004/04/18 20:40:31  hurdler
// go 1.42
//
// Revision 1.25  2004/04/18 12:40:15  hurdler
// Jive's request for saving screenshots
//
// Revision 1.24  2003/10/15 14:09:24  darkwolf95
// Fixed screenshots filename bug
//
// Revision 1.23  2003/08/11 15:12:20  hurdler
// go final + translucent HUD + fix spawn in net game
//
// Revision 1.22  2003/06/11 04:44:11  ssntails
// High-res patch drawer added.
//
// Revision 1.21  2003/06/10 21:48:06  ssntails
// Variable flat size support (32x32 to 2048x2048)
//
// Revision 1.20  2001/08/09 21:35:23  hurdler
// Add translucent 3D water in hw mode
//
// Revision 1.19  2001/05/16 21:21:15  bpereira
// no message
//
// Revision 1.18  2001/04/01 17:35:07  bpereira
// no message
//
// Revision 1.17  2001/02/28 17:50:56  bpereira
// no message
//
// Revision 1.16  2001/02/24 13:35:22  bpereira
// no message
//
// Revision 1.15  2001/01/31 17:15:09  hurdler
// Add cv_scalestatusbar in hardware mode
//
// Revision 1.14  2001/01/25 18:56:27  bpereira
// no message
//
// Revision 1.13  2000/11/02 19:49:39  bpereira
// no message
//
// Revision 1.12  2000/10/04 16:21:57  hurdler
// small clean-up
//
// Revision 1.11  2000/09/14 10:42:47  hurdler
// Fix compiling problem under win32
//
// Revision 1.10  2000/09/10 10:48:13  metzgermeister
// *** empty log message ***
//
// Revision 1.9  2000/08/31 14:30:57  bpereira
// no message
//
// Revision 1.8  2000/08/11 19:11:57  metzgermeister
// *** empty log message ***
//
// Revision 1.7  2000/04/27 17:48:47  hurdler
// colormap code in hardware mode is now the default
//
// Revision 1.6  2000/04/24 15:22:47  hurdler
// Support colormap for text
//
// Revision 1.5  2000/04/23 00:30:47  hurdler
// fix a small bug in skin color
//
// Revision 1.4  2000/04/22 21:08:23  hurdler
// I like it better like that
//
// Revision 1.3  2000/04/14 16:34:26  hurdler
// some nice changes for coronas
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      miscellaneous drawing (mainly 2d)
//
//-----------------------------------------------------------------------------


#include "hw_glob.h"
#include "hw_drv.h"

#include "../m_misc.h"      //FIL_WriteFile()
#include "../r_draw.h"      //viewborderlump
#include "../r_main.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../v_video.h"

#ifndef LINUX // unix does not need this 19991024 by Kin
#ifndef __MACOS__
#include <io.h>
#endif
#else
#define O_BINARY 0
#include <unistd.h>
#endif // normalunix
#include <fcntl.h>
#include "../i_video.h"  // for rendermode != render_glide

float   gr_patch_scalex;
float   gr_patch_scaley;

#ifdef WIN32
#pragma pack(1)
#endif
typedef struct {  // sizeof() = 18
  char  id_field_length;
  char  color_map_type;
  char  image_type;
  char  dummy[5];
/*short c_map_origin;
  short c_map_length;
  char  c_map_size;*/
  short x_origin;
  short y_origin;
  short width;
  short height;
  char  image_pix_size;
  char  image_descriptor;
} TGAHeader, *PTGAHeader;
#ifdef WIN32
#pragma pack()
#endif
typedef unsigned char GLRGB[3];
void saveTGA(char *file_name, int width, int height, GLRGB *buffer);

#define BLENDMODE PF_Translucent

//
// -----------------+
// HWR_DrawPatch    : Draw a 'tile' graphic
// Notes            : x,y : positions relative to the original Doom resolution
//                  : textes(console+score) + menus + status bar
// -----------------+
void HWR_DrawPatch (GlidePatch_t* gpatch, int x, int y, int option)
{
    FOutVector      v[4];

//  3--2
//  | /|
//  |/ |
//  0--1
    float sdupx = vid.fdupx*2;
    float sdupy = vid.fdupy*2;
    float pdupx = vid.fdupx*2;
    float pdupy = vid.fdupy*2;

    // make patch ready in hardware cache
    HWR_GetPatch (gpatch);

    if( option & V_NOSCALEPATCH )
        pdupx = pdupy = 2.0f;
    if( option & V_NOSCALESTART )
        sdupx = sdupy = 2.0f;

    v[0].x = v[3].x = (x*sdupx-gpatch->leftoffset*pdupx)/vid.width - 1;
    v[2].x = v[1].x = (x*sdupx+(gpatch->width-gpatch->leftoffset)*pdupx)/vid.width - 1;
    v[0].y = v[1].y = 1-(y*sdupy-gpatch->topoffset*pdupy)/vid.height;
    v[2].y = v[3].y = 1-(y*sdupy+(gpatch->height-gpatch->topoffset)*pdupy)/vid.height;

    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    v[0].sow = v[3].sow = 0.0f;
    v[2].sow = v[1].sow = gpatch->max_s;
    v[0].tow = v[1].tow = 0.0f;
    v[2].tow = v[3].tow = gpatch->max_t;

    // clip it since it is used for bunny scroll in doom I
    if (option & V_TRANSLUCENTPATCH)
    {
        FSurfaceInfo Surf;
        Surf.FlatColor.s.red = Surf.FlatColor.s.green = Surf.FlatColor.s.blue = 0xff;
        Surf.FlatColor.s.alpha = cv_grtranslucenthud.value;
        HWD.pfnDrawPolygon( &Surf, v, 4, PF_Modulated | BLENDMODE | PF_Clip | PF_NoZClip | PF_NoDepthTest);
    }
    else
        HWD.pfnDrawPolygon( NULL, v, 4, BLENDMODE | PF_Clip | PF_NoZClip | PF_NoDepthTest);
}

// Draws a patch 2x as small SSNTails 06-10-2003
void HWR_DrawSmallPatch (GlidePatch_t* gpatch, int x, int y, int option, byte *colormap)
{
    FOutVector      v[4];

    float sdupx = vid.fdupx;
    float sdupy = vid.fdupy;
    float pdupx = vid.fdupx;
    float pdupy = vid.fdupy;

    // make patch ready in hardware cache
    HWR_GetMappedPatch (gpatch, colormap);

    if( option & V_NOSCALEPATCH )
        pdupx = pdupy = 2.0f;
    if( option & V_NOSCALESTART )
        sdupx = sdupy = 2.0f;

    v[0].x = v[3].x = (x*sdupx-gpatch->leftoffset*pdupx)/vid.width - 1;
    v[2].x = v[1].x = (x*sdupx+(gpatch->width-gpatch->leftoffset)*pdupx)/vid.width - 1;
    v[0].y = v[1].y = 1-(y*sdupy-gpatch->topoffset*pdupy)/vid.height;
    v[2].y = v[3].y = 1-(y*sdupy+(gpatch->height-gpatch->topoffset)*pdupy)/vid.height;

    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    v[0].sow = v[3].sow = 0.0f;
    v[2].sow = v[1].sow = gpatch->max_s;
    v[0].tow = v[1].tow = 0.0f;
    v[2].tow = v[3].tow = gpatch->max_t;

    // clip it since it is used for bunny scroll in doom I
    HWD.pfnDrawPolygon( NULL, v, 4, BLENDMODE | PF_Clip | PF_NoZClip | PF_NoDepthTest);
}

//
// HWR_DrawMappedPatch(): Like HWR_DrawPatch but with translated color
//
void HWR_DrawMappedPatch (GlidePatch_t* gpatch, int x, int y, int option, byte *colormap)
{
    FOutVector      v[4];

    float sdupx = vid.fdupx*2;
    float sdupy = vid.fdupy*2;
    float pdupx = vid.fdupx*2;
    float pdupy = vid.fdupy*2;

    // make patch ready in hardware cache
    HWR_GetMappedPatch (gpatch, colormap);

    if( option & V_NOSCALEPATCH )
        pdupx = pdupy = 2.0f;
    if( option & V_NOSCALESTART )
        sdupx = sdupy = 2.0f;

    v[0].x = v[3].x = (x*sdupx-gpatch->leftoffset*pdupx)/vid.width - 1;
    v[2].x = v[1].x = (x*sdupx+(gpatch->width-gpatch->leftoffset)*pdupx)/vid.width - 1;
    v[0].y = v[1].y = 1-(y*sdupy-gpatch->topoffset*pdupy)/vid.height;
    v[2].y = v[3].y = 1-(y*sdupy+(gpatch->height-gpatch->topoffset)*pdupy)/vid.height;

    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    v[0].sow = v[3].sow = 0.0f;
    v[2].sow = v[1].sow = gpatch->max_s;
    v[0].tow = v[1].tow = 0.0f;
    v[2].tow = v[3].tow = gpatch->max_t;

    // clip it since it is used for bunny scroll in doom I
    if (option & V_TRANSLUCENTPATCH)
    {
        FSurfaceInfo Surf;
        Surf.FlatColor.s.red = Surf.FlatColor.s.green = Surf.FlatColor.s.blue = 0xff;
        Surf.FlatColor.s.alpha = cv_grtranslucenthud.value;
        HWD.pfnDrawPolygon( &Surf, v, 4, PF_Modulated | BLENDMODE | PF_Clip | PF_NoZClip | PF_NoDepthTest);
    }
    else
        HWD.pfnDrawPolygon( NULL, v, 4, BLENDMODE | PF_Clip | PF_NoZClip | PF_NoDepthTest);
}

void HWR_DrawPic(int x, int y, int lumpnum)
{
    FOutVector      v[4];
    GlidePatch_t    *patch;

    // make pic ready in hardware cache
    patch = HWR_GetPic( lumpnum );

//  3--2
//  | /|
//  |/ |
//  0--1

    v[0].x = v[3].x = 2.0*(float)x/vid.width - 1;
    v[2].x = v[1].x = 2.0*(float)(x+patch->width*vid.fdupx)/vid.width - 1;
    v[0].y = v[1].y =1-2.0*(float)y/vid.height;
    v[2].y = v[3].y =1-2.0*(float)(y+patch->height*vid.fdupy)/vid.height;

    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    v[0].sow = v[3].sow =  0;
    v[2].sow = v[1].sow =  patch->max_s;
    v[0].tow = v[1].tow =  0;
    v[2].tow = v[3].tow =  patch->max_t;


    //Hurdler: Boris, the same comment as above... but maybe for pics
    // it not a problem since they don't have any transparent pixel
    // if I'm right !?
    // But then, the question is: why not 0 instead of PF_Masked ?
    // or maybe PF_Environment ??? (like what I said above)
    // BP: PF_Environment don't change anything ! and 0 is undifined
    if (cv_grtranslucenthud.value != 255)
    {
        FSurfaceInfo Surf;
        Surf.FlatColor.s.red = Surf.FlatColor.s.green = Surf.FlatColor.s.blue = 0xff;
        Surf.FlatColor.s.alpha = cv_grtranslucenthud.value;
        HWD.pfnDrawPolygon( &Surf, v, 4, PF_Modulated | BLENDMODE | PF_NoDepthTest | PF_Clip | PF_NoZClip);
    }
    else
        HWD.pfnDrawPolygon( NULL, v, 4, BLENDMODE | PF_NoDepthTest | PF_Clip | PF_NoZClip);
}

// ==========================================================================
//                                                            V_VIDEO.C STUFF
// ==========================================================================


// --------------------------------------------------------------------------
// Fills a box of pixels using a flat texture as a pattern
// --------------------------------------------------------------------------
void HWR_DrawFlatFill (int x, int y, int w, int h, int flatlumpnum)
{
    FOutVector  v[4];
    double flatsize;
    int flatflag;
    int size;

    size = W_LumpLength(flatlumpnum);

    switch(size)
    {
        case 4194304: // 2048x2048 lump
            flatsize = 2048.0f;
            flatflag = 2047;
            break;
        case 1048576: // 1024x1024 lump
            flatsize = 1024.0f;
            flatflag = 1023;
            break;
        case 262144:// 512x512 lump
            flatsize = 512.0f;
            flatflag = 511;
            break;
        case 65536: // 256x256 lump
            flatsize = 256.0f;
            flatflag = 255;
            break;
        case 16384: // 128x128 lump
            flatsize = 128.0f;
            flatflag = 127;
            break;
        case 1024: // 32x32 lump
            flatsize = 32.0f;
            flatflag = 31;
            break;
        default: // 64x64 lump
            flatsize = 64.0f;
            flatflag = 63;
            break;
    }

//  3--2
//  | /|
//  |/ |
//  0--1

    v[0].x = v[3].x = (x - 160.0f)/160.0f;
    v[2].x = v[1].x = ((x+w) - 160.0f)/160.0f;
    v[0].y = v[1].y = -(y - 100.0f)/100.0f;
    v[2].y = v[3].y = -((y+h) - 100.0f)/100.0f;

    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    // flat is 64x64 lod and texture offsets are [0.0, 1.0]
    v[0].sow = v[3].sow = (x & flatflag)/flatsize;
    v[2].sow = v[1].sow = v[0].sow + w/flatsize;
    v[0].tow = v[1].tow = (y & flatflag)/flatsize;
    v[2].tow = v[3].tow = v[0].tow + h/flatsize;

    HWR_GetFlat (flatlumpnum);

    //Hurdler: Boris, the same comment as above... but maybe for pics
    // it not a problem since they don't have any transparent pixel
    // if I'm right !?
    // BTW, I see we put 0 for PFs, and If I'm right, that
    // means we take the previous PFs as default
    // how can we be sure they are ok?
    HWD.pfnDrawPolygon( NULL, v, 4, PF_NoDepthTest); //PF_Translucent );
}


// --------------------------------------------------------------------------
// Fade down the screen so that the menu drawn on top of it looks brighter
// --------------------------------------------------------------------------
//  3--2
//  | /|
//  |/ |
//  0--1
void HWR_FadeScreenMenuBack( unsigned long color, int height )
{
    FOutVector  v[4];
    FSurfaceInfo Surf;

    // setup some neat-o translucency effect
    if (!height) //cool hack 0 height is full height
        height = vid.height;

    v[0].x = v[3].x = -1.0f;
    v[2].x = v[1].x =  1.0f;
    v[0].y = v[1].y =  1.0f-((height<<1)/(float)vid.height);
    v[2].y = v[3].y =  1.0f;
    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    v[0].sow = v[3].sow = 0.0f;
    v[2].sow = v[1].sow = 1.0f;
    v[0].tow = v[1].tow = 1.0f;
    v[2].tow = v[3].tow = 0.0f;

    Surf.FlatColor.rgba = UINT2RGBA(color);
    Surf.FlatColor.s.alpha = (0xff/2) * ((float)height / vid.height);    //calum: varies console alpha
    HWD.pfnDrawPolygon( &Surf, v, 4, PF_NoTexture|PF_Modulated|PF_Translucent|PF_NoDepthTest);
}


// ==========================================================================
//                                                             R_DRAW.C STUFF
// ==========================================================================

// ------------------
// HWR_DrawViewBorder
// Fill the space around the view window with a Doom flat texture, draw the
// beveled edges.
// 'clearlines' is useful to clear the heads up messages, when the view
// window is reduced, it doesn't refresh all the view borders.
// ------------------
extern int st_borderpatchnum;
void HWR_DrawViewBorder (int clearlines)
{
    int         x,y;
    int         top,side;
    int         baseviewwidth,baseviewheight;
    int         basewindowx,basewindowy;
    GlidePatch_t*   patch;
extern float gr_baseviewwindowy,gr_viewwindowx,gr_viewheight,gr_viewwidth;

//    if (gr_viewwidth == vid.width)
//        return;

    if (!clearlines)
        clearlines = BASEVIDHEIGHT; //refresh all

    // calc view size based on original game resolution
    baseviewwidth  = gr_viewwidth/vid.fdupx; //(cv_viewsize.value * BASEVIDWIDTH/10)&~7;

    baseviewheight = gr_viewheight/vid.fdupy;
    top  = gr_baseviewwindowy/vid.fdupy;
    side = gr_viewwindowx/vid.fdupx;

    // top
    HWR_DrawFlatFill (0, 0,
                     BASEVIDWIDTH, (top<clearlines ? top : clearlines),
                     st_borderpatchnum);

    // left
    if (top<clearlines)
        HWR_DrawFlatFill (0, top,
                         side, (clearlines-top < baseviewheight ? clearlines-top : baseviewheight),
                         st_borderpatchnum);

    // right
    if (top<clearlines)
        HWR_DrawFlatFill (side + baseviewwidth, top,
                         side, (clearlines-top < baseviewheight ? clearlines-top : baseviewheight),
                         st_borderpatchnum);

    // bottom
    if (top+baseviewheight<clearlines)
        HWR_DrawFlatFill (0, top+baseviewheight,
                         BASEVIDWIDTH, BASEVIDHEIGHT,
                         st_borderpatchnum);

    //
    // draw the view borders
    //

    basewindowx = (BASEVIDWIDTH - baseviewwidth)>>1;
    if (baseviewwidth==BASEVIDWIDTH)
        basewindowy = 0;
    else
        basewindowy = top;

    // top edge
    if (clearlines > basewindowy-8) {
        patch = W_CachePatchNum (viewborderlump[BRDR_T],PU_CACHE);
        for (x=0 ; x<baseviewwidth; x+=8)
            HWR_DrawPatch (patch,basewindowx+x,basewindowy-8,0);
    }

    // bottom edge
    if (clearlines > basewindowy+baseviewheight) {
        patch = W_CachePatchNum (viewborderlump[BRDR_B],PU_CACHE);
        for (x=0 ; x<baseviewwidth ; x+=8)
            HWR_DrawPatch (patch,basewindowx+x,basewindowy+baseviewheight,0);
    }

    // left edge
    if (clearlines > basewindowy) {
        patch = W_CachePatchNum (viewborderlump[BRDR_L],PU_CACHE);
        for (y=0 ; y<baseviewheight && (basewindowy+y < clearlines); y+=8)
            HWR_DrawPatch (patch,basewindowx-8,basewindowy+y,0);
    }

    // right edge
    if (clearlines > basewindowy) {
        patch = W_CachePatchNum (viewborderlump[BRDR_R],PU_CACHE);
        for (y=0 ; y<baseviewheight && (basewindowy+y < clearlines); y+=8)
            HWR_DrawPatch (patch,basewindowx+baseviewwidth,basewindowy+y,0);
    }

    // Draw beveled corners.
    if (clearlines > basewindowy-8)
        HWR_DrawPatch (W_CachePatchNum (viewborderlump[BRDR_TL],PU_CACHE),
                       basewindowx-8,
                       basewindowy-8,0);

    if (clearlines > basewindowy-8)
        HWR_DrawPatch (W_CachePatchNum (viewborderlump[BRDR_TR],PU_CACHE),
                       basewindowx+baseviewwidth,
                       basewindowy-8,0);

    if (clearlines > basewindowy+baseviewheight)
        HWR_DrawPatch (W_CachePatchNum (viewborderlump[BRDR_BL],PU_CACHE),
                       basewindowx-8,
                       basewindowy+baseviewheight,0);

    if (clearlines > basewindowy+baseviewheight)
        HWR_DrawPatch (W_CachePatchNum (viewborderlump[BRDR_BR],PU_CACHE),
                       basewindowx+baseviewwidth,
                       basewindowy+baseviewheight,0);
}


// ==========================================================================
//                                                     AM_MAP.C DRAWING STUFF
// ==========================================================================

// Clear the automap part of the screen
void HWR_clearAutomap( void )
{
    FRGBAFloat fColor = { 0,0,0,1 };

    //FIXTHIS faB - optimize by clearing only colors ?
    //HWD.pfnSetBlend ( PF_NoOcclude );

    // minx,miny,maxx,maxy
    HWD.pfnGClipRect( 0, 0, vid.width , vid.height - stbarheight, 0.9f );
    HWD.pfnClearBuffer( true, true, &fColor );
    HWD.pfnGClipRect( 0, 0, vid.width, vid.height, 0.9f );
}


// -----------------+
// HWR_drawAMline   : draw a line of the automap (the clipping is already done in automap code)
// Arg              : color is a RGB 888 value
// -----------------+
void HWR_drawAMline( fline_t* fl, int color )
{
    F2DCoord  v1, v2;
    RGBA_t    color_rgba;

    color_rgba = V_GetColor( color );

    v1.x = ((float)fl->a.x-(vid.width/2.0f))*(2.0f/vid.width);
    v1.y = ((float)fl->a.y-(vid.height/2.0f))*(2.0f/vid.height);

    v2.x = ((float)fl->b.x-(vid.width/2.0f))*(2.0f/vid.width);
    v2.y = ((float)fl->b.y-(vid.height/2.0f))*(2.0f/vid.height);

    HWD.pfnDraw2DLine( &v1, &v2, color_rgba );
}


// -----------------+
// HWR_DrawFill     : draw flat coloured rectangle, with no texture
// -----------------+
void HWR_DrawFill( int x, int y, int w, int h, int color )
{
    FOutVector  v[4];
    FSurfaceInfo Surf;

//  3--2
//  | /|
//  |/ |
//  0--1
    v[0].x = v[3].x = (x - 160.0f)/160.0f;
    v[2].x = v[1].x = ((x+w) - 160.0f)/160.0f;
    v[0].y = v[1].y = -(y - 100.0f)/100.0f;
    v[2].y = v[3].y = -((y+h) - 100.0f)/100.0f;

    //Hurdler: do we still use this argb color? if not, we should remove it
    v[0].argb = v[1].argb = v[2].argb = v[3].argb = 0xff00ff00; //;
    v[0].z = v[1].z = v[2].z = v[3].z = 1.0f;

    v[0].sow = v[3].sow = 0.0f;
    v[2].sow = v[1].sow = 1.0f;
    v[0].tow = v[1].tow = 0.0f;
    v[2].tow = v[3].tow = 1.0f;

    Surf.FlatColor = V_GetColor( color );

    HWD.pfnDrawPolygon( &Surf, v, 4, PF_Modulated|PF_NoTexture| PF_NoDepthTest );
}


// --------------------------------------------------------------------------
// screen shot
// --------------------------------------------------------------------------
extern consvar_t cv_screenshotdir;
extern char *startupwadfiles[MAX_WADFILES];

boolean HWR_Screenshot (char *lbmname)
{
    int     i, off;
    byte*   bufw;
    unsigned short* bufr;
    byte*   dest;
    unsigned short  rgb565;

    bufr = malloc(vid.width*vid.height*2);
    if (!bufr)
        return false;
    bufw = malloc(vid.width*vid.height*3);
    if (!bufw)
    {
        free(bufr);
        return false;
    }

    //returns 16bit 565 RGB
    HWD.pfnReadRect (0, 0, vid.width, vid.height, vid.width*2, bufr);

    for (dest = bufw,i=0; i<vid.width*vid.height; i++)
    {
        rgb565 = bufr[i];
        *(dest++) = (rgb565 & 31) <<3;
        *(dest++) = ((rgb565 >> 5) & 63) <<2;
        *(dest++) = ((rgb565 >> 11) & 31) <<3;
    }
    free(bufr);

    if (cv_screenshotdir.string[0]) // Hurdler: Jive's request (save file in other directory)
    {
        #ifdef WIN32
        char sep = '\\';
        #else
        char sep = '/';
        #endif
        int i;
        char wadname[MAX_WADPATH];
        char *shortwadname = NULL;
        strcpy(wadname, "DOOM");
        for (i=1; startupwadfiles[i]; i++) // seach the first "real" wad file (also skip iwad).
        {
            char *wadfile = startupwadfiles[i];
            int pos = strlen(wadfile) - 4;
            if ((pos >= 0) && !strncmp(&wadfile[pos], ".wad", 4))
            {
                strcpy(wadname, wadfile);
                wadname[pos] = '\0';
                break;
            }
        }
        shortwadname = strrchr(wadname, sep);
        sprintf(lbmname, "%s%c%s0000.tga", cv_screenshotdir.string, sep, shortwadname ? shortwadname : wadname);
    }
    else
    {
        strcpy(lbmname,"DOOM0000.tga");
    }
    off = strlen(lbmname) - 8;
    // find a file name to save it to
    for (i=0 ; i<10000; i++)
    {
        lbmname[off + 0] = '0' + ((i/1000) % 10);
        lbmname[off + 1] = '0' + ((i/100) % 10);
        lbmname[off + 2] = '0' + ((i/10) % 10);
        lbmname[off + 3] = '0' + ((i/1) % 10);
        if (access(lbmname,0) == -1) // file doesn't exist, save the file
        {
            saveTGA(lbmname, vid.width, vid.height, (GLRGB *)bufw);
            free(bufw);
            return true;
        }
    }
    free(bufw);
    return false;
}


// --------------------------------------------------------------------------
// save screenshots with TGA format
// --------------------------------------------------------------------------
void saveTGA(char *file_name, int width, int height, GLRGB *buffer)
{
    int fd;
    long size;
    TGAHeader tga_hdr;

    fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
    if (fd < 0)
        return;

    memset(&tga_hdr, 0, sizeof(tga_hdr));
    tga_hdr.width = SHORT(width);
    tga_hdr.height = SHORT(height);
    tga_hdr.image_pix_size = 24;
    tga_hdr.image_type = 2;
    tga_hdr.image_descriptor = 32;
    size = (long)width * (long)height * 3L;

    write(fd, &tga_hdr, sizeof(TGAHeader));
    write(fd, buffer, size);
    close(fd);
}
