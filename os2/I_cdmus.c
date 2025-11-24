// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: I_cdmus.c,v 1.2 2000/08/10 11:07:51 ydario Exp $
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
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
// $Log: I_cdmus.c,v $
// Revision 1.2  2000/08/10 11:07:51  ydario
// fix CRLF
//
// Revision 1.1  2000/08/09 11:42:47  ydario
// OS/2 specific platform code
//
// Revision 1.6  2000/04/28 19:28:00  metzgermeister
// changed to CDROMPLAYMSF for CD music
//
// Revision 1.5  2000/04/07 23:12:38  metzgermeister
// fixed some minor bugs
//
// Revision 1.4  2000/03/28 16:18:42  linuxcub
// Added a command to the Linux sound-server which sets a master volume...
//
// Revision 1.3  2000/03/22 18:53:53  metzgermeister
// Ripped CD code out of Quake and put it here
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      cd music interface
//
//-----------------------------------------------------------------------------


#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "doomtype.h"
#include "i_sound.h"
#include "command.h"
#include "m_argv.h"

#define MAX_CD_TRACKS 256

byte   cdaudio_started=0;   // for system startup/shutdown

static boolean cdValid = false;
static boolean playing = false;
static boolean wasPlaying = false;
static boolean initialized = false;
static boolean enabled = false;
static boolean playLooping = false;
static byte    playTrack;
static byte    maxTrack;
static byte    cdRemap[MAX_CD_TRACKS];
static int     cdvolume = -1;

CV_PossibleValue_t cd_volume_cons_t[]={{0,"MIN"},{31,"MAX"},{0,NULL}};

consvar_t cd_volume = {"cd_volume","31",CV_SAVE, cd_volume_cons_t};
consvar_t cdUpdate  = {"cd_update","1",CV_SAVE};

static int cdfile = -1;
static char cd_dev[64] = "/dev/cdrom";


static int CDAudio_GetAudioDiskInfo(void)
{
	return 0;
}

static boolean CDAudio_GetStartStop(struct cdrom_msf *msf, int track, struct cdrom_tocentry *entry)
{
    return true;
}

static void I_EjectCD(void)
{
}

static void Command_Cd_f (void)
{
}

void I_StopCD(void)
{
}

void I_PauseCD (void)
{
}

// continue after a pause
void I_ResumeCD (void)
{
}


void I_ShutdownCD (void)
{
}

void I_InitCD (void)
{
	return ;
}



// loop/go to next track when track is finished (if cd_update var is true)
// update the volume when it has changed (from console/menu) 
// FIXME: Why do we have Setvolume then ???
// TODO: check for cd change and restart music ?
//
void I_UpdateCD (void)
{
}


// play the cd
void I_PlayCD (int track, boolean looping)
{
}


// volume : logical cd audio volume 0-31 (hardware is 0-255)
int I_SetVolumeCD (int volume)
{
    return 0;
}
