// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: ogl_x11.c,v 1.11 2001/03/09 21:53:56 metzgermeister Exp $
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
// $Log: ogl_x11.c,v $
// Revision 1.11  2001/03/09 21:53:56  metzgermeister
// *** empty log message ***
//
// Revision 1.10  2001/02/19 17:43:38  hurdler
// Fix the problem of fullbright with Matrox's drivers under Linux
//
// Revision 1.9  2001/02/14 20:59:27  hurdler
// fix texture bug under Linux
//
// Revision 1.8  2001/02/13 20:37:27  metzgermeister
// *** empty log message ***
//
// Revision 1.7  2000/08/11 16:32:29  metzgermeister
// *** empty log message ***
//
// Revision 1.6  2000/05/13 19:54:54  metzgermeister
// no tex flush on setmode
//
// Revision 1.5  2000/04/12 19:32:29  metzgermeister
// added GetRenderer function
//
// Revision 1.4  2000/04/07 23:10:15  metzgermeister
// fullscreen support under X in Linux
//
// Revision 1.3  2000/03/07 03:31:14  hurdler
// fix linux compilation
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      X11 specific part of the OpenGL API for Doom Legacy
//
//-----------------------------------------------------------------------------


#ifdef VID_X11

#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <GL/glx.h>

#include "r_opengl.h"


// **************************************************************************
//                                                                    GLOBALS
// **************************************************************************

#ifdef DEBUG_TO_FILE
HANDLE logstream = -1;
#endif

static GLXContext ctx   = NULL;
static Display *dpy     = NULL;
static Window win       = 0; // metzgermeister: No pointer!
static XVisualInfo *vis = NULL;

static char rendererString[256] = "Not Probed Yet";

#define MAX_VIDEO_MODES   32
static  vmode_t     video_modes[MAX_VIDEO_MODES];
int     oglflags = 0; // FIXME: do we have to handle this under Linux as well?
                      // Hurdler: now yes (due to that fullbright bug with g200/g400 card)

// **************************************************************************
//                                                                  FUNCTIONS
// **************************************************************************

EXPORT void _init() {
#ifdef DEBUG_TO_FILE
   logstream = open("ogllog.txt",O_WRONLY|O_CREAT);
#endif
}

EXPORT void _fini() {
#ifdef DEBUG_TO_FILE
   if(logstream!=-1) close(logstream);
#endif
}


//
// FAB --- SORRY, THIS SHOULD BE UPDATED LIKE ABOVE, PLUS THE LINUX ADDS
//
EXPORT Window HWRAPI( HookXwin ) (Display *dsp,int width,int height, boolean vidmode_active)
{
    char *renderer;
    int scrnum;
    int attrib[] = { GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 16, /* bug? 19990908 by Kin */
        None };
    unsigned long mask;
    Window root;
    XSetWindowAttributes attr;

    DBG_Printf ("HookXwin()\n");

    if (ctx != NULL) {        // si ce n'est pas la premiere fois qu'on
	// this flush destroys textures with the UTAH DRI driver !?
        //Flush(); // Flush here, otherwise textures will be trashed after resolution change
        //glXMakeCurrent(NULL, NULL); // initialise l'environnement OpenGL, il
        glXDestroyContext(dpy,ctx);// faut d'abord supprimer l'ancien
        ctx = NULL; 
        // not very clean; use a function UnhookXwin instead?
        XDestroyWindow(dsp, win);
        win = 0;
    }

    dpy = dsp;
    scrnum = DefaultScreen( dsp );
    root = RootWindow( dsp, scrnum );
    vis = glXChooseVisual(dsp,scrnum,attrib);
    if (!vis) {
        return 0;
    }

    /* window attributes */
    if (vidmode_active) {
        mask = CWColormap | CWSaveUnder | CWBackingStore | 
            CWEventMask | CWOverrideRedirect;
        
        attr.override_redirect = True;
        attr.backing_store = NotUseful;
        attr.save_under = False;
    }
    else {
        mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
        
        attr.background_pixel = 0;
        attr.border_pixel = 0;
    }

    // is this acceptable by GLX? 19991226 by Kin
    attr.colormap = XCreateColormap( dsp, root, vis->visual, AllocNone);
    attr.event_mask = KeyPressMask | KeyReleaseMask
#ifndef POLL_POINTER
        | PointerMotionMask | ButtonPressMask | ButtonReleaseMask
#endif
        | ExposureMask | StructureNotifyMask;

    win = XCreateWindow(dsp, 
                        root, 
                        0, 0, 
                        width, height,
                        0, 
                        vis->depth, 
                        InputOutput,
                        vis->visual, 
                        mask, 
                        &attr);
    XMapWindow(dsp, win);
    //SetupPixelFormat();
    if ((ctx=glXCreateContext(dpy,vis,NULL,True))==NULL) {
        DBG_Printf("glXCreateContext() FAILED\n");
        return 0;
    }
    if (!glXMakeCurrent(dpy, win, ctx)) {
        DBG_Printf("glXMakeCurrent() FAILED\n");
        return 0;
    }

    renderer = strdup(glGetString(GL_RENDERER));
    DBG_Printf("Vendor     : %s\n", glGetString(GL_VENDOR));
    DBG_Printf("Renderer   : %s\n", renderer);
    DBG_Printf("Version    : %s\n", glGetString(GL_VERSION));
    DBG_Printf("Extensions : %s\n", glGetString(GL_EXTENSIONS));

    // disable advanced features not working on somes hardware
    if( strstr(renderer, "G200" ) )   oglflags |= GLF_NOTEXENV;
    if( strstr(renderer, "G400" ) )   oglflags |= GLF_NOTEXENV;
    free(renderer);
    DBG_Printf("oglflags   : 0x%X\n", oglflags );

    screen_depth = vis->depth;
    if( screen_depth > 16)
        textureformatGL = GL_RGBA;
    else
        textureformatGL = GL_RGB5_A1;

    SetModelView( width, height );
    SetStates();

    // we need to clear the depth buffer. Very important!!!
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //Flush(); // peut être qu'il faut mettre çà dans resize ou create

    //lvid->buffer = NULL;   // unless we use the software view
    //lvid->direct = NULL;   // direct access to video memory, old DOS crap
    //lvid->numpages = 2;    // this is normally not used

    return win; // on renvoie une valeur pour dire que cela s'est bien pass
}


// -----------------+
// Shutdown         : Shutdown OpenGL, restore the display mode
// -----------------+
EXPORT void HWRAPI( Shutdown ) ( void )
{
    DBG_Printf ("HWRAPI Shutdown()\n");

    if(ctx != NULL) {
       Flush();
       //glXMakeCurrent(NULL,0,0);
       glXDestroyContext(dpy,ctx);
    }
    DBG_Printf ("HWRAPI Shutdown(DONE)\n");
}


// -----------------+
// FinishUpdate     : Swap front and back buffers
// -----------------+
EXPORT void HWRAPI( FinishUpdate ) (int waitvbl)
{
    // DBG_Printf ("FinishUpdate()\n");
    // TODO: implement waitvbl
    glXSwapBuffers(dpy,win);
}


// -----------------+
// SetPalette       : Set the color lookup table for paletted textures
//                  : in OpenGL, we store values for conversion of paletted graphics when
//                  : they are downloaded to the 3D card.
// -----------------+
EXPORT void HWRAPI( SetPalette ) ( RGBA_t *pal, RGBA_t *gamma )
{
    int i;
    //DBG_Printf ("SetPalette()\n");

    for (i=0; i<256; i++) {
        myPaletteData[i].s.red   = MIN((pal[i].s.red*gamma->s.red)/127,     255);
        myPaletteData[i].s.green = MIN((pal[i].s.green*gamma->s.green)/127, 255);
        myPaletteData[i].s.blue  = MIN((pal[i].s.blue*gamma->s.blue)/127,   255);
        myPaletteData[i].s.alpha = pal[i].s.alpha;
    }
    // on a changé de palette, il faut recharger toutes les textures
    Flush();
}

EXPORT char *HWRAPI( GetRenderer ) (void)
{
    strcpy(rendererString, glGetString(GL_RENDERER));

    return rendererString;
}

#endif

























































































































































































