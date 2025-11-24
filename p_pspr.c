// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_pspr.c,v 1.11 2003/01/19 21:24:26 bock Exp $
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
// $Log: p_pspr.c,v $
// Revision 1.11  2003/01/19 21:24:26  bock
// Make sources buildable on FreeBSD 5-CURRENT.
//
// Revision 1.10  2001/08/02 19:15:59  bpereira
// fix player reset in secret level of doom2
//
// Revision 1.9  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.8  2001/05/27 13:42:48  bpereira
// no message
//
// Revision 1.7  2001/04/04 20:24:21  judgecutor
// Added support for the 3D Sound
//
// Revision 1.6  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.5  2000/10/21 08:43:30  bpereira
// no message
//
// Revision 1.4  2000/10/01 10:18:18  bpereira
// no message
//
// Revision 1.3  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Weapon sprite animation, weapon objects.
//      Action functions for weapons.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "d_event.h"
#include "p_local.h"
#include "p_pspr.h"
#include "p_inter.h"
#include "s_sound.h"
#include "g_game.h"
#include "g_input.h"
#include "r_main.h"
#include "m_random.h"

#include "hardware/hw3sound.h"

#define LOWERSPEED              FRACUNIT*6
#define RAISESPEED              FRACUNIT*6

#define WEAPONBOTTOM            128*FRACUNIT
#define WEAPONTOP               32*FRACUNIT

#define FLAME_THROWER_TICS      (10*TICRATE)
#define MAGIC_JUNK              1234
#define MAX_MACE_SPOTS          8

//
// P_SetPsprite
//
void P_SetPsprite ( player_t*     player,
                    int           position,
                    statenum_t    stnum )
{
    pspdef_t*   psp;
    state_t*    state;

    psp = &player->psprites[position];

    do
    {
        if (!stnum)
        {
            // object removed itself
            psp->state = NULL;
            break;
        }
#ifdef PARANOIA
        if(stnum>=NUMSTATES)
            I_Error("P_SetPsprite : state %d unknown\n",stnum);
#endif
        state = &states[stnum];
        psp->state = state;
        psp->tics = state->tics;        // could be 0
/* UNUSED
        if (state->misc1)
        {
            // coordinate set
            psp->sx = state->misc1 << FRACBITS;
            psp->sy = state->misc2 << FRACBITS;
        }
*/
        // Call action routine.
        // Modified handling.
        if (state->action.acp2)
        {
            state->action.acp2(player, psp);
            if (!psp->state)
                break;
        }

        stnum = psp->state->nextstate;

    } while (!psp->tics);
    // an initial state of 0 could cycle through
}



//
// P_CalcSwing
//
/* BP: UNUSED

fixed_t         swingx;
fixed_t         swingy;

void P_CalcSwing (player_t*     player)
{
    fixed_t     swing;
    int         angle;

    // OPTIMIZE: tablify this.
    // A LUT would allow for different modes,
    //  and add flexibility.

    swing = player->bob;

    angle = (FINEANGLES/70*leveltime)&FINEMASK;
    swingx = FixedMul ( swing, finesine[angle]);

    angle = (FINEANGLES/70*leveltime+FINEANGLES/2)&FINEMASK;
    swingy = -FixedMul ( swingx, finesine[angle]);
}
*/


//
// P_BringUpWeapon
// Starts bringing the pending weapon up
// from the bottom of the screen.
// Uses player
//
void P_BringUpWeapon (player_t* player)
{
    statenum_t  newstate;

    if (player->pendingweapon == wp_nochange)
        player->pendingweapon = player->readyweapon;

    if (player->pendingweapon == wp_chainsaw)
        S_StartAttackSound(player->mo, sfx_sawup);

#ifdef PARANOIA
    if(player->pendingweapon>=NUMWEAPONS)
    {
         I_Error("P_BringUpWeapon : pendingweapon %d\n",player->pendingweapon);
    }
#endif
    
    newstate = player->weaponinfo[player->pendingweapon].upstate;

    player->pendingweapon = wp_nochange;
    player->psprites[ps_weapon].sy = WEAPONBOTTOM;

    P_SetPsprite (player, ps_weapon, newstate);
}

//
// P_CheckAmmo
// Returns true if there is enough ammo to shoot.
// If not, selects the next weapon to use.
//
boolean P_CheckAmmo (player_t* player)
{
    ammotype_t          ammo;
    int                 count;

    ammo = player->weaponinfo[player->readyweapon].ammo;

    // Minimal amount for one shot varies.
    count = player->weaponinfo[player->readyweapon].ammopershoot;

    // Some do not need ammunition anyway.
    // Return if current ammunition sufficient.
    if (ammo == am_noammo || player->ammo[ammo] >= count)
        return true;

    // Out of ammo, pick a weapon to change to.
    // Preferences are set here.
     // added by Boris : preferred weapons order
    if(!player->originalweaponswitch)
         VerifFavoritWeapon(player);
    else // eof Boris
    if( gamemode == heretic )
        do
        {
            if(player->weaponowned[wp_skullrod]
                && player->ammo[am_skullrod] > player->weaponinfo[wp_skullrod].ammopershoot)
            {
                player->pendingweapon = wp_skullrod;
            }
            else if(player->weaponowned[wp_blaster]
                && player->ammo[am_blaster] > player->weaponinfo[wp_blaster].ammopershoot)
            {
                player->pendingweapon = wp_blaster;
            }
            else if(player->weaponowned[wp_crossbow]
                && player->ammo[am_crossbow] > player->weaponinfo[wp_crossbow].ammopershoot)
            {
                player->pendingweapon = wp_crossbow;
            }
            else if(player->weaponowned[wp_mace]
                && player->ammo[am_mace] > player->weaponinfo[wp_mace].ammopershoot)
            {
                player->pendingweapon = wp_mace;
            }
            else if(player->ammo[am_goldwand] > player->weaponinfo[wp_goldwand].ammopershoot)
            {
                player->pendingweapon = wp_goldwand;
            }
            else if(player->weaponowned[wp_gauntlets])
            {
                player->pendingweapon = wp_gauntlets;
            }
            else if(player->weaponowned[wp_phoenixrod]
                && player->ammo[am_phoenixrod] > player->weaponinfo[wp_phoenixrod].ammopershoot)
            {
                player->pendingweapon = wp_phoenixrod;
            }
            else
            {
                player->pendingweapon = wp_staff;
            }
        } while(player->pendingweapon == wp_nochange);
    else
        do
        {
            if (player->weaponowned[wp_plasma]
                && player->ammo[am_cell]>=player->weaponinfo[wp_plasma].ammopershoot
                && (gamemode != shareware) )
            {
                player->pendingweapon = wp_plasma;
            }
            else if (player->weaponowned[wp_supershotgun]
                && player->ammo[am_shell]>=player->weaponinfo[wp_supershotgun].ammopershoot
                && (gamemode == commercial) )
            {
                player->pendingweapon = wp_supershotgun;
            }
            else if (player->weaponowned[wp_chaingun]
                && player->ammo[am_clip]>=player->weaponinfo[wp_chaingun].ammopershoot)
            {
                player->pendingweapon = wp_chaingun;
            }
            else if (player->weaponowned[wp_shotgun]
                && player->ammo[am_shell]>=player->weaponinfo[wp_shotgun].ammopershoot)
            {
                player->pendingweapon = wp_shotgun;
            }
            else if (player->ammo[am_clip]>=player->weaponinfo[wp_pistol].ammopershoot)
            {
                player->pendingweapon = wp_pistol;
            }
            else if (player->weaponowned[wp_chainsaw])
            {
                player->pendingweapon = wp_chainsaw;
            }
            else if (player->weaponowned[wp_missile]
                && player->ammo[am_misl]>=player->weaponinfo[wp_missile].ammopershoot)
            {
                player->pendingweapon = wp_missile;
            }
            else if (player->weaponowned[wp_bfg]
                && player->ammo[am_cell]>=player->weaponinfo[wp_bfg].ammopershoot
                && (gamemode != shareware) )
            {
                player->pendingweapon = wp_bfg;
            }
            else
            {
                // If everything fails.
                player->pendingweapon = wp_fist;
            }
            
        } while (player->pendingweapon == wp_nochange);

    // Now set appropriate weapon overlay.
    P_SetPsprite (player,
                  ps_weapon,
                  player->weaponinfo[player->readyweapon].downstate);

    return false;
}


//
// P_FireWeapon.
//
void P_FireWeapon (player_t* player)
{
    statenum_t  newstate;

    if (!P_CheckAmmo (player))
        return;

    if( gamemode == heretic )
    {
        P_SetMobjState(player->mo, S_PLAY_ATK2);
        newstate = player->refire ? player->weaponinfo[player->readyweapon].holdatkstate
                                  : player->weaponinfo[player->readyweapon].atkstate;
        // Play the sound for the initial gauntlet attack
        if( player->readyweapon == wp_gauntlets && !player->refire )
            S_StartSound(player->mo, sfx_gntuse);
    }
    else
    {
        P_SetMobjState (player->mo, S_PLAY_ATK1);
        newstate = player->weaponinfo[player->readyweapon].atkstate;
    }
    P_SetPsprite (player, ps_weapon, newstate);
    P_NoiseAlert (player->mo, player->mo);
}



//
// P_DropWeapon
// Player died, so put the weapon away.
//
void P_DropWeapon (player_t* player)
{
    P_SetPsprite (player,
                  ps_weapon,
                  player->weaponinfo[player->readyweapon].downstate);
}



//
// A_WeaponReady
// The player can fire the weapon
// or change to another weapon at this time.
// Follows after getting weapon up,
// or after previous attack/fire sequence.
//
void A_WeaponReady ( player_t*     player,
                     pspdef_t*     psp )
{
    if(player->chickenTics)
    { // Change to the chicken beak
        P_ActivateBeak(player);
        return;
    }

    // get out of attack state
    if (player->mo->state == &states[S_PLAY_ATK1]
        || player->mo->state == &states[S_PLAY_ATK2] )
    {
        P_SetMobjState (player->mo, S_PLAY);
    }

    if (player->readyweapon == wp_chainsaw
        && psp->state == &states[S_SAW])
    {
        S_StartAttackSound(player->mo, sfx_sawidl);
    }
    // Check for staff PL2 active sound
    if((player->readyweapon == wp_staff)
        && (psp->state == &states[S_STAFFREADY2_1])
        && P_Random() < 128)
    {
        S_StartAttackSound(player->mo, sfx_stfcrk);
    }

    // check for change
    //  if player is dead, put the weapon away
    if (player->pendingweapon != wp_nochange || !player->health)
    {
        // change weapon
        //  (pending weapon should allready be validated)
        P_SetPsprite (player, ps_weapon, player->weaponinfo[player->readyweapon].downstate);
        return;
    }

    // check for fire
    //  the missile launcher and bfg do not auto fire
    if (player->cmd.buttons & BT_ATTACK)
    {
        if ( !player->attackdown
             || (player->readyweapon != wp_missile
                 && (player->readyweapon != wp_bfg || gamemode == heretic)) )
        {
            player->attackdown = true;
            P_FireWeapon (player);
            return;
        }
    }
    else
        player->attackdown = false;
#ifndef CLIENTPREDICTION2    
    {
    int         angle;
    // bob the weapon based on movement speed
    angle = (128*leveltime/NEWTICRATERATIO)&FINEMASK;
    psp->sx = FRACUNIT + FixedMul (player->bob, finecosine[angle]);
    angle &= FINEANGLES/2-1;
    psp->sy = WEAPONTOP + FixedMul (player->bob, finesine[angle]);
    }
#endif
}

// client prediction stuff
void A_TicWeapon(player_t*     player,
                 pspdef_t*     psp )
{
    if( (void*)psp->state->action.acp2 == (void*)A_WeaponReady && psp->tics == psp->state->tics )
    {
        int         angle;
        
        // bob the weapon based on movement speed
        angle = (128*localgametic/NEWTICRATERATIO)&FINEMASK;
        psp->sx = FRACUNIT + FixedMul (player->bob, finecosine[angle]);
        angle &= FINEANGLES/2-1;
        psp->sy = WEAPONTOP + FixedMul (player->bob, finesine[angle]);
    }
}


//
// A_ReFire
// The player can re-fire the weapon
// without lowering it entirely.
//
void A_ReFire
( player_t*     player,
  pspdef_t*     psp )
{

    // check for fire
    //  (if a weaponchange is pending, let it go through instead)
    if ( (player->cmd.buttons & BT_ATTACK)
         && player->pendingweapon == wp_nochange
         && player->health)
    {
        player->refire++;
        P_FireWeapon (player);
    }
    else
    {
        player->refire = 0;
        P_CheckAmmo (player);
    }
}


void
A_CheckReload
( player_t*     player,
  pspdef_t*     psp )
{
    P_CheckAmmo (player);
#if 0
    if (player->ammo[am_shell]<2)
        P_SetPsprite (player, ps_weapon, S_DSNR1);
#endif
}



//
// A_Lower
// Lowers current weapon,
//  and changes weapon at bottom.
//
void A_Lower ( player_t*     player,
               pspdef_t*     psp )
{
    if(player->chickenTics)
        psp->sy = WEAPONBOTTOM;
    else
        psp->sy += LOWERSPEED;

    // Is already down.
    if (psp->sy < WEAPONBOTTOM )
        return;

    // Player is dead.
    if (player->playerstate == PST_DEAD)
    {
        psp->sy = WEAPONBOTTOM;

        // don't bring weapon back up
        return;
    }

    // The old weapon has been lowered off the screen,
    // so change the weapon and start raising it
    if (!player->health)
    {
        // Player is dead, so keep the weapon off screen.
        P_SetPsprite (player,  ps_weapon, S_NULL);
        return;
    }

    player->readyweapon = player->pendingweapon;

    P_BringUpWeapon (player);
}


//
// A_Raise
//
void A_Raise( player_t*     player,
              pspdef_t*     psp )
{
    psp->sy -= RAISESPEED;

    if (psp->sy > WEAPONTOP )
        return;

    psp->sy = WEAPONTOP;

    // The weapon has been raised all the way,
    //  so change to the ready state.
    P_SetPsprite (player, ps_weapon, 
                  player->weaponinfo[player->readyweapon].readystate);
}



//
// A_GunFlash
//
void
A_GunFlash
( player_t*     player,
  pspdef_t*     psp )
{
    P_SetMobjState (player->mo, S_PLAY_ATK2);
    P_SetPsprite (player,ps_flash,
                  player->weaponinfo[player->readyweapon].flashstate);
}



//
// WEAPON ATTACKS
//


//
// A_Punch
//
void
A_Punch
( player_t*     player,
  pspdef_t*     psp )
{
    angle_t     angle;
    int         damage;
    int         slope;

    damage = (P_Random ()%10+1)<<1;

    if (player->powers[pw_strength])
        damage *= 10;

    angle = player->mo->angle;
    angle += (P_Random()<<18); // WARNING: don't put this in one line 
    angle -= (P_Random()<<18); // else this expretion is ambiguous (evaluation order not diffined)

    slope = P_AimLineAttack (player->mo, angle, MELEERANGE);
    P_LineAttack (player->mo, angle, MELEERANGE, slope, damage);

    // turn to face target
    if (linetarget)
    {
        S_StartAttackSound(player->mo, sfx_punch);
        player->mo->angle = R_PointToAngle2 (player->mo->x,
                                             player->mo->y,
                                             linetarget->x,
                                             linetarget->y);
    }
}


//
// A_Saw
//
void
A_Saw
( player_t*     player,
  pspdef_t*     psp )
{
    angle_t     angle;
    int         damage;
    int         slope;

    damage = 2*(P_Random ()%10+1);
    angle = player->mo->angle;
    angle += (P_Random()<<18); // WARNING: don't put this in one line 
    angle -= (P_Random()<<18); // else this expretion is ambiguous (evaluation order not diffined)

    // use meleerange + 1 se the puff doesn't skip the flash
    slope = P_AimLineAttack (player->mo, angle, MELEERANGE+1);
    P_LineAttack (player->mo, angle, MELEERANGE+1, slope, damage);

    if (!linetarget)
    {
        S_StartAttackSound(player->mo, sfx_sawful);
        return;
    }
    S_StartAttackSound(player->mo, sfx_sawhit);

    // turn to face target
    angle = R_PointToAngle2 (player->mo->x, player->mo->y,
                             linetarget->x, linetarget->y);
    if (angle - player->mo->angle > ANG180)
    {
        if (angle - player->mo->angle < -ANG90/20)
            player->mo->angle = angle + ANG90/21;
        else
            player->mo->angle -= ANG90/20;
    }
    else
    {
        if (angle - player->mo->angle > ANG90/20)
            player->mo->angle = angle - ANG90/21;
        else
            player->mo->angle += ANG90/20;
    }
    player->mo->flags |= MF_JUSTATTACKED;
}



//
// A_FireMissile : rocket launcher fires a rocket
//
void A_FireMissile ( player_t*     player,
                     pspdef_t*     psp )
{
    player->ammo[player->weaponinfo[player->readyweapon].ammo] -= player->weaponinfo[player->readyweapon].ammopershoot;
    //added:16-02-98: added player arg3
    P_SpawnPlayerMissile (player->mo, MT_ROCKET);
}


//
// A_FireBFG
//
void A_FireBFG ( player_t*     player,
                 pspdef_t*     psp )
{
    player->ammo[player->weaponinfo[player->readyweapon].ammo] -= player->weaponinfo[player->readyweapon].ammopershoot;
    //added:16-02-98:added player arg3
    P_SpawnPlayerMissile (player->mo, MT_BFG);
}



//
// A_FirePlasma
//
void A_FirePlasma ( player_t*     player,
                    pspdef_t*     psp )
{
    player->ammo[player->weaponinfo[player->readyweapon].ammo] -= player->weaponinfo[player->readyweapon].ammopershoot;

    P_SetPsprite (player,
                  ps_flash,
                  player->weaponinfo[player->readyweapon].flashstate+(P_Random ()&1) );

    //added:16-02-98: added player arg3
    P_SpawnPlayerMissile (player->mo, MT_PLASMA);
}



//
// P_BulletSlope
// Sets a slope so a near miss is at aproximately
// the height of the intended target
//
fixed_t         bulletslope;

//added:16-02-98: Fab comments: autoaim for the bullet-type weapons
void P_BulletSlope (mobj_t* mo)
{
    angle_t     an;

    //added:18-02-98: if AUTOAIM, try to aim at something
    if(!mo->player->autoaim_toggle || !cv_allowautoaim.value || demoversion<=111)
        goto notagetfound;

    // see which target is to be aimed at
    an = mo->angle;
    bulletslope = P_AimLineAttack (mo, an, 16*64*FRACUNIT);

    if (!linetarget)
    {
        an += 1<<26;
        bulletslope = P_AimLineAttack (mo, an, 16*64*FRACUNIT);
        if (!linetarget)
        {
            an -= 2<<26;
            bulletslope = P_AimLineAttack (mo, an, 16*64*FRACUNIT);
        }
        if(!linetarget)
        {
notagetfound:
            if(demoversion>=128)
                bulletslope = AIMINGTOSLOPE(mo->player->aiming);
            else
                bulletslope = (mo->player->aiming<<FRACBITS)/160;
        }
    }
}


//
// P_GunShot
//
//added:16-02-98: used only for player (pistol,shotgun,chaingun)
//                supershotgun use p_lineattack directely
void P_GunShot ( mobj_t*       mo,
                 boolean       accurate )
{
    angle_t     angle;
    int         damage;

    damage = 5*(P_Random ()%3+1);
    angle = mo->angle;

    if (!accurate)
    {
        angle += (P_Random()<<18); // WARNING: don't put this in one line 
        angle -= (P_Random()<<18); // else this expretion is ambiguous (evaluation order not diffined)
    }

    P_LineAttack (mo, angle, MISSILERANGE, bulletslope, damage);
}


//
// A_FirePistol
//
void A_FirePistol ( player_t*     player,
                    pspdef_t*     psp )
{
    S_StartAttackSound(player->mo, sfx_pistol);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    player->ammo[player->weaponinfo[player->readyweapon].ammo]--;

    P_SetPsprite (player,
                  ps_flash,
                  player->weaponinfo[player->readyweapon].flashstate);

    P_BulletSlope (player->mo);
    P_GunShot (player->mo, !player->refire);
}


//
// A_FireShotgun
//
void A_FireShotgun ( player_t*     player,
                     pspdef_t*     psp )
{
    int         i;

    S_StartAttackSound(player->mo, sfx_shotgn);
    P_SetMobjState (player->mo, S_PLAY_ATK2);

    player->ammo[player->weaponinfo[player->readyweapon].ammo]--;
    P_SetPsprite (player,
                  ps_flash,
                  player->weaponinfo[player->readyweapon].flashstate);

    P_BulletSlope (player->mo);
    for (i=0 ; i<7 ; i++)
        P_GunShot (player->mo, false);
}



//
// A_FireShotgun2 (SuperShotgun)
//
void A_FireShotgun2 ( player_t*     player,
                      pspdef_t*     psp )
{
    int         i;
    angle_t     angle;
    int         damage;

    S_StartAttackSound(player->mo, sfx_dshtgn);
    P_SetMobjState (player->mo, S_PLAY_ATK2);

    player->ammo[player->weaponinfo[player->readyweapon].ammo]-=2;

    P_SetPsprite (player,
                  ps_flash,
                  player->weaponinfo[player->readyweapon].flashstate);

    P_BulletSlope (player->mo);

    for (i=0 ; i<20 ; i++)
    {
        int slope = bulletslope + (P_SignedRandom()<<5);
        damage = 5*(P_Random ()%3+1);
        angle = player->mo->angle + (P_SignedRandom() << 19);
        P_LineAttack (player->mo,
                      angle,
                      MISSILERANGE,
                      slope, damage);
    }
}


//
// A_FireCGun
//
void A_FireCGun ( player_t*     player,
                  pspdef_t*     psp )
{
    S_StartAttackSound(player->mo, sfx_pistol);

    if (!player->ammo[player->weaponinfo[player->readyweapon].ammo])
        return;

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    player->ammo[player->weaponinfo[player->readyweapon].ammo]--;

    P_SetPsprite (player,
                  ps_flash,
                  player->weaponinfo[player->readyweapon].flashstate
                      + psp->state
                      - &states[S_CHAIN1] );

    P_BulletSlope (player->mo);
    P_GunShot (player->mo, !player->refire);
}



//
// Flash light when fire gun
//
void A_Light0 (player_t *player, pspdef_t *psp)
{
    player->extralight = 0;
}

void A_Light1 (player_t *player, pspdef_t *psp)
{
    player->extralight = 1;
}

void A_Light2 (player_t *player, pspdef_t *psp)
{
    player->extralight = 2;
}


//
// A_BFGSpray
// Spawn a BFG explosion on every monster in view
//
void A_BFGSpray (mobj_t* mo)
{
    int     i;
    int     j;
    int     damage;
    angle_t an;
    mobj_t  *extrabfg;;

    // offset angles from its attack angle
    for (i=0 ; i<40 ; i++)
    {
        an = mo->angle - ANG90/2 + ANG90/40*i;

        // mo->target is the originator (player)
        //  of the missile
        P_AimLineAttack (mo->target, an, 16*64*FRACUNIT);

        if (!linetarget)
            continue;

        extrabfg = P_SpawnMobj (linetarget->x,
                                linetarget->y,
                                linetarget->z + (linetarget->height>>2),
                                MT_EXTRABFG);
        extrabfg->target = mo->target;

        damage = 0;
        for (j=0;j<15;j++)
            damage += (P_Random()&7) + 1;

        //BP: use extramobj as inflictor so we have the good death message
        P_DamageMobj (linetarget, extrabfg, mo->target, damage);
    }
}


//
// A_BFGsound
//
void
A_BFGsound
( player_t*     player,
  pspdef_t*     psp )
{
    S_StartAttackSound(player->mo, sfx_bfg);
}



//
// P_SetupPsprites
// Called at start of level for each player.
//
void P_SetupPsprites (player_t* player)
{
    int i;

    // remove all psprites
    for (i=0 ; i<NUMPSPRITES ; i++)
        player->psprites[i].state = NULL;

    // spawn the gun
    player->pendingweapon = player->readyweapon;
    P_BringUpWeapon (player);
}




//
// P_MovePsprites
// Called every tic by player thinking routine.
//
void P_MovePsprites (player_t* player)
{
    int         i;
    pspdef_t*   psp;
    state_t*    state;

    psp = &player->psprites[0];
    for (i=0 ; i<NUMPSPRITES ; i++, psp++)
    {
        // a null state means not active
        if ( (state = psp->state) )
        {
            // drop tic count and possibly change state

            // a -1 tic count never changes
            if (psp->tics != -1)
            {
                psp->tics--;
                if (!psp->tics)
                    P_SetPsprite (player, i, psp->state->nextstate);
            }
        }
    }

    player->psprites[ps_flash].sx = player->psprites[ps_weapon].sx;
    player->psprites[ps_flash].sy = player->psprites[ps_weapon].sy;
}
