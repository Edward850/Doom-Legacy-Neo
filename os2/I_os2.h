#ifndef __I_OS2_H__
#define __I_OS2_H__

#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_OS2MM
#include <os2.h>
#include <os2me.h>
#include <dive.h>

//
// full screen dive access
//
#define WM_GetVideoModeTable  0x04A2
#define WM_SetVideoMode       0x04A0
#define WM_NotifyVideoModeChange 0x04A1
#define WS_DesktopDive    0x00000000L   // Desktop dive window style
#define WS_MaxDesktopDive 0x00000001L   // Maximized desktop dive window style
#define WS_FullScreenDive 0x00000002L   // Full-screen 320x200x256 dive style

/*   Menu items definitions
*/
#define ID_MAINWND      256
#define ID_MAINWND	256
#define ID_OPTIONS	257
#define ID_SNAP		258
#define ID_SNAP2	259
#define ID_SNAPFULL	264
#define ID_EXIT		260
#define ID_NEWTEXT	261
#define ID_START	262
#define ID_PAUSE	263
#define ID_PALINIT	265
#define ID_PALCHGD	266
#define ID_PALCLOSE	267
#define ID_INIT         268
#define ID_FSMODE	269
#define ID_DESKMODE	270
#define ID_SWMOUSE	271
#define ID_RUNGAME	272
#define ID_MAXMODE	273

#define ID_DIALOG       262
#define ID_EF_11         11
#define ID_EF_12         12
#define ID_EF_13         13
#define ID_EF_14         14
#define ID_EF_15         15
#define ID_EF_16         16
#define ID_EF_17         17
#define ID_EF_18         18
#define ID_EF_19         19
#define ID_EF_20         20

#define SEM32_EVENTMUTEX "\\sem32\\eventmutex"

/*   Maximum number of files to support
*/
#define  MAX_FILE        14       /* Maximum number of bitmap files    */
#define  MAX_BUFFERS      2


//
//   OS/2 data structure
//
typedef struct _WINDATA
{
   HAB    hab;
   HMQ    hmq;                      /* Message queue handle                 */
   ULONG  ulToEnd;                  /* stop running threads                */
   //int    loading;                  /* Doom/2 is loading                   */
   BOOL   fCheckFps;                /* check for fps rating */

   BOOL   fChgSrcPalette;           /* Flag for change of source palette   */
   BOOL   fDataInProcess;           /* ????  Visual region enable/disable  */

   char      title[ 128];                /* Frame window title                  */
   HWND      hwndFrame;                  /* Frame window handle                 */
   HWND      hwndClient;                 /* Client window handle                */
   DIVE_CAPS DiveCaps;
   FOURCC    fccFormats[100];            /* Color format code                   */
   ULONG     ulColorBits;                /* Color bit depth                     */
   ULONG     ulNumColors;                /* Number of colors in bitmaps         */
   ULONG     ulWidth;                    /* Bitmap width in pels                */
   ULONG     ulHeight;                   /* Bitmap Height in pels               */
   FOURCC    fccColorFormat;             /* Bitmap color format                 */
   
   TID      tidPMThread;                 /* Thread ID for PM               */
   TID      tidDoomThread;               /* Thread ID for Doom/2           */
   TID      tidBlitThread;               /* Thread ID for blitting routine */

   HDIVE    hDive;                       /* DIVE handle                    */
   BOOL     fFSBase;                     /* FS DIVE support in base OS     */
   BOOL     fSwitching;                  /* 1 to prevent action of WindowSetBlit */
   ULONG    ulWindowStyle;               /* current window style           */
   BOOL     fBlitReady;                  /* ready for blitting             */
   ULONG    currentImage;                /* current working image          */
   ULONG    ulImage;                     /* dive image buffer              */
   PBYTE    pbBuffer;                    /* dive image buffer              */
   ULONG    ulImage2;                    /* dive second image buffer       */
   PBYTE    pbBuffer2;                   /* dive second image buffer       */
   HMTX     hmtxEventSem;                /* event queue mutex              */

   USHORT   usDartID;                    /* Amp Mixer device id     */
   ULONG    ulBufferCount;               /* Current file buffer     */
   ULONG    ulNumBuffers;                /* Number of file buffers  */
   ULONG    ulNumReturnedBuffers;        /* Num of returned buffers */
   MCI_MIX_BUFFER       MixBuffers[MAX_BUFFERS];   /* Device buffers          */
   MCI_MIXSETUP_PARMS   MixSetupParms;   /* Mixer parameters        */
   MCI_BUFFER_PARMS     BufferParms;     /* Device buffer parms     */
   int                  flag;            /* flag for buffer play    */
   int                  FillBuffer;      /* current fill buffer     */
   int                  PlayBuffer;      /* current play buffer     */

   USHORT   usMidiID;                    /* Midi device id          */
   int      looping;                     /* loop music flag         */
   int      midiVolume;                  /* Midi music volume       */
   BOOL     fPassedDevice;

   BOOL fShiftPressed; 		/* Status of the SHIFT key */
   BOOL fAltPressed;		/* Status of the ALT key   */
   BOOL fRunMode;		/* Status of Run Mode      */
   int  last_char;		/* Last ASCII char pressed */

} WINDATA, *PWINDATA;

typedef  PBYTE    *PPBYTE;


//
// extern OS/2 data
//
extern WINDATA* pmData;
extern int      appActive;                      //app window is active

//
// function prototypes
//
void     D_OS2DoomMain( void);

void  InitDART( PWINDATA);
void  ShutdownDART( PWINDATA);
void  PlayDART( PWINDATA);
int   RegisterMIDI( PWINDATA, void*, int);
void  OpenMIDI( PWINDATA);
int   PlayMIDI( PWINDATA, int looping);
void  PauseMIDI( PWINDATA);
void  ResumeMIDI( PWINDATA);
void  ShutdownMIDI( PWINDATA);
void  SetMIDIVolume( PWINDATA midiData, int vol);
void  MciError( int);

int   InitDIVE( PWINDATA);
int   InitDIVEBuffer( PWINDATA);
void  ShutdownDIVE( PWINDATA);
void  DiveVREnabled( PWINDATA);
void  PrepareForModeChange(WINDATA *this, MPARAM mp1, MPARAM mp2);
void  WindowSetBlit (WINDATA* this, int on);
void  RealizePalette(WINDATA *this);

#endif // __I_OS2_H__
