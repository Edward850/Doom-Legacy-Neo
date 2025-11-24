
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_WIN
#include "i_os2.h"

#include "i_system.h"
#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "keys.h"


/* Function prototypes
*/
MRESULT EXPENTRY DoomWindowProc ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY MyDlgProc ( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 );
void     DoomThread( void*);

/* Global definitions
*/
PWINDATA  pmData;

/*
 * Functions that query information about windows
 */

int      ScreenWidth() {
   return WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
}


int      ScreenHeight() {
   return WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
}

void     SnapWindow(WINDATA *this, LONG width, LONG height)
{
   LONG nheight, nwidth;

   if (this->ulWindowStyle != WS_DesktopDive)
      return;

   if (!height) {
      height = (width * this->ulHeight) / this->ulWidth;
   }

   nheight = height + WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER) * 2
		    + WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER) * 2
		    + WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
   nwidth = width + WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER) * 2;

   WinSetWindowPos( this->hwndFrame, HWND_TOP,
		   (ScreenWidth() - nwidth) / 2,
		   (ScreenHeight() - nheight) / 2,
		   nwidth, nheight,
		   SWP_SIZE | SWP_ACTIVATE | SWP_MOVE | SWP_SHOW);

} /* SnapWindow */


void     BlitThread( void* arg)
{
   ULONG    ulTime0, ulTime1;     // output buffer for DosQuerySysInfo
   ULONG    ulNumFrames=0;        // Frame counter
   ULONG    ulFramesToTime=32;     // Interval of frames to get time
   PWINDATA this = (PWINDATA) arg;

   while( !this->ulToEnd) {

         // skip if dive not enabled
      if (!this->hDive)
         continue;

         // skip if blit not enabled
      if (!this->fBlitReady)
         continue;

         // Check if it's time to start the blit-time counter.
      if (!ulNumFrames++ )
         DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &ulTime0, 4L );

         // blit image
      DiveBlitImage( this->hDive, this->ulImage2, DIVE_BUFFER_SCREEN);
         // reset blitting flag
      this->fBlitReady = FALSE;

         // skip if rating not enabled
      if (!this->fCheckFps)
         continue;

         // Check to see if we have enough frames for a fairly accurate read.
      if (ulNumFrames>=ulFramesToTime) {
            DosQuerySysInfo ( QSV_MS_COUNT, QSV_MS_COUNT, &ulTime1, 4L );
            ulTime1 -= ulTime0;
            if (ulTime1)
               sprintf( this->title, "(%d) %5.2f frames per second.",
                        ulFramesToTime,
                        (float)ulFramesToTime * 1000.0 / (float)ulTime1 );
            else
               sprintf( this->title, "(%d) Lots of frames per second.",
                        ulFramesToTime );
            WinPostMsg( this->hwndFrame, WM_COMMAND, (PVOID)ID_NEWTEXT, 0);
            ulNumFrames = 0;

            /* Adjust number of frames to time based on last set.
            */
            if ( ulTime1 < 250 )
               ulFramesToTime <<= 1;
            if ( ulTime1 > 3000 )
               ulFramesToTime >>= 1;
      }

         // Let other threads and processes have some time.
      DosSleep( 0);
   }
   return;
}


int      PostEvent( event_t* evt)
{
   APIRET rc;

      // request with 1s timeout
   rc = DosRequestMutexSem( pmData->hmtxEventSem, 100);
   if (rc==ERROR_TIMEOUT) {
      printf( "PostEvent: can't request semaphore\n");
      return 1;
   }

      // post of current event
   D_PostEvent( evt);

      // release resource
   rc = DosReleaseMutexSem( pmData->hmtxEventSem);

   return 0;
}


/*
 * This function translates OS/2 scan codes -> DOOM keys
 */

int PostCharEvent(WINDATA *this, PCHRMSG pchmsg)
{
   static event_t ev;

   memset( &ev, 0, sizeof( ev));

      // use last char, for keyup events
   if (this->last_char) {
      ev.type = ev_keyup;
      ev.data1 = this->last_char;
      PostEvent(&ev);
      this->last_char = 0;
   }

   memset( &ev, 0, sizeof( ev));
   ev.type = (pchmsg->fs & KC_KEYUP) ? ev_keyup : ev_keydown;

   if (pchmsg->fs & KC_VIRTUALKEY) {

         // process virtual keys
      switch( pchmsg->vkey) {

      case VK_INSERT:   ev.data1 = KEY_INS;  break;
      case VK_DELETE:   ev.data1 = KEY_DEL;  break;
      case VK_HOME:     ev.data1 = KEY_HOME;  break;
      case VK_END:      ev.data1 = KEY_END;  break;
      case VK_PAGEUP:   ev.data1 = KEY_PGUP;  break;
      case VK_PAGEDOWN: ev.data1 = KEY_PGDN;  break;

      case VK_LEFT:  ev.data1 = KEY_LEFTARROW;  break;
      case VK_RIGHT: ev.data1 = KEY_RIGHTARROW; break;
      case VK_UP:    ev.data1 = KEY_UPARROW;    break;
      case VK_DOWN:  ev.data1 = KEY_DOWNARROW;  break;
      case VK_ESC:   ev.data1 = KEY_ESCAPE;     break;
      case VK_NEWLINE:
      case VK_ENTER: ev.data1 = KEY_ENTER;      break;
      case VK_TAB:   ev.data1 = KEY_TAB;        break;
      case VK_SPACE:     ev.data1 = ' ';            break;
      case VK_BACKSPACE: ev.data1 = KEY_BACKSPACE;  break;
      case VK_PAUSE:
      case VK_NUMLOCK:ev.data1 = KEY_PAUSE;     break;
      case VK_CTRL:  ev.data1 = KEY_CTRL;       break;
      case VK_F1:
	  /* Hack! OS/2 grabs this key and only returns the keyup event */
	  ev.type = ev_keydown;
	  PostEvent(&ev);
	  ev.data1 = KEY_F1;
	  break;
      case VK_F2:    ev.data1 = KEY_F2;         break;
      case VK_F3:    ev.data1 = KEY_F3;         break;
      case VK_F4:    ev.data1 = KEY_F4;         break;
      case VK_F5:    ev.data1 = KEY_F5;         break;
      case VK_F6:
	  /* desktop -> fullscreen */
	  if (pchmsg->fs & KC_SHIFT) {
	     WinPostMsg( this->hwndClient, WM_COMMAND, (MPARAM)ID_FSMODE, 0 );
	     return 1;
	  }
          ev.data1 = KEY_F6;
          break;
      case VK_F7:
	  /* fullscreen -> desktop */
	  if (pchmsg->fs & KC_SHIFT) {
	     WinPostMsg( this->hwndClient, WM_COMMAND, (MPARAM)ID_DESKMODE, 0);		      	
	     return 1;
	  }
#ifdef USE_MOUSE
	  if (pchmsg->fs & KC_ALT) {
	      WinPostMsg( this->hwndClient, WM_COMMAND, (MPARAM)ID_SWMOUSE,
			  (MPARAM)FALSE );
	      return 1;
	  }
#endif
	  ev.data1 = KEY_F7;
	  break;
      case VK_F8:
	  /*  -> max desktop */
	  if (pchmsg->fs & KC_SHIFT) {
	     WinPostMsg( this->hwndClient, WM_COMMAND, (MPARAM)ID_MAXMODE, 0);		      	
	     return 1;
	  }
	  ev.data1 = KEY_F8;
	  break;
      case VK_F9:
	  if (pchmsg->fs & KC_SHIFT) {
	      WinPostMsg(this->hwndClient, WM_COMMAND, (MPARAM)ID_SNAP, 0);
	      return 1;
	  }
	  ev.data1 = KEY_F9;
	  break;
      case VK_F10:
	  if (pchmsg->fs & KC_SHIFT) {
	      WinPostMsg(this->hwndClient, WM_COMMAND, (MPARAM)ID_SNAP2, 0);
	      return 1;
	  }
	  ev.data1 = KEY_F10;
	  break;
      case VK_F11:
	  if (pchmsg->fs & KC_SHIFT) {
	     /* switch to largest conformant size */
	     WinSendMsg(this->hwndClient, WM_COMMAND, (MPARAM)ID_SNAPFULL, 0);
	     return 1;
	  }
	  ev.data1 = KEY_F11;
	  break;
      case VK_F12:   ev.data1 = KEY_F12;        break;
      default:
         return 0;
         break;
      }

      //printf( "virtualkey %x event %x\n", pchmsg->vkey, ev.data1);

   } else if (pchmsg->fs & KC_CHAR) {    // normal keys
      /* For usual keys no key up event is generated so we must remember
         the last key down event to send a key-up later */
      this->last_char = ev.data1 = tolower(pchmsg->chr) == '+' ? KEY_EQUALS : tolower( pchmsg->chr);
   } else {
      return 0;
   }

      // send event to Doom/2 thread
   if (ev.data1)
      PostEvent(&ev);

   return 1;
}

/*
 * This routine treats all the appl. defined commands for the DIVE window.
 */

MRESULT WindowCommands(WINDATA *this, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   BOOL  bHandled = TRUE;

   switch ((ULONG)mp1) {
   case ID_SNAP:
      SnapWindow(this, this->ulWidth, this->ulHeight);
      CONS_Printf("Switching to 1X window mode...\n");
      break;
   case ID_SNAP2:
      SnapWindow(this, this->ulWidth * 2, this->ulHeight * 2);
      CONS_Printf("Switching to 2X window mode...\n");
      break;
   case ID_SNAPFULL:
      SnapWindow(this, ScreenWidth(), 0);
      CONS_Printf("Switching to max screen width window mode...\n");
      break;
   case ID_DESKMODE:
      if (this->fFSBase && this->ulWindowStyle != WS_DesktopDive)
	 WinSendMsg( this->hwndFrame, WM_SetVideoMode,
		     (MPARAM)WS_DesktopDive, 0 );
      CONS_Printf("Switching to desktop mode...\n");
      break;
   case ID_FSMODE:
      if (this->fFSBase && this->ulWindowStyle != WS_FullScreenDive)
	 WinSendMsg( this->hwndFrame, WM_SetVideoMode,
		     (MPARAM)WS_FullScreenDive, 0 );
      CONS_Printf("Switching fullscreen mode...\n");
      break;
   case ID_MAXMODE:            // switch to max desktop size
      WinSendMsg( this->hwndFrame, WM_SetVideoMode, (MPARAM)WS_MaxDesktopDive, 0);
      CONS_Printf("Switching to max desktop size mode...\n");
      break;
   case ID_EXIT:
      /* Post to quit the dispatch message loop. */
      WinPostMsg( this->hwndFrame, WM_CLOSE, 0, 0 );
      break;

   case ID_NEWTEXT:
      WinSetWindowText( this->hwndFrame, this->title);
      break;

   case ID_PALINIT:
#ifdef USE_PALETTE_MGR
      CreatePalette(this);
#else
      RealizePalette(this);
#endif
      break;
   case ID_SWMOUSE:
      //SwitchMouseUse(this, (BOOL)mp2);
      break;
   default:
      bHandled = FALSE;
      break;
   }

   return bHandled;
} /* WindowCommands */


//
// handle main window events
//
MRESULT EXPENTRY DoomWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   BOOL      bHandled = TRUE;
   PWINDATA  this;              /* Pointer to window data               */
   ULONG     rc;
   MCI_GENERIC_PARMS    GenericParms;
   event_t   ev;

   /* Get the pointer to window data
   */
   this = (PWINDATA)WinQueryWindowULong (hwnd, 0);
   if (this) {
      switch( msg) {

      case WM_ERASEBACKGROUND:
         return (MRESULT) 1;

      case WM_NotifyVideoModeChange:
         PrepareForModeChange(this, mp1, mp2);
         break;

      case MM_MCINOTIFY:
         printf( "DoomWindowProc: MM_MCINOTIFY\n");
         if (SHORT1FROMMP( mp1)==MCI_NOTIFY_SUCCESSFUL &&
             SHORT1FROMMP( mp2)==this->usMidiID &&
             SHORT2FROMMP( mp2)==MCI_PLAY &&
             this->looping) {
            printf( "WindowProc:MM_MCINOTIFY MCI_PLAY midi loop again\n");
            ShutdownMIDI( this);
            OpenMIDI( this);
            PlayMIDI( this, this->looping);      // play again
         }
         break;

      case MM_MCIPASSDEVICE:
            // Check if we are gaining or passing use of the amp-mixer device
         printf( "DoomWindowProc: MM_MCIPASSDEVICE\n");
         if (SHORT1FROMMP( mp2)==MCI_GAINING_USE)
            this->fPassedDevice = FALSE;
         else
            this->fPassedDevice = TRUE;
         break;

      case WM_ACTIVATE:
            // Check if this window is the active window and if we have passed
            // use of the Amp-Mixer device. If yes, then send MCI_ACQUIREDEVICE
            // message.
         if ((BOOL)mp1 && this->fPassedDevice && this->usDartID) {
            printf( "WindowProc: send MCI_ACQUIREDEVICE\n");
            GenericParms.hwndCallback = hwnd;
            rc = mciSendCommand( this->usDartID,
                                 MCI_ACQUIREDEVICE,
                                 MCI_NOTIFY | MCI_ACQUIRE_QUEUE,
                                 (PVOID) &GenericParms,
                                 0);
            if (rc)
               MciError( rc);
            rc = mciSendCommand( this->usMidiID,
                                 MCI_ACQUIREDEVICE,
                                 MCI_NOTIFY | MCI_ACQUIRE_QUEUE,
                                 (PVOID) &GenericParms,
                                 0);
            if (rc)
               MciError( rc);
         }

         // if losing focus, pause game
         if ((BOOL)mp1 == FALSE) {
            ev.data1 = KEY_PAUSE;
            ev.type = ev_keydown;
            D_PostEvent (&ev);
         }
         break;

      case WM_CHAR:
         if (PostCharEvent(this, CHARMSG(&msg)))
            return TRUE;
         break;

      case WM_COMMAND:
         bHandled = WindowCommands(this, msg, mp1, mp2);
         break;

      case WM_VRNDISABLED:
         WindowSetBlit(this, 0);
         break;

      case WM_VRNENABLED:
         WindowSetBlit(this, 1);
         break;

      case WM_REALIZEPALETTE:
            /* This tells DIVE that the physical palette may have changed.
            */
         if (this->ulColorBits==8)
            DiveSetDestinationPalette( this->hDive, 0, this->ulNumColors, 0);
         break;

      case WM_CLOSE:
            /* Post to quit the dispatch message loop.
            */
#ifdef DEBUG
         printf( "DoomWindowProc: WM_CLOSE\n");
#endif
         WindowSetBlit( this, 0);
         WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
         ev.data1 = KEY_ESCAPE;      //to exit network synchronization
         ev.type = ev_keydown;
         D_PostEvent (&ev);
         break;

      default:
         bHandled = FALSE;
         break;
      }
   } else {
      bHandled = FALSE;
      printf( "DoomWindowProc: this is NULL (msg#%x)\n", msg);
   }


   if (!bHandled)       // Let PM handle this message.
      return WinDefWindowProc ( hwnd, msg, mp1, mp2 );

   return ( FALSE );
}

void     DoomThread( void*arg)
{
   HAB    hab = WinInitialize ( 0 );
   HMQ    hmq = WinCreateMsgQueue( hab, 0 );

   // currently starts DirectInput 
   CONS_Printf ("I_StartupSystem() ...\n");
   I_StartupSystem();

   // startup Doom Legacy
   CONS_Printf ("D_DoomMain() ...\n");
   D_DoomMain (); 
   CONS_Printf ("Entering main app loop...\n");

   // never return
   D_DoomLoop ();
}

//
// init PM window
//
void     InitPMSession( void* arg)
{
   QMSG      qmsg;
   ULONG     flCreate;
   PSZ       pszMyWindow = "DoomLegacy2";
   PSZ       pszTitleText = "Doom LEGACY";

   /* Initialize the presentation manager, and create a message queue.
   */
   pmData->hab = WinInitialize ( 0 );
   pmData->hmq = WinCreateMsgQueue( pmData->hab, 0 );
   pmData->ulToEnd = 0;

      // Register a window class, and create a standard window.
   WinRegisterClass( pmData->hab, pszMyWindow, DoomWindowProc, 0, sizeof(ULONG) );
   flCreate = FCF_TASKLIST | FCF_TITLEBAR | FCF_ICON | FCF_DLGBORDER |
              FCF_MINMAX | FCF_SHELLPOSITION;

   pmData->hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                              0, &flCreate,
                                              pszMyWindow,
                                              pszTitleText,
                                              WS_SYNCPAINT,
                                              0, ID_MAINWND,
                                              &(pmData->hwndClient));

   WinSetWindowULong( pmData->hwndClient, 0, (ULONG)pmData);

#if 0
      // start Doom/2 thread
   pmData->tidDoomThread = _beginthread( DoomThread, 0L, 64*1024L, (void*) NULL);
   DosSetPriority( PRTYS_THREAD, PRTYC_IDLETIME,  1, pmData->tidDoomThread);
      // start blit thread
   if (M_CheckParm( "-blitthread")) {
      pmData->tidBlitThread = _beginthread( BlitThread, 0L, 64*1024L, (void*)pmData);
      DosSetPriority( PRTYS_THREAD, PRTYC_IDLETIME, 1, pmData->tidBlitThread);
      //DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL,  -1, pmData->tidBlitThread);
      //DosSetPriority( PRTYS_THREAD, PRTYC_REGULAR,  1, pmData->tidBlitThread);
   }
#endif

      // set window default size
   pmData->ulWidth  = 320;
   pmData->ulHeight = 200;
   SnapWindow( pmData, pmData->ulWidth, pmData->ulHeight);

      // Turn on visible region notification.
   WinSetVisibleRegionNotify ( pmData->hwndClient, TRUE );

      // set the flag for the first time simulation of palette of bitmap data
   pmData->fChgSrcPalette = FALSE;
   pmData->fDataInProcess = TRUE;

      // show main window
   WinShowWindow( pmData->hwndFrame, TRUE);

#if 0
      // While there are still messages, dispatch them.
   while ( WinGetMsg ( pmData->hab, &qmsg, 0, 0, 0 ) )
      WinDispatchMsg ( pmData->hab, &qmsg );

      // notify exit to other threads
   pmData->ulToEnd = 1;
#endif
}


void     ShutdownPMSession( void)
{
#if 0
      // wait end of threads
   if (pmData->tidDoomThread)
      DosWaitThread( &(pmData->tidDoomThread), DCWW_WAIT);
   if (pmData->tidBlitThread)
      DosWaitThread( &(pmData->tidBlitThread), DCWW_WAIT);
#endif

      // Turn off visible region notificationm tidy up, and terminate.
   WinSetVisibleRegionNotify ( pmData->hwndClient, FALSE );

      // Process for termination
   WinDestroyWindow( pmData->hwndFrame );
   WinDestroyMsgQueue( pmData->hmq);
   WinTerminate( pmData->hab);

#ifdef DEBUG
   printf( "ShutdownPMSession: DONE\n");
#endif
}

void     I_StartupPMSession( void)
{
   APIRET rc;

      // Allocate a buffer for the window data
   pmData = (PWINDATA) malloc (sizeof(WINDATA));
   memset( pmData, 0, sizeof(WINDATA));

      // create owned event mutex
   rc = DosCreateMutexSem( SEM32_EVENTMUTEX, &pmData->hmtxEventSem, 0, 0);
   if (rc) {
      printf( "main: can't create mutex rc=%x\n", (unsigned int) rc);
   }

      // verify test fps flag
   if (M_CheckParm( "-testfps"))
      pmData->fCheckFps = TRUE;

      // init PM
   InitPMSession( NULL);
   I_AddExitFunc(ShutdownPMSession);
/*
      // shutdown PM
   ShutdownPMSession();
*/
}
