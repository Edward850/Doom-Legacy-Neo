// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: soundsrv.h,v 1.2 2000/02/27 00:42:12 hurdler Exp $
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
// $Log: soundsrv.h,v $
// Revision 1.2  2000/02/27 00:42:12  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      UNIX soundserver, separate process. 
//
//-----------------------------------------------------------------------------

#ifndef __SNDSERVER_H__
#define __SNDSERVER_H__

#define SAMPLECOUNT             512
#define MIXBUFFERSIZE   (SAMPLECOUNT*2*2)
#define SPEED                   11025


void I_InitMusic(void);

void
I_InitSound
( int           samplerate,
  int           samplesound );

void
I_SubmitOutputBuffer
( void*         samples,
  int           samplecount );

void I_ShutdownSound(void);
void I_ShutdownMusic(void);

#endif
