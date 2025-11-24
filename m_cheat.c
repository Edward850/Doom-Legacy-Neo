// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: m_cheat.c,v 1.6 2003/07/13 13:16:15 hurdler Exp $
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
// $Log: m_cheat.c,v $
// Revision 1.6  2003/07/13 13:16:15  hurdler
// go RC1
//
// Revision 1.5  2001/08/20 21:37:34  hurdler
// fix palette in splitscreen + hardware mode
//
// Revision 1.4  2001/02/10 12:27:14  bpereira
// no message
//
// Revision 1.3  2000/11/02 17:50:07  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Cheat sequence checking.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "dstrings.h"

#include "am_map.h"
#include "m_cheat.h"
#include "g_game.h"

#include "r_local.h"
#include "p_local.h"
#include "p_inter.h"

#include "m_cheat.h"

#include "i_sound.h"    // for I_PlayCD()
#include "s_sound.h"
#include "v_video.h"
#include "st_stuff.h"
#include "w_wad.h"

static boolean HandleCheats(byte key);

// ==========================================================================
//                             CHEAT Structures
// ==========================================================================

byte cheat_mus_seq[] = {
    0xb2, 0x26, 0xb6, 0xae, 0xea, 1, 0, 0, 0xff
};

//Fab:19-07-98: idcd xx : change cd track
byte cheat_cd_seq[] = {
    0xb2, 0x26, 0xe2, 0x26, 1, 0, 0, 0xff
};

byte cheat_choppers_seq[] = {
    0xb2, 0x26, 0xe2, 0x32, 0xf6, 0x2a, 0x2a, 0xa6, 0x6a, 0xea, 0xff    // id...
};

byte cheat_god_seq[] = {
    0xb2, 0x26, 0x26, 0xaa, 0x26, 0xff  // iddqd
};

byte cheat_ammo_seq[] = {
    0xb2, 0x26, 0xf2, 0x66, 0xa2, 0xff  // idkfa
};

byte cheat_ammonokey_seq[] = {
    0xb2, 0x26, 0x66, 0xa2, 0xff        // idfa
};

// Smashing Pumpkins Into Small Pieces Of Putrid Debris.
byte cheat_noclip_seq[] = {
    0xb2, 0x26, 0xea, 0x2a, 0xb2,       // idspispopd
    0xea, 0x2a, 0xf6, 0x2a, 0x26, 0xff
};

//
byte cheat_commercial_noclip_seq[] = {
    0xb2, 0x26, 0xe2, 0x36, 0xb2, 0x2a, 0xff    // idclip
};

//added:28-02-98: new cheat to fly around levels using jump !!
byte cheat_fly_around_seq[] = {
    0xb2, 0x26, SCRAMBLE('f'), SCRAMBLE('l'), SCRAMBLE('y'), 0xff       // idfly
};

byte cheat_powerup_seq[7][10] = {
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0x6e, 0xff},       // beholdv
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0xea, 0xff},       // beholds
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0xb2, 0xff},       // beholdi
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0x6a, 0xff},       // beholdr
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0xa2, 0xff},       // beholda
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0x36, 0xff},       // beholdl
    {0xb2, 0x26, 0x62, 0xa6, 0x32, 0xf6, 0x36, 0x26, 0xff}      // behold
};

byte cheat_clev_seq[] = {
    0xb2, 0x26, 0xe2, 0x36, 0xa6, 0x6e, 1, 0, 0, 0xff   // idclev
};

// my position cheat
byte cheat_mypos_seq[] = {
    0xb2, 0x26, 0xb6, 0xba, 0x2a, 0xf6, 0xea, 0xff      // idmypos
};

byte cheat_amap_seq[] = { 0xb2, 0x26, 0x26, 0x2e, 0xff };
cheatseq_t cheat_amap = { cheat_amap_seq, 0 };

// Now what?
cheatseq_t cheat_mus = { cheat_mus_seq, 0 };
cheatseq_t cheat_cd = { cheat_cd_seq, 0 };
cheatseq_t cheat_god = { cheat_god_seq, 0 };
cheatseq_t cheat_ammo = { cheat_ammo_seq, 0 };
cheatseq_t cheat_ammonokey = { cheat_ammonokey_seq, 0 };
cheatseq_t cheat_noclip = { cheat_noclip_seq, 0 };
cheatseq_t cheat_commercial_noclip = { cheat_commercial_noclip_seq, 0 };

//added:28-02-98:
cheatseq_t cheat_fly_around = { cheat_fly_around_seq, 0 };

cheatseq_t cheat_powerup[7] = {
    {cheat_powerup_seq[0], 0},
    {cheat_powerup_seq[1], 0},
    {cheat_powerup_seq[2], 0},
    {cheat_powerup_seq[3], 0},
    {cheat_powerup_seq[4], 0},
    {cheat_powerup_seq[5], 0},
    {cheat_powerup_seq[6], 0}
};

cheatseq_t cheat_choppers = { cheat_choppers_seq, 0 };
cheatseq_t cheat_clev = { cheat_clev_seq, 0 };
cheatseq_t cheat_mypos = { cheat_mypos_seq, 0 };

// ==========================================================================
//                        CHEAT SEQUENCE PACKAGE
// ==========================================================================

static byte cheat_xlate_table[256];

void cht_Init()
{
    int i;
    for (i = 0; i < 256; i++)
        cheat_xlate_table[i] = SCRAMBLE(i);
}

//
// Called in st_stuff module, which handles the input.
// Returns a 1 if the cheat was successful, 0 if failed.
//
int cht_CheckCheat(cheatseq_t * cht, char key)
{
    int rc = 0;

    if (!cht->p)
        cht->p = cht->sequence; // initialize if first time

    if (*cht->p == 0)
        *(cht->p++) = key;
    else if (cheat_xlate_table[(byte) key] == *cht->p)
        cht->p++;
    else
        cht->p = cht->sequence;

    if (*cht->p == 1)
        cht->p++;
    else if (*cht->p == 0xff)   // end of sequence character
    {
        cht->p = cht->sequence;
        rc = 1;
    }

    return rc;
}

void cht_GetParam(cheatseq_t * cht, char *buffer)
{

    byte *p, c;

    p = cht->sequence;
    while (*(p++) != 1);

    do
    {
        c = *p;
        *(buffer++) = c;
        *(p++) = 0;
    }
    while (c && *p != 0xff);

    if (*p == 0xff)
        *buffer = 0;

}

// added 2-2-98 for compatibility with dehacked
int idfa_armor = 200;
int idfa_armor_class = 2;
int idkfa_armor = 200;
int idkfa_armor_class = 2;
int god_health = 100;

static player_t *plyr;

boolean cht_Responder(event_t * ev)
{
    int i;
    char *msg;

    if (ev->type == ev_keydown)
    {
        if (gamemode == heretic)
            return HandleCheats(ev->data1);

        msg = NULL;

        // added 17-5-98
        plyr = &players[consoleplayer];
        // b. - enabled for more debug fun.
        // if (gameskill != sk_nightmare) {

        if (cht_CheckCheat(&cheat_amap, ev->data1))
            am_cheating = (am_cheating + 1) % 3;
        else
            // 'dqd' cheat for toggleable god mode
        if (cht_CheckCheat(&cheat_god, ev->data1))
        {
            plyr->cheats ^= CF_GODMODE;
            if (plyr->cheats & CF_GODMODE)
            {
                if (plyr->mo)
                    plyr->mo->health = god_health;

                plyr->health = god_health;
                //plyr->message = STSTR_DQDON;
                msg = STSTR_DQDON;
            }
            else
                //plyr->message = STSTR_DQDOFF;
                msg = STSTR_DQDOFF;
        }
        // 'fa' cheat for killer fucking arsenal
        else if (cht_CheckCheat(&cheat_ammonokey, ev->data1))
        {
            plyr->armorpoints = idfa_armor;
            plyr->armortype = idfa_armor_class;

            for (i = 0; i < NUMWEAPONS; i++)
                plyr->weaponowned[i] = true;

            for (i = 0; i < NUMAMMO; i++)
                plyr->ammo[i] = plyr->maxammo[i];

            //plyr->message = STSTR_FAADDED;
            msg = STSTR_FAADDED;
        }
        // 'kfa' cheat for key full ammo
        else if (cht_CheckCheat(&cheat_ammo, ev->data1))
        {
            plyr->armorpoints = idkfa_armor;
            plyr->armortype = idkfa_armor_class;

            for (i = 0; i < NUMWEAPONS; i++)
                plyr->weaponowned[i] = true;

            for (i = 0; i < NUMAMMO; i++)
                plyr->ammo[i] = plyr->maxammo[i];

            plyr->cards = it_allkeys;

            //plyr->message = STSTR_KFAADDED;
            msg = STSTR_KFAADDED;
        }
        // 'mus' cheat for changing music
        else if (cht_CheckCheat(&cheat_mus, ev->data1))
        {
            char buf[3];
            int musnum;

            plyr->message = STSTR_MUS;
            cht_GetParam(&cheat_mus, buf);

            if (gamemode == commercial)
            {
                musnum = mus_runnin + (buf[0] - '0') * 10 + buf[1] - '0' - 1;

                if (((buf[0] - '0') * 10 + buf[1] - '0') > 35)
                    //plyr->message = STSTR_NOMUS;
                    msg = STSTR_NOMUS;
                else
                    S_ChangeMusic(musnum, 1);
            }
            else
            {
                musnum = mus_e1m1 + (buf[0] - '1') * 9 + (buf[1] - '1');

                if (((buf[0] - '1') * 9 + buf[1] - '1') > 31)
                    //plyr->message = STSTR_NOMUS;
                    msg = STSTR_NOMUS;
                else
                    S_ChangeMusic(musnum, 1);
            }
        }

        // 'cd' for changing cd track quickly
        //NOTE: the cheat uses the REAL track numbers, not remapped ones
        /*else if (cht_CheckCheat(&cheat_cd, ev->data1))
        {
            char buf[3];

            cht_GetParam(&cheat_cd, buf);

            plyr->message = "Changing cd track...\n";
            I_PlayCD((buf[0] - '0') * 10 + (buf[1] - '0'), true);
        }*/

        // Simplified, accepting both "noclip" and "idspispopd".
        // no clipping mode cheat
        else if (cht_CheckCheat(&cheat_noclip, ev->data1) || cht_CheckCheat(&cheat_commercial_noclip, ev->data1))
        {
            plyr->cheats ^= CF_NOCLIP;

            if (plyr->cheats & CF_NOCLIP)
                //plyr->message = STSTR_NCON;
                msg = STSTR_NCON;
            else
                //plyr->message = STSTR_NCOFF;
                msg = STSTR_NCOFF;
        }

        // 'behold?' power-up cheats
        for (i = 0; i < 6; i++)
        {
            if (cht_CheckCheat(&cheat_powerup[i], ev->data1))
            {
                if (!plyr->powers[i])
                    P_GivePower(plyr, i);
                else if (i != pw_strength)
                    plyr->powers[i] = 1;
                else
                    plyr->powers[i] = 0;

                //plyr->message = STSTR_BEHOLDX;
                msg = STSTR_BEHOLDX;
            }
        }

        // 'behold' power-up menu
        if (cht_CheckCheat(&cheat_powerup[6], ev->data1))
        {
            //plyr->message = STSTR_BEHOLD;
            msg = STSTR_BEHOLD;
        }
        // 'choppers' invulnerability & chainsaw
        else if (cht_CheckCheat(&cheat_choppers, ev->data1))
        {
            plyr->weaponowned[wp_chainsaw] = true;
            plyr->powers[pw_invulnerability] = true;

            //plyr->message = STSTR_CHOPPERS;
            msg = STSTR_CHOPPERS;
        }
        // 'mypos' for player position
        else if (cht_CheckCheat(&cheat_mypos, ev->data1))
        {
            //plyr->message = buf;
            CONS_Printf(va("ang=%i;x,y=(%i,%i)\n", players[statusbarplayer].mo->angle / ANGLE_1, players[statusbarplayer].mo->x >> FRACBITS, players[statusbarplayer].mo->y >> FRACBITS));

        }
        else
            //added:28-02-98: new fly cheat using jump key
        if (cht_CheckCheat(&cheat_fly_around, ev->data1))
        {
            plyr->cheats ^= CF_FLYAROUND;
            if (plyr->cheats & CF_FLYAROUND)
                //plyr->message = "FLY MODE ON : USE JUMP KEY";
                msg = "FLY MODE ON : USE JUMP KEY\n";
            else
                //plyr->message = "FLY MODE OFF";
                msg = "FLY MODE OFF\n";
        }

        // 'clev' change-level cheat
        if (cht_CheckCheat(&cheat_clev, ev->data1))
        {
            char buf[3];
            int epsd;
            int map;

            cht_GetParam(&cheat_clev, buf);

            if (gamemode == commercial)
            {
                epsd = 0;
                map = (buf[0] - '0') * 10 + buf[1] - '0';
            }
            else
            {
                epsd = buf[0] - '0';
                map = buf[1] - '0';
                // added 3-1-98
                if (epsd < 1)
                    return false;
            }

            // Catch invalid maps.
            //added:08-01-98:moved if (epsd<1)...  up
            if (map < 1)
                return false;

            // Ohmygod - this is not going to work.
            if ((gamemode == retail) && ((epsd > 4) || (map > 9)))
                return false;

            if ((gamemode == registered) && ((epsd > 3) || (map > 9)))
                return false;

            if ((gamemode == shareware) && ((epsd > 1) || (map > 9)))
                return false;

            if ((gamemode == commercial) && ((epsd > 1) || (map > 34)))
                return false;

            // So be it.
            //plyr->message = STSTR_CLEV;
            msg = STSTR_CLEV;
            G_DeferedInitNew(gameskill, G_BuildMapName(epsd, map), false);
        }

        // append a newline to the original doom messages
        if (msg)
            CONS_Printf("%s\n", msg);
    }
    return false;
}

// command that can be typed at the console !

void Command_CheatNoClip_f(void)
{
    player_t *plyr;
    if (multiplayer)
        return;

    plyr = &players[consoleplayer];

    plyr->cheats ^= CF_NOCLIP;

    if (plyr->cheats & CF_NOCLIP)
        CONS_Printf(STSTR_NCON);
    else
        CONS_Printf(STSTR_NCOFF);

}

void Command_CheatGod_f(void)
{
    player_t *plyr;

    if (multiplayer)
        return;

    plyr = &players[consoleplayer];

    plyr->cheats ^= CF_GODMODE;
    if (plyr->cheats & CF_GODMODE)
    {
        if (plyr->mo)
            plyr->mo->health = god_health;

        plyr->health = god_health;
        CONS_Printf("%s\n", STSTR_DQDON);
    }
    else
        CONS_Printf("%s\n", STSTR_DQDOFF);
}

void Command_CheatGimme_f(void)
{
    char *s;
    int i, j, k;
    player_t *plyr;

    if (multiplayer && !(cv_splitscreen.value && server))
        return;

    if (COM_Argc() < 2)
    {
        CONS_Printf("gimme [health] [ammo] [armor] ...\n");
        return;
    }

    plyr = &players[consoleplayer];

    for (k = 0; k < (cv_splitscreen.value ? 2 : 1); k++)
    {
        if (k == 1)
        {
            plyr = &players[secondarydisplayplayer];
        }

    for (i = 1; i < COM_Argc(); i++)
    {
        s = COM_Argv(i);

        if (!strncmp(s, "health", 6))
        {
            if (plyr->mo)
                plyr->mo->health = god_health;

            plyr->health = god_health;

            CONS_Printf("got health\n");
        }
        else if (!strncmp(s, "ammo", 4))
        {
            for (j = 0; j < NUMAMMO; j++)
                plyr->ammo[j] = plyr->maxammo[j];

            CONS_Printf("got ammo\n");
        }
        else if (!strncmp(s, "armor", 5))
        {
            plyr->armorpoints = idfa_armor;
            plyr->armortype = idfa_armor_class;

            CONS_Printf("got armor\n");
        }
        else if (!strncmp(s, "keys", 4))
        {
            plyr->cards = it_allkeys;

            CONS_Printf("got keys\n");
        }
        else if (!strncmp(s, "weapons", 7))
        {
            for (j = 0; j < NUMWEAPONS; j++)
                plyr->weaponowned[j] = true;

            for (j = 0; j < NUMAMMO; j++)
                plyr->ammo[j] = plyr->maxammo[j];

            CONS_Printf("got weapons\n");
        }
        else
            //
            // WEAPONS
            //
        if (!strncmp(s, "chainsaw", 8))
        {
            plyr->weaponowned[wp_chainsaw] = true;

            CONS_Printf("got chainsaw\n");
        }
        else if (!strncmp(s, "shotgun", 7))
        {
            plyr->weaponowned[wp_shotgun] = true;
            plyr->ammo[am_shell] = plyr->maxammo[am_shell];

            CONS_Printf("got shotgun\n");
        }
        else if (!strncmp(s, "supershotgun", 12))
        {
            if (gamemode == commercial) // only in Doom2
            {
                plyr->weaponowned[wp_supershotgun] = true;
                plyr->ammo[am_shell] = plyr->maxammo[am_shell];

                CONS_Printf("got super shotgun\n");
            }
        }
        else if (!strncmp(s, "rocket", 6))
        {
            plyr->weaponowned[wp_missile] = true;
            plyr->ammo[am_misl] = plyr->maxammo[am_misl];

            CONS_Printf("got rocket launcher\n");
        }
        else if (!strncmp(s, "plasma", 6))
        {
            plyr->weaponowned[wp_plasma] = true;
            plyr->ammo[am_cell] = plyr->maxammo[am_cell];

            CONS_Printf("got plasma\n");
        }
        else if (!strncmp(s, "bfg", 3))
        {
            plyr->weaponowned[wp_bfg] = true;
            plyr->ammo[am_cell] = plyr->maxammo[am_cell];

            CONS_Printf("got bfg\n");
        }
        else if (!strncmp(s, "chaingun", 8))
        {
            plyr->weaponowned[wp_chaingun] = true;
            plyr->ammo[am_clip] = plyr->maxammo[am_clip];

            CONS_Printf("got chaingun\n");
        }
        else
            //
            // SPECIAL ITEMS
            //
        if (!strncmp(s, "berserk", 7))
        {
            if (!plyr->powers[pw_strength])
                P_GivePower(plyr, pw_strength);
            CONS_Printf("got berserk strength\n");
        }
        //22/08/99: added by Hurdler
        else if (!strncmp(s, "map", 3))
        {
            am_cheating = 1;
            CONS_Printf("got map\n");
        }
        //
        else if (!strncmp(s, "fullmap", 7))
        {
            am_cheating = 2;
            CONS_Printf("got map and things\n");
        }
        else
            CONS_Printf("can't give '%s' : unknown\n", s);

    }
}
}

// heretic cheat

#define CHEAT_ENCRYPT(a) SCRAMBLE(a)

typedef struct Cheat_s
{
    void (*func) (player_t * player, struct Cheat_s * cheat);
    byte *sequence;
    byte *pos;
    int args[2];
    int currentArg;
} Cheat_t;

static boolean CheatAddKey(Cheat_t * cheat, byte key, boolean * eat);
static void CheatGodFunc(player_t * player, Cheat_t * cheat);
static void CheatNoClipFunc(player_t * player, Cheat_t * cheat);
static void CheatWeaponsFunc(player_t * player, Cheat_t * cheat);
static void CheatPowerFunc(player_t * player, Cheat_t * cheat);
static void CheatHealthFunc(player_t * player, Cheat_t * cheat);
static void CheatKeysFunc(player_t * player, Cheat_t * cheat);
//static void CheatSoundFunc(player_t *player, Cheat_t *cheat);
static void CheatTickerFunc(player_t * player, Cheat_t * cheat);
static void CheatArtifact1Func(player_t * player, Cheat_t * cheat);
static void CheatArtifact2Func(player_t * player, Cheat_t * cheat);
static void CheatArtifact3Func(player_t * player, Cheat_t * cheat);
static void CheatWarpFunc(player_t * player, Cheat_t * cheat);
static void CheatChickenFunc(player_t * player, Cheat_t * cheat);
static void CheatMassacreFunc(player_t * player, Cheat_t * cheat);
static void CheatIDKFAFunc(player_t * player, Cheat_t * cheat);
static void CheatIDDQDFunc(player_t * player, Cheat_t * cheat);

// Toggle god mode
static byte CheatGodSeq[] = {
    CHEAT_ENCRYPT('q'),
    CHEAT_ENCRYPT('u'),
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('c'),
    CHEAT_ENCRYPT('k'),
    CHEAT_ENCRYPT('e'),
    CHEAT_ENCRYPT('n'),
    0xff
};

// Toggle no clipping mode
static byte CheatNoClipSeq[] = {
    CHEAT_ENCRYPT('k'),
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('t'),
    CHEAT_ENCRYPT('t'),
    CHEAT_ENCRYPT('y'),
    0xff
};

// Get all weapons and ammo
static byte CheatWeaponsSeq[] = {
    CHEAT_ENCRYPT('r'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('b'),
    CHEAT_ENCRYPT('o'),
    0xff
};

// Toggle tome of power
static byte CheatPowerSeq[] = {
    CHEAT_ENCRYPT('s'),
    CHEAT_ENCRYPT('h'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('z'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('m'),
    0xff, 0
};

// Get full health
static byte CheatHealthSeq[] = {
    CHEAT_ENCRYPT('p'),
    CHEAT_ENCRYPT('o'),
    CHEAT_ENCRYPT('n'),
    CHEAT_ENCRYPT('c'),
    CHEAT_ENCRYPT('e'),
    0xff
};

// Get all keys
static byte CheatKeysSeq[] = {
    CHEAT_ENCRYPT('s'),
    CHEAT_ENCRYPT('k'),
    CHEAT_ENCRYPT('e'),
    CHEAT_ENCRYPT('l'),
    0xff, 0
};

// Toggle ticker
static byte CheatTickerSeq[] = {
    CHEAT_ENCRYPT('t'),
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('c'),
    CHEAT_ENCRYPT('k'),
    CHEAT_ENCRYPT('e'),
    CHEAT_ENCRYPT('r'),
    0xff, 0
};

// Get an artifact 1st stage (ask for type)
static byte CheatArtifact1Seq[] = {
    CHEAT_ENCRYPT('g'),
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('e'),
    0xff
};

// Get an artifact 2nd stage (ask for count)
static byte CheatArtifact2Seq[] = {
    CHEAT_ENCRYPT('g'),
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('e'),
    0, 0xff, 0
};

// Get an artifact final stage
static byte CheatArtifact3Seq[] = {
    CHEAT_ENCRYPT('g'),
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('e'),
    0, 0, 0xff
};

// Warp to new level
static byte CheatWarpSeq[] = {
    CHEAT_ENCRYPT('e'),
    CHEAT_ENCRYPT('n'),
    CHEAT_ENCRYPT('g'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('g'),
    CHEAT_ENCRYPT('e'),
    0, 0, 0xff, 0
};

// Save a screenshot
static byte CheatChickenSeq[] = {
    CHEAT_ENCRYPT('c'),
    CHEAT_ENCRYPT('o'),
    CHEAT_ENCRYPT('c'),
    CHEAT_ENCRYPT('k'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('d'),
    CHEAT_ENCRYPT('o'),
    CHEAT_ENCRYPT('o'),
    CHEAT_ENCRYPT('d'),
    CHEAT_ENCRYPT('l'),
    CHEAT_ENCRYPT('e'),
    CHEAT_ENCRYPT('d'),
    CHEAT_ENCRYPT('o'),
    CHEAT_ENCRYPT('o'),
    0xff, 0
};

// Kill all monsters
static byte CheatMassacreSeq[] = {
    CHEAT_ENCRYPT('m'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('s'),
    CHEAT_ENCRYPT('s'),
    CHEAT_ENCRYPT('a'),
    CHEAT_ENCRYPT('c'),
    CHEAT_ENCRYPT('r'),
    CHEAT_ENCRYPT('e'),
    0xff, 0
};

static byte CheatIDKFASeq[] = {
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('d'),
    CHEAT_ENCRYPT('k'),
    CHEAT_ENCRYPT('f'),
    CHEAT_ENCRYPT('a'),
    0xff, 0
};

static byte CheatIDDQDSeq[] = {
    CHEAT_ENCRYPT('i'),
    CHEAT_ENCRYPT('d'),
    CHEAT_ENCRYPT('d'),
    CHEAT_ENCRYPT('q'),
    CHEAT_ENCRYPT('d'),
    0xff, 0
};

static Cheat_t Cheats[] = {
    {CheatGodFunc, CheatGodSeq, NULL, {0, 0}, 0},
    {CheatNoClipFunc, CheatNoClipSeq, NULL, {0, 0}, 0},
    {CheatWeaponsFunc, CheatWeaponsSeq, NULL, {0, 0}, 0},
    {CheatPowerFunc, CheatPowerSeq, NULL, {0, 0}, 0},
    {CheatHealthFunc, CheatHealthSeq, NULL, {0, 0}, 0},
    {CheatKeysFunc, CheatKeysSeq, NULL, {0, 0}, 0},
//      { CheatSoundFunc, CheatSoundSeq, NULL, 0, 0, 0 },
    {CheatTickerFunc, CheatTickerSeq, NULL, {0, 0}, 0},
    {CheatArtifact1Func, CheatArtifact1Seq, NULL, {0, 0}, 0},
    {CheatArtifact2Func, CheatArtifact2Seq, NULL, {0, 0}, 0},
    {CheatArtifact3Func, CheatArtifact3Seq, NULL, {0, 0}, 0},
    {CheatWarpFunc, CheatWarpSeq, NULL, {0, 0}, 0},
    {CheatChickenFunc, CheatChickenSeq, NULL, {0, 0}, 0},
    {CheatMassacreFunc, CheatMassacreSeq, NULL, {0, 0}, 0},
    {CheatIDKFAFunc, CheatIDKFASeq, NULL, {0, 0}, 0},
    {CheatIDDQDFunc, CheatIDDQDSeq, NULL, {0, 0}, 0},
    {NULL, NULL, NULL, {0, 0}, 0}       // Terminator
};

//--------------------------------------------------------------------------
//
// FUNC HandleCheats
//
// Returns true if the caller should eat the key.
//
//--------------------------------------------------------------------------

static boolean HandleCheats(byte key)
{
    int i;
    boolean eat;

    if (netgame || gameskill == sk_nightmare)
    {   // Can't cheat in a net-game, or in nightmare mode
        return (false);
    }
    if (players[consoleplayer].health <= 0)
    {   // Dead players can't cheat
        return (false);
    }
    eat = false;
    for (i = 0; Cheats[i].func != NULL; i++)
    {
        if (CheatAddKey(&Cheats[i], key, &eat))
        {
            Cheats[i].func(&players[consoleplayer], &Cheats[i]);
            S_StartSound(NULL, sfx_dorcls);
        }
    }
    return (eat);
}

//--------------------------------------------------------------------------
//
// FUNC CheatAddkey
//
// Returns true if the added key completed the cheat, false otherwise.
//
//--------------------------------------------------------------------------

static boolean CheatAddKey(Cheat_t * cheat, byte key, boolean * eat)
{
    if (!cheat->pos)
    {
        cheat->pos = cheat->sequence;
        cheat->currentArg = 0;
    }
    if (*cheat->pos == 0)
    {
        *eat = true;
        cheat->args[cheat->currentArg++] = key;
        cheat->pos++;
    }
    else if (cheat_xlate_table[key] == *cheat->pos)
    {
        cheat->pos++;
    }
    else
    {
        cheat->pos = cheat->sequence;
        cheat->currentArg = 0;
    }
    if (*cheat->pos == 0xff)
    {
        cheat->pos = cheat->sequence;
        cheat->currentArg = 0;
        return (true);
    }
    return (false);
}

//--------------------------------------------------------------------------
//
// CHEAT FUNCTIONS
//
//--------------------------------------------------------------------------

static void CheatGodFunc(player_t * player, Cheat_t * cheat)
{
    player->cheats ^= CF_GODMODE;
    if (player->cheats & CF_GODMODE)
    {
        P_SetMessage(player, TXT_CHEATGODON, false);
    }
    else
    {
        P_SetMessage(player, TXT_CHEATGODOFF, false);
    }
    st_recalc = true;
}

static void CheatNoClipFunc(player_t * player, Cheat_t * cheat)
{
    player->cheats ^= CF_NOCLIP;
    if (player->cheats & CF_NOCLIP)
    {
        P_SetMessage(player, TXT_CHEATNOCLIPON, false);
    }
    else
    {
        P_SetMessage(player, TXT_CHEATNOCLIPOFF, false);
    }
}

static void CheatWeaponsFunc(player_t * player, Cheat_t * cheat)
{
    int i;
    //extern boolean *WeaponInShareware;

    player->armorpoints = 200;
    player->armortype = 2;
    if (!player->backpack)
    {
        for (i = 0; i < NUMAMMO; i++)
        {
            player->maxammo[i] *= 2;
        }
        player->backpack = true;
    }
    for (i = 0; i < NUMWEAPONS - 1; i++)
    {
        player->weaponowned[i] = true;
    }
    if (shareware)
    {
        player->weaponowned[wp_skullrod] = false;
        player->weaponowned[wp_phoenixrod] = false;
        player->weaponowned[wp_mace] = false;
    }
    for (i = 0; i < NUMAMMO; i++)
    {
        player->ammo[i] = player->maxammo[i];
    }
    P_SetMessage(player, TXT_CHEATWEAPONS, false);
}

static void CheatPowerFunc(player_t * player, Cheat_t * cheat)
{
    if (player->powers[pw_weaponlevel2])
    {
        player->powers[pw_weaponlevel2] = 0;
        P_SetMessage(player, TXT_CHEATPOWEROFF, false);
    }
    else
    {
        P_UseArtifact(player, arti_tomeofpower);
        P_SetMessage(player, TXT_CHEATPOWERON, false);
    }
}

static void CheatHealthFunc(player_t * player, Cheat_t * cheat)
{
    if (player->chickenTics)
    {
        player->health = player->mo->health = MAXCHICKENHEALTH;
    }
    else
    {
        player->health = player->mo->health = MAXHEALTH;
    }
    P_SetMessage(player, TXT_CHEATHEALTH, false);
}

static void CheatKeysFunc(player_t * player, Cheat_t * cheat)
{
    extern int playerkeys;

    player->cards |= it_allkeys;
    playerkeys = 7;     // Key refresh flags
    P_SetMessage(player, TXT_CHEATKEYS, false);
}

static void CheatTickerFunc(player_t * player, Cheat_t * cheat)
{
    cv_ticrate.value = !cv_ticrate.value;
    if (cv_ticrate.value)
    {
        P_SetMessage(player, TXT_CHEATTICKERON, false);
    }
    else
    {
        P_SetMessage(player, TXT_CHEATTICKEROFF, false);
    }
}

static void CheatArtifact1Func(player_t * player, Cheat_t * cheat)
{
    P_SetMessage(player, TXT_CHEATARTIFACTS1, false);
}

static void CheatArtifact2Func(player_t * player, Cheat_t * cheat)
{
    P_SetMessage(player, TXT_CHEATARTIFACTS2, false);
}

static void CheatArtifact3Func(player_t * player, Cheat_t * cheat)
{
    int i;
    int j;
    artitype_t type;
    int count;

    type = cheat->args[0] - 'a' + 1;
    count = cheat->args[1] - '0';
    if (type == 26 && count == 0)
    {   // All artifacts
        for (i = arti_none + 1; i < NUMARTIFACTS; i++)
        {
            if (shareware && (i == arti_superhealth || i == arti_teleport))
            {
                continue;
            }
            for (j = 0; j < 16; j++)
            {
                P_GiveArtifact(player, i, NULL);
            }
        }
        P_SetMessage(player, TXT_CHEATARTIFACTS3, false);
    }
    else if (type > arti_none && type < NUMARTIFACTS && count > 0 && count < 10)
    {
        if (shareware && (type == arti_superhealth || type == arti_teleport))
        {
            P_SetMessage(player, TXT_CHEATARTIFACTSFAIL, false);
            return;
        }
        for (i = 0; i < count; i++)
        {
            P_GiveArtifact(player, type, NULL);
        }
        P_SetMessage(player, TXT_CHEATARTIFACTS3, false);
    }
    else
    {   // Bad input
        P_SetMessage(player, TXT_CHEATARTIFACTSFAIL, false);
    }
}

static void CheatWarpFunc(player_t * player, Cheat_t * cheat)
{
    int episode;
    int map;
    char *mapname;

    episode = cheat->args[0] - '0';
    map = cheat->args[1] - '0';
    mapname = G_BuildMapName(episode, map);
    if (W_CheckNumForName(mapname) > 0)
    {
        G_DeferedInitNew(gameskill, mapname, false);
        P_SetMessage(player, TXT_CHEATWARP, false);
    }
}

static void CheatChickenFunc(player_t * player, Cheat_t * cheat)
{
    extern boolean P_UndoPlayerChicken(player_t * player);

    if (player->chickenTics)
    {
        if (P_UndoPlayerChicken(player))
        {
            P_SetMessage(player, TXT_CHEATCHICKENOFF, false);
        }
    }
    else if (P_ChickenMorphPlayer(player))
    {
        P_SetMessage(player, TXT_CHEATCHICKENON, false);
    }
}

static void CheatMassacreFunc(player_t * player, Cheat_t * cheat)
{
    P_Massacre();
    P_SetMessage(player, TXT_CHEATMASSACRE, false);
}

static void CheatIDKFAFunc(player_t * player, Cheat_t * cheat)
{
    int i;
    if (player->chickenTics)
    {
        return;
    }
    for (i = 1; i < 8; i++)
    {
        player->weaponowned[i] = false;
    }
    player->pendingweapon = wp_staff;
    P_SetMessage(player, TXT_CHEATIDKFA, true);
}

static void CheatIDDQDFunc(player_t * player, Cheat_t * cheat)
{
    P_DamageMobj(player->mo, NULL, player->mo, 10000);
    P_SetMessage(player, TXT_CHEATIDDQD, true);
}
