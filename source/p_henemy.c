// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_henemy.c,v 1.6 2001/08/12 15:21:04 bpereira Exp $
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
// $Log: p_henemy.c,v $
// Revision 1.6  2001/08/12 15:21:04  bpereira
// see my log
//
// Revision 1.5  2001/05/27 13:42:47  bpereira
// no message
//
// Revision 1.4  2001/04/17 22:26:07  calumr
// Initial Mac add
//
// Revision 1.3  2001/02/24 13:35:20  bpereira
// no message
//
//
//
// DESCRIPTION:
//   this file is include by P_enemy.c
//   it contain all heretic enemey specific
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "doomstat.h"
#include "g_game.h"
#include "am_map.h"
#include "g_input.h"
#include "m_cheat.h"
#include "p_local.h"
#include "r_defs.h"
#include "v_video.h"
#include "st_stuff.h"
#include "i_system.h"
#include "i_video.h"
#include "r_state.h"
#include "dstrings.h"
#include "keys.h"
#include "w_wad.h"
#include "z_zone.h"
#include "r_draw.h"
#include "r_main.h"
#include "p_info.h"
#include "d_main.h"
#include "sounds.h"

// Macros

#define MAX_BOSS_SPOTS 8


// Types

typedef struct
{
        fixed_t x;
        fixed_t y;
        angle_t angle;
} BossSpot_t;

// Private Data

static int BossSpotCount;
static BossSpot_t BossSpots[MAX_BOSS_SPOTS];

// Proto

extern boolean P_CheckMeleeRange(mobj_t* actor);

//----------------------------------------------------------------------------
//
// PROC P_InitMonsters
//
// Called at level load.
//
//----------------------------------------------------------------------------

void P_InitMonsters(void)
{
        BossSpotCount = 0;
}

//----------------------------------------------------------------------------
//
// PROC P_AddBossSpot
//
//----------------------------------------------------------------------------

void P_AddBossSpot(fixed_t x, fixed_t y, angle_t angle)
{
    if(BossSpotCount == MAX_BOSS_SPOTS)
    {
        // BP:not a critical problem 
        CONS_Printf("Too many boss spots.");
        return;
    }
    BossSpots[BossSpotCount].x = x;
    BossSpots[BossSpotCount].y = y;
    BossSpots[BossSpotCount].angle = angle;
    BossSpotCount++;
}


//---------------------------------------------------------------------------
//
// FUNC P_LookForMonsters
//
//---------------------------------------------------------------------------

#define MONS_LOOK_RANGE (20*64*FRACUNIT)
#define MONS_LOOK_LIMIT 64

boolean P_LookForMonsters(mobj_t *actor)
{
        int count;
        mobj_t *mo;
        thinker_t *think;

        if(!P_CheckSight(players[0].mo, actor))
        { // Player can't see monster
                return(false);
        }
        count = 0;
        for(think = thinkercap.next; think != &thinkercap; think = think->next)
        {
                if(think->function.acp1 != (actionf_p1)P_MobjThinker)
                { // Not a mobj thinker
                        continue;
                }
                mo = (mobj_t *)think;
                if(!(mo->flags&MF_COUNTKILL) || (mo == actor) || (mo->health <= 0))
                { // Not a valid monster
                        continue;
                }
                if(P_AproxDistance(actor->x-mo->x, actor->y-mo->y)
                        > MONS_LOOK_RANGE)
                { // Out of range
                        continue;
                }
                if(P_Random() < 16)
                { // Skip
                        continue;
                }
                if(count++ > MONS_LOOK_LIMIT)
                { // Stop searching
                        return(false);
                }
                if(!P_CheckSight(actor, mo))
                { // Out of sight
                        continue;
                }
                // Found a target monster
                actor->target = mo;
                return(true);
        }
        return(false);
}

/*
===============================================================================

                                                ACTION ROUTINES

===============================================================================
*/


//----------------------------------------------------------------------------
//
// PROC A_DripBlood
//
//----------------------------------------------------------------------------

void A_DripBlood(mobj_t *actor)
{
        mobj_t *mo;
        int r,s;

        // evaluation order isn't define in C
        r = P_SignedRandom();
        s = P_SignedRandom();
        mo = P_SpawnMobj(actor->x+(r<<11),
                         actor->y+(s<<11), actor->z, MT_BLOOD);
        mo->momx = P_SignedRandom()<<10;
        mo->momy = P_SignedRandom()<<10;
        mo->flags2 |= MF2_LOGRAV;
}

//----------------------------------------------------------------------------
//
// PROC A_KnightAttack
//
//----------------------------------------------------------------------------

void A_KnightAttack(mobj_t *actor)
{
        if(!actor->target)
        {
                return;
        }
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(3));
                S_StartSound(actor, sfx_kgtat2);
                return;
        }
        // Throw axe
        S_StartSound(actor, actor->info->attacksound);
        if(actor->type == MT_KNIGHTGHOST || P_Random() < 40)
        { // Red axe
                P_SpawnMissile(actor, actor->target, MT_REDAXE);
                return;
        }
        // Green axe
        P_SpawnMissile(actor, actor->target, MT_KNIGHTAXE);
}

//----------------------------------------------------------------------------
//
// PROC A_ImpExplode
//
//----------------------------------------------------------------------------

void A_ImpExplode(mobj_t *actor)
{
        mobj_t *mo;

        mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_IMPCHUNK1);
        mo->momx = P_SignedRandom()<<10;
        mo->momy = P_SignedRandom()<<10;
        mo->momz = 9*FRACUNIT;
        mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_IMPCHUNK2);
        mo->momx = P_SignedRandom()<<10;
        mo->momy = P_SignedRandom()<<10;
        mo->momz = 9*FRACUNIT;
        if(actor->special1 == 666)
        { // Extreme death crash
            P_SetMobjState(actor, S_IMP_XCRASH1);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_BeastPuff
//
//----------------------------------------------------------------------------

void A_BeastPuff(mobj_t *actor)
{
    if(P_Random() > 64)
    {
        int r,s,t;
        r = P_SignedRandom();
        s = P_SignedRandom();
        t = P_SignedRandom();
        
        P_SpawnMobj(actor->x+(r<<10),
                    actor->y+(s<<10),
                    actor->z+(t<<10), MT_PUFFY);
    }
}

//----------------------------------------------------------------------------
//
// PROC A_ImpMeAttack
//
//----------------------------------------------------------------------------

void A_ImpMeAttack(mobj_t *actor)
{
        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, 5+(P_Random()&7));
        }
}

//----------------------------------------------------------------------------
//
// PROC A_ImpMsAttack
//
//----------------------------------------------------------------------------

void A_ImpMsAttack(mobj_t *actor)
{
        mobj_t *dest;
        angle_t an;
        int dist;

        if(!actor->target || P_Random() > 64)
        {
                P_SetMobjState(actor, actor->info->seestate);
                return;
        }
        dest = actor->target;
        actor->flags |= MF_SKULLFLY;
        S_StartSound(actor, actor->info->attacksound);
        A_FaceTarget(actor);
        an = actor->angle >> ANGLETOFINESHIFT;
        actor->momx = FixedMul(12*FRACUNIT, finecosine[an]);
        actor->momy = FixedMul(12*FRACUNIT, finesine[an]);
        dist = P_AproxDistance(dest->x-actor->x, dest->y-actor->y);
        dist = dist/(12*FRACUNIT);
        if(dist < 1)
        {
                dist = 1;
        }
        actor->momz = (dest->z+(dest->height>>1)-actor->z)/dist;
}

//----------------------------------------------------------------------------
//
// PROC A_ImpMsAttack2
//
// Fireball attack of the imp leader.
//
//----------------------------------------------------------------------------

void A_ImpMsAttack2(mobj_t *actor)
{
        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, 5+(P_Random()&7));
                return;
        }
        P_SpawnMissile(actor, actor->target, MT_IMPBALL);
}

//----------------------------------------------------------------------------
//
// PROC A_ImpDeath
//
//----------------------------------------------------------------------------

void A_ImpDeath(mobj_t *actor)
{
        actor->flags &= ~MF_SOLID;
        actor->flags2 |= MF2_FOOTCLIP;

        actor->flags  |= MF_CORPSE|MF_DROPOFF;
        actor->height >>= 2;
        actor->radius -= (actor->radius>>4);      //for solid corpses

        if(actor->z <= actor->floorz)
        {
            P_SetMobjState(actor, S_IMP_CRASH1);
            actor->flags &= ~MF_CORPSE;
        }
}

//----------------------------------------------------------------------------
//
// PROC A_ImpXDeath1
//
//----------------------------------------------------------------------------

void A_ImpXDeath1(mobj_t *actor)
{
        actor->flags &= ~MF_SOLID;
        actor->flags |= MF_NOGRAVITY;
        actor->flags  |= MF_CORPSE|MF_DROPOFF;
        actor->height >>= 2;
        actor->radius -= (actor->radius>>4);      //for solid corpses

        actor->flags2 |= MF2_FOOTCLIP;
        actor->special1 = 666; // Flag the crash routine
}

//----------------------------------------------------------------------------
//
// PROC A_ImpXDeath2
//
//----------------------------------------------------------------------------

void A_ImpXDeath2(mobj_t *actor)
{
        actor->flags &= ~MF_NOGRAVITY;
        if(actor->z <= actor->floorz)
        {
            P_SetMobjState(actor, S_IMP_CRASH1);
            actor->flags &= ~MF_CORPSE;
        }
}

//----------------------------------------------------------------------------
//
// FUNC P_UpdateChicken
//
// Returns true if the chicken morphs.
//
//----------------------------------------------------------------------------

#define TELEFOGHEIGHT (32*FRACUNIT)
boolean P_UpdateChicken(mobj_t *actor, int tics)
{
        mobj_t *fog;
        fixed_t x;
        fixed_t y;
        fixed_t z;
        mobjtype_t moType;
        mobj_t *mo;
        mobj_t oldChicken;

        actor->special1 -= tics;
        if(actor->special1 > 0)
        {
                return(false);
        }
        moType = actor->special2;
        x = actor->x;
        y = actor->y;
        z = actor->z;
        oldChicken = *actor;
        P_SetMobjState(actor, S_FREETARGMOBJ);
        mo = P_SpawnMobj(x, y, z, moType);
        if(P_TestMobjLocation(mo) == false)
        { // Didn't fit
                P_RemoveMobj(mo);
                mo = P_SpawnMobj(x, y, z, MT_CHICKEN);
                mo->angle = oldChicken.angle;
                mo->flags = oldChicken.flags;
                mo->health = oldChicken.health;
                mo->target = oldChicken.target;
                mo->special1 = 5*TICRATE; // Next try in 5 seconds
                mo->special2 = moType;
                return(false);
        }
        mo->angle = oldChicken.angle;
        mo->target = oldChicken.target;
        fog = P_SpawnMobj(x, y, z+TELEFOGHEIGHT, MT_TFOG);
        S_StartSound(fog, sfx_telept);
        return(true);
}

//----------------------------------------------------------------------------
//
// PROC A_ChicAttack
//
//----------------------------------------------------------------------------

void A_ChicAttack(mobj_t *actor)
{
        if(P_UpdateChicken(actor, 18))
        {
                return;
        }
        if(!actor->target)
        {
                return;
        }
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, 1+(P_Random()&1));
        }
}

//----------------------------------------------------------------------------
//
// PROC A_ChicLook
//
//----------------------------------------------------------------------------

void A_ChicLook(mobj_t *actor)
{
        if(P_UpdateChicken(actor, 10))
        {
                return;
        }
        A_Look(actor);
}

//----------------------------------------------------------------------------
//
// PROC A_ChicChase
//
//----------------------------------------------------------------------------

void A_ChicChase(mobj_t *actor)
{
        if(P_UpdateChicken(actor, 3))
        {
                return;
        }
        A_Chase(actor);
}

//----------------------------------------------------------------------------
//
// PROC A_ChicPain
//
//----------------------------------------------------------------------------

void A_ChicPain(mobj_t *actor)
{
        if(P_UpdateChicken(actor, 10))
        {
                return;
        }
        S_StartSound(actor, actor->info->painsound);
}

//----------------------------------------------------------------------------
//
// PROC A_Feathers
//
//----------------------------------------------------------------------------

void A_Feathers(mobj_t *actor)
{
        int i;
        int count;
        mobj_t *mo;

        if(actor->health > 0)
        { // Pain
                count = P_Random() < 32 ? 2 : 1;
        }
        else
        { // Death
                count = 5+(P_Random()&3);
        }
        for(i = 0; i < count; i++)
        {
                mo = P_SpawnMobj(actor->x, actor->y, actor->z+20*FRACUNIT,
                        MT_FEATHER);
                mo->target = actor;
                mo->momx = P_SignedRandom()<<8;
                mo->momy = P_SignedRandom()<<8;
                mo->momz = FRACUNIT+(P_Random()<<9);
                P_SetMobjState(mo, S_FEATHER1+(P_Random()&7));
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MummyAttack
//
//----------------------------------------------------------------------------

void A_MummyAttack(mobj_t *actor)
{
        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(2));
                S_StartSound(actor, sfx_mumat2);
                return;
        }
        S_StartSound(actor, sfx_mumat1);
}

//----------------------------------------------------------------------------
//
// PROC A_MummyAttack2
//
// Mummy leader missile attack.
//
//----------------------------------------------------------------------------

void A_MummyAttack2(mobj_t *actor)
{
        mobj_t *mo;

        if(!actor->target)
        {
                return;
        }
        //S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(2));
                return;
        }
        mo = P_SpawnMissile(actor, actor->target, MT_MUMMYFX1);
        //mo = P_SpawnMissile(actor, actor->target, MT_EGGFX);
        if(mo != NULL)
        {
                mo->tracer = actor->target;
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MummyFX1Seek
//
//----------------------------------------------------------------------------

void A_MummyFX1Seek(mobj_t *actor)
{
        P_SeekerMissile(actor, ANGLE_1*10, ANGLE_1*20);
}

//----------------------------------------------------------------------------
//
// PROC A_MummySoul
//
//----------------------------------------------------------------------------

void A_MummySoul(mobj_t *mummy)
{
        mobj_t *mo;

        mo = P_SpawnMobj(mummy->x, mummy->y, mummy->z+10*FRACUNIT, MT_MUMMYSOUL);
        mo->momz = FRACUNIT;
}

//----------------------------------------------------------------------------
//
// PROC A_Sor1Pain
//
//----------------------------------------------------------------------------

void A_Sor1Pain(mobj_t *actor)
{
        actor->special1 = 20; // Number of steps to walk fast
        A_Pain(actor);
}

//----------------------------------------------------------------------------
//
// PROC A_Sor1Chase
//
//----------------------------------------------------------------------------

void A_Sor1Chase(mobj_t *actor)
{
        if(actor->special1)
        {
                actor->special1--;
                actor->tics -= 3;
        }
        A_Chase(actor);
}

//----------------------------------------------------------------------------
//
// PROC A_Srcr1Attack
//
// Sorcerer demon attack.
//
//----------------------------------------------------------------------------

void A_Srcr1Attack(mobj_t *actor)
{
        mobj_t *mo;
        fixed_t momz;
        angle_t angle;

        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(8));
                return;
        }
        if(actor->health > (actor->info->spawnhealth/3)*2)
        { // Spit one fireball
                P_SpawnMissile(actor, actor->target, MT_SRCRFX1);
        }
        else
        { // Spit three fireballs
                mo = P_SpawnMissile(actor, actor->target, MT_SRCRFX1);
                if(mo)
                {
                        momz = mo->momz;
                        angle = mo->angle;
                        P_SpawnMissileAngle(actor, MT_SRCRFX1, angle-ANGLE_1*3, momz);
                        P_SpawnMissileAngle(actor, MT_SRCRFX1, angle+ANGLE_1*3, momz);
                }
                if(actor->health < actor->info->spawnhealth/3)
                { // Maybe attack again
                        if(actor->special1)
                        { // Just attacked, so don't attack again
                                actor->special1 = 0;
                        }
                        else
                        { // Set state to attack again
                                actor->special1 = 1;
                                P_SetMobjState(actor, S_SRCR1_ATK4);
                        }
                }
        }
}

//----------------------------------------------------------------------------
//
// PROC A_SorcererRise
//
//----------------------------------------------------------------------------

void A_SorcererRise(mobj_t *actor)
{
        mobj_t *mo;

        actor->flags &= ~MF_SOLID;
        mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_SORCERER2);
        P_SetMobjState(mo, S_SOR2_RISE1);
        mo->angle = actor->angle;
        mo->target = actor->target;
}

//----------------------------------------------------------------------------
//
// PROC P_DSparilTeleport
//
//----------------------------------------------------------------------------

void P_DSparilTeleport(mobj_t *actor)
{
        int i;
        fixed_t x;
        fixed_t y;
        fixed_t prevX;
        fixed_t prevY;
        fixed_t prevZ;
        mobj_t *mo;

        if(!BossSpotCount)
        { // No spots
                return;
        }
        i = P_Random();
        do
        {
                i++;
                x = BossSpots[i%BossSpotCount].x;
                y = BossSpots[i%BossSpotCount].y;
        } while(P_AproxDistance(actor->x-x, actor->y-y) < 128*FRACUNIT);
        prevX = actor->x;
        prevY = actor->y;
        prevZ = actor->z;
        if(P_TeleportMove(actor, x, y))
        {
                mo = P_SpawnMobj(prevX, prevY, prevZ, MT_SOR2TELEFADE);
                S_StartSound(mo, sfx_telept);
                P_SetMobjState(actor, S_SOR2_TELE1);
                S_StartSound(actor, sfx_telept);
                actor->z = actor->floorz;
                actor->angle = BossSpots[i%BossSpotCount].angle;
                actor->momx = actor->momy = actor->momz = 0;
        }
}

//----------------------------------------------------------------------------
//
// PROC A_Srcr2Decide
//
//----------------------------------------------------------------------------

void A_Srcr2Decide(mobj_t *actor)
{
        static int chance[] =
        {
                192, 120, 120, 120, 64, 64, 32, 16, 0
        };

        if(!BossSpotCount)
        { // No spots
                return;
        }
        if(P_Random() < chance[actor->health/(actor->info->spawnhealth/8)])
        {
                P_DSparilTeleport(actor);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_Srcr2Attack
//
//----------------------------------------------------------------------------

void A_Srcr2Attack(mobj_t *actor)
{
        int chance;

        if(!actor->target)
        {
                return;
        }
        S_StartSound(NULL, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(20));
                return;
        }
        chance = actor->health < actor->info->spawnhealth/2 ? 96 : 48;
        if(P_Random() < chance)
        { // Wizard spawners
                P_SpawnMissileAngle(actor, MT_SOR2FX2,
                        actor->angle-ANG45, FRACUNIT/2);
                P_SpawnMissileAngle(actor, MT_SOR2FX2,
                        actor->angle+ANG45, FRACUNIT/2);
        }
        else
        { // Blue bolt
                P_SpawnMissile(actor, actor->target, MT_SOR2FX1);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_BlueSpark
//
//----------------------------------------------------------------------------

void A_BlueSpark(mobj_t *actor)
{
    int i;
    mobj_t *mo;
    
    for(i = 0; i < 2; i++)
    {
        mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_SOR2FXSPARK);
        mo->momx = P_SignedRandom()<<9;
        mo->momy = P_SignedRandom()<<9;
        mo->momz = FRACUNIT+(P_Random()<<8);
    }
}

//----------------------------------------------------------------------------
//
// PROC A_GenWizard
//
//----------------------------------------------------------------------------

void A_GenWizard(mobj_t *actor)
{
        mobj_t *mo;
        mobj_t *fog;

        mo = P_SpawnMobj(actor->x, actor->y,
                actor->z-mobjinfo[MT_WIZARD].height/2, MT_WIZARD);
        if(P_TestMobjLocation(mo) == false)
        { // Didn't fit
                P_RemoveMobj(mo);
                return;
        }
        actor->momx = actor->momy = actor->momz = 0;
        P_SetMobjState(actor, mobjinfo[actor->type].deathstate);
        actor->flags &= ~MF_MISSILE;
        fog = P_SpawnMobj(actor->x, actor->y, actor->z, MT_TFOG);
        S_StartSound(fog, sfx_telept);
}

//----------------------------------------------------------------------------
//
// PROC A_Sor2DthInit
//
//----------------------------------------------------------------------------

void A_Sor2DthInit(mobj_t *actor)
{
        actor->special1 = 7; // Animation loop counter
        P_Massacre(); // Kill monsters early
}

//----------------------------------------------------------------------------
//
// PROC A_Sor2DthLoop
//
//----------------------------------------------------------------------------

void A_Sor2DthLoop(mobj_t *actor)
{
        if(--actor->special1)
        { // Need to loop
                P_SetMobjState(actor, S_SOR2_DIE4);
        }
}

//----------------------------------------------------------------------------
//
// D'Sparil Sound Routines
//
//----------------------------------------------------------------------------

void A_SorZap(mobj_t *actor) {S_StartSound(NULL, sfx_sorzap);}
void A_SorRise(mobj_t *actor) {S_StartSound(NULL, sfx_sorrise);}
void A_SorDSph(mobj_t *actor) {S_StartSound(NULL, sfx_sordsph);}
void A_SorDExp(mobj_t *actor) {S_StartSound(NULL, sfx_sordexp);}
void A_SorDBon(mobj_t *actor) {S_StartSound(NULL, sfx_sordbon);}
void A_SorSightSnd(mobj_t *actor) {S_StartSound(NULL, sfx_sorsit);}

//----------------------------------------------------------------------------
//
// PROC A_MinotaurAtk1
//
// Melee attack.
//
//----------------------------------------------------------------------------

void A_MinotaurAtk1(mobj_t *actor)
{
        player_t *player;

        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, sfx_stfpow);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(4));
                if((player = actor->target->player) != NULL)
                { // Squish the player
                        player->deltaviewheight = -16*FRACUNIT;
                }
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MinotaurDecide
//
// Choose a missile attack.
//
//----------------------------------------------------------------------------

#define MNTR_CHARGE_SPEED (13*FRACUNIT)

void A_MinotaurDecide(mobj_t *actor)
{
        angle_t angle;
        mobj_t *target;
        int dist;

        target = actor->target;
        if(!target)
        {
                return;
        }
        S_StartSound(actor, sfx_minsit);
        dist = P_AproxDistance(actor->x-target->x, actor->y-target->y);
        if(target->z+target->height > actor->z
                && target->z+target->height < actor->z+actor->height
                && dist < 8*64*FRACUNIT
                && dist > 1*64*FRACUNIT
                && P_Random() < 150)
        { // Charge attack
                // Don't call the state function right away
                P_SetMobjStateNF(actor, S_MNTR_ATK4_1);
                actor->flags |= MF_SKULLFLY;
                A_FaceTarget(actor);
                angle = actor->angle>>ANGLETOFINESHIFT;
                actor->momx = FixedMul(MNTR_CHARGE_SPEED, finecosine[angle]);
                actor->momy = FixedMul(MNTR_CHARGE_SPEED, finesine[angle]);
                actor->special1 = TICRATE/2; // Charge duration
        }
        else if(target->z == target->floorz
                && dist < 9*64*FRACUNIT
                && P_Random() < 220)
        { // Floor fire attack
                P_SetMobjState(actor, S_MNTR_ATK3_1);
                actor->special2 = 0;
        }
        else
        { // Swing attack
                A_FaceTarget(actor);
                // Don't need to call P_SetMobjState because the current state
                // falls through to the swing attack
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MinotaurCharge
//
//----------------------------------------------------------------------------

void A_MinotaurCharge(mobj_t *actor)
{
        mobj_t *puff;

        if(actor->special1)
        {
                puff = P_SpawnMobj(actor->x, actor->y, actor->z, MT_PHOENIXPUFF);
                puff->momz = 2*FRACUNIT;
                actor->special1--;
        }
        else
        {
                actor->flags &= ~MF_SKULLFLY;
                P_SetMobjState(actor, actor->info->seestate);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MinotaurAtk2
//
// Swing attack.
//
//----------------------------------------------------------------------------

void A_MinotaurAtk2(mobj_t *actor)
{
        mobj_t *mo;
        angle_t angle;
        fixed_t momz;

        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, sfx_minat2);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(5));
                return;
        }
        mo = P_SpawnMissile(actor, actor->target, MT_MNTRFX1);
        if(mo)
        {
                S_StartSound(mo, sfx_minat2);
                momz = mo->momz;
                angle = mo->angle;
                P_SpawnMissileAngle(actor, MT_MNTRFX1, angle-(ANG45/8), momz);
                P_SpawnMissileAngle(actor, MT_MNTRFX1, angle+(ANG45/8), momz);
                P_SpawnMissileAngle(actor, MT_MNTRFX1, angle-(ANG45/16), momz);
                P_SpawnMissileAngle(actor, MT_MNTRFX1, angle+(ANG45/16), momz);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MinotaurAtk3
//
// Floor fire attack.
//
//----------------------------------------------------------------------------

void A_MinotaurAtk3(mobj_t *actor)
{
        mobj_t *mo;
        player_t *player;

        if(!actor->target)
        {
                return;
        }
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(5));
                if((player = actor->target->player) != NULL)
                { // Squish the player
                        player->deltaviewheight = -16*FRACUNIT;
                }
        }
        else
        {
                mo = P_SpawnMissile(actor, actor->target, MT_MNTRFX2);
                if(mo != NULL)
                {
                        S_StartSound(mo, sfx_minat1);
                }
        }
        if(P_Random() < 192 && actor->special2 == 0)
        {
                P_SetMobjState(actor, S_MNTR_ATK3_4);
                actor->special2 = 1;
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MntrFloorFire
//
//----------------------------------------------------------------------------

void A_MntrFloorFire(mobj_t *actor)
{
    mobj_t *mo;
    int r,s;
    
    actor->z = actor->floorz;
    r = P_SignedRandom();
    s = P_SignedRandom();
    mo = P_SpawnMobj(actor->x+(r<<10),
                     actor->y+(s<<10), ONFLOORZ, MT_MNTRFX3);
    mo->target = actor->target;
    mo->momx = 1; // Force block checking
    P_CheckMissileSpawn(mo);
}

//----------------------------------------------------------------------------
//
// PROC A_BeastAttack
//
//----------------------------------------------------------------------------

void A_BeastAttack(mobj_t *actor)
{
        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(3));
                return;
        }
        P_SpawnMissile(actor, actor->target, MT_BEASTBALL);
}

//----------------------------------------------------------------------------
//
// PROC A_HeadAttack
//
//----------------------------------------------------------------------------

void A_HHeadAttack(mobj_t *actor)
{
        int i;
        mobj_t *fire;
        mobj_t *baseFire;
        mobj_t *mo;
        mobj_t *target;
        int randAttack;
        static int atkResolve1[] = { 50, 150 };
        static int atkResolve2[] = { 150, 200 };
        int dist;

        // Ice ball             (close 20% : far 60%)
        // Fire column  (close 40% : far 20%)
        // Whirlwind    (close 40% : far 20%)
        // Distance threshold = 8 cells

        target = actor->target;
        if(target == NULL)
        {
                return;
        }
        A_FaceTarget(actor);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(target, actor, actor, HITDICE(6));
                return;
        }
        dist = P_AproxDistance(actor->x-target->x, actor->y-target->y)
                > 8*64*FRACUNIT;
        randAttack = P_Random();
        if(randAttack < atkResolve1[dist])
        { // Ice ball
                P_SpawnMissile(actor, target, MT_HEADFX1);
                S_StartSound(actor, sfx_hedat2);
        }
        else if(randAttack < atkResolve2[dist])
        { // Fire column
                baseFire = P_SpawnMissile(actor, target, MT_HEADFX3);
                if(baseFire != NULL)
                {
                        P_SetMobjState(baseFire, S_HHEADFX3_4); // Don't grow
                        for(i = 0; i < 5; i++)
                        {
                                fire = P_SpawnMobj(baseFire->x, baseFire->y,
                                        baseFire->z, MT_HEADFX3);
                                if(i == 0)
                                {
                                        S_StartSound(actor, sfx_hedat1);
                                }
                                fire->target = baseFire->target;
                                fire->angle = baseFire->angle;
                                fire->momx = baseFire->momx;
                                fire->momy = baseFire->momy;
                                fire->momz = baseFire->momz;
                                //fire->damage = 0;
                                fire->health = (i+1)*2;
                                P_CheckMissileSpawn(fire);
                        }
                }
        }
        else
        { // Whirlwind
                mo = P_SpawnMissile(actor, target, MT_WHIRLWIND);
                if(mo != NULL)
                {
                        mo->z -= 32*FRACUNIT;
                        mo->tracer = target;
                        mo->special2 = 50; // Timer for active sound
                        mo->health = 20*TICRATE; // Duration
                        S_StartSound(actor, sfx_hedat3);
                }
        }
}

//----------------------------------------------------------------------------
//
// PROC A_WhirlwindSeek
//
//----------------------------------------------------------------------------

void A_WhirlwindSeek(mobj_t *actor)
{
        actor->health -= 3;
        if(actor->health < 0)
        {
                actor->momx = actor->momy = actor->momz = 0;
                P_SetMobjState(actor, mobjinfo[actor->type].deathstate);
                actor->flags &= ~MF_MISSILE;
                return;
        }
        if((actor->special2 -= 3) < 0)
        {
                actor->special2 = 58+(P_Random()&31);
                S_StartSound(actor, sfx_hedat3);
        }
        if(actor->tracer
                && actor->tracer->flags&MF_SHADOW)
        {
                return;
        }
        P_SeekerMissile(actor, ANGLE_1*10, ANGLE_1*30);
}

//----------------------------------------------------------------------------
//
// PROC A_HeadIceImpact
//
//----------------------------------------------------------------------------

void A_HeadIceImpact(mobj_t *ice)
{
        int i;
        angle_t angle;
        mobj_t *shard;

        for(i = 0; i < 8; i++)
        {
                shard = P_SpawnMobj(ice->x, ice->y, ice->z, MT_HEADFX2);
                angle = i*ANG45;
                shard->target = ice->target;
                shard->angle = angle;
                angle >>= ANGLETOFINESHIFT;
                shard->momx = FixedMul(shard->info->speed, finecosine[angle]);
                shard->momy = FixedMul(shard->info->speed, finesine[angle]);
                shard->momz = -.6*FRACUNIT;
                P_CheckMissileSpawn(shard);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_HeadFireGrow
//
//----------------------------------------------------------------------------

void A_HeadFireGrow(mobj_t *fire)
{
        fire->health--;
        fire->z += 9*FRACUNIT;
        if(fire->health == 0)
        {
                //fire->damage = fire->info->damage;
                P_SetMobjState(fire, S_HHEADFX3_4);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_SnakeAttack
//
//----------------------------------------------------------------------------

void A_SnakeAttack(mobj_t *actor)
{
        if(!actor->target)
        {
                P_SetMobjState(actor, S_SNAKE_WALK1);
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        A_FaceTarget(actor);
        P_SpawnMissile(actor, actor->target, MT_SNAKEPRO_A);
}

//----------------------------------------------------------------------------
//
// PROC A_SnakeAttack2
//
//----------------------------------------------------------------------------

void A_SnakeAttack2(mobj_t *actor)
{
        if(!actor->target)
        {
                P_SetMobjState(actor, S_SNAKE_WALK1);
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        A_FaceTarget(actor);
        P_SpawnMissile(actor, actor->target, MT_SNAKEPRO_B);
}

//----------------------------------------------------------------------------
//
// PROC A_ClinkAttack
//
//----------------------------------------------------------------------------

void A_ClinkAttack(mobj_t *actor)
{
        int damage;

        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                damage = ((P_Random()%7)+3);
                P_DamageMobj(actor->target, actor, actor, damage);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_GhostOff
//
//----------------------------------------------------------------------------

void A_GhostOff(mobj_t *actor)
{
        actor->flags &= ~MF_SHADOW;
}

//----------------------------------------------------------------------------
//
// PROC A_WizAtk1
//
//----------------------------------------------------------------------------

void A_WizAtk1(mobj_t *actor)
{
        A_FaceTarget(actor);
        actor->flags &= ~MF_SHADOW;
}

//----------------------------------------------------------------------------
//
// PROC A_WizAtk2
//
//----------------------------------------------------------------------------

void A_WizAtk2(mobj_t *actor)
{
        A_FaceTarget(actor);
        actor->flags |= MF_SHADOW;
}

//----------------------------------------------------------------------------
//
// PROC A_WizAtk3
//
//----------------------------------------------------------------------------

void A_WizAtk3(mobj_t *actor)
{
        mobj_t *mo;
        angle_t angle;
        fixed_t momz;

        actor->flags &= ~MF_SHADOW;
        if(!actor->target)
        {
                return;
        }
        S_StartSound(actor, actor->info->attacksound);
        if(P_CheckMeleeRange(actor))
        {
                P_DamageMobj(actor->target, actor, actor, HITDICE(4));
                return;
        }
        mo = P_SpawnMissile(actor, actor->target, MT_WIZFX1);
        if(mo)
        {
                momz = mo->momz;
                angle = mo->angle;
                P_SpawnMissileAngle(actor, MT_WIZFX1, angle-(ANG45/8), momz);
                P_SpawnMissileAngle(actor, MT_WIZFX1, angle+(ANG45/8), momz);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_Scream
//
//----------------------------------------------------------------------------

void A_HScream(mobj_t *actor)
{
        switch(actor->type)
        {
                case MT_CHICPLAYER:
                case MT_SORCERER1:
                case MT_MINOTAUR:
                        // Make boss death sounds full volume
                        S_StartSound(NULL, actor->info->deathsound);
                        break;
                case MT_PLAYER:
                        // Handle the different player death screams
                        if(actor->special1 < 10)
                        { // Wimpy death sound
                                S_StartSound(actor, sfx_plrwdth);
                        }
                        else if(actor->health > -50)
                        { // Normal death sound
                                S_StartSound(actor, actor->info->deathsound);
                        }
                        else if(actor->health > -100)
                        { // Crazy death sound
                                S_StartSound(actor, sfx_plrcdth);
                        }
                        else
                        { // Extreme death sound
                                S_StartSound(actor, sfx_gibdth);
                        }
                        break;
                default:
                        S_StartSound(actor, actor->info->deathsound);
                        break;
        }
}

//---------------------------------------------------------------------------
//
// PROC P_DropItem
//
//---------------------------------------------------------------------------

void P_DropItem(mobj_t *source, mobjtype_t type, int special, int chance)
{
        mobj_t *mo;

        if(P_Random() > chance)
        {
                return;
        }
        mo = P_SpawnMobj(source->x, source->y,
                         source->z+(source->height>>1), type);
        mo->momx = P_SignedRandom()<<8;
        mo->momy = P_SignedRandom()<<8;
        mo->momz = FRACUNIT*5+(P_Random()<<10);
        mo->flags |= MF_DROPPED;
        mo->health = special;
}

//----------------------------------------------------------------------------
//
// PROC A_NoBlocking
//
//----------------------------------------------------------------------------

void A_NoBlocking(mobj_t *actor)
{
    A_Fall(actor);
    // Check for monsters dropping things
    switch(actor->type)
    {
        case MT_MUMMY:
        case MT_MUMMYLEADER:
        case MT_MUMMYGHOST:
        case MT_MUMMYLEADERGHOST:
            P_DropItem(actor, MT_AMGWNDWIMPY, 3, 84);
            break;
        case MT_KNIGHT:
        case MT_KNIGHTGHOST:
            P_DropItem(actor, MT_AMCBOWWIMPY, 5, 84);
            break;
        case MT_WIZARD:
            P_DropItem(actor, MT_AMBLSRWIMPY, 10, 84);
            P_DropItem(actor, MT_ARTITOMEOFPOWER, 0, 4);
            break;
        case MT_HHEAD:
            P_DropItem(actor, MT_AMBLSRWIMPY, 10, 84);
            P_DropItem(actor, MT_ARTIEGG, 0, 51);
            break;
        case MT_BEAST:
            P_DropItem(actor, MT_AMCBOWWIMPY, 10, 84);
            break;
        case MT_CLINK:
            P_DropItem(actor, MT_AMSKRDWIMPY, 20, 84);
            break;
        case MT_SNAKE:
            P_DropItem(actor, MT_AMPHRDWIMPY, 5, 84);
            break;
        case MT_MINOTAUR:
            P_DropItem(actor, MT_ARTISUPERHEAL, 0, 51);
            P_DropItem(actor, MT_AMPHRDWIMPY, 10, 84);
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------
//
// PROC A_PodPain
//
//----------------------------------------------------------------------------

void A_PodPain(mobj_t *actor)
{
    int i;
    int count;
    int chance;
    mobj_t *goo;
    
    chance = P_Random();
    if(chance < 128)
    {
        return;
    }
    count = chance > 240 ? 2 : 1;
    for(i = 0; i < count; i++)
    {
        goo = P_SpawnMobj(actor->x, actor->y,
            actor->z+48*FRACUNIT, MT_PODGOO);
        goo->target = actor;
        goo->momx = P_SignedRandom()<<9;
        goo->momy = P_SignedRandom()<<9;
        goo->momz = FRACUNIT/2+(P_Random()<<9);
    }
}

//----------------------------------------------------------------------------
//
// PROC A_RemovePod
//
//----------------------------------------------------------------------------

void A_RemovePod(mobj_t *actor)
{
        mobj_t *mo;

        if(actor->special2)
        {
                mo = (mobj_t *)actor->special2;
                if(mo->special1 > 0)
                {
                        mo->special1--;
                }
        }
}

//----------------------------------------------------------------------------
//
// PROC A_MakePod
//
//----------------------------------------------------------------------------

#define MAX_GEN_PODS 16

void A_MakePod(mobj_t *actor)
{
        mobj_t *mo;
        fixed_t x;
        fixed_t y;
        fixed_t z;

        if(actor->special1 == MAX_GEN_PODS)
        { // Too many generated pods
                return;
        }
        x = actor->x;
        y = actor->y;
        z = actor->z;
        mo = P_SpawnMobj(x, y, ONFLOORZ, MT_POD);
        if(P_CheckPosition(mo, x, y) == false)
        { // Didn't fit
                P_RemoveMobj(mo);
                return;
        }
        P_SetMobjState(mo, S_POD_GROW1);
        P_ThrustMobj(mo, P_Random()<<24, (fixed_t)(4.5*FRACUNIT));
        S_StartSound(mo, sfx_newpod);
        actor->special1++; // Increment generated pod count
        mo->special2 = (int)actor; // Link the generator to the pod
        return;
}

//----------------------------------------------------------------------------
//
// PROC P_Massacre
//
// Kills all monsters.
//
//----------------------------------------------------------------------------

void P_Massacre(void)
{
        mobj_t *mo;
        thinker_t *think;

        for(think = thinkercap.next; think != &thinkercap;
                think = think->next)
        {
                if(think->function.acp1 != (actionf_p1)P_MobjThinker)
                { // Not a mobj thinker
                        continue;
                }
                mo = (mobj_t *)think;
                if((mo->flags&MF_COUNTKILL) && (mo->health > 0))
                {
                        P_DamageMobj(mo, NULL, NULL, 10000);
                }
        }
}

//----------------------------------------------------------------------------
//
// PROC A_BossDeath
//
// Trigger special effects if all bosses are dead.
//
//----------------------------------------------------------------------------

void A_HBossDeath(mobj_t *actor)
{
        mobj_t *mo;
        thinker_t *think;
        line_t dummyLine;
        static const mobjtype_t bossType[6] =
        {
                MT_HHEAD,
                MT_MINOTAUR,
                MT_SORCERER2,
                MT_HHEAD,
                MT_MINOTAUR,
                -1
        };

        if(gamemap != 8)
        { // Not a boss level
                return;
        }
        if(actor->type != bossType[gameepisode-1])
        { // Not considered a boss in this episode
                return;
        }
        // Make sure all bosses are dead
        for(think = thinkercap.next; think != &thinkercap; think = think->next)
        {
                if(think->function.acp1 != (actionf_p1)P_MobjThinker)
                { // Not a mobj thinker
                        continue;
                }
                mo = (mobj_t *)think;
                if((mo != actor) && (mo->type == actor->type) && !(mo->flags & MF_CORPSE))
                { // Found a living boss
                        return;
                }
        }
        if(gameepisode > 1)
        { // Kill any remaining monsters
                P_Massacre();
        }
        dummyLine.tag = 666;
        EV_DoFloor(&dummyLine, lowerFloor);
}

//----------------------------------------------------------------------------
//
// PROC A_ESound
//
//----------------------------------------------------------------------------

void A_ESound(mobj_t *mo)
{
        int sound;

        switch(mo->type)
        {
                case MT_SOUNDWATERFALL:
                        sound = sfx_waterfl;
                        break;
                case MT_SOUNDWIND:
                        sound = sfx_wind;
                        break;
                default:
                        sound = sfx_None;
                        break;
        }
        S_StartSound(mo, sound);
}

//----------------------------------------------------------------------------
//
// PROC A_SpawnTeleGlitter
//
//----------------------------------------------------------------------------

void A_SpawnTeleGlitter(mobj_t *actor)
{
        mobj_t *mo;
        int r,s;

        r = P_Random();
        s = P_Random();
        mo = P_SpawnMobj(actor->x+((r&31)-16)*FRACUNIT,
                         actor->y+((s&31)-16)*FRACUNIT,
                         actor->subsector->sector->floorheight, MT_TELEGLITTER);
        mo->momz = FRACUNIT/4;
}

//----------------------------------------------------------------------------
//
// PROC A_SpawnTeleGlitter2
//
//----------------------------------------------------------------------------

void A_SpawnTeleGlitter2(mobj_t *actor)
{
        mobj_t *mo;
        int r,s;

        r = P_Random();
        s = P_Random();
        mo = P_SpawnMobj(actor->x+((r&31)-16)*FRACUNIT,
                         actor->y+((s&31)-16)*FRACUNIT,
                         actor->subsector->sector->floorheight, MT_TELEGLITTER2);
        mo->momz = FRACUNIT/4;
}

//----------------------------------------------------------------------------
//
// PROC A_AccTeleGlitter
//
//----------------------------------------------------------------------------

void A_AccTeleGlitter(mobj_t *actor)
{
        if(++actor->health > 35)
        {
                actor->momz += actor->momz/2;
        }
}

//----------------------------------------------------------------------------
//
// PROC A_InitKeyGizmo
//
//----------------------------------------------------------------------------

void A_InitKeyGizmo(mobj_t *gizmo)
{
        mobj_t *mo;
        statenum_t state;

        switch(gizmo->type)
        {
                case MT_KEYGIZMOBLUE:
                        state = S_KGZ_BLUEFLOAT1;
                        break;
                case MT_KEYGIZMOGREEN:
                        state = S_KGZ_GREENFLOAT1;
                        break;
                case MT_KEYGIZMOYELLOW:
                        state = S_KGZ_YELLOWFLOAT1;
                        break;
                default:
                        state = S_NULL;
                        break;
        }
        mo = P_SpawnMobj(gizmo->x, gizmo->y, gizmo->z+60*FRACUNIT,
                MT_KEYGIZMOFLOAT);
        P_SetMobjState(mo, state);
}

//----------------------------------------------------------------------------
//
// PROC A_VolcanoSet
//
//----------------------------------------------------------------------------

void A_VolcanoSet(mobj_t *volcano)
{
        volcano->tics = 105+(P_Random()&127);
}

//----------------------------------------------------------------------------
//
// PROC A_VolcanoBlast
//
//----------------------------------------------------------------------------

void A_VolcanoBlast(mobj_t *volcano)
{
        int i;
        int count;
        mobj_t *blast;
        angle_t angle;

        count = 1+(P_Random()%3);
        for(i = 0; i < count; i++)
        {
                blast = P_SpawnMobj(volcano->x, volcano->y,
                        volcano->z+44*FRACUNIT, MT_VOLCANOBLAST); // MT_VOLCANOBLAST
                blast->target = volcano;
                angle = P_Random()<<24;
                blast->angle = angle;
                angle >>= ANGLETOFINESHIFT;
                blast->momx = FixedMul(1*FRACUNIT, finecosine[angle]);
                blast->momy = FixedMul(1*FRACUNIT, finesine[angle]);
                blast->momz = (2.5*FRACUNIT)+(P_Random()<<10);
                S_StartSound(blast, sfx_volsht);
                P_CheckMissileSpawn(blast);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_VolcBallImpact
//
//----------------------------------------------------------------------------

void A_VolcBallImpact(mobj_t *ball)
{
        int i;
        mobj_t *tiny;
        angle_t angle;

        if(ball->z <= ball->floorz)
        {
                ball->flags |= MF_NOGRAVITY;
                ball->flags2 &= ~MF2_LOGRAV;
                ball->z += 28*FRACUNIT;
                //ball->momz = 3*FRACUNIT;
        }
        P_RadiusAttack(ball, ball->target, 25);
        for(i = 0; i < 4; i++)
        {
                tiny = P_SpawnMobj(ball->x, ball->y, ball->z, MT_VOLCANOTBLAST);
                tiny->target = ball;
                angle = i*ANG90;
                tiny->angle = angle;
                angle >>= ANGLETOFINESHIFT;
                tiny->momx = FixedMul(FRACUNIT*.7, finecosine[angle]);
                tiny->momy = FixedMul(FRACUNIT*.7, finesine[angle]);
                tiny->momz = FRACUNIT+(P_Random()<<9);
                P_CheckMissileSpawn(tiny);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_SkullPop
//
//----------------------------------------------------------------------------

void A_SkullPop(mobj_t *actor)
{
        mobj_t *mo;
        player_t *player;

        actor->flags &= ~MF_SOLID;
        mo = P_SpawnMobj(actor->x, actor->y, actor->z+48*FRACUNIT,
                MT_BLOODYSKULL);
        //mo->target = actor;
        mo->momx = P_SignedRandom()<<9;
        mo->momy = P_SignedRandom()<<9;
        mo->momz = FRACUNIT*2+(P_Random()<<6);
        // Attach player mobj to bloody skull
        player = actor->player;
        actor->player = NULL;
        mo->player = player;
        mo->health = actor->health;
        mo->angle = actor->angle;
        player->mo = mo;
        player->aiming = 0;
        player->damagecount = 32;
}

//----------------------------------------------------------------------------
//
// PROC A_CheckSkullFloor
//
//----------------------------------------------------------------------------

void A_CheckSkullFloor(mobj_t *actor)
{
        if(actor->z <= actor->floorz)
        {
                P_SetMobjState(actor, S_BLOODYSKULLX1);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_CheckSkullDone
//
//----------------------------------------------------------------------------

void A_CheckSkullDone(mobj_t *actor)
{
        if(actor->special2 == 666)
        {
                P_SetMobjState(actor, S_BLOODYSKULLX2);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_CheckBurnGone
//
//----------------------------------------------------------------------------

void A_CheckBurnGone(mobj_t *actor)
{
        if(actor->special2 == 666)
        {
                //P_SetMobjState(actor, S_PLAY_FDTH20);
        }
}

//----------------------------------------------------------------------------
//
// PROC A_FreeTargMobj
//
//----------------------------------------------------------------------------

void A_FreeTargMobj(mobj_t *mo)
{
        mo->momx = mo->momy = mo->momz = 0;
        mo->z = mo->ceilingz+4*FRACUNIT;
        mo->flags &= ~(MF_SHOOTABLE|MF_FLOAT|MF_SKULLFLY|MF_SOLID);
        mo->flags |= MF_CORPSE|MF_DROPOFF|MF_NOGRAVITY;
        mo->flags2 &= ~(MF2_PASSMOBJ|MF2_LOGRAV);
        mo->player = NULL;
}

//----------------------------------------------------------------------------
//
// PROC A_AddPlayerCorpse
//
//----------------------------------------------------------------------------

/*#define BODYQUESIZE 32
mobj_t *bodyque[BODYQUESIZE];
int bodyqueslot;*/

void A_AddPlayerCorpse(mobj_t *actor)
{
        if(bodyqueslot >= BODYQUESIZE)
        { // Too many player corpses - remove an old one
                P_RemoveMobj(bodyque[bodyqueslot%BODYQUESIZE]);
        }
        bodyque[bodyqueslot%BODYQUESIZE] = actor;
        bodyqueslot++;
}

//----------------------------------------------------------------------------
//
// PROC A_FlameSnd
//
//----------------------------------------------------------------------------

void A_FlameSnd(mobj_t *actor)
{
        S_StartSound(actor, sfx_hedat1); // Burn sound
}
