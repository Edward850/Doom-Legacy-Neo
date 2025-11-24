// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hwsym_sdl.c,v 1.4 2001/03/12 21:03:10 metzgermeister Exp $
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
// $Log: hwsym_sdl.c,v $
// Revision 1.4  2001/03/12 21:03:10  metzgermeister
//   * new symbols for rendererlib added in SDL
//   * console printout fixed for Linux&SDL
//   * Crash fixed in Linux SW renderer initialization
//
// Revision 1.3  2001/03/09 21:53:56  metzgermeister
// *** empty log message ***
//
// Revision 1.2  2001/02/24 13:35:23  bpereira
// no message
//
// Revision 1.1  2000/08/21 21:17:32  metzgermeister
// Initial import to CVS
//
//
//
// DESCRIPTION:
//      Tool for dynamic referencing of hardware rendering functions
//
//      Declaration and definition of the HW rendering 
//      functions do have the same name. Originally, the
//      implementation was stored in a separate library.
//      For SDL, we need some function to return the addresses,
//      otherwise we have a conflict with the compiler.
//      
//-----------------------------------------------------------------------------

#if 0
#include "hardware/r_opengl/r_opengl.h"
#include "ogl_sdl.h"
#include "hwsym_sdl.h"

//
// Stupid function to return function addresses
//
void *hwSym(char *funcName)
{
    if(0 == strcmp("Init", funcName))
    {
	return &Init;
    }
    else if(0 == strcmp("FinishUpdate", funcName))
    {
	return NULL; //&FinishUpdate;
    }
    else if(0 == strcmp("Draw2DLine", funcName))
    {
	return &Draw2DLine;
    }
    else if(0 == strcmp("DrawPolygon", funcName))
    {
	return &DrawPolygon;
    }
    else if(0 == strcmp("SetBlend", funcName))
    {
	return &SetBlend;
    }
    else if(0 == strcmp("ClearBuffer", funcName))
    {
	return &ClearBuffer;
    }
    else if(0 == strcmp("SetTexture", funcName))
    {
	return &SetTexture;
    }
    else if(0 == strcmp("ReadRect", funcName))
    {
	return &ReadRect;
    }
    else if(0 == strcmp("GClipRect", funcName))
    {
	return &GClipRect;
    }
    else if(0 == strcmp("ClearMipMapCache", funcName))
    {
	return &ClearMipMapCache;
    }
    else if(0 == strcmp("SetSpecialState", funcName))
    {
	return &SetSpecialState;
    }
    else if(0 == strcmp("SetPalette", funcName))
    {
	return &OglSdlSetPalette;
    }
    else if(0 == strcmp("GetTextureUsed", funcName))
    {
	return &GetTextureUsed;
    }
    else if(0 == strcmp("DrawMD2", funcName))
    {
	return &DrawMD2;
    }
    else if(0 == strcmp("SetTransform", funcName))
    {
	return &SetTransform;
    }
    else if(0 == strcmp("GetRenderVersion", funcName))
    {
        return &GetRenderVersion;
    }
    return NULL;
}
#endif
