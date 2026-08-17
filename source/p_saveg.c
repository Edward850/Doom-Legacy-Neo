// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: p_saveg.c,v 1.27 2003/08/11 13:50:01 hurdler Exp $
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
// $Log: p_saveg.c,v $
// Revision 1.27  2003/08/11 13:50:01  hurdler
// go final + translucent HUD + fix spawn in net game
//
// Revision 1.26  2003/07/21 11:33:57  hurdler
// go RC1
//
// Revision 1.25  2003/05/04 07:22:55  sburke
// Overlooked a LONG() swap when restoring the door->line of a door thinker.
//
// Revision 1.24  2003/05/04 02:36:18  sburke
// Make thorough use of READSHORT, READLONG macros to avoid endian, alignment faults.
//
// Revision 1.23  2002/09/09 20:41:53  uid22974
// Fix a bug with save/load game and FS
//
// Revision 1.22  2002/06/23 17:38:21  ssntails
// Fix for savegames with 3dfloors!
//
// Revision 1.21  2002/01/21 23:14:28  judgecutor
// Frag's Weapon Falling fixes
//
// Revision 1.20  2001/06/16 08:07:55  bpereira
// no message
//
// Revision 1.19  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.18  2001/03/03 06:17:33  bpereira
// no message
//
// Revision 1.17  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.16  2001/02/10 12:27:14  bpereira
// no message
//
// Revision 1.15  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.14  2000/11/11 13:59:45  bpereira
// no message
//
// Revision 1.13  2000/11/04 16:23:43  bpereira
// no message
//
// Revision 1.12  2000/11/02 17:50:08  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.11  2000/09/28 20:57:16  bpereira
// no message
//
// Revision 1.10  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.9  2000/07/01 09:23:49  bpereira
// no message
//
// Revision 1.8  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.7  2000/04/15 22:12:57  stroggonmeth
// Minor bug fixes
//
// Revision 1.6  2000/04/11 19:07:24  stroggonmeth
// Finished my logs, fixed a crashing bug.
//
// Revision 1.5  2000/04/04 00:32:47  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.4  2000/02/27 16:30:28  hurdler
// dead player bug fix + add allowmlook <yes|no>
//
// Revision 1.3  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      Archiving: SaveGame I/O.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "g_game.h"
#include "p_local.h"
#include "r_state.h"
#include "z_zone.h"
#include "w_wad.h"
#include "p_setup.h"
#include "byteptr.h"
#include "t_vari.h"
#include "t_script.h"
#include "t_func.h"
#include "m_random.h"

byte *save_p;

// Pads save_p to a 4-byte boundary
//  so that the load/save works on SGI&Gecko.
#ifdef SGI
// BP: this stuff isn't be removed but i think it will no more work
//     anyway what processor can't read/write unaligned data ?
#define PADSAVEP()      save_p += (4 - ((int) save_p & 3)) & 3
#else
#define PADSAVEP()
#endif

// BP: damned this #if don't work ! why ?
#if NUMWEAPONS > 8
#error please update the player_saveflags enum
#endif

typedef enum
{
    // weapons   = 0x01ff,
    BACKPACK = 0x0200,
    ORIGNWEAP = 0x0400,
    AUTOAIM = 0x0800,
    ATTACKDWN = 0x1000,
    USEDWN = 0x2000,
    JMPDWN = 0x4000,
    DIDSECRET = 0x8000,
} player_saveflags;

typedef enum
{
    // powers      = 0x00ff
    PD_REFIRE = 0x0100,
    PD_KILLCOUNT = 0x0200,
    PD_ITEMCOUNT = 0x0400,
    PD_SECRETCOUNT = 0x0800,
    PD_DAMAGECOUNT = 0x1000,
    PD_BONUSCOUNT = 0x2000,
    PD_CHICKENTICS = 0x4000,
    PD_CHICKEPECK = 0x8000,
    PD_FLAMECOUNT = 0x10000,
    PD_FLYHEIGHT = 0x20000,
} player_diff;

//
// P_ArchivePlayers
//
void P_ArchivePlayers(void)
{
    int i, j;
    int flags;
    ULONG diff;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (!playeringame[i])
            continue;

        PADSAVEP();

        flags = 0;
        diff = 0;
        for (j = 0; j < NUMPOWERS; j++)
            if (players[i].powers[j])
                diff |= 1 << j;
        if (players[i].refire)
            diff |= PD_REFIRE;
        if (players[i].killcount)
            diff |= PD_KILLCOUNT;
        if (players[i].itemcount)
            diff |= PD_ITEMCOUNT;
        if (players[i].secretcount)
            diff |= PD_SECRETCOUNT;
        if (players[i].damagecount)
            diff |= PD_DAMAGECOUNT;
        if (players[i].bonuscount)
            diff |= PD_BONUSCOUNT;
        if (players[i].chickenTics)
            diff |= PD_CHICKENTICS;
        if (players[i].chickenPeck)
            diff |= PD_CHICKEPECK;
        if (players[i].flamecount)
            diff |= PD_FLAMECOUNT;
        if (players[i].flyheight)
            diff |= PD_FLYHEIGHT;

        WRITEULONG(save_p, diff);

        WRITEANGLE(save_p, players[i].aiming);
        WRITEUSHORT(save_p, players[i].health);
        WRITEUSHORT(save_p, players[i].armorpoints);
        WRITEBYTE(save_p, players[i].armortype);

        for (j = 0; j < NUMPOWERS; j++)
            if (diff & (1 << j))
                WRITELONG(save_p, players[i].powers[j]);
        WRITEBYTE(save_p, players[i].cards);
        WRITEBYTE(save_p, players[i].readyweapon);
        WRITEBYTE(save_p, players[i].pendingweapon);
        WRITEBYTE(save_p, players[i].playerstate);

        WRITEUSHORT(save_p, players[i].addfrags);
        for (j = 0; j < MAXPLAYERS; j++)
            if (playeringame[i])
                WRITEUSHORT(save_p, players[i].frags[j]);

        for (j = 0; j < NUMWEAPONS; j++)
        {
            WRITEBYTE(save_p, players[i].favoritweapon[j]);
            if (players[i].weaponowned[j])
                flags |= 1 << j;
        }
        for (j = 0; j < NUMAMMO; j++)
        {
            WRITEUSHORT(save_p, players[i].ammo[j]);
            WRITEUSHORT(save_p, players[i].maxammo[j]);
        }
        if (players[i].backpack)
            flags |= BACKPACK;
        if (players[i].originalweaponswitch)
            flags |= ORIGNWEAP;
        if (players[i].autoaim_toggle)
            flags |= AUTOAIM;
        if (players[i].attackdown)
            flags |= ATTACKDWN;
        if (players[i].usedown)
            flags |= USEDWN;
        if (players[i].jumpdown)
            flags |= JMPDWN;
        if (players[i].didsecret)
            flags |= DIDSECRET;

        if (diff & PD_REFIRE)
            WRITELONG(save_p, players[i].refire);
        if (diff & PD_KILLCOUNT)
            WRITELONG(save_p, players[i].killcount);
        if (diff & PD_ITEMCOUNT)
            WRITELONG(save_p, players[i].itemcount);
        if (diff & PD_SECRETCOUNT)
            WRITELONG(save_p, players[i].secretcount);
        if (diff & PD_DAMAGECOUNT)
            WRITELONG(save_p, players[i].damagecount);
        if (diff & PD_BONUSCOUNT)
            WRITELONG(save_p, players[i].bonuscount);
        if (diff & PD_CHICKENTICS)
            WRITELONG(save_p, players[i].chickenTics);
        if (diff & PD_CHICKEPECK)
            WRITELONG(save_p, players[i].chickenPeck);
        if (diff & PD_FLAMECOUNT)
            WRITELONG(save_p, players[i].flamecount);
        if (diff & PD_FLYHEIGHT)
            WRITELONG(save_p, players[i].flyheight);

        WRITEBYTE(save_p, players[i].skincolor);

        for (j = 0; j < NUMPSPRITES; j++)
        {
            if (players[i].psprites[j].state)
                WRITEUSHORT(save_p, (players[i].psprites[j].state - states) + 1);
            else
                WRITEUSHORT(save_p, 0);
            WRITELONG(save_p, players[i].psprites[j].tics);
            WRITEFIXED(save_p, players[i].psprites[j].sx);
            WRITEFIXED(save_p, players[i].psprites[j].sy);
        }
        WRITEUSHORT(save_p, flags);

        if (inventory)
        {
            WRITEBYTE(save_p, players[i].inventorySlotNum);
            for (j = 0; j < players[i].inventorySlotNum; j++)
            {
                WRITEMEM(save_p, &players[i].inventory[j], sizeof(players[i].inventory[j]));
            }
        }
    }
}

//
// P_UnArchivePlayers
//
void P_UnArchivePlayers(void)
{
    int i, j;
    int flags;
    ULONG diff;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        memset(&players[i], 0, sizeof(player_t));
        if (!playeringame[i])
            continue;

        PADSAVEP();
        diff = READULONG(save_p);

        players[i].aiming = READANGLE(save_p);
        players[i].health = READUSHORT(save_p);
        players[i].armorpoints = READUSHORT(save_p);
        players[i].armortype = READBYTE(save_p);

        for (j = 0; j < NUMPOWERS; j++)
            if (diff & (1 << j))
                players[i].powers[j] = READLONG(save_p);

        players[i].cards = READBYTE(save_p);
        players[i].readyweapon = READBYTE(save_p);
        players[i].pendingweapon = READBYTE(save_p);
        players[i].playerstate = READBYTE(save_p);

        players[i].addfrags = READUSHORT(save_p);
        for (j = 0; j < MAXPLAYERS; j++)
            if (playeringame[i])
                players[i].frags[j] = READUSHORT(save_p);

        for (j = 0; j < NUMWEAPONS; j++)
            players[i].favoritweapon[j] = READBYTE(save_p);
        for (j = 0; j < NUMAMMO; j++)
        {
            players[i].ammo[j] = READUSHORT(save_p);
            players[i].maxammo[j] = READUSHORT(save_p);
        }
        if (diff & PD_REFIRE)
            players[i].refire = READLONG(save_p);
        if (diff & PD_KILLCOUNT)
            players[i].killcount = READLONG(save_p);
        if (diff & PD_ITEMCOUNT)
            players[i].itemcount = READLONG(save_p);
        if (diff & PD_SECRETCOUNT)
            players[i].secretcount = READLONG(save_p);
        if (diff & PD_DAMAGECOUNT)
            players[i].damagecount = READLONG(save_p);
        if (diff & PD_BONUSCOUNT)
            players[i].bonuscount = READLONG(save_p);
        if (diff & PD_CHICKENTICS)
            players[i].chickenTics = READLONG(save_p);
        if (diff & PD_CHICKEPECK)
            players[i].chickenPeck = READLONG(save_p);
        if (diff & PD_FLAMECOUNT)
            players[i].flamecount = READLONG(save_p);
        if (diff & PD_FLYHEIGHT)
            players[i].flyheight = READLONG(save_p);

        players[i].skincolor = READBYTE(save_p);

        for (j = 0; j < NUMPSPRITES; j++)
        {
            flags = READUSHORT(save_p);
            if (flags)
                players[i].psprites[j].state = &states[flags - 1];

            players[i].psprites[j].tics = READLONG(save_p);
            players[i].psprites[j].sx = READFIXED(save_p);
            players[i].psprites[j].sy = READFIXED(save_p);
        }

        flags = READUSHORT(save_p);

        if (inventory)
        {
            players[i].inventorySlotNum = READBYTE(save_p);
            for (j = 0; j < players[i].inventorySlotNum; j++)
            {
                READMEM(save_p, &players[i].inventory[j], sizeof(players[i].inventory[j]));
            }
        }

        for (j = 0; j < NUMWEAPONS; j++)
            players[i].weaponowned[j] = (flags & (1 << j)) != 0;

        players[i].backpack = (flags & BACKPACK) != 0;
        players[i].originalweaponswitch = (flags & ORIGNWEAP) != 0;
        players[i].autoaim_toggle = (flags & AUTOAIM) != 0;
        players[i].attackdown = (flags & ATTACKDWN) != 0;
        players[i].usedown = (flags & USEDWN) != 0;
        players[i].jumpdown = (flags & JMPDWN) != 0;
        players[i].didsecret = (flags & DIDSECRET) != 0;

        players[i].viewheight = cv_viewheight.value << FRACBITS;
        if (gamemode == heretic)
        {
            if (players[i].powers[pw_weaponlevel2])
                players[i].weaponinfo = wpnlev2info;
            else
                players[i].weaponinfo = wpnlev1info;
        }
        else
            players[i].weaponinfo = doomweaponinfo;
    }
}

#define SD_FLOORHT     0x01
#define SD_CEILHT      0x02
#define SD_FLOORPIC    0x04
#define SD_CEILPIC     0x08
#define SD_LIGHT       0x10
#define SD_SPECIAL     0x20
#define SD_DIFF2       0x40

//SoM: 4/10/2000: Fix sector related savegame bugs
// diff2 flags
#define SD_FXOFFS     0x01
#define SD_FYOFFS     0x02
#define SD_CXOFFS     0x04
#define SD_CYOFFS     0x08
#define SD_STAIRLOCK  0x10
#define SD_PREVSEC    0x20
#define SD_NEXTSEC    0x40

#define LD_FLAG     0x01
#define LD_SPECIAL  0x02
//#define LD_TAG      0x04
#define LD_S1TEXOFF 0x08
#define LD_S1TOPTEX 0x10
#define LD_S1BOTTEX 0x20
#define LD_S1MIDTEX 0x40
#define LD_DIFF2    0x80

// diff2 flags
#define LD_S2TEXOFF 0x01
#define LD_S2TOPTEX 0x02
#define LD_S2BOTTEX 0x04
#define LD_S2MIDTEX 0x08

//
// P_ArchiveWorld
//
void P_ArchiveWorld(void)
{
    int i;
    int statsec = 0, statline = 0;
    line_t *li;
    side_t *si;
    byte *put;

    // reload the map just to see difference
    mapsector_t *ms;
    mapsidedef_t *msd;
    maplinedef_t *mld;
    sector_t *ss;
    byte diff;
    byte diff2;

    ms = W_CacheLumpNum(lastloadedmaplumpnum + ML_SECTORS, PU_CACHE);
    ss = sectors;
    put = save_p;

    for (i = 0; i < numsectors; i++, ss++, ms++)
    {
        diff = 0;
        diff2 = 0;
        if (ss->floorheight != SHORT(ms->floorheight) << FRACBITS)
            diff |= SD_FLOORHT;
        if (ss->ceilingheight != SHORT(ms->ceilingheight) << FRACBITS)
            diff |= SD_CEILHT;
        //
        //  flats
        //
        // P_AddLevelFlat should not add but just return the number
        if (ss->floorpic != P_AddLevelFlat(ms->floorpic, levelflats))
            diff |= SD_FLOORPIC;
        if (ss->ceilingpic != P_AddLevelFlat(ms->ceilingpic, levelflats))
            diff |= SD_CEILPIC;

        if (ss->lightlevel != SHORT(ms->lightlevel))
            diff |= SD_LIGHT;
        if (ss->special != SHORT(ms->special))
            diff |= SD_SPECIAL;

        if (ss->floor_xoffs != 0)
            diff2 |= SD_FXOFFS;
        if (ss->floor_yoffs != 0)
            diff2 |= SD_FYOFFS;
        if (ss->ceiling_xoffs != 0)
            diff2 |= SD_CXOFFS;
        if (ss->ceiling_yoffs != 0)
            diff2 |= SD_CYOFFS;
        if (ss->stairlock < 0)
            diff2 |= SD_STAIRLOCK;
        if (ss->nextsec != -1)
            diff2 |= SD_NEXTSEC;
        if (ss->prevsec != -1)
            diff2 |= SD_PREVSEC;
        if (diff2)
            diff |= SD_DIFF2;

        if (diff)
        {
            statsec++;

            WRITESHORT(put, i);
            WRITEBYTE(put, diff);
            if (diff & SD_DIFF2)
                WRITEBYTE(put, diff2);
            if (diff & SD_FLOORHT)
                WRITEFIXED(put, ss->floorheight);
            if (diff & SD_CEILHT)
                WRITEFIXED(put, ss->ceilingheight);
            if (diff & SD_FLOORPIC)
            {
                memcpy(put, levelflats[ss->floorpic].name, 8);
                put += 8;
            }
            if (diff & SD_CEILPIC)
            {
                memcpy(put, levelflats[ss->ceilingpic].name, 8);
                put += 8;
            }
            if (diff & SD_LIGHT)
                WRITESHORT(put, (short) ss->lightlevel);
            if (diff & SD_SPECIAL)
                WRITESHORT(put, (short) ss->special);

            if (diff2 & SD_FXOFFS)
                WRITEFIXED(put, ss->floor_xoffs);
            if (diff2 & SD_FYOFFS)
                WRITEFIXED(put, ss->floor_yoffs);
            if (diff2 & SD_CXOFFS)
                WRITEFIXED(put, ss->ceiling_xoffs);
            if (diff2 & SD_CYOFFS)
                WRITEFIXED(put, ss->ceiling_yoffs);
            if (diff2 & SD_STAIRLOCK)
                WRITELONG(put, ss->stairlock);
            if (diff2 & SD_NEXTSEC)
                WRITELONG(put, ss->nextsec);
            if (diff2 & SD_PREVSEC)
                WRITELONG(put, ss->prevsec);
        }
    }
    WRITEUSHORT(put, 0xffff);

    mld = W_CacheLumpNum(lastloadedmaplumpnum + ML_LINEDEFS, PU_CACHE);
    msd = W_CacheLumpNum(lastloadedmaplumpnum + ML_SIDEDEFS, PU_CACHE);
    li = lines;
    // do lines
    for (i = 0; i < numlines; i++, mld++, li++)
    {
        diff = 0;
        diff2 = 0;

        // we don't care of map in deathmatch !
        if (((cv_deathmatch.value == 0) && (li->flags != SHORT(mld->flags))) || ((cv_deathmatch.value != 0) && ((li->flags & ~ML_MAPPED) != SHORT(mld->flags))))
            diff |= LD_FLAG;
        if (li->special != SHORT(mld->special))
            diff |= LD_SPECIAL;

        if (li->sidenum[0] != -1)
        {
            si = &sides[li->sidenum[0]];
            if (si->textureoffset != SHORT(msd[li->sidenum[0]].textureoffset) << FRACBITS)
                diff |= LD_S1TEXOFF;
            //SoM: 4/1/2000: Some textures are colormaps. Don't worry about invalid textures.
            if (R_CheckTextureNumForName(msd[li->sidenum[0]].toptexture) != -1)
                if (si->toptexture != R_TextureNumForName(msd[li->sidenum[0]].toptexture))
                    diff |= LD_S1TOPTEX;
            if (R_CheckTextureNumForName(msd[li->sidenum[0]].bottomtexture) != -1)
                if (si->bottomtexture != R_TextureNumForName(msd[li->sidenum[0]].bottomtexture))
                    diff |= LD_S1BOTTEX;
            if (R_CheckTextureNumForName(msd[li->sidenum[0]].midtexture) != -1)
                if (si->midtexture != R_TextureNumForName(msd[li->sidenum[0]].midtexture))
                    diff |= LD_S1MIDTEX;
        }
        if (li->sidenum[1] != -1)
        {
            si = &sides[li->sidenum[1]];
            if (si->textureoffset != SHORT(msd[li->sidenum[1]].textureoffset) << FRACBITS)
                diff2 |= LD_S2TEXOFF;
            if (R_CheckTextureNumForName(msd[li->sidenum[1]].toptexture) != -1)
                if (si->toptexture != R_TextureNumForName(msd[li->sidenum[1]].toptexture))
                    diff2 |= LD_S2TOPTEX;
            if (R_CheckTextureNumForName(msd[li->sidenum[1]].bottomtexture) != -1)
                if (si->bottomtexture != R_TextureNumForName(msd[li->sidenum[1]].bottomtexture))
                    diff2 |= LD_S2BOTTEX;
            if (R_CheckTextureNumForName(msd[li->sidenum[1]].midtexture) != -1)
                if (si->midtexture != R_TextureNumForName(msd[li->sidenum[1]].midtexture))
                    diff2 |= LD_S2MIDTEX;
            if (diff2)
                diff |= LD_DIFF2;

        }

        if (diff)
        {
            statline++;
            WRITESHORT(put, (short) i);
            WRITEBYTE(put, diff);
            if (diff & LD_DIFF2)
                WRITEBYTE(put, diff2);
            if (diff & LD_FLAG)
                WRITESHORT(put, li->flags);
            if (diff & LD_SPECIAL)
                WRITESHORT(put, li->special);

            si = &sides[li->sidenum[0]];
            if (diff & LD_S1TEXOFF)
                WRITEFIXED(put, si->textureoffset);
            if (diff & LD_S1TOPTEX)
                WRITESHORT(put, si->toptexture);
            if (diff & LD_S1BOTTEX)
                WRITESHORT(put, si->bottomtexture);
            if (diff & LD_S1MIDTEX)
                WRITESHORT(put, si->midtexture);

            si = &sides[li->sidenum[1]];
            if (diff2 & LD_S2TEXOFF)
                WRITEFIXED(put, si->textureoffset);
            if (diff2 & LD_S2TOPTEX)
                WRITESHORT(put, si->toptexture);
            if (diff2 & LD_S2BOTTEX)
                WRITESHORT(put, si->bottomtexture);
            if (diff2 & LD_S2MIDTEX)
                WRITESHORT(put, si->midtexture);
        }
    }
    WRITEUSHORT(put, 0xffff);

    //CONS_Printf("sector saved %d/%d, line saved %d/%d\n",statsec,numsectors,statline,numlines);
    save_p = put;
}

//
// P_UnArchiveWorld
//
void P_UnArchiveWorld(void)
{
    int i;
    line_t *li;
    side_t *si;
    byte *get;
    byte diff, diff2;

    get = save_p;

    while (1)
    {
        i = READUSHORT(get);

        if (i == 0xffff)
            break;

        diff = READBYTE(get);
        if (diff & SD_DIFF2)
            diff2 = READBYTE(get);
        else
            diff2 = 0;
        if (diff & SD_FLOORHT)
            sectors[i].floorheight = READFIXED(get);
        if (diff & SD_CEILHT)
            sectors[i].ceilingheight = READFIXED(get);
        if (diff & SD_FLOORPIC)
        {
            sectors[i].floorpic = P_AddLevelFlat(get, levelflats);
            get += 8;
        }
        if (diff & SD_CEILPIC)
        {
            sectors[i].ceilingpic = P_AddLevelFlat(get, levelflats);
            get += 8;
        }
        if (diff & SD_LIGHT)
            sectors[i].lightlevel = READSHORT(get);
        if (diff & SD_SPECIAL)
            sectors[i].special = READSHORT(get);

        if (diff2 & SD_FXOFFS)
            sectors[i].floor_xoffs = READFIXED(get);
        if (diff2 & SD_FYOFFS)
            sectors[i].floor_yoffs = READFIXED(get);
        if (diff2 & SD_CXOFFS)
            sectors[i].ceiling_xoffs = READFIXED(get);
        if (diff2 & SD_CYOFFS)
            sectors[i].ceiling_yoffs = READFIXED(get);
        if (diff2 & SD_STAIRLOCK)
            sectors[i].stairlock = READLONG(get);
        else
            sectors[i].stairlock = 0;
        if (diff2 & SD_NEXTSEC)
            sectors[i].nextsec = READLONG(get);
        else
            sectors[i].nextsec = -1;
        if (diff2 & SD_PREVSEC)
            sectors[i].prevsec = READLONG(get);
        else
            sectors[i].prevsec = -1;
    }

    while (1)
    {
        i = READUSHORT(get);

        if (i == 0xffff)
            break;
        diff = READBYTE(get);
        li = &lines[i];

        if (diff & LD_DIFF2)
            diff2 = READBYTE(get);
        else
            diff2 = 0;
        if (diff & LD_FLAG)
            li->flags = READSHORT(get);
        if (diff & LD_SPECIAL)
            li->special = READSHORT(get);

        si = &sides[li->sidenum[0]];
        if (diff & LD_S1TEXOFF)
            si->textureoffset = READFIXED(get);
        if (diff & LD_S1TOPTEX)
            si->toptexture = READSHORT(get);
        if (diff & LD_S1BOTTEX)
            si->bottomtexture = READSHORT(get);
        if (diff & LD_S1MIDTEX)
            si->midtexture = READSHORT(get);

        si = &sides[li->sidenum[1]];
        if (diff2 & LD_S2TEXOFF)
            si->textureoffset = READFIXED(get);
        if (diff2 & LD_S2TOPTEX)
            si->toptexture = READSHORT(get);
        if (diff2 & LD_S2BOTTEX)
            si->bottomtexture = READSHORT(get);
        if (diff2 & LD_S2MIDTEX)
            si->midtexture = READSHORT(get);
    }

    save_p = get;
}

//
// Thinkers
//

typedef enum
{
    MD_SPAWNPOINT = 0x000001,
    MD_POS = 0x000002,
    MD_TYPE = 0x000004,
// Eliminated MD_Z to prevent 3dfloor hiccups SSNTails 03-17-2002
    MD_MOM = 0x000008,
    MD_RADIUS = 0x000010,
    MD_HEIGHT = 0x000020,
    MD_FLAGS = 0x000040,
    MD_HEALTH = 0x000080,
    MD_RTIME = 0x000100,
    MD_STATE = 0x000200,
    MD_TICS = 0x000400,
    MD_SPRITE = 0x000800,
    MD_FRAME = 0x001000,
    MD_EFLAGS = 0x002000,
    MD_PLAYER = 0x004000,
    MD_MOVEDIR = 0x008000,
    MD_MOVECOUNT = 0x010000,
    MD_THRESHOLD = 0x020000,
    MD_LASTLOOK = 0x040000,
    MD_TARGET = 0x080000,
    MD_TRACER = 0x100000,
    MD_FRICTION = 0x200000,
    MD_MOVEFACTOR = 0x400000,
    MD_FLAGS2 = 0x800000,
    MD_SPECIAL1 = 0x1000000,
    MD_SPECIAL2 = 0x2000000,
    MD_AMMO = 0x4000000,
} mobj_diff_t;

enum
{
    tc_mobj,
    tc_ceiling,
    tc_door,
    tc_floor,
    tc_plat,
    tc_flash,
    tc_strobe,
    tc_glow,
    tc_fireflicker,
    tc_elevator,                //SoM: 3/15/2000: Add extra boom types.
    tc_scroll,
    tc_friction,
    tc_pusher,
    tc_end
} specials_e;

//
// P_ArchiveThinkers
//
//
// Things to handle:
//
// P_MobjsThinker (all mobj)
// T_MoveCeiling, (ceiling_t: sector_t * swizzle), - active list
// T_VerticalDoor, (vldoor_t: sector_t * swizzle),
// T_MoveFloor, (floormove_t: sector_t * swizzle),
// T_LightFlash, (lightflash_t: sector_t * swizzle),
// T_StrobeFlash, (strobe_t: sector_t *),
// T_Glow, (glow_t: sector_t *),
// T_PlatRaise, (plat_t: sector_t *), - active list
// BP: added missing : T_FireFlicker
//
void P_ArchiveThinkers(void)
{
    thinker_t *th;
    mobj_t *mobj;
    ULONG diff;
//    int                 i; //SoM: 3/16/2000: Removed. Not used any more.

    // save off the current thinkers
    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 == (actionf_p1) P_MobjThinker)
        {
            mobj = (mobj_t *) th;
/*
            // not a monster nor a picable item so don't save it
            if( (((mobj->flags & (MF_COUNTKILL | MF_PICKUP | MF_SHOOTABLE )) == 0)
                 && (mobj->flags & MF_MISSILE)
                 && (mobj->info->doomednum !=-1) )
                || (mobj->type == MT_BLOOD) )
                continue;
*/
            if (mobj->spawnpoint && (!(mobj->spawnpoint->options & MTF_FS_SPAWNED)) && (mobj->info->doomednum != -1))
            {
                // spawnpoint is not moddified but we must save it since it is a indentifier
                diff = MD_SPAWNPOINT;

                if ((mobj->x != mobj->spawnpoint->x << FRACBITS) || (mobj->y != mobj->spawnpoint->y << FRACBITS) || (mobj->angle != (unsigned) (ANG45 * (mobj->spawnpoint->angle / 45))))
                    diff |= MD_POS;
                if (mobj->info->doomednum != mobj->spawnpoint->type)
                    diff |= MD_TYPE;
            }
            else
            {
                // not a map spawned thing so make it from scratch
                diff = MD_POS | MD_TYPE;
            }

            // not the default but the most probable
            if ((mobj->momx != 0) || (mobj->momy != 0) || (mobj->momz != 0))
                diff |= MD_MOM;
            if (mobj->radius != mobj->info->radius)
                diff |= MD_RADIUS;
            if (mobj->height != mobj->info->height)
                diff |= MD_HEIGHT;
            if (mobj->flags != mobj->info->flags)
                diff |= MD_FLAGS;
            if (mobj->flags2 != mobj->info->flags2)
                diff |= MD_FLAGS2;
            if (mobj->health != mobj->info->spawnhealth)
                diff |= MD_HEALTH;
            if (mobj->reactiontime != mobj->info->reactiontime)
                diff |= MD_RTIME;
            if (mobj->state - states != mobj->info->spawnstate)
                diff |= MD_STATE;
            if (mobj->tics != mobj->state->tics)
                diff |= MD_TICS;
            if (mobj->sprite != mobj->state->sprite)
                diff |= MD_SPRITE;
            if (mobj->frame != mobj->state->frame)
                diff |= MD_FRAME;
            if (mobj->eflags)
                diff |= MD_EFLAGS;
            if (mobj->player)
                diff |= MD_PLAYER;

            if (mobj->movedir)
                diff |= MD_MOVEDIR;
            if (mobj->movecount)
                diff |= MD_MOVECOUNT;
            if (mobj->threshold)
                diff |= MD_THRESHOLD;
            if (mobj->lastlook != -1)
                diff |= MD_LASTLOOK;
            if (mobj->target)
                diff |= MD_TARGET;
            if (mobj->tracer)
                diff |= MD_TRACER;
            if (mobj->friction != ORIG_FRICTION)
                diff |= MD_FRICTION;
            if (mobj->movefactor != ORIG_FRICTION_FACTOR)
                diff |= MD_MOVEFACTOR;
            if (mobj->special1)
                diff |= MD_SPECIAL1;
            if (mobj->special2)
                diff |= MD_SPECIAL2;
            if (mobj->dropped_ammo_count)
                diff |= MD_AMMO;

            PADSAVEP();
            WRITEBYTE(save_p, tc_mobj);
            WRITEULONG(save_p, diff);
            // save pointer, at load time we will search this pointer to reinitilize pointers
            WRITEULONG(save_p, (ULONG) mobj);

            WRITEFIXED(save_p, mobj->z);        // Force this so 3dfloor problems don't arise. SSNTails 03-17-2002
            WRITEFIXED(save_p, mobj->floorz);

            if (diff & MD_SPAWNPOINT)
                WRITESHORT(save_p, mobj->spawnpoint - mapthings);
            if (diff & MD_TYPE)
                WRITEULONG(save_p, mobj->type);
            if (diff & MD_POS)
            {
                WRITEFIXED(save_p, mobj->x);
                WRITEFIXED(save_p, mobj->y);
                WRITEANGLE(save_p, mobj->angle);
            }
            if (diff & MD_MOM)
            {
                WRITEFIXED(save_p, mobj->momx);
                WRITEFIXED(save_p, mobj->momy);
                WRITEFIXED(save_p, mobj->momz);
            }
            if (diff & MD_RADIUS)
                WRITEFIXED(save_p, mobj->radius);
            if (diff & MD_HEIGHT)
                WRITEFIXED(save_p, mobj->height);
            if (diff & MD_FLAGS)
                WRITELONG(save_p, mobj->flags);
            if (diff & MD_FLAGS2)
                WRITELONG(save_p, mobj->flags2);
            if (diff & MD_HEALTH)
                WRITELONG(save_p, mobj->health);
            if (diff & MD_RTIME)
                WRITELONG(save_p, mobj->reactiontime);
            if (diff & MD_STATE)
                WRITEUSHORT(save_p, mobj->state - states);
            if (diff & MD_TICS)
                WRITELONG(save_p, mobj->tics);
            if (diff & MD_SPRITE)
                WRITEUSHORT(save_p, mobj->sprite);
            if (diff & MD_FRAME)
                WRITEULONG(save_p, mobj->frame);
            if (diff & MD_EFLAGS)
                WRITEULONG(save_p, mobj->eflags);
            if (diff & MD_PLAYER)
                WRITEBYTE(save_p, mobj->player - players);
            if (diff & MD_MOVEDIR)
                WRITELONG(save_p, mobj->movedir);
            if (diff & MD_MOVECOUNT)
                WRITELONG(save_p, mobj->movecount);
            if (diff & MD_THRESHOLD)
                WRITELONG(save_p, mobj->threshold);
            if (diff & MD_LASTLOOK)
                WRITELONG(save_p, mobj->lastlook);
            if (diff & MD_TARGET)
                WRITEULONG(save_p, (ULONG) mobj->target);
            if (diff & MD_TRACER)
                WRITEULONG(save_p, (ULONG) mobj->tracer);
            if (diff & MD_FRICTION)
                WRITELONG(save_p, mobj->friction);
            if (diff & MD_MOVEFACTOR)
                WRITELONG(save_p, mobj->movefactor);
            if (diff & MD_SPECIAL1)
                WRITELONG(save_p, mobj->special1);
            if (diff & MD_SPECIAL2)
                WRITELONG(save_p, mobj->special2);
            if (diff & MD_AMMO)
                WRITELONG(save_p, mobj->dropped_ammo_count);
        }
        else if (th->function.acv == (actionf_v) NULL)
        {
            //SoM: 3/15/2000: Boom stuff...
            ceilinglist_t *cl;

            for (cl = activeceilings; cl; cl = cl->next)
                if (cl->ceiling == (ceiling_t *) th)
                {
                    ceiling_t *ceiling;
                    WRITEBYTE(save_p, tc_ceiling);
                    PADSAVEP();
                    ceiling = (ceiling_t *) save_p;
                    memcpy(save_p, th, sizeof(*ceiling));
                    save_p += sizeof(*ceiling);
                    writelong(&ceiling->sector, ((ceiling_t *) th)->sector - sectors);
                }

            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_MoveCeiling)
        {
            ceiling_t *ceiling;
            WRITEBYTE(save_p, tc_ceiling);
            PADSAVEP();
            ceiling = (ceiling_t *) save_p;
            memcpy(save_p, th, sizeof(*ceiling));
            save_p += sizeof(*ceiling);
            writelong(&ceiling->sector, ((ceiling_t *) th)->sector - sectors);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_VerticalDoor)
        {
            vldoor_t *door;
            WRITEBYTE(save_p, tc_door);
            PADSAVEP();
            door = (vldoor_t *) save_p;
            memcpy(save_p, th, sizeof(*door));
            save_p += sizeof(*door);
            writelong(&door->sector, ((vldoor_t *) th)->sector - sectors);
            writelong(&door->line, ((vldoor_t *) th)->line - lines);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_MoveFloor)
        {
            floormove_t *floor;
            WRITEBYTE(save_p, tc_floor);
            PADSAVEP();
            floor = (floormove_t *) save_p;
            memcpy(save_p, th, sizeof(*floor));
            save_p += sizeof(*floor);
            writelong(&floor->sector, ((floormove_t *) th)->sector - sectors);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_PlatRaise)
        {
            plat_t *plat;
            WRITEBYTE(save_p, tc_plat);
            PADSAVEP();
            plat = (plat_t *) save_p;
            memcpy(save_p, th, sizeof(*plat));
            save_p += sizeof(*plat);
            writelong(&plat->sector, ((plat_t *) th)->sector - sectors);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_LightFlash)
        {
            lightflash_t *flash;
            WRITEBYTE(save_p, tc_flash);
            PADSAVEP();
            flash = (lightflash_t *) save_p;
            memcpy(save_p, th, sizeof(*flash));
            save_p += sizeof(*flash);
            writelong(&flash->sector, ((lightflash_t *) th)->sector - sectors);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_StrobeFlash)
        {
            strobe_t *strobe;
            WRITEBYTE(save_p, tc_strobe);
            PADSAVEP();
            strobe = (strobe_t *) save_p;
            memcpy(save_p, th, sizeof(*strobe));
            save_p += sizeof(*strobe);
            writelong(&strobe->sector, ((strobe_t *) th)->sector - sectors);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_Glow)
        {
            glow_t *glow;
            WRITEBYTE(save_p, tc_glow);
            PADSAVEP();
            glow = (glow_t *) save_p;
            memcpy(save_p, th, sizeof(*glow));
            save_p += sizeof(*glow);
            writelong(&glow->sector, ((glow_t *) th)->sector - sectors);
            continue;
        }
        else
            // BP added T_FireFlicker
        if (th->function.acp1 == (actionf_p1) T_FireFlicker)
        {
            fireflicker_t *fireflicker;
            WRITEBYTE(save_p, tc_fireflicker);
            PADSAVEP();
            fireflicker = (fireflicker_t *) save_p;
            memcpy(save_p, th, sizeof(*fireflicker));
            save_p += sizeof(*fireflicker);
            writelong(&fireflicker->sector, ((fireflicker_t *) th)->sector - sectors);
            continue;
        }
        else
            //SoM: 3/15/2000: Added extra Boom thinker types.
        if (th->function.acp1 == (actionf_p1) T_MoveElevator)
        {
            elevator_t *elevator;
            WRITEBYTE(save_p, tc_elevator);
            PADSAVEP();
            elevator = (elevator_t *) save_p;
            memcpy(save_p, th, sizeof(*elevator));
            save_p += sizeof(*elevator);
            writelong(&elevator->sector, ((elevator_t *) th)->sector - sectors);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_Scroll)
        {
            WRITEBYTE(save_p, tc_scroll);
            memcpy(save_p, th, sizeof(scroll_t));
            save_p += sizeof(scroll_t);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_Friction)
        {
            WRITEBYTE(save_p, tc_friction);
            memcpy(save_p, th, sizeof(friction_t));
            save_p += sizeof(friction_t);
            continue;
        }
        else if (th->function.acp1 == (actionf_p1) T_Pusher)
        {
            WRITEBYTE(save_p, tc_pusher);
            memcpy(save_p, th, sizeof(pusher_t));
            save_p += sizeof(pusher_t);
            continue;
        }
#ifdef PARANOIA
        else if ((int) th->function.acp1 != -1) // wait garbage colection
            I_Error("unknow thinker type 0x%X", th->function.acp1);
#endif

    }

    WRITEBYTE(save_p, tc_end);
}

// Now save the pointers, tracer and target, but at load time we must
// relink to this, the savegame contain the old position in the pointer
// field copyed in the info field temporarely, but finaly we just search
// for to old postion and relink to
static mobj_t *FindNewPosition(void *oldposition)
{
    thinker_t *th;
    mobj_t *mobj;

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        mobj = (mobj_t *) th;
        if ((void *) mobj->info == oldposition)
            return mobj;
    }
    if (devparm)
        CONS_Printf("\2not found\n");
    DEBFILE("not found\n");
    return NULL;
}

//
// P_UnArchiveThinkers
//
void P_UnArchiveThinkers(void)
{
    thinker_t *currentthinker;
    thinker_t *next;
    mobj_t *mobj;
    ULONG diff;
    int i;
    byte tclass;
    ceiling_t *ceiling;
    vldoor_t *door;
    floormove_t *floor;
    plat_t *plat;
    lightflash_t *flash;
    strobe_t *strobe;
    glow_t *glow;
    fireflicker_t *fireflicker;
    elevator_t *elevator;       //SoM: 3/15/2000
    scroll_t *scroll;
    friction_t *friction;
    pusher_t *pusher;

    // remove all the current thinkers
    currentthinker = thinkercap.next;
    while (currentthinker != &thinkercap)
    {
        next = currentthinker->next;

        mobj = (mobj_t *) currentthinker;
        if (currentthinker->function.acp1 == (actionf_p1) P_MobjThinker)
            // since this item isn't save don't remove it
/*            if( !((((mobj->flags & (MF_COUNTKILL | MF_PICKUP | MF_SHOOTABLE )) == 0)
                   && (mobj->flags & MF_MISSILE)
                   && (mobj->info->doomednum !=-1) )
                  || (mobj->type == MT_BLOOD) ) )
*/
            P_RemoveMobj((mobj_t *) currentthinker);
        else
            Z_Free(currentthinker);

        currentthinker = next;
    }
    // BP: we don't want the removed mobj come back !!!
    iquetail = iquehead = 0;
    P_InitThinkers();

    // read in saved thinkers
    while (1)
    {
        tclass = READBYTE(save_p);
        if (tclass == tc_end)
            break;      // leave the while
        switch (tclass)
        {
            case tc_mobj:
                PADSAVEP();

                diff = READULONG(save_p);
                next = (void *) READULONG(save_p);      // &mobj in the old system

                mobj = Z_Malloc(sizeof(mobj_t), PU_LEVEL, NULL);
                memset(mobj, 0, sizeof(mobj_t));

                mobj->z = READFIXED(save_p);    // Force this so 3dfloor problems don't arise. SSNTails 03-17-2002
                mobj->floorz = READFIXED(save_p);

                if (diff & MD_SPAWNPOINT)
                {
                    short spawnpointnum = READSHORT(save_p);
                    mobj->spawnpoint = &mapthings[spawnpointnum];
                    mapthings[spawnpointnum].mobj = mobj;
                }
                if (diff & MD_TYPE)
                {
                    mobj->type = READULONG(save_p);
                }
                else //if (diff & MD_SPAWNPOINT) //Hurdler: I think we must add that test ?
                {
                    for (i = 0; i < NUMMOBJTYPES; i++)
                        if (mobj->spawnpoint->type == mobjinfo[i].doomednum)
                            break;
                    if (i == NUMMOBJTYPES)
                    {
                        I_Error("Savegame corrupted\n");
                    }
                    mobj->type = i;
                }
                mobj->info = &mobjinfo[mobj->type];
                if (diff & MD_POS)
                {
                    mobj->x = READFIXED(save_p);
                    mobj->y = READFIXED(save_p);
                    mobj->angle = READANGLE(save_p);
                }
                else
                {
                    mobj->x = mobj->spawnpoint->x << FRACBITS;
                    mobj->y = mobj->spawnpoint->y << FRACBITS;
                    mobj->angle = ANG45 * (mobj->spawnpoint->angle / 45);
                }
                if (diff & MD_MOM)
                {
                    mobj->momx = READFIXED(save_p);
                    mobj->momy = READFIXED(save_p);
                    mobj->momz = READFIXED(save_p);
                }       // else null (memset)

                if (diff & MD_RADIUS)
                    mobj->radius = READFIXED(save_p);
                else
                    mobj->radius = mobj->info->radius;
                if (diff & MD_HEIGHT)
                    mobj->height = READFIXED(save_p);
                else
                    mobj->height = mobj->info->height;
                if (diff & MD_FLAGS)
                    mobj->flags = READLONG(save_p);
                else
                    mobj->flags = mobj->info->flags;
                if (diff & MD_FLAGS2)
                    mobj->flags2 = READLONG(save_p);
                else
                    mobj->flags2 = mobj->info->flags2;
                if (diff & MD_HEALTH)
                    mobj->health = READLONG(save_p);
                else
                    mobj->health = mobj->info->spawnhealth;
                if (diff & MD_RTIME)
                    mobj->reactiontime = READLONG(save_p);
                else
                    mobj->reactiontime = mobj->info->reactiontime;

                if (diff & MD_STATE)
                    mobj->state = &states[READUSHORT(save_p)];
                else
                    mobj->state = &states[mobj->info->spawnstate];
                if (diff & MD_TICS)
                    mobj->tics = READLONG(save_p);
                else
                    mobj->tics = mobj->state->tics;
                if (diff & MD_SPRITE)
                    mobj->sprite = READUSHORT(save_p);
                else
                    mobj->sprite = mobj->state->sprite;
                if (diff & MD_FRAME)
                    mobj->frame = READULONG(save_p);
                else
                    mobj->frame = mobj->state->frame;
                if (diff & MD_EFLAGS)
                    mobj->eflags = READULONG(save_p);
                if (diff & MD_PLAYER)
                {
                    i = READBYTE(save_p);
                    mobj->player = &players[i];
                    mobj->player->mo = mobj;
                    // added for angle prediction
                    if (consoleplayer == i)
                        localangle = mobj->angle;
                    if (secondarydisplayplayer == i)
                        localangle2 = mobj->angle;
                }
                if (diff & MD_MOVEDIR)
                    mobj->movedir = READLONG(save_p);
                if (diff & MD_MOVECOUNT)
                    mobj->movecount = READLONG(save_p);
                if (diff & MD_THRESHOLD)
                    mobj->threshold = READLONG(save_p);
                if (diff & MD_LASTLOOK)
                    mobj->lastlook = READLONG(save_p);
                else
                    mobj->lastlook = -1;
                if (diff & MD_TARGET)
                    mobj->target = (mobj_t *) READULONG(save_p);
                if (diff & MD_TRACER)
                    mobj->tracer = (mobj_t *) READULONG(save_p);
                if (diff & MD_FRICTION)
                    mobj->friction = READLONG(save_p);
                else
                    mobj->friction = ORIG_FRICTION;
                if (diff & MD_MOVEFACTOR)
                    mobj->movefactor = READLONG(save_p);
                else
                    mobj->movefactor = ORIG_FRICTION_FACTOR;
                if (diff & MD_SPECIAL1)
                    mobj->special1 = READLONG(save_p);
                if (diff & MD_SPECIAL2)
                    mobj->special2 = READLONG(save_p);
                if (diff & MD_AMMO)
                    mobj->dropped_ammo_count = READLONG(save_p);

                // now set deductable field
                // TODO : save this too
                mobj->skin = NULL;

                // set sprev, snext, bprev, bnext, subsector
                P_SetThingPosition(mobj);

                /*
                   mobj->floorz = mobj->subsector->sector->floorheight;
                   if( (diff & MD_Z) == 0 )
                   mobj->z = mobj->floorz;
                 */// This causes 3dfloor problems! SSNTails 03-17-2002
                if (mobj->player)
                {
                    mobj->player->viewz = mobj->player->mo->z + mobj->player->viewheight;
                    //CONS_Printf("viewz = %f\n",FIXED_TO_FLOAT(mobj->player->viewz));
                }
                mobj->ceilingz = mobj->subsector->sector->ceilingheight;
                mobj->thinker.function.acp1 = (actionf_p1) P_MobjThinker;
                P_AddThinker(&mobj->thinker);

                mobj->info = (mobjinfo_t *) next;       // temporarely, set when leave this function
                break;

            case tc_ceiling:
                PADSAVEP();
                ceiling = Z_Malloc(sizeof(*ceiling), PU_LEVEL, NULL);
                memcpy(ceiling, save_p, sizeof(*ceiling));
                save_p += sizeof(*ceiling);
                ceiling->sector = &sectors[LONG(ceiling->sector)];
                ceiling->sector->ceilingdata = ceiling;

                if (ceiling->thinker.function.acp1)
                    ceiling->thinker.function.acp1 = (actionf_p1) T_MoveCeiling;

                P_AddThinker(&ceiling->thinker);
                P_AddActiveCeiling(ceiling);
                break;

            case tc_door:
                PADSAVEP();
                door = Z_Malloc(sizeof(*door), PU_LEVEL, NULL);
                memcpy(door, save_p, sizeof(*door));
                save_p += sizeof(*door);
                door->sector = &sectors[LONG(door->sector)];
                door->sector->ceilingdata = door;
                door->line = &lines[LONG(door->line)];
                door->thinker.function.acp1 = (actionf_p1) T_VerticalDoor;
                P_AddThinker(&door->thinker);
                break;

            case tc_floor:
                PADSAVEP();
                floor = Z_Malloc(sizeof(*floor), PU_LEVEL, NULL);
                memcpy(floor, save_p, sizeof(*floor));
                save_p += sizeof(*floor);
                floor->sector = &sectors[LONG(floor->sector)];
                floor->sector->floordata = floor;
                floor->thinker.function.acp1 = (actionf_p1) T_MoveFloor;
                P_AddThinker(&floor->thinker);
                break;

            case tc_plat:
                PADSAVEP();
                plat = Z_Malloc(sizeof(*plat), PU_LEVEL, NULL);
                memcpy(plat, save_p, sizeof(*plat));
                save_p += sizeof(*plat);
                plat->sector = &sectors[LONG(plat->sector)];
                plat->sector->floordata = plat;

                if (plat->thinker.function.acp1)
                    plat->thinker.function.acp1 = (actionf_p1) T_PlatRaise;

                P_AddThinker(&plat->thinker);
                P_AddActivePlat(plat);
                break;

            case tc_flash:
                PADSAVEP();
                flash = Z_Malloc(sizeof(*flash), PU_LEVEL, NULL);
                memcpy(flash, save_p, sizeof(*flash));
                save_p += sizeof(*flash);
                flash->sector = &sectors[LONG(flash->sector)];
                flash->thinker.function.acp1 = (actionf_p1) T_LightFlash;
                P_AddThinker(&flash->thinker);
                break;

            case tc_strobe:
                PADSAVEP();
                strobe = Z_Malloc(sizeof(*strobe), PU_LEVEL, NULL);
                memcpy(strobe, save_p, sizeof(*strobe));
                save_p += sizeof(*strobe);
                strobe->sector = &sectors[LONG(strobe->sector)];
                strobe->thinker.function.acp1 = (actionf_p1) T_StrobeFlash;
                P_AddThinker(&strobe->thinker);
                break;

            case tc_glow:
                PADSAVEP();
                glow = Z_Malloc(sizeof(*glow), PU_LEVEL, NULL);
                memcpy(glow, save_p, sizeof(*glow));
                save_p += sizeof(*glow);
                glow->sector = &sectors[LONG(glow->sector)];
                glow->thinker.function.acp1 = (actionf_p1) T_Glow;
                P_AddThinker(&glow->thinker);
                break;

            case tc_fireflicker:
                PADSAVEP();
                fireflicker = Z_Malloc(sizeof(*fireflicker), PU_LEVEL, NULL);
                memcpy(fireflicker, save_p, sizeof(*fireflicker));
                save_p += sizeof(*fireflicker);
                fireflicker->sector = &sectors[LONG(fireflicker->sector)];
                fireflicker->thinker.function.acp1 = (actionf_p1) T_FireFlicker;
                P_AddThinker(&fireflicker->thinker);
                break;

            case tc_elevator:
                PADSAVEP();
                elevator = Z_Malloc(sizeof(elevator_t), PU_LEVEL, NULL);
                memcpy(elevator, save_p, sizeof(elevator_t));
                save_p += sizeof(elevator_t);
                elevator->sector = &sectors[LONG(elevator->sector)];
                elevator->sector->floordata = elevator; //jff 2/22/98
                elevator->sector->ceilingdata = elevator;       //jff 2/22/98
                elevator->thinker.function.acp1 = (actionf_p1) T_MoveElevator;
                P_AddThinker(&elevator->thinker);
                break;

            case tc_scroll:
                scroll = Z_Malloc(sizeof(scroll_t), PU_LEVEL, NULL);
                memcpy(scroll, save_p, sizeof(scroll_t));
                save_p += sizeof(scroll_t);
                scroll->thinker.function.acp1 = (actionf_p1) T_Scroll;
                P_AddThinker(&scroll->thinker);
                break;

            case tc_friction:
                friction = Z_Malloc(sizeof(friction_t), PU_LEVEL, NULL);
                memcpy(friction, save_p, sizeof(friction_t));
                save_p += sizeof(friction_t);
                friction->thinker.function.acp1 = (actionf_p1) T_Friction;
                P_AddThinker(&friction->thinker);
                break;

            case tc_pusher:
                pusher = Z_Malloc(sizeof(pusher_t), PU_LEVEL, NULL);
                memcpy(pusher, save_p, sizeof(pusher_t));
                save_p += sizeof(pusher_t);
                pusher->thinker.function.acp1 = (actionf_p1) T_Pusher;
                pusher->source = P_GetPushThing(pusher->affectee);
                P_AddThinker(&pusher->thinker);
                break;

            default:
                I_Error("P_UnarchiveSpecials:Unknown tclass %i " "in savegame", tclass);
        }
    }

    // use info field (value = oldposition) to relink mobjs
    for (currentthinker = thinkercap.next; currentthinker != &thinkercap; currentthinker = currentthinker->next)
    {
        if (currentthinker->function.acp1 == (actionf_p1) P_MobjThinker)
        {
            mobj = (mobj_t *) currentthinker;
            if (mobj->tracer)
            {
                mobj->tracer = FindNewPosition(mobj->tracer);
                if (!mobj->tracer)
                    DEBFILE(va("tracer not found on %d\n", mobj->type));
            }
            if (mobj->target)
            {
                mobj->target = FindNewPosition(mobj->target);
                if (!mobj->target)
                    DEBFILE(va("target not found on %d\n", mobj->target));

            }
        }
    }

}

//
// P_FinishMobjs
// SoM: Delay this until AFTER we load fragglescript because FS needs this
// data!
void P_FinishMobjs()
{
    thinker_t *currentthinker;
    mobj_t *mobj;

    // put info field there real value
    for (currentthinker = thinkercap.next; currentthinker != &thinkercap; currentthinker = currentthinker->next)
    {
        if (currentthinker->function.acp1 == (actionf_p1) P_MobjThinker)
        {
            mobj = (mobj_t *) currentthinker;
            mobj->info = &mobjinfo[mobj->type];
        }
    }
}

//
// P_ArchiveSpecials
//

// BP: added : itemrespawnqueue
//
void P_ArchiveSpecials(void)
{
    int i;

    // BP: added save itemrespawn queue for deathmatch
    i = iquetail;
    while (iquehead != i)
    {
        WRITELONG(save_p, itemrespawnque[i] - mapthings);
        WRITELONG(save_p, itemrespawntime[i]);
        i = (i + 1) & (ITEMQUESIZE - 1);
    }

    // end delimiter
    WRITELONG(save_p, 0xffffffff);
}

//
// P_UnArchiveSpecials
//
void P_UnArchiveSpecials(void)
{
    int i;

    // BP: added save itemrespawn queue for deathmatch
    iquetail = iquehead = 0;
    while ((i = READLONG(save_p)) != 0xffffffff)
    {
        itemrespawnque[iquehead] = &mapthings[i];
        itemrespawntime[iquehead++] = READLONG(save_p);
    }
}

/////////////////////////////////////////////////////////////////////////////
// BIG NOTE FROM SOM!
//
// SMMU/MBF use the CheckSaveGame function to dynamically expand the savegame
// buffer which would eliminate all limits on savegames... Could/Should we
// use this method?
/////////////////////////////////////////////////////////////////////////////

// SoM: Added FraggleScript stuff.
// Save all the neccesary FraggleScript data.
// we need to save the levelscript(all global
// variables) and the runningscripts (scripts
// currently suspended)

/***************** save the levelscript *************/
// make sure we remember all the global
// variables.

void P_ArchiveLevelScript()
{
    int num_variables = 0;
    int i;

    // all we really need to do is save the variables
    // count the variables first

    // count number of variables
    num_variables = 0;
    for (i = 0; i < VARIABLESLOTS; i++)
    {
        svariable_t *sv = levelscript.variables[i];
        while (sv && sv->type != svt_label)
        {
            num_variables++;
            sv = sv->next;
        }
    }

    //CheckSaveGame(sizeof(short));
    WRITESHORT(save_p, num_variables);  // write num_variables

    // go thru hash chains, store each variable
    for (i = 0; i < VARIABLESLOTS; i++)
    {
        // go thru this hashchain
        svariable_t *sv = levelscript.variables[i];

        // once we get to a label there can be no more actual
        // variables in the list to store
        while (sv && sv->type != svt_label)
        {

            //CheckSaveGame(strlen(sv->name)+10); // 10 for type and safety

            // write svariable: name

            strcpy(save_p, sv->name);
            save_p += strlen(sv->name) + 1;     // 1 extra for ending NULL

            // type
            *save_p++ = sv->type;       // store type;

            switch (sv->type)   // store depending on type
            {
                case svt_string:
                {
                    //CheckSaveGame(strlen(sv->value.s)+5); // 5 for safety
                    strcpy(save_p, sv->value.s);
                    save_p += strlen(sv->value.s) + 1;
                    break;
                }
                case svt_int:
                {
                    //CheckSaveGame(sizeof(long)); 
                    WRITELONG(save_p, sv->value.i);
                    break;
                }
                case svt_mobj:
                {
                    //CheckSaveGame(sizeof(long)); 
                    WRITEULONG(save_p, sv->value.mobj);
                    break;
                }
                case svt_fixed:
                {
                    WRITEFIXED(save_p, sv->value.fixed);
                    break;
                }
            }
            sv = sv->next;
        }
    }
}

void P_UnArchiveLevelScript()
{
    int i;
    int num_variables;

    // free all the variables in the current levelscript first

    for (i = 0; i < VARIABLESLOTS; i++)
    {
        svariable_t *sv = levelscript.variables[i];

        while (sv && sv->type != svt_label)
        {
            svariable_t *next = sv->next;
            Z_Free(sv);
            sv = next;
        }
        levelscript.variables[i] = sv;  // null or label
    }

    // now read the number of variables from the savegame file
    num_variables = READSHORT(save_p);

    for (i = 0; i < num_variables; i++)
    {
        svariable_t *sv = Z_Malloc(sizeof(svariable_t), PU_LEVEL, 0);
        int hashkey;

        // name
        sv->name = Z_Strdup(save_p, PU_LEVEL, 0);
        save_p += strlen(sv->name) + 1;

        sv->type = *save_p++;

        switch (sv->type)       // read depending on type
        {
            case svt_string:
            {
                sv->value.s = Z_Strdup(save_p, PU_LEVEL, 0);
                save_p += strlen(sv->value.s) + 1;
                break;
            }
            case svt_int:
            {
                sv->value.i = READLONG(save_p);
                break;
            }
            case svt_mobj:
            {
                ULONG *long_p = (ULONG *) save_p;
                sv->value.mobj = FindNewPosition((mobj_t *) long_p);
                long_p++;
                save_p = (char *) long_p;
                break;
            }
            case svt_fixed:
            {
                sv->value.fixed = READFIXED(save_p);
                break;
            }
            default:
                break;
        }

        // link in the new variable
        hashkey = variable_hash(sv->name);
        sv->next = levelscript.variables[hashkey];
        levelscript.variables[hashkey] = sv;
    }

}

/**************** save the runningscripts ***************/

extern runningscript_t runningscripts;  // t_script.c
runningscript_t *new_runningscript();   // t_script.c
void clear_runningscripts();    // t_script.c

// save a given runningscript
void P_ArchiveRunningScript(runningscript_t * rs)
{
    int i;
    int num_variables;

    //CheckSaveGame(sizeof(short) * 8); // room for 8 shorts
    WRITESHORT(save_p, rs->script->scriptnum);  // save scriptnum
    WRITESHORT(save_p, rs->savepoint - rs->script->data);       // offset
    WRITESHORT(save_p, rs->wait_type);
    WRITESHORT(save_p, rs->wait_data);

    // save pointer to trigger using prev
    WRITEULONG(save_p, (ULONG) rs->trigger);

    // count number of variables
    num_variables = 0;
    for (i = 0; i < VARIABLESLOTS; i++)
    {
        svariable_t *sv = rs->variables[i];
        while (sv && sv->type != svt_label)
        {
            num_variables++;
            sv = sv->next;
        }
    }
    WRITESHORT(save_p, num_variables);

    // save num_variables

    // store variables
    // go thru hash chains, store each variable
    for (i = 0; i < VARIABLESLOTS; i++)
    {
        // go thru this hashchain
        svariable_t *sv = rs->variables[i];

        // once we get to a label there can be no more actual
        // variables in the list to store
        while (sv && sv->type != svt_label)
        {

            //CheckSaveGame(strlen(sv->name)+10); // 10 for type and safety

            // write svariable: name

            strcpy(save_p, sv->name);
            save_p += strlen(sv->name) + 1;     // 1 extra for ending NULL

            // type
            *save_p++ = sv->type;       // store type;

            switch (sv->type)   // store depending on type
            {
                case svt_string:
                {
                    //CheckSaveGame(strlen(sv->value.s)+5); // 5 for safety
                    strcpy(save_p, sv->value.s);
                    save_p += strlen(sv->value.s) + 1;
                    break;
                }
                case svt_int:
                {
                    //CheckSaveGame(sizeof(long)+4); 
                    WRITELONG(save_p, sv->value.i);
                    break;
                }
                case svt_mobj:
                {
                    //CheckSaveGame(sizeof(long)+4); 
                    WRITEULONG(save_p, (ULONG) sv->value.mobj);
                    break;
                }
                case svt_fixed:
                {
                    WRITEFIXED(save_p, sv->value.fixed);
                    break;
                }
                    // others do not appear in user scripts

                default:
                    break;
            }

            sv = sv->next;
        }
    }
}

// get the next runningscript
runningscript_t *P_UnArchiveRunningScript()
{
    int i;
    int scriptnum;
    int num_variables;
    runningscript_t *rs;

    // create a new runningscript
    rs = new_runningscript();

    scriptnum = READSHORT(save_p);      // get scriptnum

    // levelscript?

    if (scriptnum == -1)
        rs->script = &levelscript;
    else
        rs->script = levelscript.children[scriptnum];

    // read out offset from save
    rs->savepoint = rs->script->data + READSHORT(save_p);
    rs->wait_type = READSHORT(save_p);
    rs->wait_data = READSHORT(save_p);

    // read out trigger thing
    rs->trigger = FindNewPosition((mobj_t *) (READULONG(save_p)));

    // get number of variables
    num_variables = READSHORT(save_p);

    // read out the variables now (fun!)

    // start with basic script slots/labels

    for (i = 0; i < VARIABLESLOTS; i++)
        rs->variables[i] = rs->script->variables[i];

    for (i = 0; i < num_variables; i++)
    {
        svariable_t *sv = Z_Malloc(sizeof(svariable_t), PU_LEVEL, 0);
        int hashkey;

        // name
        sv->name = Z_Strdup(save_p, PU_LEVEL, 0);
        save_p += strlen(sv->name) + 1;

        sv->type = *save_p++;

        switch (sv->type)       // read depending on type
        {
            case svt_string:
            {
                sv->value.s = Z_Strdup(save_p, PU_LEVEL, 0);
                save_p += strlen(sv->value.s) + 1;
                break;
            }
            case svt_int:
            {
                sv->value.i = READLONG(save_p);
                break;
            }
            case svt_mobj:
            {
                sv->value.mobj = FindNewPosition((mobj_t *) READULONG(save_p));
                break;
            }
            case svt_fixed:
            {
                sv->value.fixed = READFIXED(save_p);
                break;
            }
            default:
                break;
        }

        // link in the new variable
        hashkey = variable_hash(sv->name);
        sv->next = rs->variables[hashkey];
        rs->variables[hashkey] = sv;
    }

    return rs;
}

// archive all runningscripts in chain
void P_ArchiveRunningScripts()
{
    runningscript_t *rs;
    int num_runningscripts = 0;

    // count runningscripts
    for (rs = runningscripts.next; rs; rs = rs->next)
        num_runningscripts++;

    //CheckSaveGame(sizeof(long));

    // store num_runningscripts
    WRITEULONG(save_p, num_runningscripts);

    // now archive them
    rs = runningscripts.next;
    while (rs)
    {
        P_ArchiveRunningScript(rs);
        rs = rs->next;
    }
}

// restore all runningscripts from save_p
void P_UnArchiveRunningScripts()
{
    runningscript_t *rs;
    int num_runningscripts;
    int i;

    // remove all runningscripts first : may have been started
    // by levelscript on level load

    clear_runningscripts();

    // get num_runningscripts
    num_runningscripts = READULONG(save_p);

    for (i = 0; i < num_runningscripts; i++)
    {
        // get next runningscript
        rs = P_UnArchiveRunningScript();

        // hook into chain
        rs->next = runningscripts.next;
        rs->prev = &runningscripts;
        rs->prev->next = rs;
        if (rs->next)
            rs->next->prev = rs;
    }
}

void P_ArchiveScripts()
{
#ifdef FRAGGLESCRIPT
    // save levelscript
    P_ArchiveLevelScript();

    // save runningscripts
    P_ArchiveRunningScripts();

    // Archive the script camera.
    WRITELONG(save_p, (long) script_camera_on);
    WRITEULONG(save_p, (ULONG) script_camera.mo);
    WRITEANGLE(save_p, script_camera.aiming);
    WRITEFIXED(save_p, script_camera.viewheight);
    WRITEANGLE(save_p, script_camera.startangle);
#endif
}

void P_UnArchiveScripts()
{
#ifdef FRAGGLESCRIPT
    // restore levelscript
    P_UnArchiveLevelScript();

    // restore runningscripts
    P_UnArchiveRunningScripts();

    // Unarchive the script camera
    script_camera_on = (boolean) READLONG(save_p);
    script_camera.mo = FindNewPosition((mobj_t *) (READULONG(save_p)));
    script_camera.aiming = READANGLE(save_p);
    script_camera.viewheight = READFIXED(save_p);
    script_camera.startangle = READANGLE(save_p);
#endif
    P_FinishMobjs();
}

// =======================================================================
//          Misc
// =======================================================================
void P_ArchiveMisc()
{
    ULONG pig = 0;
    int i;

    WRITEBYTE(save_p, gameskill);
    WRITEBYTE(save_p, gameepisode);
    WRITEBYTE(save_p, gamemap);

    for (i = 0; i < MAXPLAYERS; i++)
        pig |= (playeringame[i] != 0) << i;

    WRITEULONG(save_p, pig);

    WRITEULONG(save_p, leveltime);
    WRITEBYTE(save_p, P_GetRandIndex());
}

boolean P_UnArchiveMisc()
{
    ULONG pig;
    int i;

    gameskill = READBYTE(save_p);
    gameepisode = READBYTE(save_p);
    gamemap = READBYTE(save_p);

    pig = READULONG(save_p);

    for (i = 0; i < MAXPLAYERS; i++)
    {
        playeringame[i] = (pig & (1 << i)) != 0;
        players[i].playerstate = PST_REBORN;
    }

    if (!P_SetupLevel(gameepisode, gamemap, gameskill, NULL))
        return false;

    // get the time
    leveltime = READULONG(save_p);
    P_SetRandIndex(READBYTE(save_p));

    return true;
}

void P_SaveGame(void)
{
    CV_SaveNetVars((char **) &save_p);
    P_ArchiveMisc();
    P_ArchivePlayers();
    P_ArchiveWorld();
    P_ArchiveThinkers();
    P_ArchiveSpecials();
    P_ArchiveScripts();

    WRITEBYTE(save_p, 0x1d);    // consistancy marker
}

boolean P_LoadGame(void)
{
    CV_LoadNetVars((char **) &save_p);
    if (!P_UnArchiveMisc())
        return false;
    P_UnArchivePlayers();
    P_UnArchiveWorld();
    P_UnArchiveThinkers();
    P_UnArchiveSpecials();
    P_UnArchiveScripts();

    return READBYTE(save_p) == 0x1d;
}
