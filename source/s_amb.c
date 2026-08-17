// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: s_amb.c,v 1.6 2001/06/10 21:16:01 bpereira Exp $
//
// Copyright (C) 1993-1996 by Raven Software, Corp.
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
// $Log: s_amb.c,v $
// Revision 1.6  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.5  2001/05/27 13:42:48  bpereira
// no message
//
// Revision 1.4  2001/04/04 20:24:21  judgecutor
// Added support for the 3D Sound
//
// Revision 1.3  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.2  2001/02/10 13:20:55  hurdler
// update license
//
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "g_game.h"
#include "p_local.h"
#include "p_setup.h"    //levelflats for flat animation
#include "r_data.h"
#include "m_random.h"

#include "s_sound.h"
#include "w_wad.h"
#include "z_zone.h"
#include "dstrings.h" //SoM: 3/10/2000
#include "r_main.h"   //Two extra includes.
#include "t_script.h"

// 3D sound
#include "hardware/hw3sound.h"

#define MAX_AMBIENT_SFX 8 // Per level

// Types

typedef enum
{
        afxcmd_play,            // (sound)
        afxcmd_playabsvol,      // (sound, volume)
        afxcmd_playrelvol,      // (sound, volume)
        afxcmd_delay,           // (ticks)
        afxcmd_delayrand,       // (andbits)
        afxcmd_end                      // ()
} afxcmd_t;

// Data

int *LevelAmbientSfx[MAX_AMBIENT_SFX];
int *AmbSfxPtr;
int AmbSfxCount;
int AmbSfxTics;
int AmbSfxVolume;

int AmbSndSeqInit[] =
{ // Startup
        afxcmd_end
};
int AmbSndSeq1[] =
{ // Scream
        afxcmd_play, sfx_amb1,
        afxcmd_end
};
int AmbSndSeq2[] =
{ // Squish
        afxcmd_play, sfx_amb2,
        afxcmd_end
};
int AmbSndSeq3[] =
{ // Drops
        afxcmd_play, sfx_amb3,
        afxcmd_delay, 16,
        afxcmd_delayrand, 31,
        afxcmd_play, sfx_amb7,
        afxcmd_delay, 16,
        afxcmd_delayrand, 31,
        afxcmd_play, sfx_amb3,
        afxcmd_delay, 16,
        afxcmd_delayrand, 31,
        afxcmd_play, sfx_amb7,
        afxcmd_delay, 16,
        afxcmd_delayrand, 31,
        afxcmd_play, sfx_amb3,
        afxcmd_delay, 16,
        afxcmd_delayrand, 31,
        afxcmd_play, sfx_amb7,
        afxcmd_delay, 16,
        afxcmd_delayrand, 31,
        afxcmd_end
};
int AmbSndSeq4[] =
{ // SlowFootSteps
        afxcmd_play, sfx_amb4,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb4, -3,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb4, -3,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb4, -3,
        afxcmd_delay, 15,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_end
};
int AmbSndSeq5[] =
{ // Heartbeat
        afxcmd_play, sfx_amb5,
        afxcmd_delay, 35,
        afxcmd_play, sfx_amb5,
        afxcmd_delay, 35,
        afxcmd_play, sfx_amb5,
        afxcmd_delay, 35,
        afxcmd_play, sfx_amb5,
        afxcmd_end
};
int AmbSndSeq6[] =
{ // Bells
        afxcmd_play, sfx_amb6,
        afxcmd_delay, 17,
        afxcmd_playrelvol, sfx_amb6, -8,
        afxcmd_delay, 17,
        afxcmd_playrelvol, sfx_amb6, -8,
        afxcmd_delay, 17,
        afxcmd_playrelvol, sfx_amb6, -8,
        afxcmd_end
};
int AmbSndSeq7[] =
{ // Growl
        afxcmd_play, sfx_bstsit,
        afxcmd_end
};
int AmbSndSeq8[] =
{ // Magic
        afxcmd_play, sfx_amb8,
        afxcmd_end
};
int AmbSndSeq9[] =
{ // Laughter
        afxcmd_play, sfx_amb9,
        afxcmd_delay, 16,
        afxcmd_playrelvol, sfx_amb9, -4,
        afxcmd_delay, 16,
        afxcmd_playrelvol, sfx_amb9, -4,
        afxcmd_delay, 16,
        afxcmd_playrelvol, sfx_amb10, -4,
        afxcmd_delay, 16,
        afxcmd_playrelvol, sfx_amb10, -4,
        afxcmd_delay, 16,
        afxcmd_playrelvol, sfx_amb10, -4,
        afxcmd_end
};
int AmbSndSeq10[] =
{ // FastFootsteps
        afxcmd_play, sfx_amb4,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb4, -3,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb4, -3,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb4, -3,
        afxcmd_delay, 8,
        afxcmd_playrelvol, sfx_amb11, -3,
        afxcmd_end
};

int *AmbientSfx[] =
{
        AmbSndSeq1,             // Scream
        AmbSndSeq2,             // Squish
        AmbSndSeq3,             // Drops
        AmbSndSeq4,             // SlowFootsteps
        AmbSndSeq5,             // Heartbeat
        AmbSndSeq6,             // Bells
        AmbSndSeq7,             // Growl
        AmbSndSeq8,             // Magic
        AmbSndSeq9,             // Laughter
        AmbSndSeq10             // FastFootsteps
};

//----------------------------------------------------------------------------
//
// PROC P_InitAmbientSound
//
//----------------------------------------------------------------------------

void P_InitAmbientSound(void)
{
        AmbSfxCount = 0;
        AmbSfxVolume = 0;
        AmbSfxTics = 10*TICRATE;
        AmbSfxPtr = AmbSndSeqInit;
}

//----------------------------------------------------------------------------
//
// PROC P_AddAmbientSfx
//
// Called by (P_mobj):P_SpawnMapThing during (P_setup):P_SetupLevel.
//
//----------------------------------------------------------------------------

void P_AddAmbientSfx(int sequence)
{
        if(AmbSfxCount == MAX_AMBIENT_SFX)
        {
                I_Error("Too many ambient sound sequences");
        }
        LevelAmbientSfx[AmbSfxCount++] = AmbientSfx[sequence];
}

//----------------------------------------------------------------------------
//
// PROC P_AmbientSound
//
// Called every tic by (P_tick):P_Ticker.
//
//----------------------------------------------------------------------------

void P_AmbientSound(void)
{
        afxcmd_t cmd;
        int sound;
        boolean done;

        if(!AmbSfxCount)
        { // No ambient sound sequences on current level
                return;
        }
        if(--AmbSfxTics)
        {
                return;
        }
        done = false;
        do
        {
                cmd = *AmbSfxPtr++;
                switch(cmd)
                {
                        case afxcmd_play:
                                AmbSfxVolume = M_Random()>>2;
                                S_StartAmbientSound(*AmbSfxPtr++, AmbSfxVolume);
                                break;
                        case afxcmd_playabsvol:
                                sound = *AmbSfxPtr++;
                                AmbSfxVolume = *AmbSfxPtr++;
                                S_StartAmbientSound(sound, AmbSfxVolume);
                                break;
                        case afxcmd_playrelvol:
                                sound = *AmbSfxPtr++;
                                AmbSfxVolume += *AmbSfxPtr++;
                                if(AmbSfxVolume < 0)
                                {
                                        AmbSfxVolume = 0;
                                }
                                else if(AmbSfxVolume > 127)
                                {
                                        AmbSfxVolume = 127;
                                }
                                S_StartAmbientSound(sound, AmbSfxVolume);
                                break;
                        case afxcmd_delay:
                                AmbSfxTics = *AmbSfxPtr++;
                                done = true;
                                break;
                        case afxcmd_delayrand:
                                AmbSfxTics = M_Random()&(*AmbSfxPtr++);
                                done = true;
                                break;
                        case afxcmd_end:
                                AmbSfxTics = 6*TICRATE+M_Random();
                                AmbSfxPtr = LevelAmbientSfx[M_Random()%AmbSfxCount];
                                done = true;
                                break;
                        default:
                                CONS_Printf("P_AmbientSound: Unknown afxcmd %d", cmd);
                                break;
                }
        } while(done == false);
}
