// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: i_sound.c,v 1.12 2004/04/18 12:53:42 hurdler Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log: i_sound.c,v $
// Revision 1.12  2004/04/18 12:53:42  hurdler
// fix Heretic issue with SDL and OS/2
//
// Revision 1.11  2003/07/13 13:16:15  hurdler
// go RC1
//
// Revision 1.10  2001/08/20 20:40:42  metzgermeister
// *** empty log message ***
//
// Revision 1.9  2001/05/16 22:33:35  bock
// Initial FreeBSD support.
//
// Revision 1.8  2001/05/14 19:02:58  metzgermeister
//   * Fixed floor not moving up with player on E3M1
//   * Fixed crash due to oversized string in screen message ... bad bug!
//   * Corrected some typos
//   * fixed sound bug in SDL
//
// Revision 1.7  2001/04/14 14:15:14  metzgermeister
// fixed bug no sound device
//
// Revision 1.6  2001/04/09 20:21:56  metzgermeister
// dummy for I_FreeSfx
//
// Revision 1.5  2001/03/25 18:11:24  metzgermeister
//   * SDL sound bug with swapped stereo channels fixed
//   * separate hw_trick.c now for HW_correctSWTrick(.)
//
// Revision 1.4  2001/03/09 21:53:56  metzgermeister
// *** empty log message ***
//
// Revision 1.3  2000/11/02 19:49:40  bpereira
// no message
//
// Revision 1.2  2000/09/10 10:56:00  metzgermeister
// clean up & made it work again
//
// Revision 1.1  2000/08/21 21:17:32  metzgermeister
// Initial import to CVS
//
//
// DESCRIPTION:
//      System interface for sound.
//
//-----------------------------------------------------------------------------

#ifndef __wtypes_h__
#include <wtypes.h>
#endif

#ifndef __WINDEF_
#include <windef.h>
#endif

#include <math.h>

#include <al/al.h>
#include <al/alc.h>

//#include <unistd.h>

#include "z_zone.h"

#include "m_swap.h"
#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"
#include "doomstat.h"
#include "s_sound.h"
#include "doomtype.h"

#include "d_main.h"

#include "../ymfmidi/src/ymfmidiCPlayer.h"

#define W_CacheLumpNum(num) (W_CacheLumpNum)((num),1)
#define W_CacheLumpName(name) W_CacheLumpNum (W_GetNumForName(name))

// Needed for calling the actual sound output.
#define NUM_CHANNELS            8
#define SAMPLERATE              44100   // Hz
#define NUM_MUSIC_BUFFERS     3

// Flags for the -nosound and -nomusic options
extern boolean nosound;
extern boolean nomusic;

static boolean musicStarted = false;
static boolean soundStarted = false;

struct channel_t
{
    void* origin_p;
    ALuint source;
    int handle;
    tic_t gametic;
};

typedef struct channel_t channel_s;

static ALCdevice* device;
static ALCcontext* context;
static channel_s channels[NUM_CHANNELS];
static ALuint sndBuffers[NUMSFX];
static ALuint musicSid;
static ALuint musicBid[NUM_MUSIC_BUFFERS];  // openal buffer ids.
static int songHandle = -1;
static int musicPlaying = 0;
static void* lastListener = NULL;

//
// This function loads the sound data from the WAD lump,
//  for single sound.
// [Edward] Massively rewritten, the previous implementation didn't even read the rate and length correctly.
//
static void *getsfx(const char *sfxname, int *len, int* rate)
{
    unsigned char *sfx = NULL;
    unsigned char *loadedsfx = NULL;
    int size, lumpsize;
    char name[20];
    int sfxlump;

    // Get the sound data from the WAD, allocate lump
    //  in zone memory.
    sprintf(name, "ds%s", sfxname);

    if (W_CheckNumForName(name) == -1)
    {
        if (gamemode == heretic)
            sfxlump = W_GetNumForName("keyup");
        else
            sfxlump = W_GetNumForName("dspistol");
    }
    else
        sfxlump = W_GetNumForName(name);

    lumpsize = W_LumpLength(sfxlump);
    if (lumpsize > 32)
    {
        sfx = (unsigned char*)W_CacheLumpNum(sfxlump);

        // Sample rate header
		UINT16 rate16;
        memcpy(&rate16, &sfx[0x02], sizeof(UINT16));
		*rate = rate16;

        // Length and padding
        memcpy(&size, &sfx[0x04], sizeof(INT32));

        if (size > 32 && ((size - 32) - 1) + 0x18 < lumpsize)
        {
            // Strip the padding
            size -= 32;
            loadedsfx = (float*)Z_Malloc(size, PU_STATIC, 0);

            for (unsigned int i = 0; i < size; i++)
            {
                loadedsfx[i] = sfx[i + 0x18];
            }

            *len = size;
        }
    }

	// Failed to load anything, return a dummy sound.
    if (loadedsfx == NULL)
    {
        loadedsfx = (float*)Z_Malloc(sizeof(unsigned char), PU_STATIC, 0);
        *loadedsfx = 0;
        *len = 1;
    }

    // Remove the cached lump.
    if (sfx)
    {
        Z_Free(sfx);
    }

    return (void *)loadedsfx;
}

//
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
//

static int addsfx(int sfxid, int volume, void* origin_p)
{
    static unsigned short handlenums = 0;
    static int prevChannel = NUM_CHANNELS - 1;
    int i;
    tic_t oldest = gametic;
    channel_s* channel = &channels[(prevChannel + 1) % NUM_CHANNELS];

    // Loop all channels to find oldest SFX.
    for (i = 0; i < NUM_CHANNELS; i++)
    {
        if(channels[i].gametic < oldest)
        {
            oldest = channels[i].gametic;
			channel = &channels[i];
		}
    }

	alSourceStop(channel->source);
    alGetError(); // Clear error
	alSourcei(channel->source, AL_BUFFER, sndBuffers[sfxid]);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
		CONS_Printf("addsfx: alSourcei error %x\n", error);
    }
	channel->handle = ++handlenums;
	channel->gametic = gametic;
	prevChannel = (channel - channels) / sizeof(channel_s);

    if (origin_p == lastListener)
    {
        origin_p = NULL;
    }

    mobj_t* origin = (mobj_t*)origin_p;
	channel->origin_p = origin_p;
    alSourcei(channel->source, AL_SOURCE_RELATIVE, origin ? AL_FALSE : AL_TRUE);
    fixed_t mobjz = 0;

    if (origin)
    {
        alSourcef(channel->source, AL_ROLLOFF_FACTOR, 0.5f);
        alSourcei(channel->source, AL_SOURCE_RELATIVE, AL_FALSE);
        alSourcei(channel->source, AL_REFERENCE_DISTANCE, 200);
        alSourcei(channel->source, AL_MAX_DISTANCE, 1200);
        mobjz = origin->z;
        if (origin->thinker.function.acv == 0 && lastListener)
        {
            // Object has no thinker, might be a degenerate mobj.
            mobj_t* listener = (mobj_t*)lastListener;
            mobjz = listener->z;
        }
    }
    else
    {
        alSourcef(channel->source, AL_ROLLOFF_FACTOR, 0.0f);
        alSourcei(channel->source, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcei(channel->source, AL_REFERENCE_DISTANCE, 0);
        alSourcei(channel->source, AL_MAX_DISTANCE, 0);
    }

	float gain = (float)cv_soundvolume.value / 16.0f;
    if (gain > 1.0f)
        gain *= 4.0f;
	alSourcef(channel->source, AL_GAIN, gain);
    alSource3f(channel->source, AL_POSITION,
        origin ? FIXED_TO_FLOAT(origin->x) : 0.0f,
        origin ? FIXED_TO_FLOAT(mobjz) : 0.0f,
		origin ? FIXED_TO_FLOAT(-origin->y) : 0.0f);

	alSourcePlay(channel->source);
	error = alGetError();
    if (error != AL_NO_ERROR)
	{
        CONS_Printf("addsfx: alSourcePlay error %x\n", error);
	}

    return channel->handle;
}

void I_SetSfxVolume(int volume)
{
    // Identical to DOS.
    // Basically, this should propagate
    //  the menu/config file setting
    //  to the state variable used in
    //  the mixing.

    CV_SetValue(&cv_soundvolume, volume);
    float gain = (float)cv_soundvolume.value / 16.0f;
    if (gain > 1.0f)
        gain *= 4.0f;
    for(int i = 0; i < NUM_CHANNELS; i++)
    {
        alSourcef(channels[i].source, AL_GAIN, gain);
	}
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t * sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

void *I_GetSfx(sfxinfo_t * sfx)
{
    int len;
	int rate;
    return getsfx(sfx->name, &len, &rate);
}

// FIXME: dummy for now Apr.9 2001 by Rob
void I_FreeSfx(sfxinfo_t * sfx)
{
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
int I_StartSound(int id, int vol, void* origin_p, int pitch, int priority)
{
    priority = 0;

    if (nosound)
        return 0;

    return addsfx(id, vol, origin_p);
}

void I_StopSound(int handle)
{
    int i;
    for (i = 0; i < NUM_CHANNELS; i++)
    {
        if (channels[i].handle == handle)
        {
            alSourceStop(channels[i].source);
			channels[i].gametic = 0;
			channels[i].origin_p = NULL;
            break;
        }
    }
}

int I_SoundIsPlaying(int handle)
{
    int i;
    for (i = 0; i < NUM_CHANNELS; i++)
    {
        if (channels[i].handle == handle)
        {
			ALint source_state;
            alGetSourcei(channels[i].source, AL_SOURCE_STATE, &source_state);
            return source_state == AL_PLAYING;
        }
    }
    return 0;
}

//
// Not used by SDL version
//
void I_SubmitSound(void)
{
}

static void I_QueueBuffer(ALuint bid, char* buf, const size_t buflen)
{
    if (bid == 0 || buf == NULL || buflen == 0)
    {
        return;
    }

    YMFMIDI_Generate16((unsigned short*)buf, buflen / sizeof(unsigned short) / 2);
    alBufferData(bid, AL_FORMAT_STEREO16, buf, buflen, YMFMIDI_SampleRate());
    alSourceQueueBuffers(musicSid, 1, &bid);
}

const size_t buflen = 1024 * 5;
static byte soundData[NUM_MUSIC_BUFFERS][1024 * 5];
static int GetMusicBuffer(ALuint bid)
{
    for(int i = 0; i < NUM_MUSIC_BUFFERS; i++)
    {
        if (musicBid[i] == bid)
            return i;
    }
	return 0;
}

static void I_UpdateMusic()
{
    if(!musicPlaying)
		return;

    ALint processed = 0;
	int ready = YMFMIDI_SamplesReady();
    alGetSourcei(musicSid, AL_BUFFERS_PROCESSED, &processed);
    for (ALint i = 0; i < processed; i++)
    {
        YMFMIDI_SamplesCountAdd();
        if (ready)
        {
            ready--;
            ALuint bid = 0;
            alSourceUnqueueBuffers(musicSid, 1, &bid);
            I_QueueBuffer(bid, (char*)soundData[GetMusicBuffer(bid)], buflen);

            ALint state = AL_PLAYING;
            alGetSourcei(musicSid, AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED)
            {
                alSourcePlay(musicSid);
            }
        }
    }
}

void I_UpdateSound(void* listener_p)
{
    if (gamestate == GS_LEVEL)
    {
        lastListener = listener_p;
		mobj_t* listener = (mobj_t*)listener_p;
        if (listener)
        {
            const int yaw = listener->angle >> ANGLETOFINESHIFT;

            alListener3f(AL_POSITION,
                FIXED_TO_FLOAT(listener->x),
                FIXED_TO_FLOAT(listener->z),
				FIXED_TO_FLOAT(-listener->y));

            ALfloat listenerOri[] = { FIXED_TO_FLOAT(finecosine[yaw]), 0.0f, FIXED_TO_FLOAT(-finesine[yaw]), 0.0f, 1.0f, 0.0f };
            alListenerfv(AL_ORIENTATION, listenerOri);
        }

        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            if (channels[i].origin_p)
            {
                mobj_t* origin = (mobj_t*)channels[i].origin_p;
				fixed_t mobjz = origin->z;
                if (origin->thinker.function.acv == 0 && listener)
                {
					// Object has no thinker, might be a degenerate mobj.
                    mobjz = listener->z;
                }

                alSource3f(channels[i].source, AL_POSITION,
                    FIXED_TO_FLOAT(origin->x),
                    FIXED_TO_FLOAT(mobjz),
                    FIXED_TO_FLOAT(-origin->y));
            }
		}
    }
    else
    {
		lastListener = NULL;
    }

    I_UpdateMusic();
}

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
    // I fail too see that this is used.
    // Would be using the handle to identify
    //  on which channel the sound might be active,
    //  and resetting the channel parameters.

    // UNUSED.
    handle = vol = sep = pitch = 0;
}

void I_StartupSound()
{
    int i;
    ALenum error = 0;

    if (nosound)
        return;

    nosound = true;
    // Configure sound device
    CONS_Printf("I_InitSound: ");

    device = alcOpenDevice(NULL);
    if(!device)
    {
        CONS_Printf("I_InitSound: alcOpenDevice error\n");
        return;
	}
    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context))
    {
        CONS_Printf("I_InitSound: alcCreateContext error\n");
        return;
    }

    ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    alListener3f(AL_POSITION, 0, 0, 1.0f);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListenerfv(AL_ORIENTATION, listenerOri);

	memset(channels, 0, sizeof(channels));
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        alGenSources((ALuint)1, &channels[i].source);
        alSourcef(channels[i].source, AL_PITCH, 1);
        alSourcef(channels[i].source, AL_GAIN, 1);
        alSource3f(channels[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(channels[i].source, AL_VELOCITY, 0, 0, 0);
        alSourcei(channels[i].source, AL_LOOPING, AL_FALSE);
    }

    alGenBuffers((ALuint)NUMSFX, sndBuffers);
    //floatenum = alGetEnumValue("AL_FORMAT_MONO_FLOAT32");

    // Initialize external data (all sounds) at start, keep static.
    CONS_Printf("I_InitSound: (%d sfx)", NUMSFX);

    for (i = 1; i < NUMSFX; i++)
    {
        // Alias? Example is the chaingun sound linked to pistol.
        if (S_sfx[i].name)
        {
            if (!S_sfx[i].link)
            {
                // Load data from WAD file.
                int rate;
                int len;
                S_sfx[i].data = getsfx(S_sfx[i].name, &len, &rate);
				alBufferData(sndBuffers[i], AL_FORMAT_MONO8, S_sfx[i].data, len, rate);
				error = alGetError();
                if (error != AL_NO_ERROR)
                {
                    CONS_Printf("I_InitSound: Error loading sound %.6s: %x\n", S_sfx[i].name, error);
				}
            }
            else
            {
                // Previously loaded already?
                S_sfx[i].data = S_sfx[i].link->data;
                //lengths[i] = lengths[(S_sfx[i].link - S_sfx) / sizeof(sfxinfo_t)];
            }
        }
    }

    CONS_Printf(" pre-cached all sound data\n");

    // Finished initialization.
    CONS_Printf("I_InitSound: sound module ready\n");

    nosound = false;
    soundStarted = true;
}

//
// MUSIC API.
//

void I_ShutdownMusic(void)
{
    if (nomusic)
        return;

    if (!musicStarted)
        return;

	YMFMIDI_Shutdown();

    CONS_Printf("I_ShutdownMusic: shut down\n");
    musicStarted = false;

}

void I_InitMusic(void)
{
    if (nosound)
    {
        nomusic = true;
        return;
    }

    if (nomusic)
        return;

    alGenBuffers(NUM_MUSIC_BUFFERS, musicBid);
    alGenSources(1, &musicSid);
    alSourcei(musicSid, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(musicSid, AL_ROLLOFF_FACTOR, 0.0f);
    alSource3f(musicSid, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcef(musicSid, AL_GAIN, 1.0f);

	int numChips = 8;
#if _DEBUG
	numChips = 1;
#endif
	YMFMIDI_Init(numChips, 3); // 8 chips, OPL3
	YMFMIDI_SetStereo(1);
	YMFMIDI_SetGain(15.0f);
    CONS_Printf("I_InitMusic: music initialized\n");
    musicStarted = true;
}

void I_PlaySong(int handle, int looping)
{
    if (nomusic)
        return;

    if (handle == songHandle)
    {
        for (int i = 0; i < NUM_MUSIC_BUFFERS; i++)
        {
            I_QueueBuffer(musicBid[i], soundData[i], buflen);
        }
        alSourcePlay(musicSid);
        musicPlaying = true;
		YMFMIDI_SetLoop(looping != 0);
    }
}

void I_PauseSong(int handle)
{
    if (nomusic)
        return;

    I_StopSong(handle);
}

void I_ResumeSong(int handle)
{
    if (nomusic)
        return;

    I_PlaySong(handle, true);
}

void I_StopSong(int handle)
{
    if (nomusic)
        return;
    
    alSourceStop(musicSid);
    alSourcei(musicSid, AL_BUFFER, 0);  // clear the buffer queue.
    YMFMIDI_Reset();
    musicPlaying = false;
}

void I_UnRegisterSong(int handle)
{
    if (nomusic)
        return;
}

int I_RegisterSong(void *data, int len)
{
    int err;
    ULONG midlength;
    FILE *midfile;

    if (nomusic)
        return 0;

	static int patchesLoaded = 0;
    if (!patchesLoaded)
    {
        int lumpnum = W_CheckNumForName("GENMIDI");
        int genlen = W_LumpLength(lumpnum);
        byte* gendata = (byte*)W_CacheLumpNum(lumpnum);
        YMFMIDI_LoadPatches(gendata, genlen);
        patchesLoaded = 1;
    }

	int handle = YMFMIDI_LoadSequence((const unsigned char*)data, (unsigned int)len);
    if (handle == 1)
        return ++songHandle;
    else
		return -1;
}

void I_SetMusicVolume(int volume)
{
    if (nomusic)
        return;

	YMFMIDI_SetGain((double)volume);
}

void I_ShutdownSound(void)
{

}