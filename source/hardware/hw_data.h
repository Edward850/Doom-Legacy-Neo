// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw_data.h,v 1.9 2003/06/05 20:51:36 hurdler Exp $
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
// $Log: hw_data.h,v $
// Revision 1.9  2003/06/05 20:51:36  hurdler
// remove the BOOL define
//
// Revision 1.8  2000/10/04 16:21:57  hurdler
// small clean-up
//
// Revision 1.7  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.6  2000/04/27 17:48:47  hurdler
// colormap code in hardware mode is now the default
//
// Revision 1.5  2000/04/24 15:22:47  hurdler
// Support colormap for text
//
// Revision 1.4  2000/04/22 16:09:14  hurdler
// support skin color in hardware mode
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
//      defines structures and exports for the standard 3D driver DLL used by Doom Legacy
//
//-----------------------------------------------------------------------------


#ifndef _HWR_DATA_
#define _HWR_DATA_

#ifdef __WIN32__
#include <windows.h>
#endif

#ifdef VID_X11
#include <GL/glx.h>
#endif

#include "../doomdef.h"
//THIS MUST DISAPPEAR!!!
#include "hw_glide.h"
#include "../screen.h"


// ==========================================================================
//                                                               TEXTURE INFO
// ==========================================================================

// grInfo.data holds the address of the graphics data cached in heap memory
//                NULL if the texture is not in Doom heap cache.
struct GlideMipmap_s {
    GrTexInfo       grInfo;         //for TexDownloadMipMap
    FxU32           flags;
    unsigned short  height;
    unsigned short  width;
    unsigned int    downloaded;     // the dll driver have it in there cache ?

    struct GlideMipmap_s    *nextcolormap;    
    byte                    *colormap;

    // opengl/glide
    struct GlideMipmap_s* nextmipmap;// glide  : the FIFO list of texture in the memory
                                     //          _DO NOT TUCH IT_
                                     // opengl : liste of all texture in opengl driver
    // glide only
    FxU32           cachepos;        //offset in hardware cache
    FxU32           mipmapSize;      //size of mipmap
};
typedef struct GlideMipmap_s GlideMipmap_t;


//
// Doom texture info, as cached for hardware rendering
//
struct GlideTexture_s {
        GlideMipmap_t mipmap;
        float       scaleX;             //used for scaling textures on walls
        float       scaleY;
};
typedef struct GlideTexture_s GlideTexture_t;


// a cached patch as converted to hardware format, holding the original patch_t
// header so that the existing code can retrieve ->width, ->height as usual
// This is returned by W_CachePatchNum()/W_CachePatchName(), when rendermode
// is 'render_glide'. Else it returns the normal patch_t data.

struct GlidePatch_s {
        // the 4 first fields come right away from the original patch_t
        short               width;
        short               height;
        short               leftoffset;     // pixels to the left of origin
        short               topoffset;      // pixels below the origin
        //
        float               max_s,max_t;
        int                 patchlump;      // the software patch lump num for when the hardware patch
                                            // was flushed, and we need to re-create it
        GlideMipmap_t       mipmap;
};
typedef struct GlidePatch_s GlidePatch_t;

#endif //_HWR_DATA_
