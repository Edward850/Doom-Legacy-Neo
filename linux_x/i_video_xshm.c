// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_video_xshm.c,v 1.30 2002/07/01 19:59:59 metzgermeister Exp $
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
// $Log: i_video_xshm.c,v $
// Revision 1.30  2002/07/01 19:59:59  metzgermeister
// *** empty log message ***
//
// Revision 1.29  2001/12/31 16:56:39  metzgermeister
// see Dec 31 log
// .
//
// Revision 1.28  2001/08/20 20:40:42  metzgermeister
// *** empty log message ***
//
// Revision 1.27  2001/04/27 13:32:14  bpereira
// no message
//
// Revision 1.26  2001/04/14 14:17:52  metzgermeister
// clean up
//
// Revision 1.25  2001/03/30 17:12:52  bpereira
// no message
//
// Revision 1.24  2001/03/12 21:03:10  metzgermeister
//   * new symbols for rendererlib added in SDL
//   * console printout fixed for Linux&SDL
//   * Crash fixed in Linux SW renderer initialization
//
// Revision 1.23  2001/02/24 13:35:22  bpereira
// no message
//
// Revision 1.22  2001/02/19 23:55:26  hurdler
// Update to match win32 source code
//
// Revision 1.21  2001/02/13 20:37:27  metzgermeister
// *** empty log message ***
//
// Revision 1.20  2001/01/25 22:15:45  bpereira
// added heretic support
//
// Revision 1.19  2000/11/04 16:23:45  bpereira
// no message
//
// Revision 1.18  2000/11/02 19:49:40  bpereira
// no message
//
// Revision 1.17  2000/10/08 13:30:02  bpereira
// no message
//
// Revision 1.16  2000/09/10 10:51:02  metzgermeister
// added vid_wait
//
// Revision 1.15  2000/08/21 21:14:31  metzgermeister
// Voodoo3/Banshee fix
//
// Revision 1.14  2000/08/11 19:11:07  metzgermeister
// *** empty log message ***
//
// Revision 1.12  2000/05/13 19:53:54  metzgermeister
// voodoo fullscreen & remote display
//
// Revision 1.11  2000/05/07 08:29:03  metzgermeister
// added windowed voodoo mode
//
// Revision 1.10  2000/04/22 20:27:35  metzgermeister
// support for immediate fullscreen switching
//
// Revision 1.9  2000/04/18 23:47:00  metzgermeister
// bugfixes
//
// Revision 1.8  2000/04/16 20:10:51  metzgermeister
// screensaver handling
//
// Revision 1.7  2000/04/12 19:32:50  metzgermeister
// new Voodoo and fullscreen support
//
// Revision 1.6  2000/04/07 23:10:15  metzgermeister
// fullscreen support under X in Linux
//
// Revision 1.5  2000/03/23 22:40:49  metzgermeister
// added support for automatic 3dfx fullscreen
//
// Revision 1.4  2000/03/07 03:30:49  hurdler
// fix linux compilation
//
// Revision 1.3  2000/03/06 15:19:58  hurdler
// Add Bell Kin's changes
//
// Revision 1.2  2000/02/27 00:42:12  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <X11/extensions/XShm.h>
#include <X11/extensions/xf86vmode.h>

#ifdef WITH_DGA
#include <X11/extensions/xf86dga.h>
#endif


// Had to dig up XShm.c for this one.
// It is in the libXext, but not in the X headers.
//#if defined(LINUX)
int XShmGetEventBase( Display* dpy );
//#endif

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dlfcn.h> // added 19990831 by Kin
#include <sys/socket.h>

#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#if defined(SCOOS5) || defined(SCOUW2) || defined(SCOUW7)
#include "strcmp.h"
#endif
// it must be here 19990117 by Kin
#include "doomdef.h"

#define DONTDEFINEBOOL
#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "m_menu.h"
#include "d_main.h"
#include "s_sound.h"
// added for 1.27 19990220 by Kin
#include "g_input.h"
#include "st_stuff.h"
#include "g_game.h"
#include "i_video.h"
#include "hardware/hw_main.h"
#include "hardware/hw_drv.h"
#include "hardware/hw_glob.h"
#include "console.h"
#include "command.h"
#include "d_clisrv.h" // for dedicated

void VID_PrepareModeList(void);

// maximum number of windowed modes for X11 (see windowedModes[][])
#define MAXWINMODES (6) 
#define NUM_VOODOOMODES (3)

// resolution threshold for hires mode
#define HIRES_HORIZ (640)
#define HIRES_VERT  (400)
 
static boolean haveVoodoo = false;

extern consvar_t cv_fullscreen; // for fullscreen support under X and GLX

extern short color8to16[];
static boolean showkey=false; // force to no 19990118 by Kin
static boolean verbose=false; // enable/disable some printouts
static boolean vidmode_ext; // Are videmode extensions available?
static boolean vidmode_active;
static int num_fullvidmodes;
static int num_vidmodes = 0;
static int lowest_vidmode;

static XF86VidModeModeInfo **vidmodes;
static char vidModeName[33][32]; // allow 33 different modes
static int vidmap[33];

// added for 1.27 19990220 by Kin
rendermode_t    rendermode=render_soft;
boolean highcolor = false;

// synchronize page flipping with screen refresh
// unused and for compatibilityy reason 
consvar_t       cv_vidwait = {"vid_wait","1",CV_SAVE,CV_OnOff};

byte graphics_started = 0;
// To disable fullscreen at startup; is set in VID_PrepareModeList
boolean allow_fullscreen = false;

static Display*        X_display=NULL;
static Window          X_mainWindow=0;
static Colormap        X_cmap;
static Visual*         X_visual;
static GC              X_gc;
static XEvent          X_event;
static int             X_screen;
static XVisualInfo     X_visualinfo;
static XSizeHints      X_size;
static XWMHints        X_wm;
static XClassHint      X_class;
static Atom            X_wm_delwin;
static int             X_width;
static int             X_height;
static XImage*         image = NULL;

static Window   dummy;
static int      dont_care;

static boolean localDisplay = true;

// MIT SHared Memory extension.
boolean         doShm;

XShmSegmentInfo X_shminfo;
int             X_shmeventtype;

// Mouse handling.
boolean         Mousegrabbed = false;

event_t event;

// X visual mode
static int      x_depth=1;
static int      x_bpp=1;
static int      x_pseudo=1;
static unsigned short* x_colormap2 = 0;
static unsigned char* x_colormap3 = 0;
static unsigned long* x_colormap4 = 0;
static unsigned long x_red_mask = 0;
static unsigned long x_green_mask = 0;
static unsigned long x_blue_mask = 0;
static unsigned char x_red_offset = 0;
static unsigned char x_green_offset = 0;
static unsigned char x_blue_offset = 0;

// X11 video modes from which to choose from.

static int windowedModes[MAXWINMODES][2] = {
   {1024, 768},
   {800, 600},
   {640, 480},
   {512, 384},
   {400, 300},
   {320, 200}};

// These are modes for 3dfx voodoo graphics (loopthrough) cards
static int voodooModes[NUM_VOODOOMODES][2] = {
    {800, 600},
    {640, 480},
    {512, 384}};

static void determineVidModes(void)
{
   int i, firstEntry;
   boolean finished;
   int lowest_vidmode;

   if(vidmode_ext) {
       // get fullscreen modes
       XF86VidModeGetAllModeLines(X_display, X_screen, &num_fullvidmodes, &vidmodes);
       num_vidmodes = num_fullvidmodes;
       
       // initialize mapping
       for(i=0; i<num_vidmodes; i++) 
           vidmap[i] = i;
       
       // bubble sort modes
       do {
           int temp;
           
           finished = true;
           
           for(i=0; i<num_vidmodes-1; i++) {
               if(vidmodes[vidmap[i  ]]->hdisplay * vidmodes[vidmap[i  ]]->vdisplay <
                  vidmodes[vidmap[i+1]]->hdisplay * vidmodes[vidmap[i+1]]->vdisplay) {
                   
                   temp = vidmap[i];
                   vidmap[i] = vidmap[i+1];
                   vidmap[i+1] = temp;
                   
                   finished = false;
               }
           }
       } while(!finished);
       
       // exclude modes which are too large (to prevent X-Server problems)
       firstEntry = num_vidmodes;
       for(i=0; i<num_vidmodes; i++) {
           if(vidmodes[vidmap[i]]->hdisplay <= windowedModes[0][0] &&
              vidmodes[vidmap[i]]->vdisplay <= windowedModes[0][1]) {
               
               firstEntry = i;
               break;
           }
       }
       
       // copy modes
       for(i=0; i<num_vidmodes-firstEntry; i++) {
           vidmap[i] = vidmap[i+firstEntry];
       }
       num_vidmodes -= firstEntry;
       
       lowest_vidmode = num_vidmodes - 1;
   }
   else {
       num_vidmodes = 0;
   }
   return;
}


static void checkVidModeExtension(void) 
{
   int MajorVersion, MinorVersion;

   MajorVersion = MinorVersion = 0;

   // disable extensions for non-local displays
   if(!localDisplay){
       vidmode_ext = false;
       return;
   }
   
   if (!XF86VidModeQueryVersion(X_display, &MajorVersion, &MinorVersion)) { 
      vidmode_ext = false;
   } else {
      CONS_Printf("Using XFree86-VidModeExtension Version %d.%d\n", MajorVersion, MinorVersion);
      vidmode_ext = true;
   }
   return;
}

static void findVisual(void) 
{
   
   X_screen = DefaultScreen(X_display);
   if (XMatchVisualInfo(X_display, X_screen, 8, PseudoColor, &X_visualinfo))
      { x_depth = 1; x_pseudo = 1; }
   else if
      (XMatchVisualInfo(X_display, X_screen, 16, TrueColor, &X_visualinfo))
      { x_depth = 2; x_pseudo = 0; }
   else if
      (XMatchVisualInfo(X_display, X_screen, 24, TrueColor, &X_visualinfo))
      { x_depth = 3; x_pseudo = 0; }
   else
      I_Error("no supported visual found");
   X_visual = X_visualinfo.visual;
   
   return;
}

static void determineColorMask(void) 
{

   x_red_mask = X_visual->red_mask;
   x_green_mask = X_visual->green_mask;
   x_blue_mask = X_visual->blue_mask;
   
   if (x_depth==3) {
      switch (x_red_mask) {
#ifdef BIGEND
      case 0x000000ff: x_red_offset = 3; break;
      case 0x0000ff00: x_red_offset = 2; break;
      case 0x00ff0000: x_red_offset = 1; break;
      case 0xff000000: x_red_offset = 0; break;
#else
      case 0x000000ff: x_red_offset = 0; break;
      case 0x0000ff00: x_red_offset = 1; break;
      case 0x00ff0000: x_red_offset = 2; break;
      case 0xff000000: x_red_offset = 3; break;
#endif
      }
      switch (x_green_mask) {
#ifdef BIGEND
      case 0x000000ff: x_green_offset = 3; break;
      case 0x0000ff00: x_green_offset = 2; break;
      case 0x00ff0000: x_green_offset = 1; break;
      case 0xff000000: x_green_offset = 0; break;
#else
      case 0x000000ff: x_green_offset = 0; break;
      case 0x0000ff00: x_green_offset = 1; break;
      case 0x00ff0000: x_green_offset = 2; break;
      case 0xff000000: x_green_offset = 3; break;
#endif
      }
      switch (x_blue_mask) {
#ifdef BIGEND
      case 0x000000ff: x_blue_offset = 3; break;
      case 0x0000ff00: x_blue_offset = 2; break;
      case 0x00ff0000: x_blue_offset = 1; break;
      case 0xff000000: x_blue_offset = 0; break;
#else
      case 0x000000ff: x_blue_offset = 0; break;
      case 0x0000ff00: x_blue_offset = 1; break;
      case 0x00ff0000: x_blue_offset = 2; break;
      case 0xff000000: x_blue_offset = 3; break;
#endif
      }
   }
   if (x_depth==2) {
      // for 16bpp, x_*_offset specifies the number of bits to shift
      unsigned long mask;
      
      mask = x_red_mask;
      x_red_offset = 0;
      while (!(mask&1)) {
         x_red_offset++;
         mask >>= 1;
      }
      x_red_mask = 8;
      while (mask&1) {
         x_red_mask--;
         mask >>= 1;
      }
      
      mask = x_green_mask;
      x_green_offset = 0;
      while (!(mask&1)) {
         x_green_offset++;
         mask >>= 1;
      }
      x_green_mask = 8;
      while (mask&1) {
         x_green_mask--;
         mask >>= 1;
      }
      
      mask = x_blue_mask;
      x_blue_offset = 0;
      while (!(mask&1)) {
         x_blue_offset++;
         mask >>= 1;
      }
      x_blue_mask = 8;
      while (mask&1) {
         x_blue_mask--;
         mask >>= 1;
      }
   }
   return;
}

static void determineBPP(void) 
{
   int count;
   XPixmapFormatValues* X_pixmapformats;
   
   X_pixmapformats = XListPixmapFormats(X_display,&count);
   
   if (X_pixmapformats) {
      int i;
      x_bpp=0;
      for (i=0;i<count;i++) {
         if (X_pixmapformats[i].depth == x_depth*8) {
            x_bpp = X_pixmapformats[i].bits_per_pixel/8; break;
         }
      }
      if (x_bpp==0)
         I_Error("Could not determine bits_per_pixel");
      XFree(X_pixmapformats);
   } else
      I_Error("Could not get list of pixmap formats");
   return;
}

static char *initDisplay(void)
{
    int pnum;
    char *displayname, *d, displaycopy[256];
    
    // check for command-line display name
    if((pnum = M_CheckParm("-display")) && (pnum < myargc-1)) 
        displayname = myargv[pnum+1];
    else
        displayname = NULL;
    
    // open the display
    X_display = XOpenDisplay(displayname);

    if (!X_display)
    {
        if (displayname)
            I_Error("Could not open display [%s]", displayname);
        else
            I_Error("Could not open display (DISPLAY=[%s])", getenv("DISPLAY"));
    }
    
    if(!displayname) 
        displayname = (char *) getenv("DISPLAY");
    
    // check for local display
    if(displayname) {
        strncpy(displaycopy, displayname, 256);
        d = displaycopy;
        while (*d && (*d != ':')) d++;
        if (*d) *d = 0;
        if (!(!strcasecmp(displaycopy, "unix") || !*displaycopy))
            localDisplay = false;
    }
    
    return displayname;
}

static void checkForShm(char *displayname) // FIXME: why do we need displayname ??
{
    if(rendermode==render_soft) {
        // check for the MITSHM extension
        doShm = XShmQueryExtension(X_display);

        // even if it's available, make sure it's a local connection
        if(doShm && localDisplay) {
            doShm = true;
            fprintf(stderr, "Using MITSHM extension\n");
        }
        else
            doShm = false;
    } else //if(rendermode==render_soft)
        doShm = false;
    
    return;
}

static void createColorMap()
{
   if (x_pseudo)
      X_cmap = XCreateColormap(X_display, RootWindow(X_display, X_screen),
                               X_visual, AllocAll);
   else if (x_bpp==2)
      x_colormap2 = &color8to16[0]; // cheat...19990119 by Kin
   else if (x_bpp==3)
      x_colormap3 = malloc(3*256);
   else if (x_bpp==4)
      x_colormap4 = malloc(4*256);
   return;
}

static int dirtyMapTable[256] =
{0, 0, 0, 0, 0, 0, 0, 0, 0, 65307, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48,
 45, 61, 65288, 65289, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91,
 93, 65293, 65507, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 39, 96,
 65505, 92, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, 65506, 65450, 65511,
 32, 65509, 65470, 65471, 65472, 65473, 65474, 65475, 65476, 65477, 65478,
 65479, 65407, 65300, 65429, 65431, 65434, 65453, 65430, 65437, 65432, 65451,
 65436, 65433, 65435, 65438, 65439, 0, 0, 0, 65480, 65481, 65360, 65362, 65365,
 65361, 0, 65363, 65367, 65364, 65366, 65379, 65535, 65421, 65312, 65299, 65377,
 65455, 65514, 0, 65515, 65516, 65518, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
//
//  Translates the key currently in X_event
//
static int xlatekey(void)
{
    KeyCode keycode;
    KeySym keysym;
    int rc;

    keycode = X_event.xkey.keycode;
    keysym = XKeycodeToKeysym(X_display, keycode, 0);

    if(con_keymap == english) {
        rc = keysym;
    }
    else {
        rc = dirtyMapTable[keycode];
    }

    switch(rc)
    {
      case XK_Left  : rc = KEY_LEFTARROW;     break;
      case XK_Right : rc = KEY_RIGHTARROW;    break;
      case XK_Down  : rc = KEY_DOWNARROW;     break;
      case XK_Up    : rc = KEY_UPARROW;       break;

      case XK_Escape:   rc = KEY_ESCAPE;        break;
      case XK_Return:   rc = KEY_ENTER;         break;
      case XK_Tab:      rc = KEY_TAB;           break;
      case XK_F1:       rc = KEY_F1;            break;
      case XK_F2:       rc = KEY_F2;            break;
      case XK_F3:       rc = KEY_F3;            break;
      case XK_F4:       rc = KEY_F4;            break;
      case XK_F5:       rc = KEY_F5;            break;
      case XK_F6:       rc = KEY_F6;            break;
      case XK_F7:       rc = KEY_F7;            break;
      case XK_F8:       rc = KEY_F8;            break;
      case XK_F9:       rc = KEY_F9;            break;
      case XK_F10:      rc = KEY_F10;           break;
      case XK_F11:      rc = KEY_F11;           break;
      case XK_F12:      rc = KEY_F12;           break;
      // hey, it's not a sparc 19990128 by Kin
      case XK_BackSpace: rc = KEY_BACKSPACE;    break;
      case XK_Delete:   rc = KEY_DEL;   break;

      case XK_Pause:    rc = KEY_PAUSE;         break;

      case XK_KP_Equal:
      case XK_KP_Add:
      case XK_equal:    rc = KEY_EQUALS;        break;

      case XK_KP_Subtract:
      case XK_minus:    rc = KEY_MINUS;         break;

      case XK_Shift_L:
      case XK_Shift_R:
        rc = KEY_SHIFT;
        break;
        
      case XK_Caps_Lock:
        rc = KEY_CAPSLOCK;
        break;

      case XK_Multi_key:
      case XK_Control_L:
      case XK_Control_R:
        rc = KEY_CTRL;
        break;
        
      case XK_Mode_switch:
      case XK_Alt_L:
      case XK_Meta_L:
      case XK_Alt_R:
      case XK_Meta_R:
        rc = KEY_ALT;
        break;

      // I forgot them..... 19990128 by Kin
      case XK_Page_Up: rc = KEY_PGUP; break;
      case XK_Page_Down: rc = KEY_PGDN; break;
      case XK_End: rc = KEY_END; break;
      case XK_Home: rc = KEY_HOME; break;
      case XK_Insert: rc = KEY_INS; break;
      
      // metzgermeister: keypad layout as in g_input.c
      case XK_KP_Insert    : rc = KEY_KEYPAD0;  break;
      case XK_KP_End       : rc = KEY_KEYPAD1;  break;
      case XK_KP_Down      : rc = KEY_KEYPAD2;  break;
      case XK_KP_Page_Down : rc = KEY_KEYPAD3;  break;
      case XK_KP_Left      : rc = KEY_KEYPAD4;  break;
      case XK_KP_Begin     : rc = KEY_KEYPAD5;  break;
      case XK_KP_Right     : rc = KEY_KEYPAD6;  break;
      case XK_KP_Home      : rc = KEY_KEYPAD7;  break;
      case XK_KP_Up        : rc = KEY_KEYPAD8;  break;
      case XK_KP_Page_Up   : rc = KEY_KEYPAD9;  break;
      case XK_KP_Delete    : rc = KEY_KPADDEL;  break;
      case XK_KP_Divide    : rc = KEY_KPADSLASH; break;
      case XK_KP_Enter     : rc = KEY_ENTER;    break;

      default:
        if (rc >= XK_space && rc <= XK_asciitilde)
            rc = rc - XK_space + ' ';
        if (rc >= 'A' && rc <= 'Z')
            rc = rc - 'A' + 'a';
        break;
    }

    if (showkey)
      fprintf(stdout,"Key: %d\n", rc);

    return rc;

}



//
// I_StartFrame
//
void I_StartFrame(void)
{
        /* frame syncronous IO operations not needed for X11 */
}

static int      lastmousex = 0;
static int      lastmousey = 0;
#ifdef POLL_POINTER
static int      newmousex;
static int      newmousey;
// last button state from dos code... 19990110 by Kin
static int      lastbuttons = 0;
#endif
boolean         shmFinished;

#ifdef LJOYSTICK
extern void I_GetJoyEvent();
#endif
#ifdef LMOUSE2
extern void I_GetMouse2Event();
#endif
void I_GetEvent(void)
{

    event_t event;
#ifdef LJOYSTICK
    I_GetJoyEvent();
#endif
#ifdef LMOUSE2
    I_GetMouse2Event();
#endif
    // put event-grabbing stuff in here
    XNextEvent(X_display, &X_event);
    switch (X_event.type)
    {
      case KeyPress:
        event.type = ev_keydown;
        event.data1 = xlatekey();
        D_PostEvent(&event);
        break;

      case KeyRelease:
        event.type = ev_keyup;
        event.data1 = xlatekey();
        D_PostEvent(&event);
        break;

#ifndef POLL_POINTER
      case ButtonPress:
        if (cv_usemouse.value) {
/*
            int buttons,i,j=1,k;
            event.type = ev_keydown;
            buttons =
                (X_event.xbutton.state & Button1Mask ? 1 : 0)
                | (X_event.xbutton.state & Button2Mask ? 2 : 0)
                | (X_event.xbutton.state & Button3Mask ? 4 : 0)
                | (X_event.xbutton.button == Button1 ? 1 : 0)
                | (X_event.xbutton.button == Button2 ? 2 : 0)
                | (X_event.xbutton.button == Button3 ? 4 : 0);
            k=(buttons ^ lastbuttons); // only changed bit to 1
            lastbuttons = buttons;
            for(i=0;i<MOUSEBUTTONS;i++,j<<=1)
                if(k & j) {
                    event.data1=KEY_MOUSE1+i;
                    D_PostEvent(&event);
                    break; // right? 19990110 by Kin
                }
*/
           int button = X_event.xbutton.button;
           if(button && button-1 < MOUSEBUTTONS)
           {
                   event.type = ev_keydown;
                   if(button==4)
                   {
                       event.data1 = KEY_MOUSEWHEELUP;
                   }
                   else if(button==5)
                   {
                       event.data1 = KEY_MOUSEWHEELDOWN;
                   }
                   else
                   {
                       event.data1=KEY_MOUSE1+button-1;
                   }
                   D_PostEvent(&event);
	   }
        }
        break;

      case ButtonRelease:
        if (cv_usemouse.value)
        {
/*
          int buttons,i,j=1,k;
          event.type = ev_keyup;
          buttons =
            (X_event.xbutton.state & Button1Mask ? 1 : 0)
            | (X_event.xbutton.state & Button2Mask ? 2 : 0)
            | (X_event.xbutton.state & Button3Mask ? 4 : 0);
          // suggest parentheses around arithmetic in operand of |
          buttons =
            buttons
            ^ (X_event.xbutton.button == Button1 ? 1 : 0)
            ^ (X_event.xbutton.button == Button2 ? 2 : 0)
            ^ (X_event.xbutton.button == Button3 ? 4 : 0);
          k=(buttons ^ lastbuttons); // only changed bit to 1
          lastbuttons = buttons;
          for(i=0;i<MOUSEBUTTONS;i++,j<<=1)
                if(k & j)
                {
                    event.data1=KEY_MOUSE1+i;
                    D_PostEvent(&event);
                    break; // right? 19990110 by Kin
                }
*/
           int button = X_event.xbutton.button;
           if(button && button-1 < MOUSEBUTTONS)
           {
                   event.type = ev_keyup;
                   if(button==4||button==5)
                   {
			   //ignore
                   }
                   else
                   {
                       event.data1=KEY_MOUSE1+button-1;
		       D_PostEvent(&event);
                   }
	   }
        }
        break;

      case MotionNotify:
        if (cv_usemouse.value)
        {
#ifdef WITH_DGA
          event.type = ev_mouse;
	  event.data1 = 0;
	  event.data2 = X_event.xmotion.x_root << 2;
	  event.data3 = -X_event.xmotion.y_root << 2;
          D_PostEvent(&event);
#else
          // If the event is from warping the pointer back to middle
          // of the screen then ignore it.
          if ((X_event.xmotion.x == X_width/2) &&
            (X_event.xmotion.y == X_height/2)) {
            lastmousex = X_event.xmotion.x;
            lastmousey = X_event.xmotion.y;
            break;
          } else {
            event.data2 = (X_event.xmotion.x - lastmousex) << 2;
            lastmousex = X_event.xmotion.x;
            event.data3 = (lastmousey - X_event.xmotion.y) << 2;
            lastmousey = X_event.xmotion.y;
          }
          event.type = ev_mouse;
          event.data1 =
            (X_event.xmotion.state & Button1Mask ? 1 : 0)
            | (X_event.xmotion.state & Button2Mask ? 2 : 0)
            | (X_event.xmotion.state & Button3Mask ? 4 : 0);
          D_PostEvent(&event);
          // Warp the pointer back to the middle of the window
          //  or we cannot move any further if it's at a border.
          if ((X_event.xmotion.x < 1) || (X_event.xmotion.y < 1)
            || (X_event.xmotion.x > X_width-2)
            || (X_event.xmotion.y > X_height-2))
          {
                XWarpPointer(X_display,
                          None,
                          X_mainWindow,
                          0, 0,
                          0, 0,
                          X_width/2, X_height/2);
          }
#endif // #WITH_DGA
        }
        break;
#endif
        
      case UnmapNotify:
        if (!demoplayback) {
          paused = true;
          S_PauseSound();
        }
        break;

      case MapNotify:
        if (!demoplayback) {
          paused = false;
          S_ResumeSound();
        }
        break;

      case ClientMessage:
        if (X_event.xclient.data.l[0] == X_wm_delwin) {
          M_QuitResponse('y');
        }
        break;

      case Expose:
      case ConfigureNotify:
        break;
        
      default:
        if (doShm && X_event.type == X_shmeventtype)
            shmFinished = true;
        break;

    }

}

static Cursor createnullcursor(Display* display, Window root)
{
    Pixmap cursormask;
    XGCValues xgc;
    GC gc;
    XColor dummycolour;
    Cursor cursor;

    cursormask = XCreatePixmap(display, root, 1, 1, 1/*depth*/);
    xgc.function = GXclear;
    gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
    XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor(display, cursormask, cursormask,
                                 &dummycolour,&dummycolour, 0,0);
    XFreePixmap(display,cursormask);
    XFreeGC(display,gc);
    return cursor;
}

static void disableScreensaver()
{
   int timeout, interval, prefer_blanking, allow_exposures;
   
   XGetScreenSaver(X_display, &timeout, &interval, &prefer_blanking, &allow_exposures);
   XSetScreenSaver(X_display,        0,  interval,  prefer_blanking,  allow_exposures);
   XGetScreenSaver(X_display, &timeout, &interval, &prefer_blanking, &allow_exposures);
      
}

static void enableScreensaver()
{
   int timeout, interval, prefer_blanking, allow_exposures;
   
   XGetScreenSaver(X_display, &timeout, &interval, &prefer_blanking, &allow_exposures);
   XSetScreenSaver(X_display,        -1,  interval,  prefer_blanking,  allow_exposures);
   XGetScreenSaver(X_display, &timeout, &interval, &prefer_blanking, &allow_exposures);
      
}

static void doGrabMouse(void)
{
    if(!X_display || cv_usemouse.value == 0 || M_CheckParm("-nomouse"))
        return;

    if(!Mousegrabbed) {
#ifdef WITH_DGA
	CONS_Printf("enable DGA mouse\n");
	XF86DGADirectVideo(X_display, X_screen, XF86DGADirectMouse);
#endif
        XGrabPointer(X_display, X_mainWindow, True,
#ifndef POLL_POINTER
                     ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
#else
                     0,
#endif
                     GrabModeAsync, GrabModeAsync,
                     X_mainWindow, None, CurrentTime);
        XGrabKeyboard(X_display, X_mainWindow, True, 
                      GrabModeAsync, GrabModeAsync,
                      CurrentTime);
#ifdef POLL_POINTER
        XQueryPointer(X_display, X_mainWindow, &dummy, &dummy,
                      &dont_care, &dont_care,
                      &lastmousex, &lastmousey,
                      &dont_care);
#endif
        disableScreensaver();
    }

    Mousegrabbed = true;
    return;
}

static void doUngrabMouse(void)
{
    if(!X_display)
        return;

    if(Mousegrabbed) {
#ifdef WITH_DGA
	CONS_Printf("disable DGA mouse\n");
	XF86DGADirectVideo(X_display, X_screen, 0);
#endif
        XUngrabKeyboard(X_display, CurrentTime);
        XUngrabPointer(X_display, CurrentTime);
        enableScreensaver();
	XSync(X_display, False);
    }

    Mousegrabbed = false;
    return;
}


void I_StartupMouse(void)
{
    if(vidmode_active || (haveVoodoo && !M_CheckParm("-winvoodoo"))) {
        doGrabMouse();
    }
    else {
        if(cv_usemouse.value) {
            doGrabMouse();
        }
        else {
            doUngrabMouse();
        }
    }
    return;
}

//
// I_OsPolling
//
void I_OsPolling(void)
{
#ifdef POLL_POINTER
    unsigned int mask;
#endif

    if (!X_display)
        return;

    if (cv_usemouse.value) {
#ifdef POLL_POINTER
        XQueryPointer(X_display, X_mainWindow, &dummy, &dummy,
                      &dont_care, &dont_care,
                      &newmousex, &newmousey,
                      &mask);
        { // process mouse 19990130 by Kin
            int buttons,i,j=1,k;
            buttons =
                (mask & Button1Mask ? 1 : 0)
                | (mask & Button2Mask ? 2 : 0)
                | (mask & Button3Mask ? 4 : 0)
		//doesn't work, why?
                | (mask & Button4Mask ? 8 : 0)
                | (mask & Button5Mask ? 16 : 0);
            k=(buttons ^ lastbuttons); // only changed bit to 1
            for(i=0;i<MOUSEBUTTONS;i++,j<<=1)
                if(k & j){
                    if(buttons & j) event.type = ev_keydown;
                    else event.type = ev_keyup;
		    if(i==4) event.data1=KEY_MOUSEWHEELUP;
		    else if(i==5) event.data1=KEY_MOUSEWHEELDOWN;
                    else event.data1=KEY_MOUSE1+i;
                    D_PostEvent(&event);
                    // don't break!!! 19990130 by Kin
                }
            lastbuttons = buttons;
        }
        
        event.type = ev_mouse;
        //          event.data1 = ((mask & Button1Mask) ? 1 : 0) |
        //            ((mask & Button2Mask) ? 2 : 0) |
        //            ((mask & Button3Mask) ? 4 : 0);
        event.data2 = (newmousex - lastmousex) << 2;
        event.data3 = -((newmousey - lastmousey) << 2);
        D_PostEvent(&event);
        if ((newmousex < 1) || (newmousey < 1)
            || (newmousex > X_width-2)
            || (newmousey > X_height-2)) {
            
            XWarpPointer(X_display,
                         None,
                         X_mainWindow,
                         0, 0,
                         0, 0,
                         X_width/2, X_height/2);
            lastmousex = X_width/2;
            lastmousey = X_height/2;
        } else {
            lastmousex = newmousex;
            lastmousey = newmousey;
        }
#endif
    }
    
    while (XPending(X_display))
        I_GetEvent();

    //reset wheel like in win32, I don't understand it but works
    gamekeydown[KEY_MOUSEWHEELUP] = 0;
    gamekeydown[KEY_MOUSEWHEELDOWN] = 0;
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void)
{
        /* empty */
}

//
// I_FinishUpdate
//
void I_FinishUpdate(void)
{
    static int  lasttic;
    int         tics;
    int         i;
   if(rendermode==render_soft) {
    // draws little dots on the bottom of the screen
    if (devparm)
    {

        i = I_GetTime();
        tics = i - lasttic;
        lasttic = i;
        if (tics > 20) tics = 20;

        for (i=0 ; i<tics*2 ; i+=2)
            screens[0][ (vid.height-2)*vid.width + i + 3] = 0xff;
        for ( ; i<20*2 ; i+=2)
            screens[0][ (vid.height-2)*vid.width + i + 3] = 0x0;
    
    }

    // colormap transformation dependend on X server color depth
    if (x_bpp == 2) {
       int x,y;
       int xstart = vid.width-1;
       unsigned char* ilineptr;
       unsigned short* olineptr;
       y = vid.height;      
       while (y--) {
          olineptr =  (unsigned short *) &(image->data[y*X_width*x_bpp]);
          ilineptr = (unsigned char*) (screens[0]+y*X_width);
          x = xstart;
          do {
             olineptr[x] = x_colormap2[ilineptr[x]];
          } while (x--);
       }
    }
    else if (x_bpp == 3) {
       int x,y;
       int xstart = vid.width-1;
       unsigned char* ilineptr;
       unsigned char* olineptr;
       y = vid.height;
       while (y--) {
          olineptr =  (unsigned char *) &image->data[y*X_width*x_bpp];
          ilineptr = (unsigned char*) (screens[0]+y*X_width);
          x = xstart;
         do {
            memcpy(olineptr+3*x,x_colormap3+3*ilineptr[x],3);
         } while (x--);
       }
    }
    else if (x_bpp == 4) {
       int x,y;
       int xstart = vid.width-1;
       unsigned char* ilineptr;
       unsigned int* olineptr;
       y = vid.height;      
       while (y--) {
          olineptr =  (unsigned int *) &(image->data[y*X_width*x_bpp]);
          ilineptr = (unsigned char*) (screens[0]+y*X_width);
          x = xstart;
          do {
             olineptr[x] = x_colormap4[ilineptr[x]];
          } while (x--);
       }
    } else { // bpp = 1, multiply = 1 19990125 by Kin
      // VID_BlitLinearScreen does not work????
      //VID_BlitLinearScreen ( screens[0], image->data,vid.width,
      //  vid.height, vid.width, vid.width );
      memcpy(image->data,screens[0],vid.width*vid.height);
    }

    if (doShm)
    {
       
        if (!XShmPutImage(X_display,
                          X_mainWindow,
                          X_gc,
                          image,
                          0, 0,
                          0, 0,
                          X_width, X_height,
                          True ))
            I_Error("XShmPutImage() failed\n");

        // wait for it to finish and processes all input events
        shmFinished = false;
        do
        {
            if (XPending(X_display))
                I_GetEvent();
            else
                I_WaitVBL(1);
        } while (!shmFinished);

    }
    else
    {

        // draw the image
        XPutImage(      X_display,
                        X_mainWindow,
                        X_gc,
                        image,
                        0, 0,
                        0, 0,
                        X_width, X_height );

    }
   } else {
       HWD.pfnFinishUpdate(cv_vidwait.value); 
   }
}


//
// I_ReadScreen
//
void I_ReadScreen(byte* scr)
{
    memcpy (scr, screens[0], vid.width*vid.height);
}


//
// Palette stuff.
//
static XColor colors[256];

static void UploadNewPalette(Colormap cmap, RGBA_t *palette)
{

    register int        i;
    register int        c;
    static boolean      firstcall = true;

#ifdef __cplusplus
    if (X_visualinfo.c_class == PseudoColor && X_visualinfo.depth == 8)
#else
    if (X_visualinfo.class == PseudoColor && X_visualinfo.depth == 8)
#endif
        {
            // initialize the colormap
            if (firstcall)
            {
                firstcall = false;
                for (i=0 ; i<256 ; i++)
                {
                    colors[i].pixel = i;
                    colors[i].flags = DoRed|DoGreen|DoBlue;
                }
            }

            // set the X colormap entries
            for (i=0 ; i<256 ; i++,palette++)
            {
                c = palette->s.red;
                colors[i].red = (c<<8) + c;
                c = palette->s.green;
                colors[i].green = (c<<8) + c;
                c = palette->s.blue;
                colors[i].blue = (c<<8) + c;
            }

            // store the colors to the current colormap
            XStoreColors(X_display, cmap, colors, 256);

        }
}

static void EmulateNewPalette(RGBA_t *palette)
{
    register int        i;

    for (i=0 ; i<256 ; i++,palette++) 
    {
        switch(x_bpp) {
        case 2: x_colormap2[i] =
                   ((palette->s.red>>x_red_mask)<<x_red_offset) |
                   ((palette->s.green>>x_green_mask)<<x_green_offset) |
                   ((palette->s.blue>>x_blue_mask)<<x_blue_offset);
                break;
        case 3: x_colormap3[3*i+x_red_offset]   = palette->s.red;
                x_colormap3[3*i+x_green_offset] = palette->s.green;
                x_colormap3[3*i+x_blue_offset]  = palette->s.blue;
                break;
        case 4: x_colormap4[i] = 0;
               ((unsigned char*)(x_colormap4+i))[x_red_offset] = 
                   palette->s.red;
               ((unsigned char*)(x_colormap4+i))[x_green_offset] =
                   palette->s.green;
               ((unsigned char*)(x_colormap4+i))[x_blue_offset] =
                   palette->s.blue;
               break;
        }
    }
}

//
// I_SetPalette
//
void I_SetPalette(RGBA_t* palette)
{
    if( rendermode == render_soft )
    {
        if (x_pseudo)
            UploadNewPalette(X_cmap, palette);
        else
            EmulateNewPalette(palette);
    }
}


//
// This function is probably redundant,
//  if XShmDetach works properly.
// ddt never detached the XShm memory,
//  thus there might have been stale
//  handles accumulating.
//
static void grabsharedmemory(int size)
{

  int                   key = ('d'<<24) | ('o'<<16) | ('o'<<8) | 'm';
  struct shmid_ds       shminfo;
  int                   minsize = 320*200;
  int                   id;
  int                   rc;
  int                   pollution=5;
  
  // try to use what was here before
  do
  {
    id = shmget((key_t) key, minsize, 0777); // just get the id
    if (id != -1)
    {
      rc=shmctl(id, IPC_STAT, &shminfo); // get stats on it
      if (!rc) 
      {
        if (shminfo.shm_nattch)
        {
          fprintf(stderr, "User %d appears to be running "
                  "DOOM.  Is that wise?\n", shminfo.shm_cpid);
          key++;
        }
        else
        {
          if (getuid() == shminfo.shm_perm.cuid)
          {
            rc = shmctl(id, IPC_RMID, 0);
            if (!rc)
              fprintf(stderr,
                      "Was able to kill my old shared memory\n");
            else
              I_Error("Was NOT able to kill my old shared memory");
            
            id = shmget((key_t)key, size, IPC_CREAT|0777);
            if (id==-1)
              I_Error("Could not get shared memory");
            
            rc=shmctl(id, IPC_STAT, &shminfo);
            
            break;
          }
          if (size >= shminfo.shm_segsz)
          {
            fprintf(stderr,
                    "will use %d's stale shared memory\n",
                    shminfo.shm_cpid);
            break;
          }
          else
          {
            fprintf(stderr,
                    "warning: can't use stale "
                    "shared memory belonging to id %d, "
                    "key=0x%x\n",
                    shminfo.shm_cpid, key);
            key++;
          }
        }
      }
      else
      {
        I_Error("could not get stats on key=%d", key);
      }
    }
    else
    {
      id = shmget((key_t)key, size, IPC_CREAT|0777);
      if (id==-1)
      {
        extern int errno;
        fprintf(stderr, "errno=%d\n", errno);
        I_Error("Could not get any shared memory");
      }
      break;
    }
  } while (--pollution);
  
  if (!pollution)
  {
    I_Error("Sorry, system too polluted with stale "
            "shared memory segments.\n");
    }   
  
  X_shminfo.shmid = id;
  
  // attach to the shared memory segment
  image->data = X_shminfo.shmaddr = shmat(id, 0, 0);
  if(verbose)
  {
      fprintf(stderr, "shared memory id=%d, addr=0x%x\n", id,
              (int) (image->data));
  }
  return;
}

// return number of fullscreen + X11 modes
int   VID_NumModes(void) {

    if(haveVoodoo)
        return NUM_VOODOOMODES;
    else if(cv_fullscreen.value && vidmode_ext) 
        return num_vidmodes;
    else
        return MAXWINMODES;
}

char  *VID_GetModeName(int modenum) {
    static boolean displayWarning = true;
    
    // display a warning message if no lores modes are available under fullscreen
    if(displayWarning && cv_fullscreen.value && vidmode_ext && !haveVoodoo) {
        displayWarning = false; // do it only once

        if(vidmodes[vidmap[lowest_vidmode]]->hdisplay >= HIRES_HORIZ ||
           vidmodes[vidmap[lowest_vidmode]]->vdisplay >= HIRES_VERT) {
            // violation of hierarchical structure; use callback function instead?
            M_StartMessage("You don't have lores modes\nin your XF86Config\n\nPlease read the FAQ\n",NULL,MM_NOTHING);
        }
    }
    
    if(haveVoodoo) { // voodoo modes
        if(modenum >= NUM_VOODOOMODES)
            return NULL;
        
        sprintf(&vidModeName[modenum][0], "fx %dx%d",
                voodooModes[modenum][0],
                voodooModes[modenum][1]);
    }
    else if(cv_fullscreen.value && vidmode_ext) { // fullscreen modes
        if(modenum >= num_vidmodes)
            return NULL;
        
        sprintf(&vidModeName[modenum][0], "%dx%d", 
                vidmodes[vidmap[modenum]]->hdisplay,
                vidmodes[vidmap[modenum]]->vdisplay);
    }
    else { // X11 modes
        if(modenum > MAXWINMODES)
            return NULL;
        
        sprintf(&vidModeName[modenum][0], "X11 %dx%d",
                windowedModes[modenum][0],
                windowedModes[modenum][1]);
    }
    return &vidModeName[modenum][0];
}


int VID_GetModeForSize( int w, int h) {
    static boolean first_override=true;

    int best_fit, i;
   
    if(haveVoodoo) {
        best_fit = 1; // standard mode if no other found
        for (i = 0; i < NUM_VOODOOMODES; i++) {
            if (w == voodooModes[i][0] &&
                h == voodooModes[i][1]) {
                best_fit = i;
            }
        }
    }
    // scan fullscreen modes
    else if(cv_fullscreen.value && vidmode_ext) {
        best_fit = -1; 
        
        for (i = 0; i < num_vidmodes; i++) {
            if (w == vidmodes[vidmap[i]]->hdisplay &&
                h == vidmodes[vidmap[i]]->vdisplay) {
                best_fit = i;
            }
        }

        // !!! first_override prevents switching to fullscreen is no lores modes are available and no matching mode is found at startup
        // any other valid mode (windowed, match) disables first_override not to prevent the user from switching

        if(best_fit == -1) { // no fitting vidmode found
            if(first_override) { // overwrite lowest_vidmode setting the first time if resolution too high
                first_override=false;

                if(vidmodes[vidmap[lowest_vidmode]]->hdisplay *
                   vidmodes[vidmap[lowest_vidmode]]->vdisplay < HIRES_HORIZ*HIRES_VERT) { 
                    best_fit = lowest_vidmode; // use lowest fullscreen mode if it is not too hires
                }
                else { // if lowest fullscreen mode is too hires use lowest windowed mode
                    CV_SetValue(&cv_fullscreen, 0);
                    VID_PrepareModeList();
                    best_fit = MAXWINMODES-1;
                }
            }
            else {
                best_fit = lowest_vidmode;
            }
        }
        else if(first_override) first_override = false; // disable first_override
    }
        
    else { // windowed modes
        best_fit = MAXWINMODES-1;
        
        if(first_override) first_override = false; // disable first_override

        for (i = 0; i < MAXWINMODES; i++) {
            if (w == windowedModes[i][0] &&
                h == windowedModes[i][1]) {
                best_fit = i;
            }
        }
    }
    return best_fit ;
}

static void destroyWindow(void)
{
    doUngrabMouse();

    if(rendermode==render_soft)
    {
       if( vid.buffer )
       {
           free(vid.buffer);
           vid.buffer = vid.direct = screens[0] = NULL; // I want to have an access violation if something goes wrong
       }
       
       if(doShm) {
           if (!XShmDetach(X_display, &X_shminfo))
               I_Error("XShmDetach() failed in destroyWindow()");
           
           // Release shared memory.
           shmdt(X_shminfo.shmaddr);
           shmctl(X_shminfo.shmid, IPC_RMID, 0);
           
           if( image )
           {
               XDestroyImage(image); // is this correct? there is no shm counterpart
               image = NULL;
           }
       }
       else {
           if( image )
           {
               XDestroyImage(image);
               image = NULL;
           }
       }

       if( X_mainWindow )
       {
           XDestroyWindow(X_display, X_mainWindow);
           X_mainWindow=0;
       }
    }
    else {
        ; // actually do nothing here; HookXwin takes care; not very clean, but it's legacy
    }

   return;
}

static int createWindow(boolean isWindowedMode, int modenum)
{
   int                  oktodraw;
   unsigned long        attribmask;
   XSetWindowAttributes attribs;
   XGCValues            xgcvalues;
   XTextProperty        windowName, iconName;
   int                  valuemask;
   char                 *window_name = "Legacy";
   char                 *icon_name = window_name;

   // change to the mode
   if(isWindowedMode && vidmode_ext) {
      XF86VidModeSwitchToMode(X_display, X_screen, vidmodes[0]);
      vidmode_active = false;
   }
   else if(isWindowedMode && !vidmode_ext) { // probably not necessary
      vidmode_active = false;
   }
   else {
      XF86VidModeSwitchToMode(X_display, X_screen, vidmodes[vidmap[modenum]]);
      vidmode_active = true;
      // Move the viewport to top left
      XF86VidModeSetViewPort(X_display, X_screen, 0, 0);
   }
    if(rendermode==render_soft) {
    // setup attributes for main window
    if (vidmode_active) {
       attribmask = CWColormap | CWSaveUnder | CWBackingStore | 
          CWEventMask | CWOverrideRedirect;

       attribs.override_redirect = True;
       attribs.backing_store = NotUseful;
       attribs.save_under = False;
    } else
       attribmask = CWBorderPixel | CWColormap | CWEventMask;

    attribs.event_mask = KeyPressMask | KeyReleaseMask
#ifndef POLL_POINTER
       | PointerMotionMask | ButtonPressMask | ButtonReleaseMask
#endif
       | ExposureMask | StructureNotifyMask;
    
    attribs.colormap = X_cmap;
    attribs.border_pixel = 0;
    
    // create the main window
    X_mainWindow = XCreateWindow(X_display,
                                 RootWindow(X_display, X_screen),
                                 0, 0, // x, y,
                                 X_width, X_height,
                                 0, // borderwidth
                                 8*x_depth, // depth
                                 InputOutput,
                                 X_visual,
                                 attribmask,
                                 &attribs);

    if(!X_mainWindow)
        return 0;

    // create the GC
    valuemask = GCGraphicsExposures;
    xgcvalues.graphics_exposures = False;
    X_gc = XCreateGC(X_display,
                     X_mainWindow,
                     valuemask,
                     &xgcvalues );
    } else {
      X_mainWindow = HWD.pfnHookXwin(X_display, X_width, X_height, vidmode_active);
      if(X_mainWindow == 0) {
        return 0;
      }
    }

    // moved here
    XDefineCursor(X_display, X_mainWindow,
                  createnullcursor( X_display, X_mainWindow ) );

    // set size hints for window manager, so that resizing isn't possible
    X_size.flags = USPosition | PSize | PMinSize | PMaxSize;
    X_size.min_width = X_width;
    X_size.min_height = X_height;
    X_size.max_width = X_width;
    X_size.max_height = X_height;

    // window and icon name for the window manager
    XStringListToTextProperty(&window_name, 1, &windowName);
    XStringListToTextProperty(&icon_name, 1, &iconName);

    // window manager hints
    X_wm.initial_state = NormalState;
    X_wm.input = True;
    X_wm.flags = StateHint | InputHint;

    // property class, in case we get a configuration file sometime
    X_class.res_name = "legacy";
    X_class.res_class = "Legacy";

    // set the properties
    XSetWMProperties(X_display, X_mainWindow, &windowName, &iconName,
                0 /*argv*/, 0 /*argc*/, &X_size, &X_wm, &X_class);

    // set window manager protocol
    X_wm_delwin = XInternAtom(X_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(X_display, X_mainWindow, &X_wm_delwin, 1);

    // map the window
    XMapWindow(X_display, X_mainWindow);

    if (vidmode_active) {
       XMoveWindow(X_display, X_mainWindow, 0, 0);
       XRaiseWindow(X_display, X_mainWindow);
       XWarpPointer(X_display, None, X_mainWindow, 0, 0, 0, 0, 0, 0);
       XFlush(X_display);
       // Move the viewport to top left
       XF86VidModeSetViewPort(X_display, X_screen, 0, 0);
    }
    XFlush(X_display);

    // wait until it is OK to draw
    oktodraw = 0;
    while (!oktodraw)
    {
        XNextEvent(X_display, &X_event);
        if (X_event.type == Expose
            && !X_event.xexpose.count)
        {
            oktodraw = 1;
        }
    }

    // set the focus to the window
    XSetInputFocus(X_display, X_mainWindow, RevertToPointerRoot, CurrentTime);

    // get the pointer coordinates so that the first pointer move won't
    // be completely wrong
    XQueryPointer(X_display, X_mainWindow, &dummy, &dummy,
                  &dont_care, &dont_care,
                  &lastmousex, &lastmousey,
                  &dont_care);
  if(rendermode==render_soft) {
    if (doShm)
    {

        X_shmeventtype = XShmGetEventBase(X_display) + ShmCompletion;

        // create the image
        image = XShmCreateImage(X_display,
                                X_visual,
                                8*x_depth,
                                ZPixmap,
                                0,
                                &X_shminfo,
                                X_width,
                                X_height );

        grabsharedmemory(image->bytes_per_line * image->height);



        if (!image->data)
        {
            perror("");
            I_Error("shmat() failed in InitGraphics()");
        }

        // get the X server to attach to it
        if (!XShmAttach(X_display, &X_shminfo))
            I_Error("XShmAttach() failed in InitGraphics()");

    }
    else
    {
        image = XCreateImage(X_display,
                             X_visual,
                             8*x_depth,
                             ZPixmap,
                             0,
                             (char*)malloc(X_width * X_height * x_depth),
                             X_width, X_height,
                             8*x_depth,
                             X_width*x_bpp );

    }

    //    if (multiply == 1 && x_depth == 1)
    //  vid.buffer = vid.direct = screens[0] =
    //        (unsigned char *) (image->data);
    //    else
    // forced to use this in legacy 19990125 by Kin
    vid.buffer = vid.direct = screens[0] =
       (unsigned char *) malloc (vid.width * vid.height * NUMSCREENS);

    // added for 1.27 19990220 by Kin
    graphics_started = 1;
  } else {
    HWR_Startup();
    graphics_started = ((X_mainWindow==0)?0:1);
  }
   return 1;
}

void VID_PrepareModeList(void)
{
   int i, firstEntry;
   boolean finished;

    static boolean isVoodooChecked = false;
    char *rendererString, *voodooString;
    
    if(dedicated)
	return;
    
    // I cannot detect the Voodoo earlier, so I try to catch it here
    if(!isVoodooChecked) {
        if(rendermode == render_opengl) {
            rendererString = HWD.pfnGetRenderer();
            voodooString = strstr(rendererString, "Voodoo_Graphics"); // FIXME: if Mesa ever decides to change the this spotword, we're deep in the shit
            
            if(voodooString != NULL) {
                haveVoodoo = true;
            }
        }
        isVoodooChecked = true;
    }

    if(haveVoodoo) // nothing to do
        return;
   if(vidmode_ext && cv_fullscreen.value) {
      num_vidmodes = num_fullvidmodes;

      // initialize mapping
      for(i=0; i<num_vidmodes; i++) 
         vidmap[i] = i;
      
      // bubble sort modes
      do {
         int temp;

         finished = true;

         for(i=0; i<num_vidmodes-1; i++) {
            if(vidmodes[vidmap[i  ]]->hdisplay * vidmodes[vidmap[i  ]]->vdisplay <
               vidmodes[vidmap[i+1]]->hdisplay * vidmodes[vidmap[i+1]]->vdisplay) {

               temp = vidmap[i];
               vidmap[i] = vidmap[i+1];
               vidmap[i+1] = temp;

               finished = false;
            }
         }
      } while(!finished);

      // exclude modes which are too large (to prevent X-Server problems)
      firstEntry = num_vidmodes;
      for(i=0; i<num_vidmodes; i++) {
          if(vidmodes[vidmap[i]]->hdisplay <= 1024 && // FIXME: get rid of "magic" numbers
             vidmodes[vidmap[i]]->vdisplay <= 768) {
              
              firstEntry = i;
              break;
         }
      }

      // copy modes
      for(i=0; i<num_vidmodes-firstEntry; i++) {
         vidmap[i] = vidmap[i+firstEntry];
      }
      num_vidmodes -= firstEntry;

      if(num_vidmodes > 0) { // do we have any fullscreen modes at all?
          lowest_vidmode = num_vidmodes - 1;
      }
      else {
          CV_SetValue(&cv_fullscreen, 0);
          CONS_Printf("Only modes above 1024x768 available\nSwitching to windowed mode ...\n");
      }
   }
   else {
       num_vidmodes = 0;
   }
   allow_fullscreen = true;

   return;
}

int VID_SetMode(int modenum) {

    boolean   isWindowedMode;
    
    if(haveVoodoo) {
        if(modenum >= NUM_VOODOOMODES)
            return -1;
        
        destroyWindow();
        isWindowedMode = true;
        vid.width = voodooModes[modenum][0];
        vid.height = voodooModes[modenum][1];
    }
    else if (cv_fullscreen.value && vidmode_ext) { // fullscreen
        if(modenum >= num_vidmodes)
            return -1;

        destroyWindow();
        isWindowedMode = false;
        vid.width = vidmodes[vidmap[modenum]]->hdisplay;
        vid.height = vidmodes[vidmap[modenum]]->vdisplay;
    }
    else { // X11
        if(modenum >= MAXWINMODES)
            return -1;
        
        destroyWindow();
        isWindowedMode = true;
        vid.width = windowedModes[modenum][0];
        vid.height = windowedModes[modenum][1];
    }
    
    vid.rowbytes = vid.width;
    vid.recalc = 1;
    X_width = vid.width; // FIXME: shall we clean up X_[width,height]?
    X_height = vid.height; // they are identical to vid.[width,height]!!!
    CONS_Printf("Setting mode: %dx%d\n", vid.width, vid.height);
    
    if(!createWindow(isWindowedMode, modenum))
        return 0;
    
    I_StartupMouse();
    
    vid.modenum = modenum;

    return 1;
}

void I_StartupGraphics(void)
{

    char      *displayname;
    //int        pnum;
    void      *dlptr;
    static int firsttime = 1; // dirty
    
    if(dedicated)
    {
	rendermode = render_none;
	return;
    }
    
    if (!firsttime)
       return;
    firsttime = 0;

   // FIXME: catch other signals as well?
    signal(SIGINT, (void (*)(int)) I_Quit);
    signal(SIGTERM, (void (*)(int)) I_Quit); // shutdown gracefully if terminated

    // setup vid 19990110 by Kin
    vid.bpp = 1; // not optimized yet...

    // default size for startup
    vid.width = X_width = 320;
    vid.height = X_height = 200;

    CV_RegisterVar (&cv_vidwait);

    displayname = initDisplay();

    if(M_CheckParm("-opengl")) {
        // only set MESA_GLX_FX if not set by set user
        if(!getenv("MESA_GLX_FX"))
        {
            if(M_CheckParm("-winvoodoo"))
            {
                // use windowed mode for voodoo cards if requested
                putenv("MESA_GLX_FX=window");
                putenv("SSTV2_VGA_PASS=1");
                putenv("SSTV2_NOSHUTDOWN=1");
            }
            else
            {
                // Tell Mesa GLX to use 3Dfx driver in fullscreen mode.
                putenv("MESA_GLX_FX=fullscreen");
            }
            
            // Disable 3Dfx Glide splash screen
            putenv("FX_GLIDE_NO_SPLASH=0");
        }
        
       rendermode = render_opengl;

       // try to open library in CWD
       dlptr = dlopen("./r_opengl.so",RTLD_NOW | RTLD_GLOBAL);

       if(!dlptr) {
	   // try to open in LIBPATH
	   dlptr = dlopen("r_opengl.so",RTLD_NOW | RTLD_GLOBAL);
       }
       
       if(!dlptr)
       {
	   fprintf(stderr,"Error opening r_opengl.so!\n%s\n",dlerror());
	   rendermode = render_soft;
       } else {
	   HWD.pfnInit = dlsym(dlptr,"Init");
	   HWD.pfnShutdown = dlsym(dlptr,"Shutdown");
	   HWD.pfnHookXwin = dlsym(dlptr,"HookXwin");
	   HWD.pfnSetPalette = dlsym(dlptr,"SetPalette");
	   HWD.pfnFinishUpdate = dlsym(dlptr,"FinishUpdate");
	   HWD.pfnDraw2DLine = dlsym(dlptr,"Draw2DLine");
	   HWD.pfnDrawPolygon = dlsym(dlptr,"DrawPolygon");
	   //HWD.pfnGetState = dlsym(dlptr,"GetState");
	   HWD.pfnSetBlend = dlsym(dlptr,"SetBlend");
	   HWD.pfnClearBuffer = dlsym(dlptr,"ClearBuffer");
	   HWD.pfnSetTexture = dlsym(dlptr,"SetTexture");
	   HWD.pfnReadRect = dlsym(dlptr,"ReadRect");
	   HWD.pfnGClipRect = dlsym(dlptr,"GClipRect");
	   HWD.pfnClearMipMapCache = dlsym(dlptr,"ClearMipMapCache");
	   HWD.pfnSetSpecialState = dlsym(dlptr,"SetSpecialState");
	   HWD.pfnGetRenderer = dlsym(dlptr, "GetRenderer");
	   //FIXME: check if all this is ok:
	   HWD.pfnDrawMD2 = dlsym(dlptr, "DrawMD2");
	   HWD.pfnSetTransform = dlsym(dlptr, "SetTransform");
	   HWD.pfnGetTextureUsed = dlsym(dlptr, "GetTextureUsed");
	   HWD.pfnGetRenderVersion = dlsym(dlptr, "GetRenderVersion");
	   
	   // check gl renderer lib
	   if (HWD.pfnGetRenderVersion() != VERSION)
	   {
	       I_Error ("The version of the renderer doesn't match the version of the executable\nBe sure you have installed Doom Legacy properly.\n");
	   }   
       }
    }
    
    checkVidModeExtension();

    determineVidModes();

    findVisual();

    determineColorMask();

    determineBPP();
  
    checkForShm(displayname);

    createColorMap();
    
    createWindow(true, // is windowed
                 0);   // dummy modenum

    // startupscreen does not need a grabbed mouse
    doUngrabMouse();
    
    graphics_started = 1;

    return;
}

void I_ShutdownGraphics(void)
{
  // was graphics initialized anyway?
  if (!X_display)
        return;

  destroyWindow();

  // return to normal mode
  if (vidmode_ext /*active*/) {
      XF86VidModeSwitchToMode(X_display, X_screen, vidmodes[0]);     
  }

  if(rendermode != render_soft) {
      HWD.pfnShutdown();
  }

  XCloseDisplay(X_display);
}
