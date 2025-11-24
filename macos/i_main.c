// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_main.c,v 1.1 2001/04/17 22:23:38 calumr Exp $
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
// $Log: i_main.c,v $
// Revision 1.1  2001/04/17 22:23:38  calumr
// Initial add
//
// Revision 1.1  2000/08/21 21:17:32  metzgermeister
// Initial import to CVS
//
// Revision 1.3  2000/04/23 16:19:52  bpereira
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
//      Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

#include <Carbon/Carbon.h>
#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"

OSErr RequiredCheck (const AppleEvent *theAppleEvent)
{
	OSErr  myErr;
	DescType  typeCode;
	Size  actualSize;

	myErr = AEGetAttributePtr (theAppleEvent, keyMissedKeywordAttr, typeWildCard, &typeCode, 0L, 0, &actualSize);
	if (myErr == errAEDescNotFound) return (noErr);
	if (myErr == noErr) return (errAEEventNotHandled);
		
	return (myErr);
}

pascal OSErr HandleODOC (const AppleEvent *theAppleEvent, AppleEvent *reply, long myRefCon)
{
	#pragma unused (reply, myRefCon)
	long  itemsInList;
	int  i;
	OSErr  err;
	AEDescList  docList;

	err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList, &docList);
	if (err) return (err);
	
	err = RequiredCheck (theAppleEvent);
	if (err) return (err);
	
	err = AECountItems (&docList, &itemsInList);
	if (err) return (err);

	for (i = 1; i <= itemsInList; i++)
	{
		FSSpec  fileSpec;
		AEKeyword  theKeyword;
		DescType  typeCode;
		Size  actualSize;

		// Get the fileSpec
		err = AEGetNthPtr (&docList, i, typeFSS, &theKeyword, &typeCode, (Ptr) &fileSpec, sizeof (FSSpec), &actualSize);
		if (err) return (err);

		// Convert it to a full pathname and add
		{
			FSRef new_file;
			UInt32 len = 256;
			char path[256], command[256];
				
			err = FSpMakeFSRef(&fileSpec, &new_file);
			err = FSRefMakePath(&new_file, path, len);
			
			sprintf(command, "addfile \"%s\"\n", path);
			COM_BufAddText(command);
		}
	}
	
	return (noErr);
}

OSErr HandleQUIT (const AppleEvent *theAppleEvent, AppleEvent *reply, long myRefCon)
{
	OSErr  myErr;

	myErr = RequiredCheck (theAppleEvent);
	if (myErr) return (myErr);
	
	I_Quit();
	
	return (noErr);
}

/**************************************************************************
 * HandleOAPP
 *
 * Open a file through the OAPP apple event. This stops the bouncing
 * icon in the Dock. 
 *************************************************************************/

OSErr HandleOAPP (const AppleEvent *theAppleEvent, AppleEvent *reply, long myRefCon)
{
	OSErr  myErr;

	myErr = RequiredCheck (theAppleEvent);
	if (myErr) return (myErr);

	return (noErr);	
}

void InitAppleEvents (void)
{
	AEInstallEventHandler (kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP (HandleQUIT), 0, false);
	AEInstallEventHandler (kCoreEventClass, kAEOpenApplication, NewAEEventHandlerUPP (HandleOAPP), 0, false);
	AEInstallEventHandler (kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerUPP (HandleODOC), 0, false);
}	

static void GrabFirstEvent (void)
{
	EventRecord theEvent;
	
	do
		WaitNextEvent (everyEvent, &theEvent, 0, NULL);
	while (theEvent.what != kHighLevelEvent);
	
	switch (theEvent.what)
	{
		case kHighLevelEvent:
			AEProcessAppleEvent (&theEvent);
			break;
	}
}

int main (int argc, char** argv) 
{
	myargv = argv;
    myargc = argc;
	
	InitAppleEvents();
	
    D_DoomMain ();
	GrabFirstEvent();
	while (1)
		RunApplicationEventLoop();
	//while (1)
	//{
	//	D_DoomLoop ();
	//}
    return 0;
}