// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_hpspr.c,v 1.3 2001/05/27 13:42:47 bpereira Exp $
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
// $Log: p_hpspr.c,v $
// Revision 1.3  2001/05/27 13:42:47  bpereira
// no message
//
// Revision 1.2  2001/02/24 13:35:20  bpereira
// no message
//
//
//
// DESCRIPTION:
//   this file is include by P_pspr.c
//   it contain all heretic player sprite specific
//
//-----------------------------------------------------------------------------

// P_pspr.c
#include "doomdef.h"
#include "g_game.h"
#include "p_local.h"
#include "r_main.h"
#include "r_state.h"
#include "s_sound.h"
#include "m_random.h"
// Macros

#define FLAME_THROWER_TICS      (10*TICRATE)
#define LOWERSPEED FRACUNIT*6
#define RAISESPEED FRACUNIT*6
#define WEAPONBOTTOM 128*FRACUNIT
#define WEAPONTOP 32*FRACUNIT
#define MAGIC_JUNK 1234
#define MAX_MACE_SPOTS 8

mobjtype_t PuffType;

static int MaceSpotCount;
static struct
{
        fixed_t x;
        fixed_t y;
} MaceSpots[MAX_MACE_SPOTS];

fixed_t bulletslope;


weaponinfo_t wpnlev1info[NUMWEAPONS] =
{
        { // Staff
                am_noammo,              // ammo
                0,
                S_STAFFUP,              // upstate
                S_STAFFDOWN,            // downstate
                S_STAFFREADY,           // readystate
                S_STAFFATK1_1,          // atkstate
                S_STAFFATK1_1,          // holdatkstate
                S_NULL                  // flashstate
        },
        { // Gold wand
                am_goldwand,            // ammo
                USE_GWND_AMMO_1,
                S_GOLDWANDUP,           // upstate
                S_GOLDWANDDOWN,         // downstate
                S_GOLDWANDREADY,        // readystate
                S_GOLDWANDATK1_1,       // atkstate
                S_GOLDWANDATK1_1,       // holdatkstate
                S_NULL                  // flashstate
        },
        { // Crossbow
                am_crossbow,            // ammo
                USE_CBOW_AMMO_1,
                S_CRBOWUP,              // upstate
                S_CRBOWDOWN,            // downstate
                S_CRBOW1,               // readystate
                S_CRBOWATK1_1,          // atkstate
                S_CRBOWATK1_1,          // holdatkstate
                S_NULL                  // flashstate
        },
        { // Blaster
                am_blaster,             // ammo
                USE_BLSR_AMMO_1,
                S_BLASTERUP,            // upstate
                S_BLASTERDOWN,          // downstate
                S_BLASTERREADY,         // readystate
                S_BLASTERATK1_1,        // atkstate
                S_BLASTERATK1_3,        // holdatkstate
                S_NULL                  // flashstate
        },
        { // Skull rod
                am_skullrod,            // ammo
                USE_SKRD_AMMO_1,
                S_HORNRODUP,            // upstate
                S_HORNRODDOWN,          // downstate
                S_HORNRODREADY,         // readystae
                S_HORNRODATK1_1,        // atkstate
                S_HORNRODATK1_1,        // holdatkstate
                S_NULL                  // flashstate
        },
        { // Phoenix rod
                am_phoenixrod,          // ammo
                USE_PHRD_AMMO_1,
                S_PHOENIXUP,            // upstate
                S_PHOENIXDOWN,          // downstate
                S_PHOENIXREADY,         // readystate
                S_PHOENIXATK1_1,        // atkstate
                S_PHOENIXATK1_1,        // holdatkstate
                S_NULL                  // flashstate
        },
        { // Mace
                am_mace,                // ammo
                USE_MACE_AMMO_1,
                S_MACEUP,               // upstate
                S_MACEDOWN,             // downstate
                S_MACEREADY,            // readystate
                S_MACEATK1_1,           // atkstate
                S_MACEATK1_2,           // holdatkstate
                S_NULL                  // flashstate
        },
        { // Gauntlets
                am_noammo,              // ammo
                0,
                S_GAUNTLETUP,           // upstate
                S_GAUNTLETDOWN,         // downstate
                S_GAUNTLETREADY,        // readystate
                S_GAUNTLETATK1_1,       // atkstate
                S_GAUNTLETATK1_3,       // holdatkstate
                S_NULL                  // flashstate
        },
        { // Beak
                am_noammo,              // ammo
                0,
                S_BEAKUP,               // upstate
                S_BEAKDOWN,             // downstate
                S_BEAKREADY,            // readystate
                S_BEAKATK1_1,           // atkstate
                S_BEAKATK1_1,           // holdatkstate
                S_NULL                  // flashstate
        }
};

weaponinfo_t wpnlev2info[NUMWEAPONS] =
{
        { // Staff
                am_noammo,              // ammo
                0,
                S_STAFFUP2,             // upstate
                S_STAFFDOWN2,           // downstate
                S_STAFFREADY2_1,        // readystate
                S_STAFFATK2_1,          // atkstate
                S_STAFFATK2_1,          // holdatkstate
                S_NULL                  // flashstate
        },
        { // Gold wand
                am_goldwand,            // ammo
                USE_GWND_AMMO_2,
                S_GOLDWANDUP,           // upstate
                S_GOLDWANDDOWN,         // downstate
                S_GOLDWANDREADY,        // readystate
                S_GOLDWANDATK2_1,       // atkstate
                S_GOLDWANDATK2_1,       // holdatkstate
                S_NULL                  // flashstate
        },
        { // Crossbow
                am_crossbow,            // ammo
                USE_CBOW_AMMO_2,
                S_CRBOWUP,              // upstate
                S_CRBOWDOWN,            // downstate
                S_CRBOW1,               // readystate
                S_CRBOWATK2_1,          // atkstate
                S_CRBOWATK2_1,          // holdatkstate
                S_NULL                  // flashstate
        },
        { // Blaster
                am_blaster,             // ammo
                USE_BLSR_AMMO_2,
                S_BLASTERUP,            // upstate
                S_BLASTERDOWN,          // downstate
                S_BLASTERREADY,         // readystate
                S_BLASTERATK2_1,        // atkstate
                S_BLASTERATK2_3,        // holdatkstate
                S_NULL                  // flashstate
        },
        { // Skull rod
                am_skullrod,            // ammo
                USE_SKRD_AMMO_2,
                S_HORNRODUP,            // upstate
                S_HORNRODDOWN,          // downstate
                S_HORNRODREADY,         // readystae
                S_HORNRODATK2_1,        // atkstate
                S_HORNRODATK2_1,        // holdatkstate
                S_NULL                  // flashstate
        },
        { // Phoenix rod
                am_phoenixrod,          // ammo
                USE_PHRD_AMMO_2,
                S_PHOENIXUP,            // upstate
                S_PHOENIXDOWN,          // downstate
                S_PHOENIXREADY,         // readystate
                S_PHOENIXATK2_1,        // atkstate
                S_PHOENIXATK2_2,        // holdatkstate
                S_NULL                  // flashstate
        },
        { // Mace
                am_mace,                // ammo
                USE_MACE_AMMO_2,
                S_MACEUP,               // upstate
                S_MACEDOWN,             // downstate
                S_MACEREADY,            // readystate
                S_MACEATK2_1,           // atkstate
                S_MACEATK2_1,           // holdatkstate
                S_NULL                  // flashstate
        },
        { // Gauntlets
                am_noammo,              // ammo
                0,
                S_GAUNTLETUP2,          // upstate
                S_GAUNTLETDOWN2,        // downstate
                S_GAUNTLETREADY2_1,     // readystate
                S_GAUNTLETATK2_1,       // atkstate
                S_GAUNTLETATK2_3,       // holdatkstate
                S_NULL                  // flashstate
        },
        { // Beak
                am_noammo,              // ammo
                0,
                S_BEAKUP,               // upstate
                S_BEAKDOWN,             // downstate
                S_BEAKREADY,            // readystate
                S_BEAKATK2_1,           // atkstate
                S_BEAKATK2_1,           // holdatkstate
                S_NULL                  // flashstate
        }
};

//---------------------------------------------------------------------------
//
// PROC P_OpenWeapons
//
// Called at level load before things are loaded.
//
//---------------------------------------------------------------------------

void P_OpenWeapons(void)
{
        MaceSpotCount = 0;
}

//---------------------------------------------------------------------------
//
// PROC P_AddMaceSpot
//
//---------------------------------------------------------------------------

void P_AddMaceSpot(mapthing_t *mthing)
{
        if(MaceSpotCount == MAX_MACE_SPOTS)
        {
                I_Error("Too many mace spots.");
        }
        MaceSpots[MaceSpotCount].x = mthing->x<<FRACBITS;
        MaceSpots[MaceSpotCount].y = mthing->y<<FRACBITS;
        MaceSpotCount++;
}

//---------------------------------------------------------------------------
//
// PROC P_RepositionMace
//
// Chooses the next spot to place the mace.
//
//---------------------------------------------------------------------------

void P_RepositionMace(mobj_t *mo)
{
        int spot;
        subsector_t *ss;

        P_UnsetThingPosition(mo);
        spot = P_Random()%MaceSpotCount;
        mo->x = MaceSpots[spot].x;
        mo->y = MaceSpots[spot].y;
        ss = R_PointInSubsector(mo->x, mo->y);
        mo->z = mo->floorz = ss->sector->floorheight;
        mo->ceilingz = ss->sector->ceilingheight;
        P_SetThingPosition(mo);
}

//---------------------------------------------------------------------------
//
// PROC P_CloseWeapons
//
// Called at level load after things are loaded.
//
//---------------------------------------------------------------------------

void P_CloseWeapons(void)
{
        int spot;

        if(!MaceSpotCount)
        { // No maces placed
                return;
        }
        if(!cv_deathmatch.value && P_Random() < 64)
        { // Sometimes doesn't show up if not in deathmatch
                return;
        }
        spot = P_Random()%MaceSpotCount;
        P_SpawnMobj(MaceSpots[spot].x, MaceSpots[spot].y, ONFLOORZ, MT_WMACE);
}

//---------------------------------------------------------------------------
//
// PROC P_ActivateBeak
//
//---------------------------------------------------------------------------

void P_ActivateBeak(player_t *player)
{
        player->pendingweapon = wp_nochange;
        player->readyweapon = wp_beak;
        player->psprites[ps_weapon].sy = WEAPONTOP;
        P_SetPsprite(player, ps_weapon, S_BEAKREADY);
}

//---------------------------------------------------------------------------
//
// PROC P_PostChickenWeapon
//
//---------------------------------------------------------------------------

void P_PostChickenWeapon(player_t *player, weapontype_t weapon)
{
        if(weapon == wp_beak)
        { // Should never happen
                weapon = wp_staff;
        }
        player->pendingweapon = wp_nochange;
        player->readyweapon = weapon;
        player->psprites[ps_weapon].sy = WEAPONBOTTOM;
        P_SetPsprite(player, ps_weapon, wpnlev1info[weapon].upstate);
}

//---------------------------------------------------------------------------
//
// PROC P_UpdateBeak
//
//---------------------------------------------------------------------------

void P_UpdateBeak(player_t *player, pspdef_t *psp)
{
        psp->sy = WEAPONTOP+(player->chickenPeck<<(FRACBITS-1));
}

//---------------------------------------------------------------------------
//
// PROC A_BeakReady
//
//---------------------------------------------------------------------------

void A_BeakReady(player_t *player, pspdef_t *psp)
{
        if(player->cmd.buttons&BT_ATTACK)
        { // Chicken beak attack
                player->attackdown = true;
                P_SetMobjState(player->mo, S_CHICPLAY_ATK1);
                if(player->powers[pw_weaponlevel2])
                {
                        P_SetPsprite(player, ps_weapon, S_BEAKATK2_1);
                }
                else
                {
                        P_SetPsprite(player, ps_weapon, S_BEAKATK1_1);
                }
                P_NoiseAlert(player->mo, player->mo);
        }
        else
        {
                if(player->mo->state == &states[S_CHICPLAY_ATK1])
                { // Take out of attack state
                        P_SetMobjState(player->mo, S_CHICPLAY);
                }
                player->attackdown = false;
        }
}

//---------------------------------------------------------------------------
//
// PROC A_BeakRaise
//
//---------------------------------------------------------------------------

void A_BeakRaise(player_t *player, pspdef_t *psp)
{
        psp->sy = WEAPONTOP;
        P_SetPsprite(player, ps_weapon,
                wpnlev1info[player->readyweapon].readystate);
}

//****************************************************************************
//
// WEAPON ATTACKS
//
//****************************************************************************

//----------------------------------------------------------------------------
//
// PROC A_BeakAttackPL1
//
//----------------------------------------------------------------------------

void A_BeakAttackPL1(player_t *player, pspdef_t *psp)
{
        angle_t angle;
        int damage;
        int slope;

        damage = 1+(P_Random()&3);
        angle = player->mo->angle;
        slope = P_AimLineAttack(player->mo, angle, MELEERANGE);
//      PuffType = MT_BEAKPUFF;
        P_LineAttack(player->mo, angle, MELEERANGE, slope, damage);
        if(linetarget)
        {
                player->mo->angle = R_PointToAngle2(player->mo->x,
                        player->mo->y, linetarget->x, linetarget->y);
        }
        S_StartSound(player->mo, sfx_chicpk1+(P_Random()%3));
        player->chickenPeck = 12;
        psp->tics -= P_Random()&7;
}

//----------------------------------------------------------------------------
//
// PROC A_BeakAttackPL2
//
//----------------------------------------------------------------------------

void A_BeakAttackPL2(player_t *player, pspdef_t *psp)
{
        angle_t angle;
        int damage;
        int slope;

        damage = HITDICE(4);
        angle = player->mo->angle;
        slope = P_AimLineAttack(player->mo, angle, MELEERANGE);
        PuffType = MT_BEAKPUFF;
        P_LineAttack(player->mo, angle, MELEERANGE, slope, damage);
        if(linetarget)
        {
                player->mo->angle = R_PointToAngle2(player->mo->x,
                        player->mo->y, linetarget->x, linetarget->y);
        }
        S_StartSound(player->mo, sfx_chicpk1+(P_Random()%3));
        player->chickenPeck = 12;
        psp->tics -= P_Random()&3;
}

//----------------------------------------------------------------------------
//
// PROC A_StaffAttackPL1
//
//----------------------------------------------------------------------------

void A_StaffAttackPL1(player_t *player, pspdef_t *psp)
{
        angle_t angle;
        int damage;
        int slope;

        damage = 5+(P_Random()&15);
        angle = player->mo->angle;
        angle += P_SignedRandom()<<18;
        slope = P_AimLineAttack(player->mo, angle, MELEERANGE);
        PuffType = MT_STAFFPUFF;
        P_LineAttack(player->mo, angle, MELEERANGE, slope, damage);
        if(linetarget)
        {
                //S_StartSound(player->mo, sfx_stfhit);
                // turn to face target
                player->mo->angle = R_PointToAngle2(player->mo->x,
                        player->mo->y, linetarget->x, linetarget->y);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_StaffAttackPL2
//
//----------------------------------------------------------------------------

void A_StaffAttackPL2(player_t *player, pspdef_t *psp)
{
        angle_t angle;
        int damage;
        int slope;

        // P_inter.c:P_DamageMobj() handles target momentums
        damage = 18+(P_Random()&63);
        angle = player->mo->angle;
        angle += P_SignedRandom()<<18;
        slope = P_AimLineAttack(player->mo, angle, MELEERANGE);
        PuffType = MT_STAFFPUFF2;
        P_LineAttack(player->mo, angle, MELEERANGE, slope, damage);
        if(linetarget)
        {
                //S_StartSound(player->mo, sfx_stfpow);
                // turn to face target
                player->mo->angle = R_PointToAngle2(player->mo->x,
                        player->mo->y, linetarget->x, linetarget->y);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_FireBlasterPL1
//
//----------------------------------------------------------------------------

void A_FireBlasterPL1(player_t *player, pspdef_t *psp)
{
    mobj_t *mo;
    angle_t angle;
    int damage;
    
    mo = player->mo;
    S_StartSound(mo, sfx_gldhit);
    player->ammo[am_blaster] -= USE_BLSR_AMMO_1;
    P_BulletSlope(mo);
    damage = HITDICE(4);
    angle = mo->angle;
    if(player->refire)
        angle += P_SignedRandom()<<18;
    PuffType = MT_BLASTERPUFF1;
    P_LineAttack(mo, angle, MISSILERANGE, bulletslope, damage);
    S_StartSound(player->mo, sfx_blssht);
}

//----------------------------------------------------------------------------
//
// PROC P_BlasterMobjThinker
//
// Thinker for the ultra-fast blaster PL2 ripper-spawning missile.
//
//----------------------------------------------------------------------------

void P_BlasterMobjThinker(mobj_t *mobj)
{
        int i;
        fixed_t xfrac;
        fixed_t yfrac;
        fixed_t zfrac;
        fixed_t z;
        boolean changexy;

        // Handle movement
        if(mobj->momx || mobj->momy ||
           (mobj->z != mobj->floorz) || mobj->momz)
        {
                xfrac = mobj->momx>>3;
                yfrac = mobj->momy>>3;
                zfrac = mobj->momz>>3;
                changexy = xfrac || yfrac;
                for(i = 0; i < 8; i++)
                {
                        if(changexy)
                        {
                                if(!P_TryMove(mobj, mobj->x+xfrac, mobj->y+yfrac, true))
                                { // Blocked move
                                        P_ExplodeMissile(mobj);
                                        return;
                                }
                        }
                        mobj->z += zfrac;
                        if(mobj->z <= mobj->floorz)
                        { // Hit the floor
                                mobj->z = mobj->floorz;
                                P_HitFloor(mobj);
                                P_ExplodeMissile(mobj);
                                return;
                        }
                        if(mobj->z+mobj->height > mobj->ceilingz)
                        { // Hit the ceiling
                                mobj->z = mobj->ceilingz-mobj->height;
                                P_ExplodeMissile(mobj);
                                return;
                        }
                        if(changexy && (P_Random() < 64))
                        {
                                z = mobj->z-8*FRACUNIT;
                                if(z < mobj->floorz)
                                {
                                        z = mobj->floorz;
                                }
                                P_SpawnMobj(mobj->x, mobj->y, z, MT_BLASTERSMOKE);
                        }
                }
        }
        // Advance the state
        if(mobj->tics != -1)
        {
                mobj->tics--;
                while(!mobj->tics)
                {
                        if(!P_SetMobjState(mobj, mobj->state->nextstate))
                        { // mobj was removed
                                return;
                        }
                }
        }
}

//----------------------------------------------------------------------------
//
// PROC A_FireBlasterPL2
//
//----------------------------------------------------------------------------

void A_FireBlasterPL2(player_t *player, pspdef_t *psp)
{
        mobj_t *mo;

        player->ammo[am_blaster] -=
                cv_deathmatch.value ? USE_BLSR_AMMO_1 : USE_BLSR_AMMO_2;
        mo = P_SpawnPlayerMissile(player->mo, MT_BLASTERFX1);
        if( mo )
            mo->thinker.function.acp1 = (actionf_p1)P_BlasterMobjThinker;

        S_StartSound(player->mo, sfx_blssht);
}

//----------------------------------------------------------------------------
//
// PROC A_FireGoldWandPL1
//
//----------------------------------------------------------------------------

void A_FireGoldWandPL1(player_t *player, pspdef_t *psp)
{
    mobj_t *mo;
    angle_t angle;
    int damage;
    
    mo = player->mo;
    player->ammo[am_goldwand] -= USE_GWND_AMMO_1;
    P_BulletSlope(mo);
    damage = 7+(P_Random()&7);
    angle = mo->angle;
    if(player->refire)
        angle += P_SignedRandom()<<18;
    PuffType = MT_GOLDWANDPUFF1;
    P_LineAttack(mo, angle, MISSILERANGE, bulletslope, damage);
    S_StartSound(player->mo, sfx_gldhit);
}

//----------------------------------------------------------------------------
//
// PROC A_FireGoldWandPL2
//
//----------------------------------------------------------------------------

void A_FireGoldWandPL2(player_t *player, pspdef_t *psp)
{
        int i;
        mobj_t *mo;
        angle_t angle;
        int damage;
        fixed_t momz;

        mo = player->mo;
        player->ammo[am_goldwand] -=
                cv_deathmatch.value ? USE_GWND_AMMO_1 : USE_GWND_AMMO_2;
        PuffType = MT_GOLDWANDPUFF2;
        P_BulletSlope(mo);
        momz = FixedMul(mobjinfo[MT_GOLDWANDFX2].speed, bulletslope);
//      P_SpawnMissileAngle(mo, MT_GOLDWANDFX2, mo->angle-(ANG45/8), momz);
//      P_SpawnMissileAngle(mo, MT_GOLDWANDFX2, mo->angle+(ANG45/8), momz);
        angle = mo->angle-(ANG45/8);
        for(i = 0; i < 5; i++)
        {
                damage = 1+(P_Random()&7);
                P_LineAttack(mo, angle, MISSILERANGE, bulletslope, damage);
                angle += ((ANG45/8)*2)/4;
        }
        S_StartSound(player->mo, sfx_gldhit);
}

//----------------------------------------------------------------------------
//
// PROC A_FireMacePL1B
//
//----------------------------------------------------------------------------

void A_FireMacePL1B(player_t *player, pspdef_t *psp)
{
        mobj_t *pmo;
        mobj_t *ball;
        angle_t angle;

        if(player->ammo[am_mace] < USE_MACE_AMMO_1)
        {
                return;
        }
        player->ammo[am_mace] -= USE_MACE_AMMO_1;
        pmo = player->mo;
        ball = P_SpawnMobj(pmo->x, pmo->y, pmo->z+28*FRACUNIT
                - FOOTCLIPSIZE*((pmo->flags2&MF2_FEETARECLIPPED) != 0), MT_MACEFX2);
        ball->momz = 2*FRACUNIT+((player->aiming)<<(FRACBITS-5));
        angle = pmo->angle;
        ball->target = pmo;
        ball->angle = angle;
        ball->z += (player->aiming)<<(FRACBITS-4);
        angle >>= ANGLETOFINESHIFT;
        ball->momx = (pmo->momx>>1)
                +FixedMul(ball->info->speed, finecosine[angle]);
        ball->momy = (pmo->momy>>1)
                +FixedMul(ball->info->speed, finesine[angle]);
        S_StartSound(ball, sfx_lobsht);
        P_CheckMissileSpawn(ball);
}

//----------------------------------------------------------------------------
//
// PROC A_FireMacePL1
//
//----------------------------------------------------------------------------

void A_FireMacePL1(player_t *player, pspdef_t *psp)
{
        mobj_t *ball;

        if(P_Random() < 28)
        {
                A_FireMacePL1B(player, psp);
                return;
        }
        if(player->ammo[am_mace] < USE_MACE_AMMO_1)
        {
                return;
        }
        player->ammo[am_mace] -= USE_MACE_AMMO_1;
        psp->sx = ((P_Random()&3)-2)*FRACUNIT;
        psp->sy = WEAPONTOP+(P_Random()&3)*FRACUNIT;
        ball = P_SPMAngle(player->mo, MT_MACEFX1, player->mo->angle
                                                  +(((P_Random()&7)-4)<<24));
        if(ball)
        {
                ball->special1 = 16; // tics till dropoff
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MacePL1Check
//
//----------------------------------------------------------------------------

void A_MacePL1Check(mobj_t *ball)
{
        angle_t angle;

        if(ball->special1 == 0)
        {
                return;
        }
        ball->special1 -= 4;
        if(ball->special1 > 0)
        {
                return;
        }
        ball->special1 = 0;
        ball->flags2 |= MF2_LOGRAV;
        angle = ball->angle>>ANGLETOFINESHIFT;
        ball->momx = FixedMul(7*FRACUNIT, finecosine[angle]);
        ball->momy = FixedMul(7*FRACUNIT, finesine[angle]);
        ball->momz -= ball->momz>>1;
}

//----------------------------------------------------------------------------
//
// PROC A_MaceBallImpact
//
//----------------------------------------------------------------------------

void A_MaceBallImpact(mobj_t *ball)
{
        if((ball->z <= ball->floorz) && (P_HitFloor(ball) != FLOOR_SOLID))
        { // Landed in some sort of liquid
                P_RemoveMobj(ball);
                return;
        }
        if((ball->health != MAGIC_JUNK) && (ball->z <= ball->floorz)
                && ball->momz)
        { // Bounce
                ball->health = MAGIC_JUNK;
                ball->momz = (ball->momz*192)>>8;
                ball->flags2 &= ~MF2_FLOORBOUNCE;
                P_SetMobjState(ball, ball->info->spawnstate);
                S_StartSound(ball, sfx_bounce);
        }
        else
        { // Explode
                ball->flags |= MF_NOGRAVITY;
                ball->flags2 &= ~MF2_LOGRAV;
                S_StartSound(ball, sfx_lobhit);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MaceBallImpact2
//
//----------------------------------------------------------------------------

void A_MaceBallImpact2(mobj_t *ball)
{
        mobj_t *tiny;
        angle_t angle;

        if((ball->z <= ball->floorz) && (P_HitFloor(ball) != FLOOR_SOLID))
        { // Landed in some sort of liquid
                P_RemoveMobj(ball);
                return;
        }
        if((ball->z != ball->floorz) || (ball->momz < 2*FRACUNIT))
        { // Explode
                ball->momx = ball->momy = ball->momz = 0;
                ball->flags |= MF_NOGRAVITY;
                ball->flags2 &= ~(MF2_LOGRAV|MF2_FLOORBOUNCE);
        }
        else
        { // Bounce
                ball->momz = (ball->momz*192)>>8;
                P_SetMobjState(ball, ball->info->spawnstate);

                tiny = P_SpawnMobj(ball->x, ball->y, ball->z, MT_MACEFX3);
                angle = ball->angle+ANG90;
                tiny->target = ball->target;
                tiny->angle = angle;
                angle >>= ANGLETOFINESHIFT;
                tiny->momx = (ball->momx>>1)+FixedMul(ball->momz-FRACUNIT,
                        finecosine[angle]);
                tiny->momy = (ball->momy>>1)+FixedMul(ball->momz-FRACUNIT,
                        finesine[angle]);
                tiny->momz = ball->momz;
                P_CheckMissileSpawn(tiny);

                tiny = P_SpawnMobj(ball->x, ball->y, ball->z, MT_MACEFX3);
                angle = ball->angle-ANG90;
                tiny->target = ball->target;
                tiny->angle = angle;
                angle >>= ANGLETOFINESHIFT;
                tiny->momx = (ball->momx>>1)+FixedMul(ball->momz-FRACUNIT,
                        finecosine[angle]);
                tiny->momy = (ball->momy>>1)+FixedMul(ball->momz-FRACUNIT,
                        finesine[angle]);
                tiny->momz = ball->momz;
                P_CheckMissileSpawn(tiny);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_FireMacePL2
//
//----------------------------------------------------------------------------

void A_FireMacePL2(player_t *player, pspdef_t *psp)
{
        mobj_t *mo;

        player->ammo[am_mace] -=
                cv_deathmatch.value ? USE_MACE_AMMO_1 : USE_MACE_AMMO_2;
        mo = P_SpawnPlayerMissile(player->mo, MT_MACEFX4);
        if(mo)
        {
                mo->momx += player->mo->momx;
                mo->momy += player->mo->momy;
                mo->momz = 2*FRACUNIT+((player->aiming)<<(FRACBITS-5));
                if(linetarget)
                {
                        mo->tracer = linetarget;
                }
        }
        S_StartSound(player->mo, sfx_lobsht);
}

//----------------------------------------------------------------------------
//
// PROC A_DeathBallImpact
//
//----------------------------------------------------------------------------

void A_DeathBallImpact(mobj_t *ball)
{
        int i;
        mobj_t *target;
        angle_t angle = 0;
        boolean newAngle;

        if((ball->z <= ball->floorz) && (P_HitFloor(ball) != FLOOR_SOLID))
        { // Landed in some sort of liquid
                P_RemoveMobj(ball);
                return;
        }
        if((ball->z <= ball->floorz) && ball->momz)
        { // Bounce
                newAngle = false;
                target = ball->tracer;
                if(target)
                {
                        if(!(target->flags&MF_SHOOTABLE))
                        { // Target died
                                ball->tracer = 0;
                        }
                        else
                        { // Seek
                                angle = R_PointToAngle2(ball->x, ball->y,
                                        target->x, target->y);
                                newAngle = true;
                        }
                }
                else
                { // Find new target
                        for(i = 0; i < 16; i++)
                        {
                                P_AimLineAttack(ball, angle, 10*64*FRACUNIT);
                                if(linetarget && ball->target != linetarget)
                                {
                                        ball->tracer = linetarget;
                                        angle = R_PointToAngle2(ball->x, ball->y,
                                                linetarget->x, linetarget->y);
                                        newAngle = true;
                                        break;
                                }
                                angle += ANGLE_45/2;
                        }
                }
                if(newAngle)
                {
                        ball->angle = angle;
                        angle >>= ANGLETOFINESHIFT;
                        ball->momx = FixedMul(ball->info->speed, finecosine[angle]);
                        ball->momy = FixedMul(ball->info->speed, finesine[angle]);
                }
                P_SetMobjState(ball, ball->info->spawnstate);
                S_StartSound(ball, sfx_pstop);
        }
        else
        { // Explode
                ball->flags |= MF_NOGRAVITY;
                ball->flags2 &= ~MF2_LOGRAV;
                S_StartSound(ball, sfx_phohit);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_SpawnRippers
//
//----------------------------------------------------------------------------

void A_SpawnRippers(mobj_t *actor)
{
        int i;
        angle_t angle;
        mobj_t *ripper;

        for(i = 0; i < 8; i++)
        {
                ripper = P_SpawnMobj(actor->x, actor->y, actor->z, MT_RIPPER);
                angle = i*ANG45;
                ripper->target = actor->target;
                ripper->angle = angle;
                angle >>= ANGLETOFINESHIFT;
                ripper->momx = FixedMul(ripper->info->speed, finecosine[angle]);
                ripper->momy = FixedMul(ripper->info->speed, finesine[angle]);
                P_CheckMissileSpawn(ripper);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_FireCrossbowPL1
//
//----------------------------------------------------------------------------

void A_FireCrossbowPL1(player_t *player, pspdef_t *psp)
{
        mobj_t *pmo;

        pmo = player->mo;
        player->ammo[am_crossbow] -= USE_CBOW_AMMO_1;
        P_SpawnPlayerMissile(pmo, MT_CRBOWFX1);
        P_SPMAngle(pmo, MT_CRBOWFX3, pmo->angle-(ANG45/10));
        P_SPMAngle(pmo, MT_CRBOWFX3, pmo->angle+(ANG45/10));
}

//----------------------------------------------------------------------------
//
// PROC A_FireCrossbowPL2
//
//----------------------------------------------------------------------------

void A_FireCrossbowPL2(player_t *player, pspdef_t *psp)
{
        mobj_t *pmo;

        pmo = player->mo;
        player->ammo[am_crossbow] -=
                cv_deathmatch.value ? USE_CBOW_AMMO_1 : USE_CBOW_AMMO_2;
        P_SpawnPlayerMissile(pmo, MT_CRBOWFX2);
        P_SPMAngle(pmo, MT_CRBOWFX2, pmo->angle-(ANG45/10));
        P_SPMAngle(pmo, MT_CRBOWFX2, pmo->angle+(ANG45/10));
        P_SPMAngle(pmo, MT_CRBOWFX3, pmo->angle-(ANG45/5));
        P_SPMAngle(pmo, MT_CRBOWFX3, pmo->angle+(ANG45/5));
}

//----------------------------------------------------------------------------
//
// PROC A_BoltSpark
//
//----------------------------------------------------------------------------

void A_BoltSpark(mobj_t *bolt)
{
    mobj_t *spark;
    
    if(P_Random() > 50)
    {
        spark = P_SpawnMobj(bolt->x, bolt->y, bolt->z, MT_CRBOWFX4);
        spark->x += P_SignedRandom()<<10;
        spark->y += P_SignedRandom()<<10;
    }
}

//----------------------------------------------------------------------------
//
// PROC A_FireSkullRodPL1
//
//----------------------------------------------------------------------------

void A_FireSkullRodPL1(player_t *player, pspdef_t *psp)
{
        mobj_t *mo;

        if(player->ammo[am_skullrod] < USE_SKRD_AMMO_1)
        {
                return;
        }
        player->ammo[am_skullrod] -= USE_SKRD_AMMO_1;
        mo = P_SpawnPlayerMissile(player->mo, MT_HORNRODFX1);
        // Randomize the first frame
        if(mo && P_Random() > 128)
        {
                P_SetMobjState(mo, S_HRODFX1_2);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_FireSkullRodPL2
//
// The special2 field holds the player number that shot the rain missile.
// The tracer field is used for the seeking routines, then as a counter
// for the sound looping.
//
//----------------------------------------------------------------------------

void A_FireSkullRodPL2(player_t *player, pspdef_t *psp)
{
    mobj_t *MissileMobj;

    player->ammo[am_skullrod] -=
        cv_deathmatch.value ? USE_SKRD_AMMO_1 : USE_SKRD_AMMO_2;
    MissileMobj = P_SpawnPlayerMissile(player->mo, MT_HORNRODFX2);
    // Use MissileMobj instead of the return value from
    // P_SpawnPlayerMissile because we need to give info to the mobj
    // even if it exploded immediately.
    if( MissileMobj )
    {
        if(multiplayer)
        { // Multi-player game
            MissileMobj->special2 = player-players;
        }
        else
        { // Always use red missiles in single player games
            MissileMobj->special2 = 2;
        }
        if(linetarget)
            MissileMobj->tracer = linetarget;
        
        S_StartSound(MissileMobj, sfx_hrnpow);
    }
}

//----------------------------------------------------------------------------
//
// PROC A_SkullRodPL2Seek
//
//----------------------------------------------------------------------------

void A_SkullRodPL2Seek(mobj_t *actor)
{
        P_SeekerMissile(actor, ANGLE_1*10, ANGLE_1*30);
}

//----------------------------------------------------------------------------
//
// PROC A_AddPlayerRain
//
//----------------------------------------------------------------------------

void A_AddPlayerRain(mobj_t *actor)
{
        int playerNum;
        player_t *player;

        playerNum = multiplayer ? actor->special2 : 0;
        if(!playeringame[playerNum])
        { // Player left the game
                return;
        }
        player = &players[playerNum];
        if(player->health <= 0)
        { // Player is dead
                return;
        }
        if(player->rain1 && player->rain2)
        { // Terminate an active rain
                if(player->rain1->health < player->rain2->health)
                {
                        if(player->rain1->health > 16)
                        {
                                player->rain1->health = 16;
                        }
                        player->rain1 = NULL;
                }
                else
                {
                        if(player->rain2->health > 16)
                        {
                                player->rain2->health = 16;
                        }
                        player->rain2 = NULL;
                }
        }
        // Add rain mobj to list
        if(player->rain1)
        {
                player->rain2 = actor;
        }
        else
        {
                player->rain1 = actor;
        }
}

//----------------------------------------------------------------------------
//
// PROC A_SkullRodStorm
//
//----------------------------------------------------------------------------

void A_SkullRodStorm(mobj_t *actor)
{
        fixed_t x;
        fixed_t y;
        mobj_t *mo;
        int playerNum;
        player_t *player;

        if(actor->health-- == 0)
        {
                P_SetMobjState(actor, S_NULL);
                playerNum = multiplayer ? actor->special2 : 0;
                if(!playeringame[playerNum])
                { // Player left the game
                        return;
                }
                player = &players[playerNum];
                if(player->health <= 0)
                { // Player is dead
                        return;
                }
                if(player->rain1 == actor)
                {
                        player->rain1 = NULL;
                }
                else if(player->rain2 == actor)
                {
                        player->rain2 = NULL;
                }
                return;
        }
        if(P_Random() < 25)
        { // Fudge rain frequency
                return;
        }
        x = actor->x+((P_Random()&127)-64)*FRACUNIT;
        y = actor->y+((P_Random()&127)-64)*FRACUNIT;
        mo = P_SpawnMobj(x, y, ONCEILINGZ, MT_RAINPLR1+actor->special2);
        mo->target = actor->target;
        mo->momx = 1; // Force collision detection
        mo->momz = -mo->info->speed;
        mo->special2 = actor->special2; // Transfer player number
        P_CheckMissileSpawn(mo);
        if(!(actor->special1&31))
        {
                S_StartSound(actor, sfx_ramrain);
        }
        actor->special1++;
}

//----------------------------------------------------------------------------
//
// PROC A_RainImpact
//
//----------------------------------------------------------------------------

void A_RainImpact(mobj_t *actor)
{
        if(actor->z > actor->floorz)
        {
                P_SetMobjState(actor, S_RAINAIRXPLR1_1+actor->special2);
        }
        else if(P_Random() < 40)
        {
                P_HitFloor(actor);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_HideInCeiling
//
//----------------------------------------------------------------------------

void A_HideInCeiling(mobj_t *actor)
{
        actor->z = actor->ceilingz+4*FRACUNIT;
}

//----------------------------------------------------------------------------
//
// PROC A_FirePhoenixPL1
//
//----------------------------------------------------------------------------

void A_FirePhoenixPL1(player_t *player, pspdef_t *psp)
{
        angle_t angle;

        player->ammo[am_phoenixrod] -= USE_PHRD_AMMO_1;
        P_SpawnPlayerMissile(player->mo, MT_PHOENIXFX1);
        //P_SpawnPlayerMissile(player->mo, MT_MNTRFX2);
        angle = player->mo->angle+ANG180;
        angle >>= ANGLETOFINESHIFT;
        player->mo->momx += FixedMul(4*FRACUNIT, finecosine[angle]);
        player->mo->momy += FixedMul(4*FRACUNIT, finesine[angle]);
}

//----------------------------------------------------------------------------
//
// PROC A_PhoenixPuff
//
//----------------------------------------------------------------------------

void A_PhoenixPuff(mobj_t *actor)
{
        mobj_t *puff;
        angle_t angle;

        P_SeekerMissile(actor, ANGLE_1*5, ANGLE_1*10);
        puff = P_SpawnMobj(actor->x, actor->y, actor->z, MT_PHOENIXPUFF);
        angle = actor->angle+ANG90;
        angle >>= ANGLETOFINESHIFT;
        puff->momx = FixedMul(FRACUNIT*1.3, finecosine[angle]);
        puff->momy = FixedMul(FRACUNIT*1.3, finesine[angle]);
        puff->momz = 0;
        puff = P_SpawnMobj(actor->x, actor->y, actor->z, MT_PHOENIXPUFF);
        angle = actor->angle-ANG90;
        angle >>= ANGLETOFINESHIFT;
        puff->momx = FixedMul(FRACUNIT*1.3, finecosine[angle]);
        puff->momy = FixedMul(FRACUNIT*1.3, finesine[angle]);
        puff->momz = 0;
}

//----------------------------------------------------------------------------
//
// PROC A_InitPhoenixPL2
//
//----------------------------------------------------------------------------

void A_InitPhoenixPL2(player_t *player, pspdef_t *psp)
{
        player->flamecount = FLAME_THROWER_TICS;
}

//----------------------------------------------------------------------------
//
// PROC A_FirePhoenixPL2
//
// Flame thrower effect.
//
//----------------------------------------------------------------------------

void A_FirePhoenixPL2(player_t *player, pspdef_t *psp)
{
        mobj_t *mo;
        mobj_t *pmo;
        angle_t angle;
        fixed_t x, y, z;
        fixed_t slope;

        if(--player->flamecount == 0)
        { // Out of flame
                P_SetPsprite(player, ps_weapon, S_PHOENIXATK2_4);
                player->refire = 0;
                return;
        }
        pmo = player->mo;
        angle = pmo->angle;
        x = pmo->x+(P_SignedRandom()<<9);
        y = pmo->y+(P_SignedRandom()<<9);
        z = pmo->z+26*FRACUNIT+((player->aiming)<<FRACBITS)/173;
        if(pmo->flags2&MF2_FEETARECLIPPED)
        {
                z -= FOOTCLIPSIZE;
        }
        slope = AIMINGTOSLOPE(player->aiming);
        mo = P_SpawnMobj(x, y, z, MT_PHOENIXFX2);
        mo->target = pmo;
        mo->angle = angle;
        mo->momx = pmo->momx+FixedMul(mo->info->speed,
                finecosine[angle>>ANGLETOFINESHIFT]);
        mo->momy = pmo->momy+FixedMul(mo->info->speed,
                finesine[angle>>ANGLETOFINESHIFT]);
        mo->momz = FixedMul(mo->info->speed, slope);
        if(!player->refire || !(leveltime%38))
        {
                S_StartSound(player->mo, sfx_phopow);
        }
        P_CheckMissileSpawn(mo);
}

//----------------------------------------------------------------------------
//
// PROC A_ShutdownPhoenixPL2
//
//----------------------------------------------------------------------------

void A_ShutdownPhoenixPL2(player_t *player, pspdef_t *psp)
{
        player->ammo[am_phoenixrod] -= USE_PHRD_AMMO_2;
}

//----------------------------------------------------------------------------
//
// PROC A_FlameEnd
//
//----------------------------------------------------------------------------

void A_FlameEnd(mobj_t *actor)
{
        actor->momz += 1.5*FRACUNIT;
}

//----------------------------------------------------------------------------
//
// PROC A_FloatPuff
//
//----------------------------------------------------------------------------

void A_FloatPuff(mobj_t *puff)
{
        puff->momz += 1.8*FRACUNIT;
}

//---------------------------------------------------------------------------
//
// PROC A_GauntletAttack
//
//---------------------------------------------------------------------------

void A_GauntletAttack(player_t *player, pspdef_t *psp)
{
        angle_t angle;
        int damage;
        int slope;
        int randVal;
        fixed_t dist;

        psp->sx = ((P_Random()&3)-2)*FRACUNIT;
        psp->sy = WEAPONTOP+(P_Random()&3)*FRACUNIT;
        angle = player->mo->angle;
        if(player->powers[pw_weaponlevel2])
        {
                damage = HITDICE(2);
                dist = 4*MELEERANGE;
                angle += P_SignedRandom()<<17;
                PuffType = MT_GAUNTLETPUFF2;
        }
        else
        {
                damage = HITDICE(2);
                dist = MELEERANGE+1;
                angle += P_SignedRandom()<<18;
                PuffType = MT_GAUNTLETPUFF1;
        }
        slope = P_AimLineAttack(player->mo, angle, dist);
        P_LineAttack(player->mo, angle, dist, slope, damage);
        if(!linetarget)
        {
                if(P_Random() > 64)
                {
                        player->extralight = !player->extralight;
                }
                S_StartSound(player->mo, sfx_gntful);
                return;
        }
        randVal = P_Random();
        if(randVal < 64)
        {
                player->extralight = 0;
        }
        else if(randVal < 160)
        {
                player->extralight = 1;
        }
        else
        {
                player->extralight = 2;
        }
        if(player->powers[pw_weaponlevel2])
        {
                P_GiveBody(player, damage>>1);
                S_StartSound(player->mo, sfx_gntpow);
        }
        else
        {
                S_StartSound(player->mo, sfx_gnthit);
        }
        // turn to face target
        angle = R_PointToAngle2(player->mo->x, player->mo->y,
                linetarget->x, linetarget->y);
        if(angle-player->mo->angle > ANG180)
        {
                if(angle-player->mo->angle < -ANG90/20)
                        player->mo->angle = angle+ANG90/21;
                else
                        player->mo->angle -= ANG90/20;
        }
        else
        {
                if(angle-player->mo->angle > ANG90/20)
                        player->mo->angle = angle-ANG90/21;
                else
                        player->mo->angle += ANG90/20;
        }
        player->mo->flags |= MF_JUSTATTACKED;
}
