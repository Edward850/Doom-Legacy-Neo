// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_system.c,v 1.18 2002/07/26 15:23:01 hurdler Exp $
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
// $Log: i_system.c,v $
// Revision 1.18  2002/07/26 15:23:01  hurdler
// near RC release
//
// Revision 1.17  2002/01/03 19:20:07  bock
// Add FreeBSD code to I_GetFreeMem.
// Modified Files:
//     makefile linux_x/i_system.c sdl/i_system.c
//
// Revision 1.16  2001/05/16 22:33:35  bock
// Initial FreeBSD support.
//
// Revision 1.15  2001/03/12 21:03:10  metzgermeister
//   * new symbols for rendererlib added in SDL
//   * console printout fixed for Linux&SDL
//   * Crash fixed in Linux SW renderer initialization
//
// Revision 1.14  2000/10/21 08:43:32  bpereira
// no message
//
// Revision 1.13  2000/10/16 07:53:57  metzgermeister
// fixed I_GetFreeMem
//
// Revision 1.12  2000/10/09 16:22:42  metzgermeister
// implemented GetFreeMem
//
// Revision 1.11  2000/10/02 18:25:47  bpereira
// no message
//
// Revision 1.10  2000/09/10 10:50:21  metzgermeister
// make it work again
//
// Revision 1.9  2000/08/11 19:11:07  metzgermeister
// *** empty log message ***
//
// Revision 1.8  2000/04/25 19:49:46  metzgermeister
// support for automatic wad search
//
// Revision 1.7  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.6  2000/04/12 19:31:37  metzgermeister
// added use_mouse to menu
//
// Revision 1.5  2000/04/07 23:12:38  metzgermeister
// fixed some minor bugs
//
// Revision 1.4  2000/03/22 18:52:56  metzgermeister
// added I_ShutdownCD to I_Quit
//
// Revision 1.3  2000/03/06 15:19:58  hurdler
// Add Bell Kin's changes
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include <stdarg.h>
#include <sys/time.h>
#ifdef LMOUSE2
#include <termios.h>
#endif
// statfs()
#ifndef FREEBSD
#include <sys/vfs.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
/*For meminfo*/
#include <sys/types.h>
#include <kvm.h>
#include <nlist.h>
#include <sys/vmmeter.h>
#include <fcntl.h>
#endif

#ifdef LJOYSTICK // linux joystick 1.x
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/joystick.h>
#endif

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#include "endtxt.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"
#include "i_joy.h"

extern void D_PostEvent(event_t*);

extern event_t         events[MAXEVENTS];
extern int             eventhead;
extern int             eventtail;

// Locations for searching the legacy.dat
#define DEFAULTWADLOCATION1 "/usr/local/games/legacy"
#define DEFAULTWADLOCATION2 "/usr/games/legacy"
#define DEFAULTSEARCHPATH1 "/usr/local"
#define DEFAULTSEARCHPATH2 "/usr/games"
#define WADKEYWORD "legacy.dat"

// holds wad path
static char returnWadPath[256];

#ifdef LJOYSTICK
int joyfd = -1;
int joyaxes = 0;
int joystick_started = 0;
int joy_scale = 1;
#endif
JoyType_t Joystick;

#ifdef LMOUSE2
int fdmouse2 = -1;
int mouse2_started = 0;
#endif

// dummy 19990119 by Kin
byte keyboard_started = 0;
void I_StartupKeyboard (void) {}
void I_StartupTimer (void) {}

void I_OutputMsg (char *fmt, ...) 
{
    va_list     argptr;

    va_start (argptr,fmt);
    vfprintf (stderr,fmt,argptr);
    va_end (argptr);
}

int I_GetKey (void) 
{
    // Warning: I_GetKey emties the event queue till next keypress
    event_t*    ev;
    int rc=0;
    
    // return the first keypress from the event queue
    for ( ; eventtail != eventhead ; eventtail = (++eventtail)&(MAXEVENTS-1) )
    {
        ev = &events[eventtail];
        if(ev->type == ev_keydown)
        {
            rc = ev->data1;
            continue;
        }
    }
    
    return rc; 
}

#ifdef LJOYSTICK
void I_JoyScale() {
  joy_scale = (cv_joyscale.value==0)?1:cv_joyscale.value;
}

void I_GetJoyEvent() {
  struct js_event jdata;
  static event_t event = {0,0,0,0};
  static int buttons = 0;
  if(!joystick_started) return;
  while(read(joyfd,&jdata,sizeof(jdata))!=-1) {
    switch(jdata.type) {
    case JS_EVENT_AXIS:
      event.type = ev_joystick;
      event.data1 = 0;
      switch(jdata.number) {
      case 0:
        event.data2 = ((jdata.value >> 5)/joy_scale)*joy_scale;
        D_PostEvent(&event);
        break;
      case 1:
        event.data3 = ((jdata.value >> 5)/joy_scale)*joy_scale;
        D_PostEvent(&event);
      default:
        break;
      }
      break;
    case JS_EVENT_BUTTON:
      if(jdata.number<JOYBUTTONS) {
        if(jdata.value) {
          if(!((buttons >> jdata.number)&1)) {
            buttons |= 1 << jdata.number;
            event.type = ev_keydown;
            event.data1 = KEY_JOY1+jdata.number;
            D_PostEvent(&event);
          }
        } else {
          if((buttons>>jdata.number)&1) {
            buttons ^= 1 << jdata.number;
            event.type = ev_keyup;
            event.data1 = KEY_JOY1+jdata.number;
            D_PostEvent(&event);
          }
        }
      }
      break;
    }
  }
}

void I_ShutdownJoystick() {
  if(joyfd!=-1) {
    close(joyfd);
    joyfd = -1;
  }
  joyaxes = 0;
  joystick_started = 0;
}

int joy_open(char *fname) {
  joyfd = open(fname,O_RDONLY|O_NONBLOCK);
  if(joyfd==-1) {
    CONS_Printf("Error opening %s!\n",fname);
    return 0;
  }
  ioctl(joyfd,JSIOCGAXES,&joyaxes);
  if(joyaxes<2) {
    CONS_Printf("Not enought axes?\n");
    joyaxes = 0;
    joyfd = -1;
    close(joyfd);
    return 0;
  }
  return joyaxes;
}
/*int joy_waitb(int fd, int *xpos,int *ypos,int *hxpos,int *hypos) {
  int i,xps,yps,hxps,hyps;
  struct js_event jdata;
  for(i=0;i<1000;i++) {
    while(read(fd,&jdata,sizeof(jdata))!=-1) {
      switch(jdata.type) {
      case JS_EVENT_AXIS:
        switch(jdata.number) {
        case 0: // x
          xps = jdata.value;
          break;
        case 1: // y
          yps = jdata.value;
          break;
        case 3: // hat x
          hxps = jdata.value;
          break;
        case 4: // hat y
          hyps = jdata.value;
        default:
          break;
        }
        break;
      case JS_EVENT_BUTTON:
        break;
      }
    }
  }
  }*/
#endif

void I_InitJoystick (void) {
#ifdef LJOYSTICK
  I_ShutdownJoystick();
  if(!strcmp(cv_usejoystick.string,"0"))
    return;
  if(!joy_open(cv_joyport.string)) return;
  joystick_started = 1;
  return;
#endif
}

#ifdef LMOUSE2
void I_GetMouse2Event() {
  static unsigned char mdata[5];
  static int i = 0,om2b = 0;
  int di,j,mlp,button;
  event_t event;
  const int mswap[8] = {0,4,1,5,2,6,3,7};
  if(!mouse2_started) return;
  for(mlp=0;mlp<20;mlp++) {
    for(;i<5;i++) {
      di = read(fdmouse2,mdata+i,1);
      if(di==-1) return;
    }
    if((mdata[0]&0xf8)!=0x80) {
      for(j=1;j<5;j++) {
        if((mdata[j]&0xf8)==0x80) {
          for(i=0;i<5-j;i++) { // shift
            mdata[i] = mdata[i+j];
          }
        }
      }
      if(i<5) continue;
    } else {
      button = mswap[~mdata[0]&0x07];
      for(j=0;j<MOUSEBUTTONS;j++) {
        if(om2b&(1<<j)) {
          if(!(button&(1<<j))) { //keyup
            event.type = ev_keyup;
            event.data1 = KEY_2MOUSE1+j;
            D_PostEvent(&event);
            om2b ^= 1 << j;
          }
        } else {
          if(button&(1<<j)) {
            event.type = ev_keydown;
            event.data1 = KEY_2MOUSE1+j;
            D_PostEvent(&event);
            om2b ^= 1 << j;
          }
        }
      }
      event.data2 = ((signed char)mdata[1])+((signed char)mdata[3]);
      event.data3 = ((signed char)mdata[2])+((signed char)mdata[4]);
      if(event.data2&&event.data3) {
        event.type = ev_mouse2;
        event.data1 = 0;
        D_PostEvent(&event);
      }
    }
    i = 0;
  }
}

void I_ShutdownMouse2() {
  if(fdmouse2!=-1) close(fdmouse2);
  mouse2_started = 0;
}

#endif

void I_StartupMouse2 (void) {
#ifdef LMOUSE2
  struct termios m2tio;
  int i,dtr,rts;
  I_ShutdownMouse2();
  if(cv_usemouse2.value == 0) return;
  if((fdmouse2 = open(cv_mouse2port.string,O_RDONLY|O_NONBLOCK|O_NOCTTY))==-1) {
    CONS_Printf("Error opening %s!\n",cv_mouse2port.string);
    return;
  }
  tcflush(fdmouse2, TCIOFLUSH);
  m2tio.c_iflag = IGNBRK;
  m2tio.c_oflag = 0;
  m2tio.c_cflag = CREAD|CLOCAL|HUPCL|CS8|CSTOPB|B1200;
  m2tio.c_lflag = 0;
  m2tio.c_cc[VTIME] = 0;
  m2tio.c_cc[VMIN] = 1;
  tcsetattr(fdmouse2, TCSANOW, &m2tio);
  strupr(cv_mouse2opt.string);
  for(i=0,rts = dtr = -1;i<strlen(cv_mouse2opt.string);i++) {
    if(cv_mouse2opt.string[i]=='D') {
      if(cv_mouse2opt.string[i+1]=='-') {
        dtr = 0;
      } else {
        dtr = 1;
      }
    }
    if(cv_mouse2opt.string[i]=='R') {
      if(cv_mouse2opt.string[i+1]=='-') {
        rts = 0;
      } else {
        rts = 1;
      }
    }
  }
  if((dtr!=-1)||(rts!=-1)) {
    if(!ioctl(fdmouse2, TIOCMGET, &i)) {
      if(!dtr) {
        i &= ~TIOCM_DTR;
      } else {
        if(dtr>0) i |= TIOCM_DTR;
      }
      if(!rts) {
        i &= ~TIOCM_RTS;
      } else {
        if(rts>0) i |= TIOCM_RTS;
      }
      ioctl(fdmouse2, TIOCMSET, &i);
    }
  }
  mouse2_started = 1;
#endif
}

// return free and total physical memory in the system

#define MEMINFO_FILE "/proc/meminfo"
#define MEMTOTAL "MemTotal:"
#define MEMFREE "MemFree:"

ULONG I_GetFreeMem(ULONG *total)
{
#ifndef FREEBSD
    char buf[1024];    
    char *memTag;
    ULONG freeKBytes;
    ULONG totalKBytes;
    int n;
    int meminfo_fd = -1;

    meminfo_fd = open(MEMINFO_FILE, O_RDONLY);
    n = read(meminfo_fd, buf, 1023);
    close(meminfo_fd);
    
    if(n<0)
    {
        // Error
        *total = 0L;
        return 0;
    }
    
    buf[n] = '\0';
    if(NULL == (memTag = strstr(buf, MEMTOTAL)))
    {
        // Error
        *total = 0L;
        return 0;
    }
        
    memTag += sizeof(MEMTOTAL);
    totalKBytes = atoi(memTag);
    
    if(NULL == (memTag = strstr(buf, MEMFREE)))
    {
        // Error
        *total = 0L;
        return 0;
    }
        
    memTag += sizeof(MEMFREE);
    freeKBytes = atoi(memTag);
    
    *total = totalKBytes << 10;
    return freeKBytes << 10;
#else /*FREEBSD*/
	/*FREEBSD defined*/
	struct  vmmeter sum;
	kvm_t *kd;
	struct nlist namelist[]= {
#define X_SUM   0
		{"_cnt"},
		{ NULL }
	};
	if ((kd = kvm_open(NULL, NULL, NULL, O_RDONLY, "kvm_open")) == NULL)
	{
		*total = 0L;
		return 0;
	}
	if (kvm_nlist(kd, namelist) != 0)
	{
		kvm_close (kd);
		*total = 0L;
		return 0;
	}
	if (kvm_read(kd,namelist[X_SUM].n_value ,&sum, sizeof(sum)) != sizeof(sum))
	{
		kvm_close (kd);
		*total = 0L;
		return 0;
	}
	kvm_close (kd);

	*total = sum.v_page_count * sum.v_page_size;
	return sum.v_free_count * sum.v_page_size;
#endif /*FREEBSD*/
}

static int quiting=0; /* prevent recursive I_Quit() */

void I_Tactile( int   on,
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
// I_GetTime
// returns time in 1/TICRATE second tics
//
tic_t  I_GetTime (void)
{
    struct timeval      tp;
    struct timezone     tzp;
    int                 newtics;
    static int          oldtics=0;
    static int          basetime=0;
  
again:
    gettimeofday(&tp, &tzp);
    if (!basetime)
        basetime = tp.tv_sec;

    // On systems with RTC drift correction or NTP we need to take
    // care about the system clock running backwards sometimes. Make
    // sure the new tic is later then the last one.
    newtics = (tp.tv_sec-basetime)*TICRATE + tp.tv_usec*TICRATE/1000000;
    if (!oldtics)
        oldtics = newtics;
    if (newtics < oldtics) {
        I_WaitVBL(1);
        goto again;
    }
    oldtics = newtics;
    return newtics;
}



//
// I_Init
//
void I_Init (void)
{
    I_StartupSound();
    I_InitMusic();
    quiting = 0;
    //  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit (void)
{
    /* prevent recursive I_Quit() */
    if(quiting) return;
    quiting = 1;
  //added:16-02-98: when recording a demo, should exit using 'q' key,
  //        but sometimes we forget and use 'F10'.. so save here too.
    if (demorecording)
        G_CheckDemoStatus();
    D_QuitNetGame ();
    I_ShutdownMusic();
    I_ShutdownSound();
    I_ShutdownCD();
   // use this for 1.28 19990220 by Kin
    M_SaveConfig (NULL);
    I_ShutdownGraphics();
    I_ShutdownSystem();
    printf("\r");
    ShowEndTxt();
    exit(0);
}

void I_WaitVBL(int count)
{
#ifdef SGI
    sginap(1);                                           
#else
#ifdef SUN
    sleep(0);
#else
    usleep (count * (1000000/70) );                                
#endif
#endif
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


//
// I_Error
//
extern boolean demorecording;

void I_Error (char *error, ...)
{
    va_list     argptr;

    // Message first.
    va_start (argptr,error);
    fprintf (stderr, "Error: ");
    vfprintf (stderr,error,argptr);
    fprintf (stderr, "\n");
    va_end (argptr);

    fflush( stderr );

    // Shutdown. Here might be other errors.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownMusic();
    I_ShutdownSound();
    I_ShutdownGraphics();
    // shutdown everything else which was registered
    I_ShutdownSystem();
    
    exit(-1);
}

#define MAX_QUIT_FUNCS     16
typedef void (*quitfuncptr)();
static quitfuncptr quit_funcs[MAX_QUIT_FUNCS] =
               { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
               };
//
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


//
//  Removes a function from the list that need to be called by
//   I_SystemShutdown().
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

//
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
      if (quit_funcs[c])
         (*quit_funcs[c])();

}

void I_GetDiskFreeSpace(long long *freespace) {
  struct statfs stfs;
  if(statfs(".",&stfs)==-1) {
    *freespace = MAXINT;
    return;
  }
  *freespace = stfs.f_bavail*stfs.f_bsize;
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
    return mkdir(dirname, unixright);
}

// check if legacy.dat exists in the given path
static boolean isWadPathOk(char *path) {
    char wad3path[256];
    
    sprintf(wad3path, "%s/%s", path, WADKEYWORD);
    
    if(access(wad3path, R_OK)) {
        return false; // no access
    }
    
    return true;
}

// search for legacy.dat in the given path
static char *searchWad(char *searchDir) 
{
    int pipeDescr[2];
    pid_t childPid;
    boolean finished;
    
    printf("Searching directory '%s' for '%s' ... please wait\n", searchDir, WADKEYWORD);
    
    if(pipe(pipeDescr) == -1) {
        fprintf(stderr, "Unable to open pipe\n");
        return NULL;
    }
    
    // generate child process
    childPid = fork();
    
    if(childPid == -1) {
        fprintf(stderr, "Unable to fork\n");
        return NULL;
    }
    
    if(childPid == 0) { // here comes the child
        close(pipeDescr[0]);
        
        // set stdout to pipe
        dup2(pipeDescr[1], STDOUT_FILENO);
        
        // execute the find command
        execlp("find", "find", searchDir, "-name", WADKEYWORD, NULL);
        exit(1); // shouldn't be reached
    }
    
    // parent
    close(pipeDescr[1]);
    
    // now we have to wait for the output of 'find'
    finished = false;
    
    while(!finished) {
        char *namePtr;
        int pathLen;
        
        pathLen = read(pipeDescr[0], returnWadPath, 256);
        
        if(pathLen == 0) { // end of "file" reached
            return NULL;
        }
        
        if(pathLen == -1) { // should not happen
            fprintf(stderr, "searchWad: reading in non-blocking mode - please fix me\n");
            return NULL;
        }
        
        namePtr = strstr(returnWadPath, WADKEYWORD); // check if we read something sensible
        if(namePtr--) {
            *namePtr = 0; //terminate string before legacy.dat
            finished = true;
        }
    }
    
    // kill child ... oops
    kill(childPid, SIGKILL);
    
    return returnWadPath;
}

// go through all possible paths and look for legacy.dat
static char *locateWad(void)
{
    char *WadPath;
    char *userhome;
    
    // does DOOMWADDIR exist?
    WadPath = getenv("DOOMWADDIR");
    if(WadPath) {
        if(isWadPathOk(WadPath)) {
            return WadPath;
        }
    }
    
    // examine current dir
    strcpy(returnWadPath, ".");
    if(isWadPathOk(returnWadPath)) {
        return returnWadPath;
    }
    
    // examine default dirs
    strcpy(returnWadPath, DEFAULTWADLOCATION1);
    if(isWadPathOk(returnWadPath)) {
        return returnWadPath;
    }
    strcpy(returnWadPath, DEFAULTWADLOCATION2);
    if(isWadPathOk(returnWadPath)) {
        return returnWadPath;
    }
    
    // find in $HOME
    userhome = getenv("HOME");
    if(userhome) {
        WadPath = searchWad(userhome);
        if(WadPath) {
            return WadPath;
        }
    }
    
    // find in /usr/local
    WadPath = searchWad(DEFAULTSEARCHPATH1);
    if(WadPath) {
        return WadPath;
    }
    // find in /usr/games
    WadPath = searchWad(DEFAULTSEARCHPATH2);
    if(WadPath) {
        return WadPath;
    }
    
    // if nothing was found
    return NULL;
}

void I_LocateWad(void) {
    char *waddir;

    waddir = locateWad();

    if(waddir) {
        chdir(waddir); // change to the directory where we found legacy.dat
    }

    return;
}
