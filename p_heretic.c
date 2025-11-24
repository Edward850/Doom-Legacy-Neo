// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_heretic.c,v 1.7 2001/07/16 22:35:41 bpereira Exp $
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
// $Log: p_heretic.c,v $
// Revision 1.7  2001/07/16 22:35:41  bpereira
// - fixed crash of e3m8 in heretic
// - fixed crosshair not drawed bug
//
// Revision 1.6  2001/05/27 13:42:47  bpereira
// no message
//
// Revision 1.5  2001/03/30 17:12:50  bpereira
// no message
//
// Revision 1.4  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.3  2001/02/10 13:20:55  hurdler
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
#include "r_main.h"
#include "r_state.h"
#include "s_sound.h"
#include "m_random.h"
#include "dstrings.h"
#include "p_heretic.h"

//---------------------------------------------------------------------------
//
// PROC A_ContMobjSound
//
//---------------------------------------------------------------------------

void A_ContMobjSound(mobj_t *actor)
{
        switch(actor->type)
        {
                case MT_KNIGHTAXE:
                        S_StartSound(actor, sfx_kgtatk);
                        break;
                case MT_MUMMYFX1:
                        S_StartSound(actor, sfx_mumhed);
                        break;
                default:
                        break;
        }
}

//----------------------------------------------------------------------------
//
// FUNC P_FaceMobj
//
// Returns 1 if 'source' needs to turn clockwise, or 0 if 'source' needs
// to turn counter clockwise.  'delta' is set to the amount 'source'
// needs to turn.
//
//----------------------------------------------------------------------------
int P_FaceMobj(mobj_t *source, mobj_t *target, angle_t *delta)
{
        angle_t diff;
        angle_t angle1;
        angle_t angle2;

        angle1 = source->angle;
        angle2 = R_PointToAngle2(source->x, source->y, target->x, target->y);
        if(angle2 > angle1)
        {
                diff = angle2-angle1;
                if(diff > ANGLE_180)
                {
                        *delta = ANGLE_MAX-diff;
                        return(0);
                }
                else
                {
                        *delta = diff;
                        return(1);
                }
        }
        else
        {
                diff = angle1-angle2;
                if(diff > ANGLE_180)
                {
                        *delta = ANGLE_MAX-diff;
                        return(1);
                }
                else
                {
                        *delta = diff;
                        return(0);
                }
        }
}

//----------------------------------------------------------------------------
//
// FUNC P_SeekerMissile
//
// The missile tracer field must be mobj_t *target.  Returns true if
// target was tracked, false if not.
//
//----------------------------------------------------------------------------

boolean P_SeekerMissile(mobj_t *actor, angle_t thresh, angle_t turnMax)
{
        int dir;
        int dist;
        angle_t delta;
        angle_t angle;
        mobj_t *target;

        target = actor->tracer;
        if(target == NULL)
        {
                return(false);
        }
        if(!(target->flags&MF_SHOOTABLE))
        { // Target died
                actor->tracer = 0;
                return(false);
        }
        dir = P_FaceMobj(actor, target, &delta);
        if(delta > thresh)
        {
                delta >>= 1;
                if(delta > turnMax)
                {
                        delta = turnMax;
                }
        }
        if(dir)
        { // Turn clockwise
                actor->angle += delta;
        }
        else
        { // Turn counter clockwise
                actor->angle -= delta;
        }
        angle = actor->angle>>ANGLETOFINESHIFT;
        actor->momx = FixedMul(actor->info->speed, finecosine[angle]);
        actor->momy = FixedMul(actor->info->speed, finesine[angle]);
        if(actor->z+actor->height < target->z ||
                target->z+target->height < actor->z)
        { // Need to seek vertically
                dist = P_AproxDistance(target->x-actor->x, target->y-actor->y);
                dist = dist/actor->info->speed;
                if(dist < 1)
                   dist = 1;
                actor->momz = (target->z+(target->height>>1)
                             -(actor->z+(actor->height>>1)))/dist;
        }
        return(true);
}

//---------------------------------------------------------------------------
//
// FUNC P_SpawnMissileAngle
//
// Returns NULL if the missile exploded immediately, otherwise returns
// a mobj_t pointer to the missile.
//
//---------------------------------------------------------------------------

mobj_t *P_SpawnMissileAngle(mobj_t *source, mobjtype_t type,
        angle_t angle, fixed_t momz)
{
        fixed_t z;
        mobj_t *mo;

        switch(type)
        {
                case MT_MNTRFX1: // Minotaur swing attack missile
                        z = source->z+40*FRACUNIT;
                        break;
                case MT_MNTRFX2: // Minotaur floor fire missile
                        z = ONFLOORZ;
                        break;
                case MT_SRCRFX1: // Sorcerer Demon fireball
                        z = source->z+48*FRACUNIT;
                        break;
                default:
                        z = source->z+32*FRACUNIT;
                        break;
        }
        if(source->flags2&MF2_FEETARECLIPPED)
        {
                z -= FOOTCLIPSIZE;
        }
        mo = P_SpawnMobj(source->x, source->y, z, type);
        if(mo->info->seesound)
        {
                S_StartSound(mo, mo->info->seesound);
        }
        mo->target = source; // Originator
        mo->angle = angle;
        angle >>= ANGLETOFINESHIFT;
        mo->momx = FixedMul(mo->info->speed, finecosine[angle]);
        mo->momy = FixedMul(mo->info->speed, finesine[angle]);
        mo->momz = momz;
        return(P_CheckMissileSpawn(mo) ? mo : NULL);
}


boolean inventory = false;

extern int GetWeaponAmmo[];
extern byte cheat_mus_seq[];
extern byte cheat_choppers_seq[];
extern byte cheat_god_seq[];
extern byte cheat_ammo_seq[];
extern byte cheat_ammonokey_seq[];
extern byte cheat_noclip_seq[];
extern byte cheat_commercial_noclip_seq[];
extern byte cheat_powerup_seq[7][10];
extern byte cheat_clev_seq[];
extern byte cheat_mypos_seq[];
extern byte cheat_amap_seq[];

void HereticPatchEngine(void)
{
    ceilmovesound = sfx_dormov;
    doorclosesound = sfx_doropn;
    inventory = true;

    // we can put such thinks in a dehacked lump, maybe for later
    S_sfx[sfx_oof].name        = "plroof";
    S_sfx[sfx_oof].priority    = 32;
    text[PD_BLUEK_NUM]   = "YOU NEED A BLUE KEY TO OPEN THIS DOOR";
    text[PD_YELLOWK_NUM] = "YOU NEED A YELLOW KEY TO OPEN THIS DOOR";
    text[PD_REDK_NUM]    = "YOU NEED A GREEN KEY TO OPEN THIS DOOR";
    S_sfx[sfx_swtchn].name     = "switch";
    S_sfx[sfx_swtchn].priority = 40;
    S_sfx[sfx_swtchx].name     = "switch";
    S_sfx[sfx_swtchx].priority = 40;
    S_sfx[sfx_telept].priority = 50;
    S_sfx[sfx_sawup].name     = "gntact";  // gauntlets
    S_sfx[sfx_sawup].priority = 32;

    mobjinfo[MT_TFOG].spawnstate = S_HTFOG1;
    sprnames[SPR_BLUD] = "BLOD";
    maxammo [am_goldwand  ] = 100;
    maxammo [am_crossbow  ] = 50;
    maxammo [am_blaster   ] = 200;
    maxammo [am_skullrod  ] = 200;
    maxammo [am_phoenixrod] = 20;
    maxammo [am_mace      ] = 150;
    clipammo[am_goldwand  ] = 5;  // used in deathmatch 1 & 3 mul by 5 (P_GiveWeapon)
    clipammo[am_crossbow  ] = 2; 
    clipammo[am_blaster   ] = 6;
    clipammo[am_skullrod  ] = 10;
    clipammo[am_phoenixrod] = 1; 
    clipammo[am_mace      ] = 10;
    GetWeaponAmmo[wp_staff     ] = 0;
    GetWeaponAmmo[wp_goldwand  ] = 25;
    GetWeaponAmmo[wp_crossbow  ] = 10;
    GetWeaponAmmo[wp_blaster   ] = 30;
    GetWeaponAmmo[wp_skullrod  ] = 50;
    GetWeaponAmmo[wp_phoenixrod] = 2;
    GetWeaponAmmo[wp_mace      ] = 50;
    GetWeaponAmmo[wp_gauntlets ] = 0;
    GetWeaponAmmo[wp_beak      ] = 0; 

    text[GOTARMOR_NUM] = "SILVER SHIELD";
    text[GOTMEGA_NUM ] = "ENCHANTED SHIELD";
    text[GOTSTIM_NUM ] = "CRYSTAL VIAL";
    text[GOTMAP_NUM  ] = "MAP SCROLL";
    text[GOTBLUECARD_NUM] = "BLUE KEY";
    text[GOTYELWCARD_NUM] = "YELLOW KEY";
    text[GOTREDCARD_NUM ] = "GREEN KEY";

    S_sfx[sfx_pistol].name = "keyup";  // for the menu
    S_music[mus_inter].name = "MUS_INTR";

    S_sfx[sfx_tink].name = "chat";
    S_sfx[sfx_tink].priority = 100;
    S_sfx[sfx_itmbk].name = "respawn";
    S_sfx[sfx_itmbk].priority = 10;

    // conflicting number for doomednum
    // so desable doom mobjs and enabled heretic's one
    mobjinfo[MT_SHOTGUY].doomednum = -1;
    mobjinfo[MT_MINOTAUR].doomednum = 9;

    mobjinfo[MT_VILE].doomednum = -1;
    mobjinfo[MT_HKNIGHT].doomednum = 64;

    mobjinfo[MT_CHAINGUY].doomednum = -1;
    mobjinfo[MT_KNIGHTGHOST].doomednum = 65;

    mobjinfo[MT_UNDEAD].doomednum = -1;
    mobjinfo[MT_IMP].doomednum = 66;

    mobjinfo[MT_KNIGHT].doomednum = -1;
    mobjinfo[MT_MUMMYGHOST].doomednum = 69;

    mobjinfo[MT_SPIDER].doomednum = -1;
    mobjinfo[MT_SORCERER1].doomednum = 7;

    mobjinfo[MT_BABY].doomednum = -1;
    mobjinfo[MT_MUMMY].doomednum = 68;

    mobjinfo[MT_CYBORG].doomednum = -1;
    mobjinfo[MT_AMMACEHEFTY].doomednum = 16;

    mobjinfo[MT_WOLFSS].doomednum = -1;
    mobjinfo[MT_ARTIINVULNERABILITY].doomednum = 84;

    mobjinfo[MT_BOSSTARGET].doomednum = -1;
    mobjinfo[MT_HMISC12].doomednum = 87;

    mobjinfo[MT_BARREL].doomednum = -1;
    mobjinfo[MT_POD].doomednum = 2035;

    mobjinfo[MT_MISC4].doomednum = -1;
    mobjinfo[MT_IMPLEADER].doomednum = 5;

    mobjinfo[MT_MISC5].doomednum = -1;
    mobjinfo[MT_AMMACEWIMPY].doomednum = 13;

    mobjinfo[MT_MISC6].doomednum = -1;
    mobjinfo[MT_HHEAD].doomednum = 6;

    mobjinfo[MT_MISC7].doomednum = -1;
    mobjinfo[MT_STALACTITESMALL].doomednum = 39;

    mobjinfo[MT_MISC8].doomednum = -1;
    mobjinfo[MT_STALAGMITELARGE].doomednum = 38;

    mobjinfo[MT_MISC9].doomednum = -1;
    mobjinfo[MT_STALACTITELARGE].doomednum = 40;

    mobjinfo[MT_MEGA].doomednum = -1;
    mobjinfo[MT_ARTIFLY].doomednum = 83;

    mobjinfo[MT_MISC21].doomednum = -1;
    mobjinfo[MT_SKULLHANG70].doomednum = 17;

    mobjinfo[MT_MISC24].doomednum = -1;
    mobjinfo[MT_HMISC1].doomednum = 8;

    mobjinfo[MT_CHAINGUN].doomednum = -1;
    mobjinfo[MT_WMACE].doomednum = 2002;

    mobjinfo[MT_SHAINSAW].doomednum = -1;
    mobjinfo[MT_HMISC13].doomednum = 2005;

    mobjinfo[MT_ROCKETLAUNCH].doomednum = -1;
    mobjinfo[MT_WPHOENIXROD].doomednum = 2003;

    mobjinfo[MT_PLASMAGUN].doomednum = -1;
    mobjinfo[MT_WSKULLROD].doomednum = 2004;

    mobjinfo[MT_SHOTGUN].doomednum = -1;
    mobjinfo[MT_HMISC15].doomednum = 2001;

    mobjinfo[MT_SUPERSHOTGUN].doomednum = -1;
    mobjinfo[MT_HMISC3].doomednum = 82;

    mobjinfo[MT_MISC29].doomednum = -1;
    mobjinfo[MT_ITEMSHIELD1].doomednum = 85;

    mobjinfo[MT_MISC30].doomednum = -1;
    mobjinfo[MT_ARTITOMEOFPOWER].doomednum = 86;

    mobjinfo[MT_MISC32].doomednum = -1;
    mobjinfo[MT_ARTIEGG].doomednum = 30;

    mobjinfo[MT_MISC33].doomednum = -1;
    mobjinfo[MT_ITEMSHIELD2].doomednum = 31;

    mobjinfo[MT_MISC34].doomednum = -1;
    mobjinfo[MT_ARTISUPERHEAL].doomednum = 32;

    mobjinfo[MT_MISC35].doomednum = -1;
    mobjinfo[MT_HMISC4].doomednum = 33;

    mobjinfo[MT_MISC36].doomednum = -1;
    mobjinfo[MT_STALAGMITESMALL].doomednum = 37;

    mobjinfo[MT_MISC37].doomednum = -1;
    mobjinfo[MT_ARTITELEPORT].doomednum = 36;

    mobjinfo[MT_MISC38].doomednum = -1;
    mobjinfo[MT_SOUNDWATERFALL].doomednum = 41;

    mobjinfo[MT_MISC39].doomednum = -1;
    mobjinfo[MT_SOUNDWIND].doomednum = 42;

    mobjinfo[MT_MISC40].doomednum = -1;
    mobjinfo[MT_PODGENERATOR].doomednum = 43;

    mobjinfo[MT_MISC41].doomednum = -1;
    mobjinfo[MT_HBARREL].doomednum = 44;

    mobjinfo[MT_MISC42].doomednum = -1;
    mobjinfo[MT_MUMMYLEADER].doomednum = 45;

    mobjinfo[MT_MISC43].doomednum = -1;
    mobjinfo[MT_MUMMYLEADERGHOST].doomednum = 46;

    mobjinfo[MT_MISC44].doomednum = -1;
    mobjinfo[MT_AMBLSRHEFTY].doomednum = 55;

    mobjinfo[MT_MISC45].doomednum = -1;
    // heretic use it for monster spawn

    mobjinfo[MT_MISC47].doomednum = -1;
    mobjinfo[MT_HMISC7].doomednum = 47;

    mobjinfo[MT_MISC48].doomednum = -1;
    mobjinfo[MT_HMISC8].doomednum = 48;

    mobjinfo[MT_MISC49].doomednum = -1;
    mobjinfo[MT_HMISC5].doomednum = 34;

    mobjinfo[MT_MISC50].doomednum = -1;
    mobjinfo[MT_HMISC2].doomednum = 35;

    mobjinfo[MT_MISC51].doomednum = -1;
    mobjinfo[MT_HMISC9].doomednum = 49;

    mobjinfo[MT_MISC52].doomednum = -1;
    mobjinfo[MT_HMISC10].doomednum = 50;

    mobjinfo[MT_MISC53].doomednum = -1;
    mobjinfo[MT_HMISC11].doomednum = 51;

    mobjinfo[MT_MISC54].doomednum = -1;
    mobjinfo[MT_TELEGLITGEN2].doomednum = 52;

    mobjinfo[MT_MISC55].doomednum = -1;
    mobjinfo[MT_HMISC14].doomednum = 53;

    mobjinfo[MT_MISC61].doomednum = -1;
    mobjinfo[MT_AMPHRDWIMPY].doomednum = 22;

    mobjinfo[MT_MISC62].doomednum = -1;
    mobjinfo[MT_WIZARD].doomednum = 15;

    mobjinfo[MT_MISC63].doomednum = -1;
    mobjinfo[MT_AMCBOWWIMPY].doomednum = 18;

    mobjinfo[MT_MISC64].doomednum = -1;
    mobjinfo[MT_AMSKRDHEFTY].doomednum = 21;

    mobjinfo[MT_MISC65].doomednum = -1;
    mobjinfo[MT_AMPHRDHEFTY].doomednum = 23;

    mobjinfo[MT_MISC66].doomednum = -1;
    mobjinfo[MT_AMSKRDWIMPY].doomednum = 20;

    mobjinfo[MT_MISC67].doomednum = -1;
    mobjinfo[MT_AMCBOWHEFTY].doomednum = 19;

    mobjinfo[MT_MISC68].doomednum = -1;
    mobjinfo[MT_AMGWNDWIMPY].doomednum = 10;

    mobjinfo[MT_MISC69].doomednum = -1;
    mobjinfo[MT_AMGWNDHEFTY].doomednum = 12;

    mobjinfo[MT_MISC70].doomednum = -1;
    mobjinfo[MT_CHANDELIER].doomednum = 28;

    mobjinfo[MT_MISC71].doomednum = -1;
    mobjinfo[MT_SKULLHANG60].doomednum = 24;

    mobjinfo[MT_MISC72].doomednum = -1;
    mobjinfo[MT_SERPTORCH].doomednum = 27;

    mobjinfo[MT_MISC73].doomednum = -1;
    mobjinfo[MT_SMALLPILLAR].doomednum = 29;

    mobjinfo[MT_MISC74].doomednum = -1;
    mobjinfo[MT_SKULLHANG45].doomednum = 25;

    mobjinfo[MT_MISC75].doomednum = -1;
    mobjinfo[MT_SKULLHANG35].doomednum = 26;

    mobjinfo[MT_MISC76].doomednum = -1;
    mobjinfo[MT_AMBLSRWIMPY].doomednum = 54;

    mobjinfo[MT_MISC77].doomednum = -1;
    mobjinfo[MT_BEAST].doomednum = 70;

    mobjinfo[MT_MISC78].doomednum = -1;
    mobjinfo[MT_AKYY].doomednum = 73;

    mobjinfo[MT_MISC79].doomednum = -1;
    mobjinfo[MT_TELEGLITGEN].doomednum = 74;

    mobjinfo[MT_MISC80].doomednum = -1;
    mobjinfo[MT_ARTIINVISIBILITY].doomednum = 75;

    mobjinfo[MT_MISC81].doomednum = -1;
    mobjinfo[MT_HMISC6].doomednum = 76;

    mobjinfo[MT_MISC84].doomednum = -1;
    mobjinfo[MT_BKYY].doomednum = 79;

    mobjinfo[MT_MISC85].doomednum = -1;
    mobjinfo[MT_CKEY].doomednum = 80;

    mobjinfo[MT_MISC86].doomednum = -1;
    mobjinfo[MT_HMISC0].doomednum = 81;
}

mobj_t LavaInflictor;

//----------------------------------------------------------------------------
//
// PROC P_InitLava
//
//----------------------------------------------------------------------------

void P_InitLava(void)
{
        memset(&LavaInflictor, 0, sizeof(mobj_t));
        LavaInflictor.type = MT_PHOENIXFX2;
        LavaInflictor.flags2 = MF2_FIREDAMAGE|MF2_NODMGTHRUST;
}

//----------------------------------------------------------------------------
//
// PROC P_HerePlayerInSpecialSector
//
// Called every tic frame that the player origin is in a special sector.
//
//----------------------------------------------------------------------------

void P_HerePlayerInSpecialSector(player_t *player)
{
    sector_t *sector;
    static int pushTab[5] = {
        2048*5,
        2048*10,
        2048*25,
        2048*30,
        2048*35
    };
    
    sector = player->mo->subsector->sector;
    // Player is not touching the floor
    if( player->mo->z != sector->floorheight )
        return;
    
    switch(sector->special)
    {
    case 7: // Damage_Sludge
        if(!(leveltime&31))
        {
            P_DamageMobj(player->mo, NULL, NULL, 4);
        }
        break;
    case 5: // Damage_LavaWimpy
        if(!(leveltime&15))
        {
            P_DamageMobj(player->mo, &LavaInflictor, NULL, 5);
            P_HitFloor(player->mo);
        }
        break;
    case 16: // Damage_LavaHefty
        if(!(leveltime&15))
        {
            P_DamageMobj(player->mo, &LavaInflictor, NULL, 8);
            P_HitFloor(player->mo);
        }
        break;
    case 4: // Scroll_EastLavaDamage
        P_Thrust(player, 0, 2048*28);
        if(!(leveltime&15))
        {
            P_DamageMobj(player->mo, &LavaInflictor, NULL, 5);
            P_HitFloor(player->mo);
        }
        break;
    case 9: // SecretArea
        player->secretcount++;
        sector->special = 0;
        break;
    case 11: // Exit_SuperDamage (DOOM E1M8 finale)
             /*
             player->cheats &= ~CF_GODMODE;
             if(!(leveltime&0x1f))
             {
             P_DamageMobj(player->mo, NULL, NULL, 20);
             }
             if(player->health <= 10)
             {
             G_ExitLevel();
             }
        */
        break;
        
    case 25: case 26: case 27: case 28: case 29: // Scroll_North
        P_Thrust(player, ANG90, pushTab[sector->special-25]);
        break;
    case 20: case 21: case 22: case 23: case 24: // Scroll_East
        P_Thrust(player, 0, pushTab[sector->special-20]);
        break;
    case 30: case 31: case 32: case 33: case 34: // Scroll_South
        P_Thrust(player, ANG270, pushTab[sector->special-30]);
        break;
    case 35: case 36: case 37: case 38: case 39: // Scroll_West
        P_Thrust(player, ANG180, pushTab[sector->special-35]);
        break;
        
    case 40: case 41: case 42: case 43: case 44: case 45:
    case 46: case 47: case 48: case 49: case 50: case 51:
        // Wind specials are handled in (P_mobj):P_XYMovement
        break;
        
    case 15: // Friction_Low
        // Only used in (P_mobj):P_XYMovement and (P_user):P_Thrust
        break;
        
    default:
        CONS_Printf("P_PlayerInSpecialSector: "
                    "unknown special %i\n", sector->special);
    }
}

//---------------------------------------------------------------------------
//
// FUNC P_GetThingFloorType
//
//---------------------------------------------------------------------------
int P_GetThingFloorType(mobj_t *thing)
{
    return thing->subsector->sector->floortype;
}
