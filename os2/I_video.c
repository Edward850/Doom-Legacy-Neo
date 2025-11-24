// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: I_video.c,v 1.7 2001/04/16 22:59:25 ydario Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log: I_video.c,v $
// Revision 1.7  2001/04/16 22:59:25  ydario
// removed unused variable
//
// Revision 1.6  2001/03/03 19:29:44  ydario
// code clean up
//
// Revision 1.5  2000/11/02 19:49:40  bpereira
// no message
//
// Revision 1.4  2000/08/16 16:31:25  ydario
// Give more timeslice to other threads
//
// Revision 1.3  2000/08/10 11:07:51  ydario
// fix CRLF
//
// Revision 1.2  2000/08/10 09:19:31  ydario
// *** empty log message ***
//
// Revision 1.1  2000/08/09 12:15:09  ydario
// OS/2 specific platform code
//
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: I_video.c,v 1.7 2001/04/16 22:59:25 ydario Exp $";

#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <signal.h>

#include "i_os2.h"

//#include "mgraph.h"

#include "doomstat.h"
#include "i_system.h"
#include "i_video.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

boolean         highcolor;
rendermode_t    rendermode=render_soft;
consvar_t       cv_vidwait = {"vid_wait","1",CV_SAVE,CV_OnOff};

#define MAXWINMODES (6) 
static char vidModeName[MAXWINMODES][32];
static int windowedModes[MAXWINMODES][2] = {
   // first is default mode
   {320, 200},
   {400, 300},
   {512, 384}, 
   {640, 480},
   {800, 600},
   {1024, 768},
};

int   VID_NumModes( void);
int   VID_GetModeForSize( int w, int h);
char* VID_GetModeName( int modenum);
int   VID_SetMode( int modenum);


//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?
}

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
   static int	lasttic;
   int		tics;
   int		i;

    // display a graph of ticrate 
    if (cv_ticrate.value )
    {
        int k,j,l;
        #define FPSPOINTS  35
        #define SCALE      4
        #define PUTDOT(xx,yy,cc) screens[0][((yy)*vid.width+(xx))*vid.bpp]=(cc)
        int fpsgraph[FPSPOINTS];

        i = I_GetTime();
        tics = i - lasttic;
        lasttic = i;
        if (tics > 20) tics = 20;

        for (i=0;i<FPSPOINTS-1;i++)
            fpsgraph[i]=fpsgraph[i+1];
        fpsgraph[FPSPOINTS-1]=20-tics;

            // draw dots
            for(j=0;j<=20*SCALE*vid.dupy;j+=2*SCALE*vid.dupy)
            {
                l=(vid.height-1-j)*vid.width*vid.bpp;
                for (i=0;i<FPSPOINTS*SCALE*vid.dupx;i+=2*SCALE*vid.dupx)
                    screens[0][l+i]=0xff;
            }

            // draw the graph
            for (i=0;i<FPSPOINTS;i++)
                for(k=0;k<SCALE*vid.dupx;k++)
                    PUTDOT(i*SCALE*vid.dupx+k, vid.height-1-(fpsgraph[i]*SCALE*vid.dupy),0xff);

    }

      // blit directly if BlitThread is not running.
      // Blit the image using DiveBlit
   if (!pmData->fDataInProcess) {
      DiveBlitImage( pmData->hDive, pmData->ulImage, DIVE_BUFFER_SCREEN);
   }
   DosSleep(0);

/*
      // Use secondary blitting thread

      // blitted previous image?
   if (pmData->fBlitReady == TRUE)
      return; // no, try again
    
      // data is ready for blitting
   memcpy( pmData->pbBuffer2, pmData->pbBuffer, vid.width * vid.height * vid.bpp);
   pmData->fBlitReady = TRUE;
*/
}


//
// This is meant to be called only by CONS_Printf() while game startup
//
void I_LoadingScreen ( PSZ msg )
{
    HPS    hps;
    RECTL  rect;

    if ( msg ) {

        hps = WinGetPS( pmData->hwndClient);
        WinQueryWindowRect( pmData->hwndClient, &rect);
        WinFillRect(hps, &rect, CLR_WHITE);
        WinDrawText( hps, strlen( msg), msg, &rect,
                     0, 0, 
                     DT_WORDBREAK | DT_TOP | DT_LEFT | DT_TEXTATTRS);
    }
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, vid.buffer, vid.width*vid.height*vid.bpp);
}

//
// I_SetPalette
//
void I_SetPalette (RGBA_t* palette)
{
   int   i, r, g, b;
   long  colors[ 256];

      // set the X colormap entries
   for (i=0 ; i<256 ; i++,palette++) {
      r = palette->s.red;
      g = palette->s.green;
      b = palette->s.blue;
      colors[i] = (r<<16) + (g<<8) + b; //(PC_RESERVED * 16777216) +
   }
      // set dive palette
   DiveSetSourcePalette( pmData->hDive, 0,
                         pmData->ulNumColors,
                         (PBYTE) colors);
}

//
//  Close the screen, restore previous video mode.
//
void I_ShutdownGraphics(void)
{
   printf( "I_ShutdownGraphics\n");

   if (!graphics_started)
      return;

   ShutdownDIVE( pmData);

   graphics_started = false;
}

//
//  Initialize video mode, setup dynamic screen size variables,
//  and allocate screens.
//
void I_StartupGraphics(void)
{
   CONS_Printf("I_StartupGraphics\n");

   if (graphics_started)
      return;

   if (M_CheckParm( "-mgl")) {
#if 0
      if (!MGL_init("..\\..\\..\\", NULL))
          MGL_fatalError("MGL init failed");
      MGL_enableAllDrivers();
      //if ((mglMode = MGL_findMode(SCREENWIDTH, SCREENHEIGHT, 8)) == -1)
      //  MGL_fatalError("Graphics mode not found");
#endif
   } else {
      InitDIVE( pmData);
   }

   //setup the videmodes list,
   CV_RegisterVar (&cv_vidwait);
   VID_SetMode(0);

   //added:03-01-98: register exit code for graphics
   I_AddExitFunc(I_ShutdownGraphics);
   graphics_started = true;
   CONS_Printf("I_StartupGraphics: DONE\n");
}

//added:30-01-98: return number of video modes in pvidmodes list
int VID_NumModes(void)
{
    return MAXWINMODES;
}

//added:03-02-98: return a video mode number from the dimensions
int VID_GetModeForSize( int w, int h)
{
    int i;

   CONS_Printf("VID_GetModeForSize: %dx%d\n", w, h);

    for (i=0; i<MAXWINMODES;i++)
        if(windowedModes[i][0]==w && windowedModes[i][1]==h)
            return i;

   CONS_Printf("VID_GetModeForSize: %dx%d not found\n", w, h);

    return 0;
}

//added:30-01-98:return the name of a video mode
char *VID_GetModeName (int modenum)
{
   sprintf( vidModeName[modenum], "%dx%d", 
            windowedModes[modenum][0],
            windowedModes[modenum][1]);
   //CONS_Printf("VID_GetModeName: %s\n", vidModeName[modenum]);
   return vidModeName[modenum];
}

// ========================================================================
// Sets a video mode
// ========================================================================
int VID_SetMode (int modenum)  //, unsigned char *palette)
{
   CONS_Printf("VID_SetMode(%d)\n", modenum);

   if (modenum >= MAXWINMODES) {
       printf("VID_SetMode modenum >= MAXWINMODES\n");
       return -1;
   }
/*
   if (pmData->pbBuffer) { // init code only once
       printf("VID_SetMode already called\n");
       return -1;
   }
*/
   // initialize vidbuffer size for setmode
   vid.width  = windowedModes[modenum][0];
   vid.height = windowedModes[modenum][1];
   //vid.aspect = pcurrentmode->aspect;
   printf("VID_SetMode %dx%d\n", vid.width, vid.height);

   // adjust window size
   pmData->ulWidth = vid.width;
   pmData->ulHeight = vid.height;
   WinPostMsg( pmData->hwndClient, WM_COMMAND, (MPARAM) ID_NEWTEXT, NULL);
   WinPostMsg( pmData->hwndClient, WM_COMMAND, (MPARAM) ID_SNAP, NULL);

   //if (pmData->pbBuffer)
   //    ShutdownDIVE( pmData);
   //pmData->pbBuffer = 0;
   InitDIVEBuffer( pmData);

   pmData->currentImage = 0;
   pmData->fDataInProcess = TRUE;
   vid.buffer = (byte*) pmData->pbBuffer; //;//

   //added:20-01-98: recalc all tables and realloc buffers based on
   //                vid values.
   vid.rowbytes = vid.width;
   vid.bpp      = 1;
   vid.recalc   = 1;
   vid.modenum  = modenum;

   printf("VID_SetMode(%d) DONE\n", modenum);
   return 1;
}
