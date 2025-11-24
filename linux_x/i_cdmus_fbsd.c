// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_cdmus_fbsd.c,v 1.1 2001/05/16 22:33:35 bock Exp $
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
// $Log: i_cdmus_fbsd.c,v $
// Revision 1.1  2001/05/16 22:33:35  bock
// Initial FreeBSD support.
//
//
// DESCRIPTION:
//      cd music interface for FreeBSD
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

#include <sys/cdio.h>
#include "doomtype.h"
#include "i_sound.h"
#include "command.h"
#include "m_argv.h"

#define MAX_CD_TRACKS 256

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
consvar_t cv_jigglecdvol = {"jigglecdvolume", "0", CV_SAVE};

static int cdfile = -1;
static char cd_dev[64] = "/dev/cdrom";


static int CDAudio_GetAudioDiskInfo(void)
{
	struct ioc_toc_header tochdr;

	cdValid = false;

	if ( ioctl(cdfile, CDIOREADTOCHEADER, &tochdr) == -1 ) {
            CONS_Printf("ioctl cdromreadtochdr failed\n");
	    ioctl( cdfile, CDIOCALLOW);
            return -1;
        }
	ioctl( cdfile, CDIOCALLOW);
        
	if (tochdr.starting_track < 1) {
            CONS_Printf("CDAudio_GetAudioDiskInfo: no music tracks\n");
            return -1;
	}

	cdValid = true;
	maxTrack = tochdr.ending_track;

	return 0;
}

static void I_EjectCD(void)
{
	if (cdfile == -1 || !enabled)
            return; // no cd init'd

        I_StopCD();

	if ( ioctl(cdfile, CDIOCEJECT) == -1 ) 
            CONS_Printf("ioctl cdromeject failed\n");
	ioctl(cdfile, CDIOCALLOW);
}

static void Command_Cd_f (void)
{
	char	*command;
	int		ret;
	int		n;

	if (!initialized)
	   return;

	if (COM_Argc() < 2) {
            CONS_Printf ("cd [on] [off] [remap] [reset] [open]\n"
                         "   [info] [play <track>] [resume]\n"
                         "   [stop] [pause] [loop <track>]\n");
            return;
        }

	command = COM_Argv (1);

	if (!strncmp(command, "on", 2)) {
            enabled = true;
            return;
	}

	if (!strncmp(command, "off", 3)) {
            I_StopCD();
            enabled = false;
            return;
	}
	
	if (!strncmp(command, "remap", 5)) {
            ret = COM_Argc() - 2;
            if (ret <= 0) {
                for (n = 1; n < MAX_CD_TRACKS; n++)
                    if (cdRemap[n] != n)
                        CONS_Printf("  %u -> %u\n", n, cdRemap[n]);
                return;
            }
            for (n = 1; n <= ret; n++)
                cdRemap[n] = atoi(COM_Argv (n+1));
            return;
	}
        
	if (!strncmp(command, "reset", 5)) {
            enabled = true;
            I_StopCD();
            
            for (n = 0; n < MAX_CD_TRACKS; n++)
                cdRemap[n] = n;
            CDAudio_GetAudioDiskInfo();
            return;
	}
        
	if (!cdValid) {
            CDAudio_GetAudioDiskInfo();
            if (!cdValid) {
                CONS_Printf("No CD in player.\n");
                return;
            }
	}

	if (!strncmp(command, "open", 4)) {
            I_EjectCD();
            cdValid = false;
            return;
	}

	if (!strncmp(command, "info", 4)) {
            CONS_Printf("%u tracks\n", maxTrack);
            if (playing)
                CONS_Printf("Currently %s track %u\n", playLooping ? "looping" : "playing", playTrack);
            else if (wasPlaying)
                CONS_Printf("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
            CONS_Printf("Volume is %d\n", cdvolume);
            return;
	}

	if (!strncmp(command, "play", 4)) {
            I_PlayCD((byte)atoi(COM_Argv (2)), false);
            return;
	}

	if (!strncmp(command, "loop", 4)) {
            I_PlayCD((byte)atoi(COM_Argv (2)), true);
            return;
	}

	if (!strncmp(command, "stop", 4)) {
            I_StopCD();
            return;
	}
        
	if (!strncmp(command, "pause", 5)) {
            I_PauseCD();
            return;
	}
        
	if (!strncmp(command, "resume", 6)) {
            I_ResumeCD();
            return;
	}
        
	CONS_Printf("Invalid command \"cd %s\"\n", COM_Argv (1));
}

void I_StopCD(void)
{
	if (cdfile == -1 || !enabled)
		return;
	
	if (!(playing || wasPlaying))
		return;

	if ( ioctl(cdfile, CDIOCSTOP, 0) == -1 )
		CONS_Printf("ioctl cdromstop failed (%d)\n", errno);
	ioctl(cdfile, CDIOCALLOW);

	wasPlaying = false;
	playing = false;
}

void I_PauseCD (void)
{
	if (cdfile == -1 || !enabled)
		return;
	
	if (!playing)
		return;

	if ( ioctl(cdfile, CDIOCPAUSE) == -1 )
		CONS_Printf("ioctl cdrompause failed (%d)\n", errno);
	ioctl(cdfile, CDIOCALLOW);

	wasPlaying = playing;
	playing = false;
}

// continue after a pause
void I_ResumeCD (void)
{
	if (cdfile == -1 || !enabled)
		return;
	
	if (!cdValid)
		return;

	if (!wasPlaying)
		return;
	
	if ( ioctl(cdfile, CDIOCRESUME) == -1 ) 
		CONS_Printf("ioctl cdromresume failed\n");

	playing = true;
        wasPlaying = false;

	if(cv_jigglecdvol.value)
	{
	    I_SetVolumeCD(31-cd_volume.value);
	    I_SetVolumeCD(cd_volume.value);
	}

	return;
}


void I_ShutdownCD (void)
{
	if (!initialized)
		return;
	I_StopCD();
	close(cdfile);
	cdfile = -1;

	initialized = false;
	enabled = false;
}

void I_InitCD (void)
{
	int i;

	// Don't start music on a dedicated server
	if (M_CheckParm("-dedicated"))
            return ;
	
	// Has been checked in d_main.c, but doesn't hurt here
	if (M_CheckParm ("-nocd"))
            return ;
	
	// New commandline switch -cddev 
	if ((i = M_CheckParm("-cddev")) != 0 && M_IsNextParm()) {
            strncpy(cd_dev, M_GetNextParm() , sizeof(cd_dev));
            cd_dev[sizeof(cd_dev) - 1] = 0;
	}

	if ((cdfile = open(cd_dev, O_RDONLY)) == -1) {
            int myerrno = errno;
            CONS_Printf("I_InitCD: open of \"%s\" failed (%i)\n", cd_dev, myerrno);
            if(EACCES == myerrno) // permission denied -> very common problem with IDE drives
                CONS_Printf("-------------------------------------\n"
                            "Permission denied to open device %s\n"
                            "Set read permission or run as root\n"
                            "if in doubt *READ THE DOCS*\n"
                            "-------------------------------------\n", cd_dev); // Shall we add a line about this in the README?
            cdfile = -1;
            return ;
	}
	for (i = 0; i < MAX_CD_TRACKS; i++)
            cdRemap[i] = i;
        
	initialized = true;
	enabled = true;
        
	if (CDAudio_GetAudioDiskInfo()) {
            CONS_Printf("I_InitCD: No CD in player.\n");
            cdValid = false;
	}

	if(cv_jigglecdvol.value)
	{
	    I_SetVolumeCD(31-cd_volume.value);
	    I_SetVolumeCD(cd_volume.value);
	}
	
	COM_AddCommand ("cd", Command_Cd_f);

	CONS_Printf("CD Audio Initialized\n");

	return ;
}



// loop/go to next track when track is finished (if cd_update var is true)
// update the volume when it has changed (from console/menu) 
// FIXME: Why do we have Setvolume then ???
// TODO: check for cd change and restart music ?
//
void I_UpdateCD (void)
{
        struct ioc_read_subchannel subchnl;
	struct cd_sub_channel_info data;
	static time_t lastchk;
	boolean was_valid;
	
	if (!enabled)
		return;

	I_SetVolumeCD(cd_volume.value);

	// FIXME: Do we have a "hicup" here every 2 secs?
	if (playing && lastchk < time(NULL)) {
            lastchk = time(NULL) + 2; //two seconds between chks
	    subchnl.address_format = CD_MSF_FORMAT;
            subchnl.data_format = CD_CURRENT_POSITION;
            subchnl.track = 0;
            subchnl.data_len = sizeof(data);
            subchnl.data = &data;
            if (ioctl(cdfile, CDIOCREADSUBCHANNEL, &subchnl) == -1 ) {
                CONS_Printf("ioctl cdromsubchnl failed\n");
		ioctl(cdfile, CDIOCALLOW);
                playing = false;
                return;
            }
            if (data.header.audio_status != CD_AS_PLAY_IN_PROGRESS &&
                data.header.audio_status != CD_AS_PLAY_PAUSED) {
                playing = false;
                if (playLooping)
                    I_PlayCD(playTrack, true);
            }
	}
}


// play the cd
void I_PlayCD (int track, boolean looping)
{
	struct ioc_read_toc_single_entry 	entry;
	struct ioc_play_track                   ti;

	if (cdfile == -1 || !enabled)
		return;
	
	if (!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if (!cdValid)
			return;
	}

	track = cdRemap[track];

	if (track < 1 || track > maxTrack)
	{
		CONS_Printf("I_PlayCD: Bad track number %u.\n", track);
		return;
	}

	// don't try to play a non-audio track
	entry.track = track;
	entry.address_format = CD_MSF_FORMAT;
	if ( ioctl(cdfile, CDIOREADTOCENTRY, &entry) == -1 )
	   {
	      CONS_Printf("ioctl cdromreadtocentry failed\n");
	      return;
	   }
	ioctl( cdfile, CDIOCALLOW);
	if (entry.entry.control == 0x04)
	{
		CONS_Printf("I_PlayCD: track %i is not audio\n", track);
		return;
	}

	if(cv_jigglecdvol.value)
	{
	    I_SetVolumeCD(31-cd_volume.value);
	    I_SetVolumeCD(cd_volume.value);
	}
	
	if (playing)
	{
		if (playTrack == track)
			return;
		I_StopCD();
	}

	ti.start_track = track;
	ti.end_track = track;
	ti.start_index = 1;
	ti.end_index = 99;

	if (ioctl(cdfile, CDIOCPLAYTRACKS, &ti) == -1)
	{
		CONS_Printf("ioctl cdiocplaytracks failed\n");
	}
	ioctl(cdfile, CDIOCALLOW);

	if(cv_jigglecdvol.value)
	{
	    I_SetVolumeCD(31-cd_volume.value);
	    I_SetVolumeCD(cd_volume.value);
	}
	
	playLooping = looping;
	playTrack = track;
	playing = true;
}


// volume : logical cd audio volume 0-31 (hardware is 0-255)
int I_SetVolumeCD (int volume)
{
        struct ioc_vol volctrl;

	if(volume < 0 || volume > 31)
	   CONS_Printf("cdvolume should be between 0-31\n");

	// volume control for CD music
	if (volume != cdvolume){
	      volctrl.vol[0] = volume/31.0 * 255.0;
	      volctrl.vol[1] = volctrl.vol[0];
	      volctrl.vol[2] = 0;
	      volctrl.vol[3] = 0;
	      
	      if(ioctl(cdfile, CDIOCSETVOL, &volctrl) == -1){
		 CONS_Printf("ioctl cdromvolctrl failed\n");
	      }
	      
	      cdvolume = volume;
	}
		    
    return 0;
}
