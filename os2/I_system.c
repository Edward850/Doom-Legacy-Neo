// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: I_system.c,v 1.9 2001/04/27 13:32:14 bpereira Exp $
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
// $Log: I_system.c,v $
// Revision 1.9  2001/04/27 13:32:14  bpereira
// no message
//
// Revision 1.8  2001/04/16 22:58:55  ydario
// now error dialog is bound to application window
//
// Revision 1.7  2001/03/31 10:16:42  ydario
// network play ESC fix
//
// Revision 1.6  2001/03/03 19:29:44  ydario
// code clean up
//
// Revision 1.5  2001/02/24 13:35:22  bpereira
// no message
//
// Revision 1.4  2000/10/16 21:21:11  hurdler
// remove unecessary code
//
// Revision 1.3  2000/08/10 14:59:41  ydario
// OS/2 port
//
// Revision 1.2  2000/08/10 11:07:51  ydario
// fix CRLF
//
// Revision 1.1  2000/08/09 12:13:38  ydario
// OS/2 specific platform code
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: I_system.c,v 1.9 2001/04/27 13:32:14 bpereira Exp $";


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define INCL_DOSDEVIOCTL
#include "I_os2.h"

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"
#include "i_joy.h"

JoyType_t Joystick;

byte    mb_used = 12;

// Do not execute cleanup code more than once. See Shutdown_xxx() routines.
byte    graphics_started=0;
byte    keyboard_started=0;
byte    sound_started=0;
boolean timer_started = false;
boolean mouse_enabled = false;
boolean joystick_detected;

void I_InitJoystick (void) {}
void I_StartupMouse(void) {}
void I_StartupMouse2(void) {}
void I_StartupTimer(void) {}
void I_StartupKeyboard (void) {}

//  Return a key that has been pushed, or 0
//  (replace getchar() at game startup)
//
int I_GetKey (void)
{
    event_t   *ev;
        
    if (eventtail != eventhead)
    {
        ev = &events[eventtail];
        eventtail = (++eventtail)&(MAXEVENTS-1);
        if (ev->type == ev_keydown)
            return ev->data1;
        else
            return 0;
    }
    return 0;
}

void
I_Tactile
( int   on,
  int   off,
  int   total )
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t        emptycmd;
ticcmd_t*       I_BaseTiccmd(void)
{
    return &emptycmd;
}


//
// I_GetFreeMem
// return free and total memory in the system
//
ULONG I_GetFreeMem(ULONG *total)
{
   ULONG pr_arena;

   DosQuerySysInfo( QSV_TOTPHYSMEM, QSV_TOTPHYSMEM, 
                    (PVOID) total, sizeof(ULONG));
   DosQuerySysInfo( QSV_MAXPRMEM, QSV_MAXPRMEM, 
                    (PVOID) &pr_arena, sizeof(ULONG));

   return pr_arena;
}


//
// I_GetTime
// returns time in 1/70th second tics
//
ULONG  I_GetTime (void)
{
    struct timeval      tp;
    struct timezone     tzp;
    int                 newtics;
    static int          basetime=0;

    gettimeofday(&tp, &tzp);
    if (!basetime)
        basetime = tp.tv_sec;
    newtics = (tp.tv_sec-basetime)*TICRATE + tp.tv_usec*TICRATE/1000000;

    return newtics;
}



//
// I_Init
//
void I_Init (void)
{
    I_StartupSound();
    I_InitMusic();
    //  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit (void)
{
#ifdef DEBUG
   printf( "I_Quit\n");
#endif

    //added:16-02-98: when recording a demo, should exit using 'q' key,
    //        but sometimes we forget and use 'F10'.. so save here too.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame ();

    // shutdown everything that was started !
    I_ShutdownSystem();
#ifdef DEBUG
   printf( "I_Quit: shut down everything\n");
#endif

    // use this for 1.28 19990220 by Kin
    M_SaveConfig (NULL);

/*
      // Send CLOSE to PM
   printf( "ShutdownPMSession: send wm_close\n");
   WinPostMsg( pmData->hwndClient, WM_CLOSE, 0, 0);
*/

#ifdef DEBUG
   printf( "I_Quit: _endthread(0)\n");
#endif
   //_endthread();
   exit(0);
}

void I_WaitVBL(int count)
{
    DosSleep( count*1000/70);
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

byte*   I_AllocLow(int length)
{
    byte*       mem;

    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}

// ----------
// I_GetEvent
// Post new events for all sorts of user-input
// ----------
static void I_GetKeyboardEvents (void);
static void I_GetMouseEvents (void);
static void I_GetJoystickEvents (void);
void I_GetEvent (void)
{
   I_GetKeyboardEvents();
   //I_GetMouseEvents();
   //I_GetJoystickEvents();
}

void I_GetKeyboardEvents(void)
{
   static event_t ev_alt_up = { ev_keyup, KEY_ALT, 0, 0 };
   static event_t ev_alt_do = { ev_keydown, KEY_ALT, 0, 0 };
   static event_t ev_shf_up = { ev_keyup, KEY_SHIFT, 0, 0 };
   static event_t ev_shf_do = { ev_keydown, KEY_SHIFT, 0, 0 };
   APIRET   rc;
   ULONG    ulReturn;
   static HFILE hkbd;
   SHIFTSTATE shiftstate;
 
   if (hkbd == NULL) {
       DosOpen( (PSZ) "kbd$",                    // open driver
                (HFILE*)&hkbd, &ulReturn, 0, 0, FILE_OPEN,
                OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE, NULL);
      //printf( "I_GetKeyboardEvents DosOpen %08x\n", hkbd);
   }

   if (!hkbd)
      return;

      // query shift state   
   rc = DosDevIOCtl( hkbd, IOCTL_KEYBOARD, KBD_GETSHIFTSTATE,
                    NULL, NULL, NULL,
                    (void*) &shiftstate, sizeof(SHIFTSTATE), &ulReturn);
   //printf( "I_GetKeyboardEvents IOCTL rc=%d, shift=%08x\n", rc, shiftstate.fsState);

   if (shiftstate.fsState & ALT) {
      if (!pmData->fAltPressed) {
         D_PostEvent(&ev_alt_do);
         pmData->fAltPressed = 1;
      }
   } else {
      if (pmData->fAltPressed) {
         D_PostEvent(&ev_alt_up);
         pmData->fAltPressed = 0;
      }
   }

   if (shiftstate.fsState & RIGHTSHIFT || shiftstate.fsState & LEFTSHIFT) {
      if (!pmData->fShiftPressed) {
         D_PostEvent(&ev_shf_do);
         pmData->fShiftPressed = 1;
      }
   } else {
      if (pmData->fShiftPressed) {
         D_PostEvent(&ev_shf_up);
         pmData->fShiftPressed = 0;
      }
   }

}

//
// I_OsPolling
//
void I_OsPolling (void)
{
    QMSG   qmsg;

        while (WinPeekMsg( pmData->hab, &qmsg, (HWND) NULL, 0, 0, PM_NOREMOVE))
        {
            if (WinGetMsg( pmData->hab, &qmsg, (HWND) NULL, 0, 0))
                WinDispatchMsg( pmData->hab, &qmsg);
            else  // winspec : this is quit message
                I_Quit ();
        }

    // this is called by the network synchronization,
    // check keys and allow escaping
    I_GetEvent();
}

//
// I_Error
//
extern boolean demorecording;

void I_Error (char *error, ...)
{
    va_list     argptr;

    // Message first.
   char  buffer[ 1024];
   va_start (argptr,error);
   vsprintf( buffer, error, argptr);
   printf( "Error: %s\n", buffer);
   WinMessageBox( HWND_DESKTOP, 
                  pmData->hwndFrame ? pmData->hwndFrame : HWND_DESKTOP,
                  (PSZ) buffer,
                  pmData->title, 
                  0, MB_OK | MB_INFORMATION );
    va_end (argptr);

    //added:18-02-98: save one time is enough!
    //if (!errorcount)
    {
        M_SaveConfig (NULL);   //save game config, cvars..
    }
        
    //added:16-02-98: save demo, could be useful for debug
    //                NOTE: demos are normally not saved here.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame ();

    // shutdown everything that was started !
    I_ShutdownSystem();

    exit(-1);
}


// ===========================================================================================
// CLEAN STARTUP & SHUTDOWN HANDLING, JUST CLOSE EVERYTHING YOU OPENED.
// ===========================================================================================
//
//
#define MAX_QUIT_FUNCS     16

typedef void (*quitfuncptr)();

static quitfuncptr quit_funcs[MAX_QUIT_FUNCS] = {
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };


//  Adds a function to the list that need to be called by I_SystemShutdown().
//
void I_AddExitFunc(void (*func)())
{
    int c;
    
    for (c=0; c<MAX_QUIT_FUNCS; c++) {
        if (!quit_funcs[c]) {
            quit_funcs[c] = func;
            break;
        }
    }
}


//  Removes a function from the list that need to be called by I_SystemShutdown().
//
void I_RemoveExitFunc(void (*func)())
{
    int c;
    
    for (c=0; c<MAX_QUIT_FUNCS; c++) {
        if (quit_funcs[c] == func) {
            while (c<MAX_QUIT_FUNCS-1) {
                quit_funcs[c] = quit_funcs[c+1];
                c++;
            }
            quit_funcs[MAX_QUIT_FUNCS-1] = NULL;
            break;
        }
    }
}


//  This stuff should get rid of the exception and page faults when
//  Doom bugs out with an error. Now it should exit cleanly.
//
int  I_StartupSystem(void)
{

    // some 'more globals than globals' things to initialize here ?
    graphics_started = false;
    keyboard_started = false;
    sound_started = false;
    timer_started = false;
    cdaudio_started = false;
        
    // check for OS type and version here ?
#ifdef NDEBUG
    signal(SIGABRT, signal_handler);
    signal(SIGFPE , signal_handler);
    signal(SIGILL , signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT , signal_handler);
#endif

    return 0;
}

//  Closes down everything. This includes restoring the initial
//  pallete and video mode, and removing whatever mouse, keyboard, and
//  timer routines have been installed.
//
//  NOTE : Shutdown user funcs. are effectively called in reverse order.
//
void I_ShutdownSystem()
{
    int c;
    
    for (c=MAX_QUIT_FUNCS-1; c>=0; c--)
    {
        if (quit_funcs[c]) {
            printf( "I_ShutdownSystem: calling func#%d\n", c);
            (*quit_funcs[c])();
        }
    }
}


void I_GetDiskFreeSpace(INT64 *freespace)
{
        *freespace = MAXINT;
}

char *I_GetUserName(void)
{
static char username[MAXPLAYERNAME];
     char  *p;
     if((p=getenv("USER"))==NULL)
         if((p=getenv("user"))==NULL)
            if((p=getenv("USERNAME"))==NULL)
               if((p=getenv("username"))==NULL)
                  return NULL;
     strncpy(username,p,MAXPLAYERNAME);

     if( strcmp(username,"")==0 )
         return NULL;
     return username;
}

int  I_mkdir(const char *dirname, int unixright)
{
    return mkdir(dirname,unixright);
}
