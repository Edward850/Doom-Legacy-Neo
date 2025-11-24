// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_input_mac.c,v 1.1 2001/04/17 22:23:38 calumr Exp $
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
// $Log: i_input_mac.c,v $
// Revision 1.1  2001/04/17 22:23:38  calumr
// Initial add
//
// Revision 1.1  2000/08/21 21:17:32  metzgermeister
// Initial import to CVS
//
//
//
// DESCRIPTION:
//      DOOM input stuff for Mac
//
//-----------------------------------------------------------------------------

#include <Carbon/Carbon.h>

#include "doomdef.h"
#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "m_menu.h"
#include "d_main.h"
#include "s_sound.h"
#include "g_input.h"
#include "st_stuff.h"
#include "g_game.h"
#include "i_video.h"
#include "console.h"
#include "command.h"

void I_StartFrame(void)
{
    return;
}

unsigned char TranslateKey(EventRef event)
{
	unsigned char keyChar;
	UInt32 keyCode;
	
	GetEventParameter(event, kEventParamKeyMacCharCodes, typeChar,
		NULL, sizeof(typeChar), NULL, &keyChar);
	GetEventParameter(event, kEventParamKeyCode, typeUInt32,
		NULL, sizeof(typeUInt32), NULL, &keyCode);
	
	switch (keyCode)
	{
		case 36: keyChar = KEY_ENTER; break;	//return
		case 76: keyChar = KEY_ENTER; break;	//enter
		
		case 122: keyChar = KEY_F1; break;
		case 120: keyChar = KEY_F2; break;
		case 99: keyChar = KEY_F3; break;
		case 118: keyChar = KEY_F4; break;
		case 96: keyChar = KEY_F5; break;
		case 97: keyChar = KEY_F6; break;
		case 98: keyChar = KEY_F7; break;
		case 100: keyChar = KEY_F8; break;
		case 101: keyChar = KEY_F9; break;
		case 109: keyChar = KEY_F10; break;
		case 103: keyChar = KEY_F11; break;
		case 111: keyChar = KEY_F12; break;
		
		case 123: keyChar = KEY_LEFTARROW; break;
		case 124: keyChar = KEY_RIGHTARROW; break;
		case 125: keyChar = KEY_DOWNARROW; break;
		case 126: keyChar = KEY_UPARROW; break;
	}
	
	//CONS_Printf("keyCode = %i keyChar = %c\n", keyCode, keyChar);
	
	return keyChar;
}

byte keyboard_started = 0;
/*---------------------------------------------------
 Carbon does not produce seperate key up events
 for scoll wheel moving up/down. Need to make one
 happen next time round. 
---------------------------------------------------*/
event_t scrollwheel_buttonup_hack;
int post_the_scroll_hack = false;

static pascal OSStatus AppEventHandlerFunc(EventHandlerCallRef handlerChain, EventRef event, void *userData)
{
	event_t doom_event;
	UInt32 event_kind;
	UInt32 event_class;
	Point mouseLoc;
	EventMouseButton which_mouse_button;
	SInt32 scroll_wheel_delta;
	OSStatus err = eventNotHandledErr;
	
	event_kind = GetEventKind(event);
	event_class = GetEventClass(event);
	
	if (event_class == kEventClassKeyboard)
	{
		switch (event_kind)
		{
			case kEventRawKeyDown:
			case kEventRawKeyUp:
				doom_event.data1 = TranslateKey(event);
				doom_event.type = (event_kind == kEventRawKeyDown) ? ev_keydown : ev_keyup;
				D_PostEvent (&doom_event);
				err = noErr;
				break;
		}
	}
	else if (event_class == kEventClassApplication)
	{
		switch (event_kind)
		{
			case kEventAppActivated:
				VID_Pause(false);
				err = noErr;
				break;
			case kEventAppDeactivated:
				VID_Pause(true);
				err = noErr;
				break;
		}
	}
	else if (event_class == kEventClassCommand)
	{
		if (event_kind == kEventCommandProcess)
		{
			HICommand command;
			
			GetEventParameter(event, kEventParamDirectObject, typeHICommand,
									NULL, sizeof(HICommand), NULL, &command);
			err = noErr;
		}
	}
	else if (event_class == kEventClassMouse)
	{
		switch (event_kind)
		{
			case kEventMouseMoved:
			case kEventMouseDragged:
				GetEventParameter(event, kEventParamMouseDelta, typeQDPoint,
									NULL, sizeof(Point), NULL, &mouseLoc);
				doom_event.type = ev_mouse;
				doom_event.data1 = 0;
				doom_event.data2 = mouseLoc.h * 10.0;
				doom_event.data3 = -mouseLoc.v * 10.0;
				D_PostEvent (&doom_event);
				err = noErr;
				break;
			case kEventMouseDown:
			case kEventMouseUp:
				GetEventParameter(event, kEventParamMouseLocation, typeQDPoint,
									NULL, sizeof(Point), NULL, &mouseLoc);
				
				GetEventParameter(event, kEventParamMouseButton, typeMouseButton,
									NULL, sizeof(EventMouseButton), NULL, &which_mouse_button);
				
				doom_event.type = event_kind == kEventMouseDown ? ev_keydown : ev_keyup;
				doom_event.data1 = KEY_MOUSE1 - 1 + which_mouse_button;
				D_PostEvent (&doom_event);
				err = noErr;
				break;
			case kEventMouseWheelMoved:
				GetEventParameter(event, kEventParamMouseWheelDelta, typeLongInteger,
									NULL, sizeof(SInt32), NULL, &scroll_wheel_delta);
				doom_event.data1 = scroll_wheel_delta > 0 ? KEY_MOUSEWHEELUP : KEY_MOUSEWHEELDOWN;
				doom_event.type = ev_keydown;
				D_PostEvent(&doom_event);
				
				scrollwheel_buttonup_hack.data1 = scroll_wheel_delta > 0 ? KEY_MOUSEWHEELUP : KEY_MOUSEWHEELDOWN;
				scrollwheel_buttonup_hack.type = ev_keyup;
				post_the_scroll_hack = true;					//make sure keyup event happens
				
				err = noErr;
				break;
		}
	}
	
	return err;
}

void EventTimerCallbackFunc(EventLoopTimerRef timer, void *userData)
{
	D_DoomLoop();
	
	if (post_the_scroll_hack)
	{
		D_PostEvent(&scrollwheel_buttonup_hack);
		post_the_scroll_hack = false;
	}
}

EventLoopTimerRef timerRef;

void I_StartupKeyboard (void)
{
	EventTypeSpec appEventList[] = {	{kEventClassMouse, kEventMouseMoved},
										{kEventClassMouse, kEventMouseDragged},
										{kEventClassKeyboard, kEventRawKeyDown},
										{kEventClassKeyboard, kEventRawKeyUp},
										{kEventClassMouse, kEventMouseDown},
										{kEventClassMouse, kEventMouseUp},
										{kEventClassMouse, kEventMouseWheelMoved},
										{kEventClassCommand,kEventCommandProcess},
										{kEventClassApplication,kEventAppActivated},
										{kEventClassApplication,kEventAppDeactivated}};
	
	if (keyboard_started)
		return;
	
    keyboard_started = 1;
	
	InstallStandardEventHandler(GetApplicationEventTarget());
	
	InstallApplicationEventHandler(NewEventHandlerUPP(AppEventHandlerFunc), 10, appEventList, 0, NULL);
	
	InstallEventLoopTimer(GetCurrentEventLoop(), NULL, 1.0 / 70.0, 
								NewEventLoopTimerUPP(EventTimerCallbackFunc), NULL, &timerRef);
	
    CONS_Printf("I_StartupKeyboard done\n");
}

void I_GetEvent(void)
{
	
}

/*-----------------------
macConfigureInput

Called from m_menu.c
Make sure to stop fullscreen (safer)
-----------------------*/
void macConfigureInput(void)
{
}

static boolean ISpConfigured = false;

void I_InitJoystick (void)
{
}

void I_StartupMouse2 (void)
{
}

void I_StartupMouse(void)
{	
	if (ISpConfigured)
		return;
	
	CONS_Printf("I_StartupMouse...\n");
		
	ISpConfigured = true;
	
	CONS_Printf("\tI_StartupMouse done\n");
}

void I_ShutdownInput(void)
{
    if (!ISpConfigured)
		return;
    
    FlushEvents(everyEvent,0);
    ShowCursor();
    
    ISpConfigured = false;
}

void I_StartTic(void)
{
    if (!graphics_started)
        return;
}