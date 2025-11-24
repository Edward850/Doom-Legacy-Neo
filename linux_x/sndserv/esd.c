// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: esd.c,v 1.1 2002/12/17 19:56:51 bock Exp $
//
// Copyright (C) 2002, DooM Legacy Team
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
// $Log: esd.c,v $
// Revision 1.1  2002/12/17 19:56:51  bock
// Add esound support without SDL.
// Sorry about this, this code is disabled by default.
// You should build with HAVE_ESD=1 flag.
//
//
//
// DESCRIPTION:
//      UNIX, replacement for soundserver for Linux with working esd.
//
//-----------------------------------------------------------------------------

#include "esd.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "soundsrv.h"

#include <sys/types.h>
#include <sys/socket.h>

int     audio_fd;
int     audio_8bit_flag = 0;

void I_InitMusic(void)
{
}

void
I_InitSound
( int   samplerate,
  int   samplesize )
{
    int sockopt = 32;
    int mode = ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_PLAY;
    audio_fd = esd_play_stream_fallback( mode, samplerate, NULL, NULL );
    if (audio_fd<=0)
    {
        audio_fd = 0;
        fprintf(stderr, "Could not open esound\n");
        return;
    }
    if (setsockopt(audio_fd, SOL_SOCKET, SO_SNDBUF, &sockopt, sizeof(sockopt)) != 0)
    {
        fprintf(stderr, "Couldn't do sockopt: ");
        perror(NULL);
    }
}

void
I_SubmitOutputBuffer
( void* samples,
  int   samplecount )
{
    if (audio_fd >= 0)
        write(audio_fd, samples, samplecount*4);
}

void I_ShutdownSound(void)
{
    if (audio_fd >= 0)
        close(audio_fd);
}

void I_ShutdownMusic(void)
{
}
