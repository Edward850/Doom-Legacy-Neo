
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_WIN
#define INCL_GPI

#include "i_os2.h"
#include "screen.h"

#include <fourcc.h>


/* If mp1 == 0 then GAMESRVR is about to switch to full screen mode
 * and we need to turn off blitting. If mp1 is non zero then GAMSRVR
 * has just finished a transition and we have to check what mode we
 * are now in. The current mode is indicated by mp2 where 0 or 1 is
 * desktop mode and anything else is full screen mode. Currently,
 * full screen mode only supports mode 13. This will be extended
 * in the future.
 */

void PrepareForModeChange(WINDATA *this, MPARAM mp1, MPARAM mp2)
{
   if (!this->hDive)
      return;

   if ((ULONG)mp1 == 0)
    {
      printf("Beginning to switch screen mode\n");
      this->fDataInProcess = TRUE;	/* Disable blitting */
      this->fSwitching = TRUE;	/* Avoid setting blit-mode until completion */
    }
   else if ((ULONG)mp1 == 1)
    {
      /* Determine the mode we are now in */
      switch ((ULONG)mp2) {
      case 0:
         printf("...done switching to desktop mode\n");
	 this->ulWindowStyle = WS_DesktopDive;
#ifdef USE_MOUSE
	 this->scrWidth = ScreenWidth();
	 this->scrHeight = ScreenHeight();
#endif
         break;
      case 1:
         printf("...done switching to maximized mode\n");
	 this->ulWindowStyle = WS_MaxDesktopDive;
#ifdef USE_MOUSE
	 this->scrWidth = ScreenWidth();
	 this->scrHeight = ScreenHeight();
#endif
         break;
      default:
         this->ulWindowStyle = WS_FullScreenDive;
         printf("...done switching to full screen\n");
#ifdef USE_MOUSE
	 this->scrWidth = 320;
	 this->scrHeight = 200;
#endif
      }
      /* Enable WindowSetBlit */
      this->fSwitching = FALSE;
      /* Enable blitting */
      WindowSetBlit(this, 1);
    }

} /* PrepareForModeChange */


//
// setup blitter on WM_VRENABLED messages
//
void  WindowSetBlit (WINDATA* this, int on)
{
   HRGN      hrgn;                  /* Region handle                        */
   HPS       hps;                   /* Presentation Space handle            */
   SETUP_BLITTER SetupBlitter;      /* structure for DiveSetupBlitter       */
   RGNRECT   rgnCtl;                /* Processing control structure         */
   RECTL     rcls[50];
   POINTL    pointl;                /* Point to offset from Desktop         */
   SWP       swp;                   // Standard window position structure

   if (!this->hDive)
      return;

   if (this->fSwitching)
      return;

   this->fDataInProcess = TRUE;

   if (!on) {
      DiveSetupBlitter(this->hDive, 0);
      return;
   }

      // If we are in full screen mode then the blitter parameters are
      // set to accomodate mode 13. Other wise we need the blitter
      // parameters to match the window properties.
      //
   if (this->ulWindowStyle == WS_FullScreenDive) {
      swp.cx = 320;                      // Set width for mode 13.
      swp.cy = 200;                      // Set height for mode 13.
      pointl.x = 0;                      // Set window corner to origin
      pointl.y = 0;
      rgnCtl.crcReturned = 1;            // Only one rectangle in full screen
      rcls[0].xLeft  = 0;
      rcls[0].xRight = 320;
      rcls[0].yBottom= 0;
      rcls[0].yTop   = 200;
   } else {

      hps = WinGetPS ( this->hwndClient);
      if ( !hps) {
         printf( "DiveVREnabled: can't get hps for client window\n");
         return;
      }

      hrgn = GpiCreateRegion ( hps, 0L, NULL );
      if (hrgn) {
          /* NOTE: If mp1 is zero, then this was just a move message.
         ** Illustrate the visible region on a WM_VRNENABLE.
         */
         WinQueryVisibleRegion ( this->hwndClient, hrgn);
         rgnCtl.ircStart     = 0;
         rgnCtl.crc          = 50;
         rgnCtl.ulDirection  = 1;

           /* Get the all ORed rectangles */
         if (GpiQueryRegionRects ( hps, hrgn, NULL,
                                   &rgnCtl, rcls) ) {
               // Now find the window position and size, relative to parent.
            WinQueryWindowPos( this->hwndClient, &swp);

               // Convert the point to offset from desktop lower left.
            pointl.x = swp.x;
            pointl.y = swp.y;
            WinMapWindowPoints( this->hwndFrame, HWND_DESKTOP, &pointl, 1);

         } else {
	    this->fDataInProcess = FALSE;            
            DiveSetupBlitter( this->hDive, 0);
            GpiDestroyRegion( hps, hrgn );
            WinReleasePS( hps );
            return;
         }
         GpiDestroyRegion( hps, hrgn );
      }
      WinReleasePS( hps );

   } // fullscreen

      // Tell DIVE about the new settings.
   SetupBlitter.ulStructLen = sizeof ( SETUP_BLITTER);
   SetupBlitter.fccSrcColorFormat = this->fccColorFormat;
   SetupBlitter.ulSrcWidth = this->ulWidth;
   SetupBlitter.ulSrcHeight = this->ulHeight;
   SetupBlitter.ulSrcPosX = 0;
   SetupBlitter.ulSrcPosY = 0;
   SetupBlitter.fInvert = FALSE;
   SetupBlitter.ulDitherType = 0;
      
   SetupBlitter.fccDstColorFormat = FOURCC_SCRN;
   SetupBlitter.ulDstWidth = swp.cx;
   SetupBlitter.ulDstHeight = swp.cy;
   SetupBlitter.lDstPosX = 0;
   SetupBlitter.lDstPosY = 0;
   SetupBlitter.lScreenPosX = pointl.x;
   SetupBlitter.lScreenPosY = pointl.y;
   SetupBlitter.ulNumDstRects = rgnCtl.crcReturned;
   SetupBlitter.pVisDstRects = rcls;
   
      // setup blitter
   DiveSetupBlitter( this->hDive, &SetupBlitter);

   this->fDataInProcess = FALSE;
}

//
// init dive access
//
int      InitDIVE( PWINDATA pwinData)
{
   ULONG     aulVersion[2];            // OS/2 version number
   UCHAR     szErrorBuf[256];          // GameSrvr
   HMODULE   hmodGameSrvr;             // GameSrvr
   PFN       pfnInitGameFrameProc;     // GameSrvr
   ULONG     pvmi;
   ULONG     ul;

   printf( "InitDIVE: frame=%x\n", pwinData->hwndFrame);
   /* Get the screen capabilities, and if the system support only 16 colors
   ** the sample should be terminated.
   */
   pwinData->DiveCaps.pFormatData = pwinData->fccFormats;
   pwinData->DiveCaps.ulFormatLength = 120;
   pwinData->DiveCaps.ulStructLen = sizeof(DIVE_CAPS);

   if (DiveQueryCaps ( &pwinData->DiveCaps, DIVE_BUFFER_SCREEN )) {
      WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
          (PSZ)"The program can not run on this system environment.",
          (PSZ)"DOOM for OS/2", 0, MB_OK | MB_INFORMATION );
      return 1;
   }

   if (pwinData->DiveCaps.ulDepth < 8 ) {
      WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
          (PSZ)"The program can not run on this system environment.",
          (PSZ)"DOOM for OS/2", 0, MB_OK | MB_INFORMATION );
      return 1;
   }

      // Get an instance of DIVE APIs.
   if (DiveOpen( &(pwinData->hDive), FALSE, 0)) {

      WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                     (PSZ)"The program can not run on this system environment.",
                     (PSZ)"DOOM for OS/2", 0, MB_OK | MB_INFORMATION );
      return 1;
   }

   pwinData->ulColorBits = 8;            // doom color depth
   /* Set how many color bitmap data is supporting */
   pwinData->ulNumColors = 1 << pwinData->ulColorBits;

   /* Set bitmap color format. */
   switch( pwinData->ulColorBits) {
   case 8:
      pwinData->fccColorFormat = FOURCC_LUT8;
      break;
   case 16:
      pwinData->fccColorFormat = FOURCC_R565;
      break;
   case 24:
      pwinData->fccColorFormat = FOURCC_BGR4;
      break;
   }

      // Turn on visible region notification.
   WinSetVisibleRegionNotify ( pwinData->hwndClient, TRUE );

      // set the flag for the first time simulation of palette of bitmap data
   pwinData->fChgSrcPalette = FALSE;
   pwinData->fDataInProcess = FALSE;
   //pwinData->fDirect = FALSE;

      // Send an invalidation message to the client.
   WinPostMsg( pwinData->hwndFrame, WM_VRNENABLED, 0L, 0L );

      // set palette
   if (pwinData->ulColorBits==8)
      DiveSetDestinationPalette( pwinData->hDive, 0, pwinData->ulNumColors, 0);

      // init full screen access
   DosQuerySysInfo( QSV_VERSION_MAJOR, QSV_VERSION_MINOR, aulVersion, 8);
      // on warp3
   if (aulVersion[0] == 20 && aulVersion[1] <= 30 ) {
          // GameSrvr ----------------------------------------------------begin---
      if (DosLoadModule( (PSZ) szErrorBuf, 256, "GAMESRVR", &hmodGameSrvr)==0) {
         ULONG pvmi;
         ULONG ul;
         if (DosQueryProcAddr( hmodGameSrvr, 1, 0, &pfnInitGameFrameProc)==0)
            (pfnInitGameFrameProc)( pwinData->hwndFrame, 0 );

         WinSendMsg( pwinData->hwndFrame, WM_GetVideoModeTable, (MPARAM)&pvmi, (MPARAM)&ul);
         pwinData->fFSBase = TRUE;
      } else {
         WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                   (PSZ)"usage: FSDIVE failed to load GAMESRVR.DLL.",
                   (PSZ)"Error!", 0, MB_OK | MB_MOVEABLE );
      }
          // GameSrvr ----------------------------------------------------end-----
   } else {                             // Warp4 Merlin
       DosLoadModule( (PSZ) szErrorBuf, 256, "PMMERGE", &hmodGameSrvr);
       if (DosQueryProcAddr( hmodGameSrvr, 6099, 0, &pfnInitGameFrameProc)==0) {
          ULONG pvmi;
          ULONG ul;
          (pfnInitGameFrameProc)( pwinData->hwndFrame, 0 );
          //WinSendMsg( pwinData->hwndFrame, WM_GetVideoModeTable, (MPARAM)&pvmi, (MPARAM)&ul);
          pwinData->fFSBase = TRUE;
       } else {
           WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                   (PSZ)"usage: FSDIVE failed to access FS functions.",
                   (PSZ)"Error!", 0, MB_OK | MB_MOVEABLE );
       }
   }
   pvmi = 0;
   ul = 0;
   if (pwinData->fFSBase) {
      WinSendMsg( pwinData->hwndFrame, WM_GetVideoModeTable, (MPARAM)&pvmi, (MPARAM)&ul);
      printf( "FullScreen available -> QueryFullScreen modes: %d,%d\n", pvmi, ul);
   }
   pwinData->ulWindowStyle = WS_DesktopDive;   // current window style
   
   printf( "InitDIVE buffer=%p done!\n", pwinData->pbBuffer);

   return 0; // init ok
}

//
// init dive buffer
//
int      InitDIVEBuffer( PWINDATA pwinData)
{
   ULONG     ulScanLineBytes;       /* Size of scan line for current window */
   ULONG     ulScanLines;           /* Number of scan lines in window       */

   printf( "InitDIVEBuffer: hwnd=%x DONE\n", (int) pwinData->hwndFrame);

   if (pwinData->pbBuffer) {
      DiveEndImageBufferAccess( pwinData->hDive, pwinData->ulImage);
      DiveFreeImageBuffer( pwinData->hDive, pwinData->ulImage);
      pwinData->pbBuffer = NULL;
   }
   if (pwinData->pbBuffer2) {
      DiveEndImageBufferAccess( pwinData->hDive, pwinData->ulImage2);
      DiveFreeImageBuffer( pwinData->hDive, pwinData->ulImage2);
      pwinData->pbBuffer2 = NULL;
   }

       // Allocate DIVE image buffer
   if (DiveAllocImageBuffer( pwinData->hDive,
                             &(pwinData->ulImage),
                             pwinData->fccColorFormat,
                             pwinData->ulWidth,
                             pwinData->ulHeight * (NUMSCREENS+1),
                             0, 0) ) {
      pwinData->hDive = 0;
      return 1;
   }
   if (DiveAllocImageBuffer( pwinData->hDive,
                             &(pwinData->ulImage2),
                             pwinData->fccColorFormat,
                             pwinData->ulWidth,
                             pwinData->ulHeight,
                             0, 0) ) {
      pwinData->hDive = 0;
      return 1;
   }

      // begin access
   ulScanLineBytes = 0;                  // leave 0, so dive will set to
   ulScanLines = 0;                      // best value
   DiveBeginImageBufferAccess( pwinData->hDive,
                               pwinData->ulImage,
                               (PPBYTE)&pwinData->pbBuffer,
                               &ulScanLineBytes,
                               &ulScanLines);
   ulScanLineBytes = 0;                  // leave 0, so dive will set to
   ulScanLines = 0;                      // best value
   DiveBeginImageBufferAccess( pwinData->hDive,
                               pwinData->ulImage2,
                               (PPBYTE)&pwinData->pbBuffer2,
                               &ulScanLineBytes,
                               &ulScanLines);

   printf( "InitDIVEBuffer: image#0 %ld, buffer=%p DONE\n", pwinData->ulImage, pwinData->pbBuffer);
   printf( "InitDIVEBuffer: image#1 %ld, buffer=%p DONE\n", pwinData->ulImage2, pwinData->pbBuffer2);

   return 0; // init ok
}

//
// close dive access
//
void     ShutdownDIVE( PWINDATA pwinData)
{
   DiveEndImageBufferAccess( pwinData->hDive, pwinData->ulImage);
   DiveEndImageBufferAccess( pwinData->hDive, pwinData->ulImage2);
   DiveFreeImageBuffer( pwinData->hDive, pwinData->ulImage);
   DiveFreeImageBuffer( pwinData->hDive, pwinData->ulImage2);

     // Make sure that we are in desk top mode
   WinPostMsg( pwinData->hwndFrame, WM_SetVideoMode, (MPARAM)WS_DesktopDive, 0);
   DiveClose( pwinData->hDive);
   pwinData->hDive = 0;
   printf( "ShutdownDIVE done!\n");
}


/**********************************************************
 *                  PALETTE MANAGER                       *
 **********************************************************/

/*
 * This function must be called on every REALIZE PALETTE
 * window message, so that DIVE gets to know the new
 * palette.
 */

void RealizePalette(WINDATA *this)
{
   if (!this->hDive)
      return;

#ifdef USE_PALETTE_MGR
   if (this->ulPalMode) {
      ULONG foo;
      WinRealizePalette(this->hwndClient, this->hps, &foo);
   }
#endif

   DiveSetDestinationPalette(this->hDive, 0, 0, 0);
} /* RealizePalette */

#ifdef USE_PALETTE_MGR

void CreatePalette(WINDATA *this)
{
   HAB hab = WinQueryAnchorBlock(this->hwndClient);
   ULONG aux[this->ulNumColors];
   ULONG count;
   SIZEL sizl;
   LONG  alCaps[CAPS_PHYS_COLORS];
   LONG  lCount = CAPS_PHYS_COLORS;
   LONG  lStart = CAPS_FAMILY;
   BOOL  fPaletteCaps;

   if (!this->ulPalMode)
      return;

   printf( "Entering CreatePalette...\n");

   /* We obtain a Device Context for our window. We next query whether this
      device allows palette changes. If true, we create an empty
      Presentation Space with an empty palette that we will update every
      time Doom's palette changes
    */

   sizl.cx =0;
   sizl.cy =0;
   this->hdc = WinOpenWindowDC(this->hwndClient);
   if ( this->hdc == NULLHANDLE )
   {
      printf( "WinOpenWindowDC Error 0x%04x\n", WinGetLastError(hab));
   }

   DevQueryCaps( this->hdc, lStart, lCount, alCaps );
   fPaletteCaps = alCaps[CAPS_ADDITIONAL_GRAPHICS] & CAPS_PALETTE_MANAGER;
   if ( fPaletteCaps == FALSE ) {
      this->ulPalMode = 0;
   }

   this->hps = GpiCreatePS(hab, this->hdc, &sizl,
			       PU_PELS | GPIF_DEFAULT
			       | GPIT_MICRO | GPIA_ASSOC);
   if ( this->hps == NULLHANDLE)
   {
      printf( "GpiCreatePS Error 0x%04x\n", WinGetLastError(hab));
   }

   /* We clear all entries, up to complete the whole Doom palette
    */
   for (count = 0; count < this->ulNumColors; count++) {
      aux[count] = PC_RESERVED * 16777216L;
   }

   /* We create a palette for this PS, and set it with these initial colors
    */
   this->hpal= GpiCreatePalette(hab, LCOL_PURECOLOR 
				    | LCOL_OVERRIDE_DEFAULT_COLORS,
				    LCOLF_CONSECRGB,
				    this->ulNumColors,
				    aux);
   if( this->hpal == NULLHANDLE || this->hpal == GPI_ERROR)
   {
      printf( "GpiCreatePalette Error 0x%04x\n", WinGetLastError(hab));
   }
   if (GpiSelectPalette(this->hps, this->hpal) == PAL_ERROR)
   {
      printf( "GpiSelectPalette Error 0x%04x\n", WinGetLastError(hab));
   }
   if( WinRealizePalette(this->hwndClient, this->hps, &count) == PAL_ERROR)
   {
      printf( "WinRealizePalette Error 0x%04x\n", WinGetLastError(hab));
   }
   printf( "Exiting CreatePalette...\n");

} /* CreatePalette */

void DestroyPalette(WINDATA *this)
{
   if ( this->ulPalMode )
   {
      ULONG foo;
      GpiSelectPalette(this->hps,0);
      GpiDeletePalette(this->hpal);
      WinRealizePalette(this->hwndClient, this->hps, &foo);
      GpiDestroyPS(this->hps);
   }
} /* DestroyPalette */

#endif
