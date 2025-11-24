// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: soundsrv.c,v 1.8 2001/05/29 22:18:41 bock Exp $
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
// $Log: soundsrv.c,v $
// Revision 1.8  2001/05/29 22:18:41  bock
// Small BSD commit: build r_opengl.so, sndserver
//
// Revision 1.7  2000/09/01 19:34:37  bpereira
// no message
//
// Revision 1.6  2000/04/30 19:50:37  metzgermeister
// no message
//
// Revision 1.5  2000/04/28 19:26:10  metzgermeister
// musserver fixed, sndserver amplified accordingly
//
// Revision 1.4  2000/04/22 20:30:00  metzgermeister
// fix amplification by 4
//
// Revision 1.3  2000/03/28 16:18:42  linuxcub
// Added a command to the Linux sound-server which sets a master volume.
//
// Revision 1.2  2000/02/27 00:42:12  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      UNIX soundserver, run as a separate process,
//       started by DOOM program.
//      Originally conceived fopr SGI Irix,
//       mostly used with Linux voxware.
//
//-----------------------------------------------------------------------------


#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#ifndef FREEBSD
#include <malloc.h>
#endif
#include <sys/stat.h>
#include <sys/time.h>

#include "sounds.h"
#include "soundsrv.h"



extern int audio_8bit_flag;


//
// Department of Redundancy Department.
//
typedef struct wadinfo_struct
{
    // should be IWAD
    char        identification[4];      
    int         numlumps;
    int         infotableofs;
    
} wadinfo_t;


typedef struct filelump_struct
{
    int         filepos;
    int         size;
    char        name[8];
    
} filelump_t;


// an internal time keeper
static int      mytime = 0;

// number of sound effects
int             numsounds;

// longest sound effect
int             longsound;

// lengths of all sound effects
int             lengths[NUMSFX];

// mixing buffer
signed short    mixbuffer[MIXBUFFERSIZE];

// file descriptor of sfx device
int             sfxdevice;                      

// file descriptor of music device
int             musdevice;                      

// the channel data pointers
unsigned char*  channels[8];

// the channel step amount
unsigned int    channelstep[8];

// 0.16 bit remainder of last step
unsigned int    channelstepremainder[8];

// the channel data end pointers
unsigned char*  channelsend[8];

// time that the channel started playing
int             channelstart[8];

// the channel handles
int             channelhandles[8];

// the channel left volume lookup
// int*            channelleftvol_lookup[8];

// the channel right volume lookup
// int*            channelrightvol_lookup[8];

// sfx id of the playing sound effect
int             channelids[8];                  

int             snd_verbose=1;

int             steptable[256];

// int             vol_lookup[128*256];
int             volume_lookup[128][256];

int             master_volume=31; /* 0..31 */

int             left_volume[8],right_volume[8];

static void derror(char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
    exit(-1);
}

int mix(void)
{

    register unsigned int       sample;
    register int                dl;
    register int                dr;
    unsigned short              sdl;
    unsigned short              sdr;
    
    signed short*               leftout;
    signed short*               rightout;
    signed short*               leftend;
    unsigned char*              bothout;
    
    int                         step;
    int                         i;
    int                         leftv[8],rightv[8];
    
    for( i=0; i<8; ++i ) {
        leftv[i] = left_volume[i]*master_volume/31;
        rightv[i] = right_volume[i]*master_volume/31;
    }

    leftout = mixbuffer;
    rightout = mixbuffer+1;
    bothout = (unsigned char *)mixbuffer;
    step = 2;

    leftend = mixbuffer + SAMPLECOUNT*step;

    // mix into the mixing buffer
    while (leftout != leftend)
    {

        dl = 0;
        dr = 0;

        if (channels[0])
        {
            sample = *channels[0];
            // dl += channelleftvol_lookup[0][sample];
            dl += volume_lookup[leftv[0]][sample];
            // dr += channelrightvol_lookup[0][sample];
            dr += volume_lookup[rightv[0]][sample];
            channelstepremainder[0] += channelstep[0];
            channels[0] += channelstepremainder[0] >> 16;
            channelstepremainder[0] &= 65536-1;

            if (channels[0] >= channelsend[0])
                channels[0] = 0;
        }

        if (channels[1])
        {
            sample = *channels[1];
            // dl += channelleftvol_lookup[1][sample];
            dl += volume_lookup[leftv[1]][sample];
            // dr += channelrightvol_lookup[1][sample];
            dr += volume_lookup[rightv[1]][sample];
            channelstepremainder[1] += channelstep[1];
            channels[1] += channelstepremainder[1] >> 16;
            channelstepremainder[1] &= 65536-1;

            if (channels[1] >= channelsend[1])
                channels[1] = 0;
        }

        if (channels[2])
        {
            sample = *channels[2];
            // dl += channelleftvol_lookup[2][sample];
            dl += volume_lookup[leftv[2]][sample];
            // dr += channelrightvol_lookup[2][sample];
            dr += volume_lookup[rightv[2]][sample];
            channelstepremainder[2] += channelstep[2];
            channels[2] += channelstepremainder[2] >> 16;
            channelstepremainder[2] &= 65536-1;

            if (channels[2] >= channelsend[2])
                channels[2] = 0;
        }
        
        if (channels[3])
        {
            sample = *channels[3];
            // dl += channelleftvol_lookup[3][sample];
            dl += volume_lookup[leftv[3]][sample];
            // dr += channelrightvol_lookup[3][sample];
            dr += volume_lookup[rightv[3]][sample];
            channelstepremainder[3] += channelstep[3];
            channels[3] += channelstepremainder[3] >> 16;
            channelstepremainder[3] &= 65536-1;

            if (channels[3] >= channelsend[3])
                channels[3] = 0;
        }
        
        if (channels[4])
        {
            sample = *channels[4];
            // dl += channelleftvol_lookup[4][sample];
            dl += volume_lookup[leftv[4]][sample];
            // dr += channelrightvol_lookup[4][sample];
            dr += volume_lookup[rightv[4]][sample];
            channelstepremainder[4] += channelstep[4];
            channels[4] += channelstepremainder[4] >> 16;
            channelstepremainder[4] &= 65536-1;

            if (channels[4] >= channelsend[4])
                channels[4] = 0;
        }
        
        if (channels[5])
        {
            sample = *channels[5];
            // dl += channelleftvol_lookup[5][sample];
            dl += volume_lookup[leftv[5]][sample];
            // dr += channelrightvol_lookup[5][sample];
            dr += volume_lookup[rightv[5]][sample];
            channelstepremainder[5] += channelstep[5];
            channels[5] += channelstepremainder[5] >> 16;
            channelstepremainder[5] &= 65536-1;

            if (channels[5] >= channelsend[5])
                channels[5] = 0;
        }
        
        if (channels[6])
        {
            sample = *channels[6];
            // dl += channelleftvol_lookup[6][sample];
            dl += volume_lookup[leftv[6]][sample];
            // dr += channelrightvol_lookup[6][sample];
            dr += volume_lookup[rightv[6]][sample];
            channelstepremainder[6] += channelstep[6];
            channels[6] += channelstepremainder[6] >> 16;
            channelstepremainder[6] &= 65536-1;

            if (channels[6] >= channelsend[6])
                channels[6] = 0;
        }
        if (channels[7])
        {
            sample = *channels[7];
            // dl += channelleftvol_lookup[7][sample];
            dl += volume_lookup[leftv[7]][sample];
            // dr += channelrightvol_lookup[7][sample];
            dr += volume_lookup[rightv[7]][sample];
            channelstepremainder[7] += channelstep[7];
            channels[7] += channelstepremainder[7] >> 16;
            channelstepremainder[7] &= 65536-1;

            if (channels[7] >= channelsend[7])
                channels[7] = 0;
        }

        // Has been char instead of short.
        // if (dl > 127) *leftout = 127;
        // else if (dl < -128) *leftout = -128;
        // else *leftout = dl;

        // if (dr > 127) *rightout = 127;
        // else if (dr < -128) *rightout = -128;
        // else *rightout = dr;
        
        dl <<= 3;
        dr <<= 3;

        if (!audio_8bit_flag)
        {
            if (dl > 0x7fff)
                *leftout = 0x7fff;
            else if (dl < -0x8000)
                *leftout = -0x8000;
            else
                *leftout = dl;

            if (dr > 0x7fff)
                *rightout = 0x7fff;
            else if (dr < -0x8000)
                *rightout = -0x8000;
            else
                *rightout = dr;
        }
        else
        {
            if (dl > 0x7fff)
                dl = 0x7fff;
            else if (dl < -0x8000)
                dl = -0x8000;
            sdl = dl ^ 0xfff8000;

            if (dr > 0x7fff)
                dr = 0x7fff;
            else if (dr < -0x8000)
                dr = -0x8000;
            sdr = dr ^ 0xfff8000;

            *bothout++ = (((sdr + sdl) / 2) >> 8);
        }

        leftout += step;
        rightout += step;

    }
    return 1;
}

static struct timeval           last={0,0};
//static struct timeval         now;

static struct timezone          whocares;

void updatesounds(void)
{

    mix();
    I_SubmitOutputBuffer(mixbuffer, SAMPLECOUNT);

}

int
addsfx
( int           sfxid,
  int           volume,
  int           step,
  int           seperation )
{
    static unsigned short       handlenums = 0;
 
    int         i;
    int         rc = -1;
    
    int         oldest = mytime;
    int         oldestnum = 0;
    int         slot;
    int         rightvol;
    int         leftvol;

    // play these sound effects
    //  only one at a time
    if ( sfxid == sfx_sawup
         || sfxid == sfx_sawidl
         || sfxid == sfx_sawful
         || sfxid == sfx_sawhit
         || sfxid == sfx_stnmov
         || sfxid == sfx_pistol )
    {
        for (i=0 ; i<8 ; i++)
        {
            if (channels[i] && channelids[i] == sfxid)
            {
                channels[i] = 0;
                break;
            }
        }
    }

    for (i=0 ; i<8 && channels[i] ; i++)
    {
        if (channelstart[i] < oldest)
        {
            oldestnum = i;
            oldest = channelstart[i];
        }
    }

    if (i == 8)
        slot = oldestnum;
    else
        slot = i;

    channels[slot] = (unsigned char *) S_sfx[sfxid].data;
    channelsend[slot] = channels[slot] + lengths[sfxid];

    if (!handlenums)
        handlenums = 100;
    
    channelhandles[slot] = rc = handlenums++;
    channelstep[slot] = step;
    channelstepremainder[slot] = 0;
    channelstart[slot] = mytime;

    // (range: 1 - 256)
    seperation += 1;

    // (x^2 seperation)
    leftvol =
        volume - (volume*seperation*seperation)/(256*256);

    seperation = seperation - 257;

    // (x^2 seperation)
    rightvol =
        volume - (volume*seperation*seperation)/(256*256);      

    // sanity check
    if (rightvol < 0 || rightvol > 127)
        derror("rightvol out of bounds");
    
    if (leftvol < 0 || leftvol > 127)
        derror("leftvol out of bounds");
    
    // get the proper lookup table piece
    //  for this volume level
    // channelleftvol_lookup[slot] = &vol_lookup[(leftvol*master_volume/31)*256];
    // channelrightvol_lookup[slot] = &vol_lookup[(rightvol*master_volume/31)*256];
    left_volume[slot] = leftvol;
    right_volume[slot] = rightvol;

    channelids[slot] = sfxid;

    return rc;

}


void outputushort(int num)
{

    static unsigned char        buff[5] = { 0, 0, 0, 0, '\n' };
    static char*                badbuff = "xxxx\n";

    // outputs a 16-bit # in hex or "xxxx" if -1.
    if (num < 0)
    {
        write(1, badbuff, 5);
    }
    else
    {
        buff[0] = num>>12;
        buff[0] += buff[0] > 9 ? 'a'-10 : '0';
        buff[1] = (num>>8) & 0xf;
        buff[1] += buff[1] > 9 ? 'a'-10 : '0';
        buff[2] = (num>>4) & 0xf;
        buff[2] += buff[2] > 9 ? 'a'-10 : '0';
        buff[3] = num & 0xf;
        buff[3] += buff[3] > 9 ? 'a'-10 : '0';
        write(1, buff, 5);
    }
}

void initdata(void)
{

    int         i;
    int         j;
    
    int*        steptablemid = steptable + 128;

    for (i=0 ;
         i<sizeof(channels)/sizeof(unsigned char *) ;
         i++)
    {
        channels[i] = 0;
    }
    
    gettimeofday(&last, &whocares);

    for (i=-128 ; i<128 ; i++)
        steptablemid[i] = pow(2.0, (i/64.0))*65536.0;

    // generates volume lookup tables
    //  which also turn the unsigned samples
    //  into signed samples
    // for (i=0 ; i<128 ; i++)
    // for (j=0 ; j<256 ; j++)
    // vol_lookup[i*256+j] = (i*(j-128))/127;
    
    for (i=0 ; i<128 ; i++)
        for (j=0 ; j<256 ; j++)
            // vol_lookup[i*256+j] = (i*(j-128)*256)/127;
            volume_lookup[i][j] = (i*(j-128)*256)/127;

}




void quit(void)
{
    I_ShutdownMusic();
    I_ShutdownSound();
    exit(0);
}



fd_set          fdset;
fd_set          scratchset;



int
main
( int           c,
  char**        v )
{

    int         done = 0;
    int         rc;
    int         nrc;
    int         sndnum;
    int         sndcnt = 0;
    int         handle = 0;
    
    unsigned char       commandbuf[10];
    struct timeval      zerowait = { 0, 0 };

    
    int         step;
    int         vol;
    int         sep;
    
    int         i;
    int         waitingtofinish=0;

    // init any data
    initdata();         

    I_InitSound(11025, 16);

    I_InitMusic();

    if (snd_verbose)
        fprintf(stderr, "ready\n");
    
    // parse commands and play sounds until done
    FD_ZERO(&fdset);
    FD_SET(0, &fdset);

    while (!done)
    {
        mytime++;

        if (!waitingtofinish)
        {
            do {
                scratchset = fdset;
                rc = select(FD_SETSIZE, &scratchset, 0, 0, &zerowait);

                if (rc > 0)
                {
                    //  fprintf(stderr, "select is true\n");
                    // got a command
                    nrc = read(0, commandbuf, 1);

                    if (!nrc)
                    {
                        done = 1;
                        rc = 0;
                    }
                    else
                    {
                        //if (snd_verbose)
                        //    fprintf(stderr, "cmd: %c", commandbuf[0]);

                        switch (commandbuf[0])
                        {
                        case 'v':
                            // get master volume
                            read(0, commandbuf, 1);
                            master_volume = (commandbuf[0] & 0x1f);
                            break;
                            
                        case 'p':
                            // play a new sound effect
                            read(0, commandbuf, 4);

                            //if (snd_verbose)
                            //{
                            //  commandbuf[9]=0;
                            //  fprintf(stderr, "%s\n", commandbuf);
                            //}

                            //  p<snd#><step><vol><sep>
                            sndnum = commandbuf[0];
                            step = commandbuf[1];
                            step = steptable[step];
                            vol = commandbuf[2];
                            sep = commandbuf[3];

                            handle = addsfx(sndnum, vol, step, sep);
                            // returns the handle
                            //  outputushort(handle);
                            break;

                        case 'l': {
                            int bln,tlen;
                            read(0, &bln, sizeof(int));
                            //fprintf(stderr,"%d in...\n",bln);
                            S_sfx[sndcnt].data = malloc(bln);
                            // hey, read on a pipewill not always
                            // fill the whole buffer 19990203 by Kin
                            for(tlen = 0; tlen < bln;) {
                                tlen+=read(0, S_sfx[sndcnt].data+tlen, bln-tlen);
                            }
                            lengths[sndcnt] = bln;
                            sndcnt++;
                            break;
                        }
                        case 'q':
                            // no '\n' 19990201 by Kin
                            //read(0, commandbuf, 1);
                            waitingtofinish = 1; rc = 0;
                            break;
                            
                            //case 's':
                            //{
                            //  int fd;
                            //  read(0, commandbuf, 3);
                            //  commandbuf[2] = 0;
                            //  fd = open((char*)commandbuf, O_CREAT|O_WRONLY, 0644);
                            //  commandbuf[0] -= commandbuf[0]>='a' ? 'a'-10 : '0';
                            //  commandbuf[1] -= commandbuf[1]>='a' ? 'a'-10 : '0';
                            //  sndnum = (commandbuf[0]<<4) + commandbuf[1];
                            //  write(fd, S_sfx[sndnum].data, lengths[sndnum]);
                            //  close(fd);
                            //}
                            //break;
                        default:
                            fprintf(stderr, "Did not recognize command %d\n",commandbuf[0]);
                            break;
                        }
                    }
                }
                else if (rc < 0)
                {
                    quit();
                }
            } while (rc > 0);
        }

        updatesounds();

        if (waitingtofinish)
        {
            for(i=0 ; i<8 && !channels[i] ; i++);
            
            if (i==8)
                done=1;
        }

    }

    quit();
    return 0;
}
