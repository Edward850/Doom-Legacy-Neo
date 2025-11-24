// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: I_system.c,v 1.5 2000/10/21 08:43:32 bpereira Exp $
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
// $Log: I_system.c,v $
// Revision 1.5  2000/10/21 08:43:32  bpereira
// no message
//
// Revision 1.4  2000/10/02 18:25:46  bpereira
// no message
//
// Revision 1.3  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//     Misc. stuff
//     Startup & Shutdown routines for music,sound,timer,keyboard,...
//     Signal handler to trap errors and exit cleanly.
//
//-----------------------------------------------------------------------------


#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <stdarg.h>
#include <sys/time.h>


#ifdef DJGPP
 #include <dpmi.h>
 #include <go32.h>
 #include <pc.h>
 #include <dos.h>
 #include <crt0.h>
 #include <sys/segments.h>
 #include <sys/nearptr.h>

 #include <keys.h>
#endif


#include "../doomdef.h"
#include "../m_misc.h"
#include "../i_video.h"
#include "../i_sound.h"
#include "../i_system.h"
#include "../d_net.h"
#include "../g_game.h"

#include "../d_main.h"

#include "../m_argv.h"

#include "../w_wad.h"
#include "../z_zone.h"
#include "../g_input.h"

#include "../console.h"

#ifdef __GNUG__
 #pragma implementation "../i_system.h"
#endif

#include "../i_joy.h"

//### let's try with Allegro ###
#define  alleg_mouse_unused
//#define  alleg_timer_unused
#define  alleg_keyboard_unused
//#define  alleg_joystick_unused
#define  alleg_gfx_driver_unused
#define  alleg_palette_unused
#define  alleg_graphics_unused
#define  alleg_vidmem_unused
#define  alleg_flic_unused
#define  alleg_sound_unused
#define  alleg_file_unused
#define  alleg_datafile_unused
#define  alleg_math_unused
#define  alleg_gui_unused
#include <allegro.h>
//### end of Allegro include ###



// Do not execute cleanup code more than once. See Shutdown_xxx() routines.
byte graphics_started=false;
byte keyboard_started=false;
byte sound_started=false;
byte timer_started=false;

/* Mouse stuff */
byte mouse_detected=false;

volatile tic_t ticcount;   //returned by I_GetTime(), updated by timer interrupt




void I_Tactile ( int   on,   int   off,   int   total )
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
//  Allocates the base zone memory,
//  this function returns a valid pointer and size,
//  else it should interrupt the program immediately.
//
//added:11-02-98: now checks if mem could be allocated, this is still
//    prehistoric... there's a lot to do here: memory locking, detection
//    of win95 etc...
//
boolean   win95;
boolean   lockmem;

void I_DetectWin95 (void)
{
    __dpmi_regs     r;

    r.x.ax = 0x160a;        // Get Windows Version
    __dpmi_int(0x2f, &r);

    if(r.x.ax || r.h.bh < 4)    // Not windows or earlier than Win95
    {
        win95 = false;
    }
    else
    {
        CONS_Printf ("Windows 95 detected\n");
        win95 = true;
    }
}

ULONG I_GetFreeMem(ULONG *total)
{
    _go32_dpmi_meminfo     info;

    _go32_dpmi_get_free_memory_information(&info);
    if( total )
        *total = info.total_physical_pages<<12; // <<12 for convert page to byte
    return info.available_physical_pages<<12;
}


/*==========================================================================*/
// I_GetTime ()
/*==========================================================================*/
tic_t inline I_GetTime (void)
{
    return ticcount;
}

byte joystick_detected=false;
JoyType_t   Joystick;

//
// I_Init
//


void I_WaitJoyButton (void)
{
     CON_Drawer ();
     I_FinishUpdate ();        // page flip or blit buffer

     do {
         poll_joystick();
     } while (!(joy_b1 || joy_b2));

     while (joy_b1 || joy_b2)
         poll_joystick();
}


void I_InitJoystick (void)
{
    //init the joystick
    joystick_detected=0;
    if (cv_usejoystick.value && !M_CheckParm("-nojoy"))
    {
        joy_type = JOY_TYPE_4BUTTON;
        if(initialise_joystick()==0) {

            switch(cv_usejoystick.value) {
               case 1 : joy_type = JOY_TYPE_4BUTTON; break;
               case 2 : joy_type = JOY_TYPE_STANDARD;    break;
               case 3 : joy_type = JOY_TYPE_6BUTTON;     break;
               case 4 : joy_type = JOY_TYPE_WINGEX;      break;
               case 5 : joy_type = JOY_TYPE_FSPRO;       break;
               // new since 1.28 support from allegro 3.11
               case 6 : joy_type = JOY_TYPE_8BUTTON;     break;
               case 7 : joy_type = JOY_TYPE_SIDEWINDER;  break;
               case 8 : joy_type = JOY_TYPE_GAMEPAD_PRO; break;
               case 9 : joy_type = JOY_TYPE_SNESPAD_LPT1;break;
               case 10: joy_type = JOY_TYPE_SNESPAD_LPT2;break;
               case 11: joy_type = JOY_TYPE_SNESPAD_LPT3;break;
               case 12: joy_type = JOY_TYPE_WINGWARRIOR; break;
            }
            // only gamepadstyle joysticks
            Joystick.bGamepadStyle=true;

            CONS_Printf("\2CENTER the joystick and press a button:"); I_WaitJoyButton ();
            initialise_joystick();
            CONS_Printf("\nPush the joystick to the UPPER LEFT corner and press a button\n"); I_WaitJoyButton ();
            calibrate_joystick_tl();
            CONS_Printf("Push the joystick to the LOWER RIGHT corner and press a button\n"); I_WaitJoyButton ();
            calibrate_joystick_br();
            if(joy_type== JOY_TYPE_WINGEX || joy_type == JOY_TYPE_FSPRO)
            {
                CONS_Printf("Put Hat at Center and press a button\n"); I_WaitJoyButton ();
                calibrate_joystick_hat(JOY_HAT_CENTRE);
                CONS_Printf("Put Hat at Up and press a button\n"); I_WaitJoyButton ();
                calibrate_joystick_hat(JOY_HAT_UP);
                CONS_Printf("Put Hat at Down and press a button\n"); I_WaitJoyButton ();
                calibrate_joystick_hat(JOY_HAT_DOWN);
                CONS_Printf("Put Hat at Left and press a button\n"); I_WaitJoyButton ();
                calibrate_joystick_hat(JOY_HAT_LEFT);
                CONS_Printf("Put Hat at Right and press a button\n"); I_WaitJoyButton ();
                calibrate_joystick_hat(JOY_HAT_RIGHT);
            }
            joystick_detected=1;
        }
        else
        {
            CONS_Printf("\2No Joystick detected.\n");
        }
    }
}



//
// I_Error
//
//added:18-02-98: put an error message (with format) on stderr
void I_OutputMsg (char *error, ...)
{
    va_list     argptr;

    va_start (argptr,error);
    vfprintf (stderr,error,argptr);
    va_end (argptr);

    // dont flush the message!
}

int errorcount=0; // fuck recursive errors
int shutdowning=false;

//added 31-12-97 : display error messy after shutdowngfx
void I_Error (char *error, ...)
{
    va_list     argptr;
    // added 11-2-98 recursive error detecting

    if(shutdowning)
    {
        errorcount++;
        if(errorcount==5)
           I_ShutdownGraphics();
        if(errorcount==6)
           I_ShutdownSystem();
        if(errorcount>7)
          exit(-1);       // recursive errors detected
    }
    shutdowning=true;

    // put message to stderr
    va_start (argptr,error);
    fprintf (stderr, "Error: ");
    vfprintf (stderr,error,argptr);
#ifdef DEBUGFILE
    if (debugfile)
    {
        fprintf (debugfile,"I_Error :");
        vfprintf (debugfile,error,argptr);
    }
#endif

    va_end (argptr);

    //added:18-02-98: save one time is enough!
    if (!errorcount)
    {
        M_SaveConfig (NULL);   //save game config, cvars..
    }

    //added:16-02-98: save demo, could be useful for debug
    //                NOTE: demos are normally not saved here.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame ();

    /* shutdown everything that was started ! */
    I_ShutdownSystem();

    fprintf (stderr, "\nPress ENTER");
    fflush( stderr );
    getchar();

    exit(-1);
}


//
// I_Quit : shutdown everything cleanly, in reverse order of Startup.
//
void I_Quit (void)
{
    byte* endoom;

    //added:16-02-98: when recording a demo, should exit using 'q' key,
    //        but sometimes we forget and use 'F10'.. so save here too.
    if (demorecording)
        G_CheckDemoStatus();

    M_SaveConfig (NULL);   //save game config, cvars..


    endoom = W_CacheLumpName("ENDOOM",PU_CACHE);


    //added:03-01-98: maybe it needs that the ticcount continues,
    // or something else that will be finished by ShutdownSystem()
    // so I do it before.
    D_QuitNetGame ();

    /* shutdown everything that was started ! */
    I_ShutdownSystem();

    puttext(1,1,80,25,endoom);
    gotoxy(1,24);

    if(shutdowning || errorcount)
        I_Error("Error detected (%d)",errorcount);

    fflush(stderr);

    exit(0);
}


//added:12-02-98: does want to work!!!! rhaaahahha
void I_WaitVBL(int count)
{
   while(count-->0);
   {
     do {
     } while (inportb(0x3DA) & 8);
     do {
     } while (!(inportb(0x3DA) & 8));
   }

}

//  Fab: this is probably to activate the 'loading' disc icon
//       it should set a flag, that I_FinishUpdate uses to know
//       whether it draws a small 'loading' disc icon on the screen or not
//
//  also it should explicitly draw the disc because the screen is
//  possibly not refreshed while loading
//
void I_BeginRead (void)
{
}

//  Fab: see above, end the 'loading' disc icon, set the flag false
//
void I_EndRead (void)
{
}

byte*   I_AllocLow(int length)
{
    byte*       mem;

    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;

}

#define MOUSE2
/* Secondary Mouse*/
#ifdef MOUSE2
_go32_dpmi_seginfo oldmouseinfo,newmouseinfo;
boolean mouse2_started=0;
USHORT  mouse2port;
byte    mouse2irq;
volatile int     handlermouse2buttons;
volatile int     handlermouse2x,handlermouse2y;
// internal use
volatile int     bytenum;
volatile byte    combytes[8];

//
// support a secondary mouse without mouse driver !
//
// take from the PC-GPE by Mark Feldman
static void I_MicrosoftMouseIntHandler()
{
  char   dx,dy;
  byte   inbyte;

  // Get the port byte
  inbyte = inportb(mouse2port);

  // Make sure we are properly "synched"
  if((inbyte & 64)== 64 || bytenum>7)
      bytenum = 0;

  // Store the byte and adjust bytenum
  combytes[bytenum] = inbyte;
  bytenum++;

  // Have we received all 3 bytes?
  if(bytenum==3)
  {
      // Yes, so process them
      dx = ((combytes[0] & 3) << 6) + combytes[1];
      dy = ((combytes[0] & 12) << 4) + combytes[2];
      handlermouse2x+= dx;
      handlermouse2y+= dy;
      handlermouse2buttons = (combytes[0] & (32+16)) >>4;
  }
  else
  if(bytenum==4) // for logitech 3 buttons
  {
      if(combytes[3] & 32)
          handlermouse2buttons |= 4;
      else
          handlermouse2buttons &= ~4;
  }

  // Acknowledge the interrupt
  outportb(0x20,0x20);
}
END_OF_FUNCTION(I_MicrosoftMouseIntHandler);

// wait ms milliseconde
void I_Delay(int ms)
{
    tic_t  starttime;

    if(timer_started)
    {
       starttime=I_GetTime()+(TICRATE*ms)/1000;
       while(starttime>=I_GetTime())
          ;
    }
    else
        delay(ms);
}

//
//  Removes the mouse2 handler.
//
void I_ShutdownMouse2()
{
    event_t event;
    int i;

    if( !mouse2_started )
        return;

    outportb(mouse2port+4,0x00);   // shutdown mouse (DTR & RTS = 0)
    I_Delay(1);
    outportb(mouse2port+1,0x00);   // disable COM interuption

    asm("cli");
    _go32_dpmi_set_protected_mode_interrupt_vector(mouse2irq, &oldmouseinfo);
    _go32_dpmi_free_iret_wrapper(&newmouseinfo);
    asm("sti");

    handlermouse2x=handlermouse2y=handlermouse2buttons=0;
    // emulate the up of all mouse buttons
    for(i=0;i<MOUSEBUTTONS;i++)
    {
        event.type=ev_keyup;
        event.data1=KEY_2MOUSE1+i;
        D_PostEvent(&event);
    }

    mouse2_started=false;
}

byte   ComIrq[4]={0x0c,0x0b,0x0c,0x0b};
USHORT ComPort[4]={0x3F8,0x2F8,0x3E8,0x2E8};
//
//  Installs the mouse2 handler.
//
void I_StartupMouse2()
{
    int i;
    boolean  found;
    __dpmi_regs r;

    if( mouse2_started )
        I_ShutdownMouse2();

    if(!cv_usemouse2.value)
        return;

    handlermouse2x=handlermouse2y=handlermouse2buttons=0;

    mouse2irq =ComIrq[cv_mouse2port.value-1];
    mouse2port=ComPort[cv_mouse2port.value-1];
    CONS_Printf("Using %s (irq %d, port 0x%x)\n",cv_mouse2port.string,mouse2irq-8,mouse2port);
    r.x.ax=0x24;
    __dpmi_int(0x33,&r);
    if(r.h.cl+8==mouse2irq)
    {
        CONS_Printf("Irq conflict with mouse 1\n"
                    "Use mouse2port to change the port\n");
        return;
    }

    // install irq wrapper
    asm("cli");
    _go32_dpmi_get_protected_mode_interrupt_vector(mouse2irq, &oldmouseinfo);
    newmouseinfo.pm_selector=_go32_my_cs();
    newmouseinfo.pm_offset=(int)I_MicrosoftMouseIntHandler;
    _go32_dpmi_allocate_iret_wrapper(&newmouseinfo);
    _go32_dpmi_set_protected_mode_interrupt_vector(mouse2irq, &newmouseinfo);

    LOCK_VARIABLE(bytenum);
    LOCK_VARIABLE(handlermouse2x);
    LOCK_VARIABLE(handlermouse2y);
    LOCK_VARIABLE(handlermouse2buttons);
    LOCK_VARIABLE(mouse2port);
    _go32_dpmi_lock_data(combytes,sizeof(combytes));
    LOCK_FUNCTION(I_MicrosoftMouseIntHandler);
    asm("sti");

    outportb(mouse2port+4,0   );   // shutdown mouse (DTR & RTS = 0)
    I_Delay(1);
    outportb(mouse2port+1,0   );   // disable COM interuption
    I_Delay(1);
    outportb(mouse2port+3,0x80);   // change status of port +0 et +1
    I_Delay(1);                    // for baudrate programmation
    outportb(mouse2port  ,0x60);   // 1200 LSB
    I_Delay(1);
    outportb(mouse2port+1,0   );   // 1200 MSB
    I_Delay(1);
    outportb(mouse2port+3,0x02);   // set port protocol 7N1
    I_Delay(1);
    outportb(mouse2port+1,0x01);   // enable COM interuption
    I_Delay(1);
    outportb(0x21,0x0);

    // wait to be sure the mouse have shutdown
    I_Delay(100);

    outportb(mouse2port+4,0x0b);   // restart mouse
    i=I_GetTime()+TICRATE;
    found=cv_usemouse2.value==2;
    while (I_GetTime()<i || !found)
       if(combytes[0]!='M')
          found=true;

    if(found || cv_usemouse2.value==2)
    {
        CONS_Printf("Microsoft compatible Secondary Mouse detected\n");

    //register shutdown mouse2 code.
    I_AddExitFunc(I_ShutdownMouse2);
    mouse2_started = true;
    }
    else
    {
        CONS_Printf("Secondary Mouse not found\n");
        // remove irq wraper
        I_ShutdownMouse2();
    }
}
#endif

//  Initialise the mouse. Doesnt need to be shutdown.
//
void I_StartupMouse (void)
{
    __dpmi_regs r;

    // mouse detection may be skipped by setting usemouse false
    if(cv_usemouse.value == 0)
    {
        mouse_detected=false;
        I_ShutdownMouse2();
        return;
    }

    //detect mouse presence
    r.x.ax=0;
    __dpmi_int(0x33,&r);

    //added:03-01-98:
    if( r.x.ax == 0 && cv_usemouse.value != 2)
    {
        mouse_detected=false;
        CONS_Printf("\2I_StartupMouse: mouse not present.\n");
    }
    else
    {
        mouse_detected=true;

    //hide cursor
    r.x.ax=2;
    __dpmi_int(0x33,&r);

    //reset mickey count
    r.x.ax=0x0b;
    __dpmi_int(0x33,&r);
    }
}

void I_GetEvent (void)
{
    __dpmi_regs r;
    event_t event;

    int i;

#ifdef MOUSE2
    // mouse may be disabled during the game by setting usemouse false
    if (mouse2_started)
    {
        //mouse movement
        static byte lastbuttons2=0;

        // post key event for buttons
        if (handlermouse2buttons!=lastbuttons2)
        {
            int j=1,k;
            k=(handlermouse2buttons ^ lastbuttons2); // only changed bit to 1
            lastbuttons2=handlermouse2buttons;

            for(i=0;i<MOUSEBUTTONS;i++,j<<=1)
                if(k & j)
                {
                    if(handlermouse2buttons & j)
                       event.type=ev_keydown;
                    else
                       event.type=ev_keyup;
                    event.data1=KEY_2MOUSE1+i;
                    D_PostEvent(&event);
                }
        }

        if ((handlermouse2x!=0)||(handlermouse2y!=0))
        {
            event.type=ev_mouse2;
            event.data1=0;
//          event.data1=buttons;    // not needed
            event.data2=handlermouse2x<<1;
            event.data3=-handlermouse2y<<1;

            D_PostEvent(&event);
            handlermouse2x=0;
            handlermouse2y=0;
        }

    }
#endif
    if(mouse_detected)
    {
        //mouse movement
        int xmickeys,ymickeys,buttons;
        static int lastbuttons=0;

        r.x.ax=0x0b;           // ask the mouvement not the position
        __dpmi_int(0x33,&r);
        xmickeys=(signed short)r.x.cx;
        ymickeys=(signed short)r.x.dx;
        r.x.ax=0x03;
        __dpmi_int(0x33,&r);
        buttons=r.x.bx;

        // post key event for buttons
        if (buttons!=lastbuttons)
        {
            int j=1,k;
            k=(buttons ^ lastbuttons); // only changed bit to 1
            lastbuttons=buttons;

            for(i=0;i<MOUSEBUTTONS;i++,j<<=1)
                if(k & j)
                {
                    if(buttons & j)
                       event.type=ev_keydown;
                    else
                       event.type=ev_keyup;
                    event.data1=KEY_MOUSE1+i;
                    D_PostEvent(&event);
                }
        }

        if ((xmickeys!=0)||(ymickeys!=0))
        {
          event.type=ev_mouse;
          event.data1=0;
//          event.data1=buttons;    // not needed
          event.data2=xmickeys;
          event.data3=-ymickeys;

          D_PostEvent(&event);
        }

    }
    //joystick
    if (joystick_detected)
    {
        static int lastjoybuttons=0;
        int joybuttons;

        poll_joystick();
        // I assume that true is 1
        joybuttons=joy_b1+(joy_b2<<1)+(joy_b3<<2)+(joy_b4<<3)
                         +(joy_b5<<4)+(joy_b6<<5)+(joy_b7<<6)+(joy_b8<<7);

        switch(joy_hat) {
          case JOY_HAT_UP   : joybuttons|=1<<10;break;
          case JOY_HAT_DOWN : joybuttons|=1<<11;break;
          case JOY_HAT_LEFT : joybuttons|=1<<12;break;
          case JOY_HAT_RIGHT: joybuttons|=1<<13;break;
        }

        // post key event for buttons
        if(joybuttons!=lastjoybuttons)
        {
            int j=1,k;
            k=(joybuttons ^ lastjoybuttons); // only changed bit to 1
            lastjoybuttons=joybuttons;

            for(i=0;i<JOYBUTTONS;i++,j<<=1)
                if(k & j)          // test the eatch bit and post the corresponding event
                {
                    if(joybuttons & j)
                       event.type=ev_keydown;
                    else
                       event.type=ev_keyup;
                    event.data1=KEY_JOY1+i;
                    D_PostEvent(&event);
                }
        }

        event.type=ev_joystick;
        event.data1=0;
        event.data2=0;
        event.data3=0;

        if(joy_left)
           event.data2=-1;
        if(joy_right)
           event.data2=1;
        if(joy_up)
           event.data3=-1;
        if(joy_down)
           event.data3=1;

        D_PostEvent(&event);
    }
}

//
//  Timer user routine called at ticrate.
//
void I_TimerISR (void)
{
   //  IO_PlayerInput();      // old doom did that
   ticcount++;

}
END_OF_FUNCTION(I_TimerISR);


//added:08-01-98: we don't use allegro_exit() so we have to do it ourselves.
void I_ShutdownTimer (void)
{
    if( !timer_started )
        return;
    remove_timer();
}


//
//  Installs the timer interrupt handler with timer speed as TICRATE.
//
void I_StartupTimer(void)
{
   ticcount = 0;

   //lock this from being swapped to disk! BEFORE INSTALLING
   LOCK_VARIABLE(ticcount);
   LOCK_FUNCTION(I_TimerISR);

   if( install_timer() != 0 )
      I_Error("I_StartupTimer: could not install timer.");

   if( install_int_ex( I_TimerISR, BPS_TO_TIMER(TICRATE) ) != 0 )
      //should never happen since we use only one.
      I_Error("I_StartupTimer: no room for callback routine.");

   //added:08-01-98: remove the timer explicitly because we don't use
   //                Allegro 's allegro_exit() shutdown code.
   I_AddExitFunc(I_ShutdownTimer);
   timer_started = true;
}


//added:07-02-98:
//
//
byte ASCIINames[128] =
{
//  0       1       2       3       4       5       6       7
//  8       9       A       B       C       D       E       F
    0  ,    27,     '1',    '2',    '3',    '4',    '5',    '6',
    '7',    '8',    '9',    '0', KEY_MINUS,KEY_EQUALS,KEY_BACKSPACE, KEY_TAB,
    'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',
    'o',    'p',    '[',    ']', KEY_ENTER,KEY_CTRL,'a',    's',
    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',
    '\'',   '`', KEY_SHIFT, '\\',   'z',    'x',    'c',    'v',
    'b',    'n',    'm',    ',',    '.',    '/', KEY_SHIFT, '*',
 KEY_ALT,KEY_SPACE,KEY_CAPSLOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
    KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,KEY_NUMLOCK,KEY_SCROLLLOCK,KEY_KEYPAD7,
 KEY_KEYPAD8,KEY_KEYPAD9,KEY_MINUSPAD,KEY_KEYPAD4,KEY_KEYPAD5,KEY_KEYPAD6,KEY_PLUSPAD,KEY_KEYPAD1,
 KEY_KEYPAD2,KEY_KEYPAD3,KEY_KEYPAD0,KEY_KPADDEL,      0,      0,      0,      KEY_F11,
    KEY_F12,0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0
};

volatile int pausepressed=0;
volatile char nextkeyextended;

static void I_KeyboardHandler()
{
    unsigned char ch;
    event_t       event;

    ch=inportb(0x60);

    if(pausepressed>0)
        pausepressed--;
    else
        if(ch==0xE1) // pause key
        {
          event.type=ev_keydown;
          event.data1=KEY_PAUSE;
          D_PostEvent(&event);
          pausepressed=5;
        }
        else
        if(ch==0xE0) // extended key handled at next call
        {
          nextkeyextended=1;
        }
        else
        {
          if((ch&0x80)==0)
            event.type=ev_keydown;
          else
            event.type=ev_keyup;

          ch&=0x7f;

          if(nextkeyextended)
          {
            nextkeyextended=0;

            if(ch==70)  // crtl-break
            {
                asm ("movb $0x79, %%al
                     call ___djgpp_hw_exception"
                     : : :"%eax","%ebx","%ecx","%edx","%esi","%edi","memory");
            }

            // remap lonely keypad slash
            if (ch==53)
                event.data1 = KEY_KPADSLASH;
            else
            // remap the bill gates keys...
            if (ch>=91 && ch<=93)
                event.data1 = ch + 0x80;    // leftwin, rightwin, menu
            else
            // remap non-keypad extended keys to a value<128, but
            // make them different than the KEYPAD keys.
            if (ch>=71 && ch<=83)
                event.data1 = 0x80 + ch + 30;
            else if (ch==28)
                event.data1 = KEY_ENTER;    // keypad enter -> return key
            else if (ch==29)
                event.data1 = KEY_CTRL;     // rctrl -> lctrl
            else if (ch==56)
                event.data1 = KEY_ALT;      // ralt -> lalt
            else
                ch = 0;
            if (ch)
                D_PostEvent(&event);
          }
          else
          {
            if (ASCIINames[ch]!=0)
              event.data1=ASCIINames[ch];
            else
              event.data1=ch+0x80;
            D_PostEvent(&event);
          }
        }

    outportb(0x20,0x20);
}
END_OF_FUNCTION(I_KeyboardHandler);

//  Return a key that has been pushed, or 0
//  (replace getchar() at game startup)
//
int I_GetKey (void)
{
    if( keyboard_started )
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

    // keyboard not started use the bios call trouth djgpp
    if(_conio_kbhit())
    {
        int key=getch();
        if(key==0) key=getch()+256;
        return key;
    }
    else
        return 0;

}

/* Keyboard handler stuff */
_go32_dpmi_seginfo oldkeyinfo,newkeyinfo;

//
//  Removes the keyboard handler.
//
void I_ShutdownKeyboard()
{
    if( !keyboard_started )
        return;

    asm("cli");
    _go32_dpmi_set_protected_mode_interrupt_vector(9, &oldkeyinfo);
    _go32_dpmi_free_iret_wrapper(&newkeyinfo);
    asm("sti");

    keyboard_started=false;
}

//
//  Installs the keyboard handler.
//
void I_StartupKeyboard()
{
    if(keyboard_started)
        return;

    nextkeyextended=0;

    asm("cli");
    _go32_dpmi_get_protected_mode_interrupt_vector(9, &oldkeyinfo);
    newkeyinfo.pm_offset=(int)I_KeyboardHandler;
    newkeyinfo.pm_selector=_go32_my_cs();
    _go32_dpmi_allocate_iret_wrapper(&newkeyinfo);
    _go32_dpmi_set_protected_mode_interrupt_vector(9, &newkeyinfo);

    LOCK_VARIABLE(nextkeyextended);
    LOCK_VARIABLE(pausepressed);
    _go32_dpmi_lock_data(ASCIINames,sizeof(ASCIINames));
    LOCK_FUNCTION(I_KeyboardHandler);

    _go32_dpmi_lock_data(events,sizeof(events));
    LOCK_VARIABLE(eventhead);
    LOCK_FUNCTION(D_PostEvent);

    asm("sti");

    //added:08-01-98:register shutdown keyboard code.
    I_AddExitFunc(I_ShutdownKeyboard);
    keyboard_started = true;
}




//added:08-01-98:
//
//  Clean Startup & Shutdown handling, as does Allegro.
//  We need this services for ourselves too, and we don't want to mix
//  with Allegro, because someone might not use Allegro.
//  (all 'exit' was renamed to 'quit')
//
#define MAX_QUIT_FUNCS     16
typedef void (*quitfuncptr)();
static quitfuncptr quit_funcs[MAX_QUIT_FUNCS] =
               { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
               };


//added:08-01-98:
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


//added:08-01-98:
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



 //added:03-01-98:
//
// signal_handler:
//  Used to trap various signals, to make sure things get shut down cleanly.
//
static void exception_handler(int num)
{
static char msg[255];
    sprintf(msg,"Doom LEGACY v%i.%i"VERSIONSTRING"\r\n"
                "This is a error of Legacy try to send the following info to programmers\r\n",VERSION/100,VERSION%100);

    //D_QuitNetGame ();  //say 'byebye' to other players when your machine
                        // crashes?... hmm... do they have to die with you???

    I_ShutdownSystem();

    _write(STDERR_FILENO, msg, strlen(msg));

    signal(num, SIG_DFL);
    raise(num);
    // TODO: write it in a log !!
}

static void break_handler(int num)
{
static char msg[] = "Oh no! Back to reality!\r\n";

    //D_QuitNetGame ();  //say 'byebye' to other players when your machine
                        // crashes?... hmm... do they have to die with you???

    I_ShutdownSystem();

    _write(STDERR_FILENO, msg, sizeof(msg)-1);

    signal(num, SIG_DFL);
    raise(num);
}


//added:08-01-98: now this replaces allegro_init()
//
//  REMEMBER: THIS ROUTINE MUST BE STARTED IN i_main.c BEFORE D_DoomMain()
//
//  This stuff should get rid of the exception and page faults when
//  Doom bugs out with an error. Now it should exit cleanly.
//
int  I_StartupSystem(void)
{
    I_DetectWin95 ();
    i_love_bill = win95;

   // some 'more globals than globals' things to initialize here ?
   graphics_started = false;
   keyboard_started = false;
   sound_started = false;
   timer_started = false;
   cdaudio_started = false;

   // check for OS type and version here ?


   signal(SIGABRT, exception_handler);
   signal(SIGFPE , exception_handler);
   signal(SIGILL , exception_handler);
   signal(SIGSEGV, exception_handler);
   signal(SIGINT , break_handler);
   signal(SIGKILL, break_handler);
   signal(SIGQUIT, break_handler);

   return 0;
}


//added:08-01-98:
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

void I_GetDiskFreeSpace(INT64 *freespace)
{
    struct diskfree_t df;
    if(_dos_getdiskfree(0,&df))
        *freespace = (unsigned long)df.avail_clusters *
                     (unsigned long)df.bytes_per_sector *
                     (unsigned long)df.sectors_per_cluster;
    else
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