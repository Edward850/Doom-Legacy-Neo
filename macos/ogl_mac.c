// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: ogl_mac.c,v 1.1 2001/04/17 22:23:38 calumr Exp $
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
// $Log: ogl_mac.c,v $
// Revision 1.1  2001/04/17 22:23:38  calumr
// Initial add
//
// Revision 1.1  2000/08/21 21:17:32  metzgermeister
// Initial import to CVS
//
//
// DESCRIPTION:
//      Mac specific part of the OpenGL API for Doom Legacy
//
//-----------------------------------------------------------------------------

#include <AGL/agl.h>
#include <AGL/gl.h>
#include <AGL/glu.h>
#include <Carbon/Carbon.h>

#include "doomdef.h"
#include "i_system.h"
#include "r_opengl.h"
#include "screen.h"	//MAXVIDWIDTH

AGLContext	ctx = NULL;
int			oglflags = 0;
int			logstream = -1;
extern int menu_height;

GLint		swapInterval = 1;

void I_FinishUpdate(void)
{
	aglSwapBuffers(ctx);
}

char OglMacSurface(WindowRef *win, int w, int h, boolean fullscreen)
{
	BitMap screenbits;
	GLint attrib[] = {	AGL_RGBA, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 16, AGL_NONE };
	GLint attrib_fullscreen[] = {	AGL_RGBA, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 16, AGL_FULLSCREEN, AGL_NONE };
	static AGLPixelFormat fmt;
	int ok;
	int hOffset, vOffset;
	Rect *window_size, r;
	
	I_OutputMsg("OglMacSurface(%i,%i)\n",w,h);
	
	SetRect(&r, 0, 0, w, h);
	
	GetQDGlobalsScreenBits(&screenbits);
	hOffset = (screenbits.bounds.right - w) / 2;
	vOffset = (screenbits.bounds.bottom - h) / 2;
	
	if (ctx)
	{
		aglSetCurrentContext(NULL);
		aglSetDrawable(ctx,NULL);
		aglDestroyContext(ctx);
		aglDestroyPixelFormat(fmt);
	}
	
	{
		fmt = aglChoosePixelFormat(NULL, 0, attrib);
		if(fmt == NULL)
			I_Error("aglChoosePixelFormat failed");
		ctx = aglCreateContext(fmt, NULL);
		if(ctx == NULL)
			I_Error("aglCreateContext failed");
/*		if (0)
			if (!aglSetFullScreen (ctx, w, h, 85, 0)) // attach fulls screen device to the context
			{
				CONS_Printf("aglSetFullScreen failed\n");
				fullscreen = false;
			}*/
	}
	
	if (fullscreen)
	{
		//window_size->top -= menu_height;
		window_size = &screenbits.bounds;
	}
	else
	{
		window_size = &r;
		OffsetRect(&r, hOffset, vOffset);
		hOffset = vOffset = 0;
	}
	
	I_OutputMsg("win = %i,%i,%i,%i\n", window_size->left, window_size->top, window_size->right,window_size->bottom);
	
	if (!*win)
		*win = NewCWindow (NULL, window_size, "\pDoomLegacy", 0, kMovableModalWindowClass, (WindowPtr)-1, 0, 0);
	
	ShowWindow(*win);
	
	ok = aglSetDrawable(ctx, GetWindowPort(*win));
	if(!ok)
		I_Error("aglSetDrawable failed");
	
	ok = aglSetCurrentContext(ctx);
	if(!ok)
		I_Error("aglSetCurrentContext failed");
	
	aglSetInteger(ctx,AGL_SWAP_INTERVAL,&swapInterval);	//not supported yet but maybe some day? prevents tearing
	
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	aglSwapBuffers(ctx);        //clears screen to black
	
	SetModelView(hOffset, vOffset, w, h);
	SetStates();
	HWR_Startup();
	
	I_OutputMsg("\tOglMacSurface done\n");

	return true;
}

void OglMacShutdown(void)
{
	CONS_Printf("OglMacShutdown\n");
	aglSetCurrentContext(NULL);
	aglSetDrawable(ctx, NULL);
	
	ShowMenuBar();
}