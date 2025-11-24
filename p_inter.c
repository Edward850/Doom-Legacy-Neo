// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_inter.c,v 1.29 2002/11/30 18:39:58 judgecutor Exp $
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
// $Log: p_inter.c,v $
// Revision 1.29  2002/11/30 18:39:58  judgecutor
// * Fix CR+LF problem
// * Fix FFW bug (player spawining istead of weapon)
//
// Revision 1.28  2002/09/27 16:40:09  tonyd
// First commit of acbot
//
// Revision 1.27  2002/08/24 22:42:03  hurdler
// Apply Robert Hogberg patches
//
// Revision 1.26  2002/07/26 15:21:36  hurdler
// near RC release
//
// Revision 1.25  2002/07/23 15:07:11  mysterial
// Messages to second player appear on his half of the screen
//
// Revision 1.24  2002/01/21 23:14:28  judgecutor
// Frag's Weapon Falling fixes
//
// Revision 1.23  2001/12/26 22:46:01  hurdler
// revert to beta 3 until it's fixed (there is at least a problem with saved game)
//
// Revision 1.22  2001/12/26 22:42:52  hurdler
// revert to beta 3 until it's fixed (there is at least a problem with saved game)
//
// Revision 1.18  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.17  2001/05/27 13:42:47  bpereira
// no message
//
// Revision 1.16  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.15  2001/05/14 19:02:58  metzgermeister
//   * Fixed floor not moving up with player on E3M1
//   * Fixed crash due to oversized string in screen message ... bad bug!
//   * Corrected some typos
//   * fixed sound bug in SDL
//
// Revision 1.14  2001/04/19 05:51:47  metzgermeister
// fixed 10 shells instead of 4 - bug
//
// Revision 1.13  2001/03/30 17:12:50  bpereira
// no message
//
// Revision 1.12  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.11  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.10  2000/11/02 17:50:07  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.9  2000/10/02 18:25:45  bpereira
// no message
//
// Revision 1.8  2000/10/01 10:18:17  bpereira
// no message
//
// Revision 1.7  2000/09/28 20:57:16  bpereira
// no message
//
// Revision 1.6  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.5  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.4  2000/04/04 00:32:46  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.3  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      Handling interactions (i.e., collisions).
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "i_system.h"   //I_Tactile currently has no effect
#include "am_map.h"
#include "dstrings.h"
#include "g_game.h"
#include "m_random.h"
#include "p_local.h"
#include "p_inter.h"
#include "s_sound.h"
#include "r_main.h"
#include "st_stuff.h"

#define BONUSADD        6


// a weapon is found with two clip loads,
// a big item has five clip loads
int     maxammo[NUMAMMO] = {200, 50, 300, 50};
int     clipammo[NUMAMMO] = {10, 4, 20, 1};

consvar_t cv_fragsweaponfalling = {"fragsweaponfalling"   ,"0", CV_NETVAR, CV_YesNo};

// added 4-2-98 (Boris) for dehacked patch
// (i don't like that but do you see another solution ?)
int     MAXHEALTH= 100;

//--------------------------------------------------------------------------
//
// PROC P_SetMessage
//
//--------------------------------------------------------------------------

boolean ultimatemsg;

void P_SetMessage(player_t *player, char *message, boolean ultmsg)
{
    if((ultimatemsg || !cv_showmessages.value) && !ultmsg)
        return;
    
    player->message = message;
    //player->messageTics = MESSAGETICS;
    //BorderTopRefresh = true;
    if( ultmsg )
        ultimatemsg = true;
}

//
// GET STUFF
//

// added by Boris : preferred weapons order
void VerifFavoritWeapon (player_t *player)
{
    int newweapon;

    if (player->pendingweapon != wp_nochange)
        return;

    newweapon = FindBestWeapon(player);

    if (newweapon != player->readyweapon)
        player->pendingweapon = newweapon;
}

int FindBestWeapon(player_t *player)
{
    int actualprior, actualweapon = 0, i;

    actualprior = -1;

    for (i = 0; i < NUMWEAPONS; i++)
    {
        // skip super shotgun for non-Doom2
        if (gamemode!=commercial && i==wp_supershotgun)
            continue;
        // skip plasma-bfg in sharware
        if (gamemode==shareware && (i==wp_plasma || i==wp_bfg))
            continue;

        if (player->weaponowned[i] &&
            actualprior < player->favoritweapon[i] &&
            player->ammo[player->weaponinfo[i].ammo] >= player->weaponinfo[i].ammopershoot)
        {
            actualweapon = i;
            actualprior = player->favoritweapon[i];
	 }
    }
    
    return actualweapon;
}

static const weapontype_t GetAmmoChange[] =
{
        wp_goldwand,
        wp_crossbow,
        wp_blaster,
        wp_skullrod,
        wp_phoenixrod,
        wp_mace
};

//
// P_GiveAmmo
// Num is the number of clip loads,
// not the individual count (0= 1/2 clip).
// Returns false if the ammo can't be picked up at all
//

boolean P_GiveAmmo ( player_t*     player,
                     ammotype_t    ammo,
                     int           count )
{
    int         oldammo;

    if (ammo == am_noammo)
        return false;

    if (ammo < 0 || ammo > NUMAMMO)
    {
        CONS_Printf ("\2P_GiveAmmo: bad type %i", ammo);
        return false;
    }

    if ( player->ammo[ammo] == player->maxammo[ammo]  )
        return false;
/*
    if (num)
        num *= clipammo[ammo];
    else
        num = clipammo[ammo]/2;
*/
    if (gameskill == sk_baby
        || gameskill == sk_nightmare)
    {
        if( gamemode == heretic )
            count += count>>1;
        else
            // give double ammo in trainer mode,
            // you'll need in nightmare
            count <<= 1;

    }


    oldammo = player->ammo[ammo];
    player->ammo[ammo] += count;

    if (player->ammo[ammo] > player->maxammo[ammo])
        player->ammo[ammo] = player->maxammo[ammo];

    // If non zero ammo,
    // don't change up weapons,
    // player was lower on purpose.
    if (oldammo)
        return true;

    // We were down to zero,
    // so select a new weapon.
    // Preferences are not user selectable.

    // Boris hack for preferred weapons order...
    if(!player->originalweaponswitch)
    {
       if(player->ammo[player->weaponinfo[player->readyweapon].ammo]
                     < player->weaponinfo[player->readyweapon].ammopershoot)
         VerifFavoritWeapon(player);
       return true;
    }
    else //eof Boris
    if( gamemode == heretic )
    {
        if( ( player->readyweapon == wp_staff
           || player->readyweapon == wp_gauntlets) 
           && player->weaponowned[GetAmmoChange[ammo]])
             player->pendingweapon = GetAmmoChange[ammo];
    }
    else
    switch (ammo)
    {
      case am_clip:
        if (player->readyweapon == wp_fist)
        {
            if (player->weaponowned[wp_chaingun])
                player->pendingweapon = wp_chaingun;
            else
                player->pendingweapon = wp_pistol;
        }
        break;

      case am_shell:
        if (player->readyweapon == wp_fist
            || player->readyweapon == wp_pistol)
        {
            if (player->weaponowned[wp_shotgun])
                player->pendingweapon = wp_shotgun;
        }
        break;

      case am_cell:
        if (player->readyweapon == wp_fist
            || player->readyweapon == wp_pistol)
        {
            if (player->weaponowned[wp_plasma])
                player->pendingweapon = wp_plasma;
        }
        break;

      case am_misl:
        if (player->readyweapon == wp_fist)
        {
            if (player->weaponowned[wp_missile])
                player->pendingweapon = wp_missile;
        }
      default:
        break;
    }

    return true;
}

// ammo get with the weapon
int GetWeaponAmmo[NUMWEAPONS] =
{
    0,  // staff       fist
   20,  // gold wand   pistol
    8,  // crossbow    shotgun
   20,  // blaster     chaingun
    2,  // skull rod   missile    
   40,  // phoenix rod plasma     
   40,  // mace        bfg        
    0,  // gauntlets   chainsaw   
    8,  // beak        supershotgun
};

static int has_ammo_dropped = 0;
//
// P_GiveWeapon
// The weapon name may have a MF_DROPPED flag ored in.
//
boolean P_GiveWeapon ( player_t*     player,
                       weapontype_t  weapon,
                       boolean       dropped )
{
    boolean     gaveammo;
    boolean     gaveweapon;
    int         ammo_count;

    if (multiplayer && (cv_deathmatch.value!=2) && !dropped )
    {
        // leave placed weapons forever on net games
        if (player->weaponowned[weapon])
            return false;

        player->bonuscount += BONUSADD;
        player->weaponowned[weapon] = true;

        if (cv_deathmatch.value)
            P_GiveAmmo (player, player->weaponinfo[weapon].ammo, 5*clipammo[player->weaponinfo[weapon].ammo]);
        else
            P_GiveAmmo (player, player->weaponinfo[weapon].ammo, GetWeaponAmmo[weapon]);

        // Boris hack preferred weapons order...
        if(player->originalweaponswitch
        || player->favoritweapon[weapon] > player->favoritweapon[player->readyweapon])
            player->pendingweapon = weapon;     // do like Doom2 original

        //added:16-01-98:changed consoleplayer to displayplayer
        //               (hear the sounds from the viewpoint)
        if (player == &players[displayplayer] || (cv_splitscreen.value && player==&players[secondarydisplayplayer]))
            S_StartSound (NULL, sfx_wpnup);
        return false;
    }

    if (player->weaponinfo[weapon].ammo != am_noammo)
    {
        // give one clip with a dropped weapon,
        // two clips with a found weapon
        if (dropped)
        {
            ammo_count = has_ammo_dropped ? 
                (has_ammo_dropped < 0 ? 0 : has_ammo_dropped) : clipammo[player->weaponinfo[weapon].ammo];
            //gaveammo = P_GiveAmmo (player, player->weaponinfo[weapon].ammo, clipammo[player->weaponinfo[weapon].ammo]);
        }
        else
        {
            //gaveammo = P_GiveAmmo (player, player->weaponinfo[weapon].ammo, GetWeaponAmmo[weapon]);
            ammo_count = GetWeaponAmmo[weapon];
        }
        gaveammo = P_GiveAmmo (player, player->weaponinfo[weapon].ammo, ammo_count);
    }
    else
        gaveammo = false;

   if (player->weaponowned[weapon])
        gaveweapon = false;
    else
    {
        gaveweapon = true;
        player->weaponowned[weapon] = true;
        if (player->originalweaponswitch
        || player->favoritweapon[weapon] > player->favoritweapon[player->readyweapon])
            player->pendingweapon = weapon;    // Doom2 original stuff
    }

    return (gaveweapon || gaveammo);
}



//
// P_GiveBody
// Returns false if the body isn't needed at all
//
boolean P_GiveBody ( player_t*     player,
                     int           num )
{
    int max;
    
    max = MAXHEALTH;
    if(player->chickenTics)
        max = MAXCHICKENHEALTH;

    if (player->health >= max)
        return false;

    player->health += num;
    if (player->health > max)
        player->health = max;
    player->mo->health = player->health;

    return true;
}



//
// P_GiveArmor
// Returns false if the armor is worse
// than the current armor.
//
boolean P_GiveArmor ( player_t*     player,
                      int           armortype )
{
    int         hits;

    hits = armortype*100;
    if (player->armorpoints >= hits)
        return false;   // don't pick up

    player->armortype = armortype;
    player->armorpoints = hits;

    return true;
}



//
// P_GiveCard
//
static boolean P_GiveCard ( player_t*     player,
                            card_t        card )
{
    if (player->cards & card )
        return false;

    player->bonuscount = BONUSADD;
    player->cards |= card;
    return true;
}


//
// P_GivePower
//
boolean P_GivePower ( player_t*     player,
                      int /*powertype_t*/   power )
{
    if (power == pw_invulnerability)
    {
        // Already have it
        if( inventory && player->powers[power] > BLINKTHRESHOLD )
            return false;

        player->powers[power] = INVULNTICS;
        return true;
    }
    if(power == pw_weaponlevel2)
    {
        // Already have it
        if( inventory && player->powers[power] > BLINKTHRESHOLD)
            return false;

        player->powers[power] = WPNLEV2TICS;
        player->weaponinfo = wpnlev2info;
        return true;
    }
    if (power == pw_invisibility)
    {
        // Already have it
        if( inventory && player->powers[power] > BLINKTHRESHOLD)
            return false;

        player->powers[power] = INVISTICS;
        player->mo->flags |= MF_SHADOW;
        return true;
    }
    if(power == pw_flight)
    {
        // Already have it
        if(player->powers[power] > BLINKTHRESHOLD)
            return(false);
        player->powers[power] = FLIGHTTICS;
        player->mo->flags2 |= MF2_FLY;
        player->mo->flags |= MF_NOGRAVITY;
        if(player->mo->z <= player->mo->floorz)
        {
            player->flyheight = 10; // thrust the player in the air a bit
        }
        return(true);
    }
    if (power == pw_infrared)
    {
        // Already have it
        if(player->powers[power] > BLINKTHRESHOLD)
            return(false);

        player->powers[power] = INFRATICS;
        return true;
    }

    if (power == pw_ironfeet)
    {
        player->powers[power] = IRONTICS;
        return true;
    }

    if (power == pw_strength)
    {
        P_GiveBody (player, 100);
        player->powers[power] = 1;
        return true;
    }

    if (player->powers[power])
        return false;   // already got it

    player->powers[power] = 1;
    return true;
}

// Boris stuff : dehacked patches hack
int max_armor=200;
int green_armor_class=1;
int blue_armor_class=2;
int maxsoul=200;
int soul_health=100;
int mega_health=200;
// eof Boris

//---------------------------------------------------------------------------
//
// FUNC P_GiveArtifact
//
// Returns true if artifact accepted.
//
//---------------------------------------------------------------------------

boolean P_GiveArtifact(player_t *player, artitype_t arti, mobj_t *mo)
{
    int i;
    
    i = 0;
    while(player->inventory[i].type != arti && i < player->inventorySlotNum)
    {
        i++;
    }
    if(i == player->inventorySlotNum)
    {
        player->inventory[i].count = 1;
        player->inventory[i].type = arti;
        player->inventorySlotNum++;
    }
    else
    {
        if(player->inventory[i].count >= MAXARTECONT)
        { // Player already has 16 of this item
            return(false);
        }
        player->inventory[i].count++;
    }
    if( player->inventory[player->inv_ptr].count == 0 )
        player->inv_ptr = i;

    if(mo && (mo->flags&MF_COUNTITEM))
    {
        player->itemcount++;
    }
    return(true);
}


//---------------------------------------------------------------------------
//
// PROC P_SetDormantArtifact
//
// Removes the MF_SPECIAL flag, and initiates the artifact pickup
// animation.
//
//---------------------------------------------------------------------------

void P_SetDormantArtifact(mobj_t *arti)
{
        arti->flags &= ~MF_SPECIAL;
        if(cv_deathmatch.value && (arti->type != MT_ARTIINVULNERABILITY)
                && (arti->type != MT_ARTIINVISIBILITY))
        {
                P_SetMobjState(arti, S_DORMANTARTI1);
        }
        else
        { // Don't respawn
                P_SetMobjState(arti, S_DEADARTI1);
        }
        S_StartSound(arti, sfx_artiup);
}

//---------------------------------------------------------------------------
//
// PROC A_RestoreArtifact
//
//---------------------------------------------------------------------------

void A_RestoreArtifact(mobj_t *arti)
{
        arti->flags |= MF_SPECIAL;
        P_SetMobjState(arti, arti->info->spawnstate);
        S_StartSound(arti, sfx_itmbk);
}

//----------------------------------------------------------------------------
//
// PROC P_HideSpecialThing
//
//----------------------------------------------------------------------------

void P_HideSpecialThing(mobj_t *thing)
{
        thing->flags &= ~MF_SPECIAL;
        thing->flags2 |= MF2_DONTDRAW;
        P_SetMobjState(thing, S_HIDESPECIAL1);
}

//---------------------------------------------------------------------------
//
// PROC A_RestoreSpecialThing1
//
// Make a special thing visible again.
//
//---------------------------------------------------------------------------

void A_RestoreSpecialThing1(mobj_t *thing)
{
        if(thing->type == MT_WMACE)
        { // Do random mace placement
                P_RepositionMace(thing);
        }
        thing->flags2 &= ~MF2_DONTDRAW;
        S_StartSound(thing, sfx_itmbk);
}

//---------------------------------------------------------------------------
//
// PROC A_RestoreSpecialThing2
//
//---------------------------------------------------------------------------

void A_RestoreSpecialThing2(mobj_t *thing)
{
        thing->flags |= MF_SPECIAL;
        P_SetMobjState(thing, thing->info->spawnstate);
}


//----------------------------------------------------------------------------
//
// PROC A_HideThing
//
//----------------------------------------------------------------------------

void A_HideThing(mobj_t *actor)
{
        //P_UnsetThingPosition(actor);
        actor->flags2 |= MF2_DONTDRAW;
}

//----------------------------------------------------------------------------
//
// PROC A_UnHideThing
//
//----------------------------------------------------------------------------

void A_UnHideThing(mobj_t *actor)
{
        //P_SetThingPosition(actor);
        actor->flags2 &= ~MF2_DONTDRAW;
}


//
// P_TouchSpecialThing
//
void P_TouchSpecialThing ( mobj_t*       special,
                           mobj_t*       toucher )
{                  
    player_t*   player;
    int         i;
    fixed_t     delta;
    int         sound;

    delta = special->z - toucher->z;

    //SoM: 3/27/2000: For some reason, the old code allowed the player to
    //grab items that were out of reach...
    if (delta > toucher->height
        || delta < -special->height)
    {
        // out of reach
        return;
    }

    // Dead thing touching.
    // Can happen with a sliding player corpse.
    if (toucher->health <= 0 || toucher->flags&MF_CORPSE)
        return;

    sound = sfx_itemup;
    player = toucher->player;

#ifdef PARANOIA
    if( !player )
        I_Error("P_TouchSpecialThing: without player\n");
#endif


    // FWF support
    has_ammo_dropped = special->dropped_ammo_count;


    // Identify by sprite.
    switch (special->sprite)
    {
      case SPR_SHLD: // Item_Shield1
        // armor
      case SPR_ARM1:
        if (!P_GiveArmor (player, green_armor_class))
            return;
        player->message = GOTARMOR;
        break;

      case SPR_SHD2: // Item_Shield2
      case SPR_ARM2:
        if (!P_GiveArmor (player, blue_armor_class))
            return;
        player->message = GOTMEGA;
        break;

        // bonus items
      case SPR_BON1:
        player->health++;               // can go over 100%
        if (player->health > 2*MAXHEALTH)
            player->health = 2*MAXHEALTH;
        player->mo->health = player->health;
        if(cv_showmessages.value==1)
            player->message = GOTHTHBONUS;
        break;

      case SPR_BON2:
        player->armorpoints++;          // can go over 100%
        if (player->armorpoints > max_armor)
            player->armorpoints = max_armor;
        if (!player->armortype)
            player->armortype = 1;
        if(cv_showmessages.value==1)
           player->message = GOTARMBONUS;
        break;

      case SPR_SOUL:
        player->health += soul_health;
        if (player->health > maxsoul)
            player->health = maxsoul;
        player->mo->health = player->health;
        player->message = GOTSUPER;
        sound = sfx_getpow;
        break;

      case SPR_MEGA:
        if (gamemode != commercial)
            return;
        player->health = mega_health;
        player->mo->health = player->health;
        P_GiveArmor (player,2);
        player->message = GOTMSPHERE;
        sound = sfx_getpow;
        break;

        // cards
        // leave cards for everyone
      case SPR_BKYY: // Key_Blue
      case SPR_BKEY:
        if( P_GiveCard (player, it_bluecard) )
        {
            player->message = GOTBLUECARD;
            if( gamemode == heretic ) sound = sfx_keyup;
        }
        if (!multiplayer)
            break;
        return;

      case SPR_CKYY: // Key_Yellow
      case SPR_YKEY:
        if( P_GiveCard (player, it_yellowcard) )
        {
            player->message = GOTYELWCARD;
            if( gamemode == heretic ) sound = sfx_keyup;
        }
        if (!multiplayer)
            break;
        return;

      case SPR_AKYY: // Key_Green
      case SPR_RKEY:
        if (P_GiveCard (player, it_redcard))
        {
            player->message = GOTREDCARD;
            if( gamemode == heretic ) sound = sfx_keyup;
        }
        if (!multiplayer)
            break;
        return;

      case SPR_BSKU:
        if (P_GiveCard (player, it_blueskull))
        {
            player->message = GOTBLUESKUL;
            if( gamemode == heretic ) sound = sfx_keyup;
        }
        if (!multiplayer)
            break;
        return;

      case SPR_YSKU:
        if (P_GiveCard (player, it_yellowskull))
        {
            player->message = GOTYELWSKUL;
            if( gamemode == heretic ) sound = sfx_keyup;
        }
        if (!multiplayer)
            break;
        return;

      case SPR_RSKU:
        if (P_GiveCard (player, it_redskull))
        {
            player->message = GOTREDSKULL;
            if( gamemode == heretic ) sound = sfx_keyup;
        }
        if (!multiplayer)
            break;
        return;

        // medikits, heals
      case SPR_PTN1: // Item_HealingPotion
      case SPR_STIM:
        if (!P_GiveBody (player, 10))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTSTIM;
        break;

      case SPR_MEDI:
        if (!P_GiveBody (player, 25))
            return;
        if(cv_showmessages.value==1)
        {
            if (player->health < 25)
                player->message = GOTMEDINEED;
            else
                player->message = GOTMEDIKIT;
        }
        break;

        // heretic Artifacts :
      case SPR_PTN2: // Arti_HealingPotion
          if(P_GiveArtifact(player, arti_health, special))
          {
              if( cv_showmessages.value==1 )
                  P_SetMessage(player, TXT_ARTIHEALTH, false);
              
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_SOAR: // Arti_Fly
          if(P_GiveArtifact(player, arti_fly, special))
          {
              P_SetMessage(player, TXT_ARTIFLY, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_INVU: // Arti_Invulnerability
          if(P_GiveArtifact(player, arti_invulnerability, special))
          {
              P_SetMessage(player, TXT_ARTIINVULNERABILITY, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_PWBK: // Arti_TomeOfPower
          if(P_GiveArtifact(player, arti_tomeofpower, special))
          {
              P_SetMessage(player, TXT_ARTITOMEOFPOWER, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_INVS: // Arti_Invisibility
          if(P_GiveArtifact(player, arti_invisibility, special))
          {
              P_SetMessage(player, TXT_ARTIINVISIBILITY, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_EGGC: // Arti_Egg
          if(P_GiveArtifact(player, arti_egg, special))
          {
              P_SetMessage(player, TXT_ARTIEGG, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_SPHL: // Arti_SuperHealth
          if(P_GiveArtifact(player, arti_superhealth, special))
          {
              P_SetMessage(player, TXT_ARTISUPERHEALTH, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_TRCH: // Arti_Torch
          if(P_GiveArtifact(player, arti_torch, special))
          {
              P_SetMessage(player, TXT_ARTITORCH, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_FBMB: // Arti_FireBomb
          if(P_GiveArtifact(player, arti_firebomb, special))
          {
              P_SetMessage(player, TXT_ARTIFIREBOMB, false);
              P_SetDormantArtifact(special);
          }
          return;
      case SPR_ATLP: // Arti_Teleport
          if(P_GiveArtifact(player, arti_teleport, special))
          {
              P_SetMessage(player, TXT_ARTITELEPORT, false);
              P_SetDormantArtifact(special);
          }
          return;

        // power ups
      case SPR_PINV:
        if (!P_GivePower (player, pw_invulnerability))
            return;
        player->message = GOTINVUL;
        sound = sfx_getpow;
        break;

      case SPR_PSTR:
        if (!P_GivePower (player, pw_strength))
            return;
        player->message = GOTBERSERK;
        if (player->readyweapon != wp_fist)
            player->pendingweapon = wp_fist;
        sound = sfx_getpow;
        break;

      case SPR_PINS:
        if (!P_GivePower (player, pw_invisibility))
            return;
        player->message = GOTINVIS;
        sound = sfx_getpow;
        break;

      case SPR_SUIT:
        if (!P_GivePower (player, pw_ironfeet))
            return;
        player->message = GOTSUIT;
        sound = sfx_getpow;
        break;

      case SPR_SPMP: // Item_SuperMap
      case SPR_PMAP:
        if (!P_GivePower (player, pw_allmap))
            return;
        player->message = GOTMAP;
        if( gamemode != heretic )
            sound = sfx_getpow;
        break;

      case SPR_PVIS:
        if (!P_GivePower (player, pw_infrared))
            return;
        player->message = GOTVISOR;
        sound = sfx_getpow;
        break;

        // heretic Ammo
      case SPR_AMG1: // Ammo_GoldWandWimpy
          if(!P_GiveAmmo(player, am_goldwand, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOGOLDWAND1, false);
          break;
      case SPR_AMG2: // Ammo_GoldWandHefty
          if(!P_GiveAmmo(player, am_goldwand, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOGOLDWAND2, false);
          break;
      case SPR_AMM1: // Ammo_MaceWimpy
          if(!P_GiveAmmo(player, am_mace, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOMACE1, false);
          break;
      case SPR_AMM2: // Ammo_MaceHefty
          if(!P_GiveAmmo(player, am_mace, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOMACE2, false);
          break;
      case SPR_AMC1: // Ammo_CrossbowWimpy
          if(!P_GiveAmmo(player, am_crossbow, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOCROSSBOW1, false);

          break;
      case SPR_AMC2: // Ammo_CrossbowHefty
          if(!P_GiveAmmo(player, am_crossbow, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOCROSSBOW2, false);
          break;
      case SPR_AMB1: // Ammo_BlasterWimpy
          if(!P_GiveAmmo(player, am_blaster, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOBLASTER1, false);
          break;
      case SPR_AMB2: // Ammo_BlasterHefty
          if(!P_GiveAmmo(player, am_blaster, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOBLASTER2, false);
          break;
      case SPR_AMS1: // Ammo_SkullRodWimpy
          if(!P_GiveAmmo(player, am_skullrod, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOSKULLROD1, false);
          break;
      case SPR_AMS2: // Ammo_SkullRodHefty
          if(!P_GiveAmmo(player, am_skullrod, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOSKULLROD2, false);
          break;
      case SPR_AMP1: // Ammo_PhoenixRodWimpy
          if(!P_GiveAmmo(player, am_phoenixrod, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOPHOENIXROD1, false);
          break;
      case SPR_AMP2: // Ammo_PhoenixRodHefty
          if(!P_GiveAmmo(player, am_phoenixrod, special->health))
          {
              return;
          }
          if( cv_showmessages.value==1 )
              P_SetMessage(player, TXT_AMMOPHOENIXROD2, false);
          break;

        // ammo
      case SPR_CLIP:
        if (special->flags & MF_DROPPED)
        {
            if (!P_GiveAmmo (player,am_clip,clipammo[am_clip]/2))
                return;
        }
        else
        {
            if (!P_GiveAmmo (player,am_clip,clipammo[am_clip]))
                return;
        }
        if(cv_showmessages.value==1)
            player->message = GOTCLIP;
        break;

      case SPR_AMMO:
        if (!P_GiveAmmo (player, am_clip,5*clipammo[am_clip]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTCLIPBOX;
        break;

      case SPR_ROCK:
        if (!P_GiveAmmo (player, am_misl,clipammo[am_misl]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTROCKET;
        break;

      case SPR_BROK:
        if (!P_GiveAmmo (player, am_misl,5*clipammo[am_misl]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTROCKBOX;
        break;

      case SPR_CELL:
        if (!P_GiveAmmo (player, am_cell,clipammo[am_cell]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTCELL;
        break;

      case SPR_CELP:
        if (!P_GiveAmmo (player, am_cell,5*clipammo[am_cell]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTCELLBOX;
        break;

      case SPR_SHEL:
        if (!P_GiveAmmo (player, am_shell,clipammo[am_shell]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTSHELLS;
        break;

      case SPR_SBOX:
        if (!P_GiveAmmo (player, am_shell,5*clipammo[am_shell]))
            return;
        if(cv_showmessages.value==1)
            player->message = GOTSHELLBOX;
        break;

      case SPR_BPAK:
        if (!player->backpack)
        {
            for (i=0 ; i<NUMAMMO ; i++)
                player->maxammo[i] *= 2;
            player->backpack = true;
        }
        for (i=0 ; i<NUMAMMO ; i++)
            P_GiveAmmo (player, i, clipammo[i]);
        player->message = GOTBACKPACK;
        break;

      case SPR_BAGH: // Item_BagOfHolding
        if(!player->backpack)
        {
            for(i = 0; i < NUMAMMO; i++)
                player->maxammo[i] *= 2;
            player->backpack = true;
        }
        P_GiveAmmo(player, am_goldwand, AMMO_GWND_WIMPY);
        P_GiveAmmo(player, am_blaster, AMMO_BLSR_WIMPY);
        P_GiveAmmo(player, am_crossbow, AMMO_CBOW_WIMPY);
        P_GiveAmmo(player, am_skullrod, AMMO_SKRD_WIMPY);
        P_GiveAmmo(player, am_phoenixrod, AMMO_PHRD_WIMPY);
        P_SetMessage(player, TXT_ITEMBAGOFHOLDING, false);
        break;

        // weapons
      case SPR_BFUG:
        if (!P_GiveWeapon (player, wp_bfg, special->flags&MF_DROPPED) )
            return;
        player->message = GOTBFG9000;
        sound = sfx_wpnup;
        break;

      case SPR_MGUN:
        if (!P_GiveWeapon (player, wp_chaingun, special->flags&MF_DROPPED) )
            return;
        player->message = GOTCHAINGUN;
        sound = sfx_wpnup;
        break;

      case SPR_CSAW:
        if (!P_GiveWeapon (player, wp_chainsaw, false) )
            return;
        player->message = GOTCHAINSAW;
        sound = sfx_wpnup;
        break;

      case SPR_LAUN:
        if (!P_GiveWeapon (player, wp_missile, special->flags&MF_DROPPED) )
            return;
        player->message = GOTLAUNCHER;
        sound = sfx_wpnup;
        break;

      case SPR_PLAS:
        if (!P_GiveWeapon (player, wp_plasma, special->flags&MF_DROPPED) )
            return;
        player->message = GOTPLASMA;
        sound = sfx_wpnup;
        break;

      case SPR_SHOT:
        if (!P_GiveWeapon (player, wp_shotgun, special->flags&MF_DROPPED ) )
            return;
        player->message = GOTSHOTGUN;
        sound = sfx_wpnup;
        break;

      case SPR_SGN2:
        if (!P_GiveWeapon (player, wp_supershotgun, special->flags&MF_DROPPED ) )
            return;
        player->message = GOTSHOTGUN2;
        sound = sfx_wpnup;
        break;

      // heretic weapons
      case SPR_WMCE: // Weapon_Mace
          if(!P_GiveWeapon(player, wp_mace,special->flags&MF_DROPPED))
          {
              return;
          }
          P_SetMessage(player, TXT_WPNMACE, false);
          sound = sfx_wpnup;
          break;
      case SPR_WBOW: // Weapon_Crossbow
          if(!P_GiveWeapon(player, wp_crossbow,special->flags&MF_DROPPED))
          {
              return;
          }
          P_SetMessage(player, TXT_WPNCROSSBOW, false);
          sound = sfx_wpnup;
          break;
      case SPR_WBLS: // Weapon_Blaster
          if(!P_GiveWeapon(player, wp_blaster,special->flags&MF_DROPPED))
          {
              return;
          }
          P_SetMessage(player, TXT_WPNBLASTER, false);
          sound = sfx_wpnup;
          break;
      case SPR_WSKL: // Weapon_SkullRod
          if(!P_GiveWeapon(player, wp_skullrod, special->flags&MF_DROPPED))
          {
              return;
          }
          P_SetMessage(player, TXT_WPNSKULLROD, false);
          sound = sfx_wpnup;
          break;
      case SPR_WPHX: // Weapon_PhoenixRod
          if(!P_GiveWeapon(player, wp_phoenixrod, special->flags&MF_DROPPED))
          {
              return;
          }
          P_SetMessage(player, TXT_WPNPHOENIXROD, false);
          sound = sfx_wpnup;
          break;
      case SPR_WGNT: // Weapon_Gauntlets
          if(!P_GiveWeapon(player, wp_gauntlets, false))
          {
              return;
          }
          P_SetMessage(player, TXT_WPNGAUNTLETS, false);
          sound = sfx_wpnup;
          break;

      default:
        // SoM: New gettable things with FraggleScript!
        //CONS_Printf ("\2P_TouchSpecialThing: Unknown gettable thing\n");
        return;
    }

    if (special->flags & MF_COUNTITEM)
        player->itemcount++;
    P_RemoveMobj ( special );
    player->bonuscount += BONUSADD;

    //added:16-01-98:consoleplayer -> displayplayer (hear sounds from viewpoint)
    if (player == &players[displayplayer] || (cv_splitscreen.value && player==&players[secondarydisplayplayer]))
        S_StartSound (NULL, sound);
}



#ifdef thatsbuggycode
//
//  Tell each supported thing to check again its position,
//  because the 'base' thing has vanished or diminished,
//  the supported things might fall.
//
//added:28-02-98:
void P_CheckSupportThings (mobj_t* mobj)
{
    fixed_t   supportz = mobj->z + mobj->height;

    while ((mobj = mobj->supportthings))
    {
        // only for things above support thing
        if (mobj->z > supportz)
            mobj->eflags |= MF_CHECKPOS;
    }
}


//
//  If a thing moves and supportthings,
//  move the supported things along.
//
//added:28-02-98:
void P_MoveSupportThings (mobj_t* mobj, fixed_t xmove, fixed_t ymove, fixed_t zmove)
{
    fixed_t   supportz = mobj->z + mobj->height;
    mobj_t    *mo = mobj->supportthings;

    while (mo)
    {
        //added:28-02-98:debug
        if (mo==mobj)
        {
            mobj->supportthings = NULL;
            break;
        }

        // only for things above support thing
        if (mobj->z > supportz)
        {
            mobj->eflags |= MF_CHECKPOS;
            mobj->momx += xmove;
            mobj->momy += ymove;
            mobj->momz += zmove;
        }

        mo = mo->supportthings;
    }
}


//
//  Link a thing to it's 'base' (supporting) thing.
//  When the supporting thing will move or change size,
//  the supported will then be aware.
//
//added:28-02-98:
void P_LinkFloorThing(mobj_t*   mobj)
{
    mobj_t*     mo;
    mobj_t*     nmo;

    // no supporting thing
    if (!(mo = mobj->floorthing))
        return;

    // link mobj 'above' the lower mobjs, so that lower supporting
    // mobjs act upon this mobj
    while ( (nmo = mo->supportthings) &&
            (nmo->z<=mobj->z) )
    {
        // dont link multiple times
        if (nmo==mobj)
            return;

        mo = nmo;
    }
    mo->supportthings = mobj;
    mobj->supportthings = nmo;
}


//
//  Unlink a thing from it's support,
//  when it's 'floorthing' has changed,
//  before linking with the new 'floorthing'.
//
//added:28-02-98:
void P_UnlinkFloorThing(mobj_t*   mobj)
{
    mobj_t*     mo;

    if (!(mo = mobj->floorthing))      // just to be sure (may happen)
       return;

    while (mo->supportthings)
    {
        if (mo->supportthings == mobj)
        {
            mo->supportthings = NULL;
            break;
        }
        mo = mo->supportthings;
    }
}
#endif


// Death messages relating to the target (dying) player
//
static void P_DeathMessages ( mobj_t*       target,
                              mobj_t*       inflictor,
                              mobj_t*       source )
{
    int     w;
    char    *str;

    if (!target || !target->player)
        return;

    if (source && source->player)
    {
        if (source->player==target->player)
        {
            if (cv_splitscreen.value)
            {
                char txt[512];
                sprintf(txt, text[DEATHMSG_SUICIDE], player_names[target->player-players]);
                CONS_Printf(txt);
                CONS_Printf("\4%s", txt);
            }
            else
                CONS_Printf(text[DEATHMSG_SUICIDE], player_names[target->player-players]);
        }
        else
        {
            if (target->health < -9000) // telefrag !
                str = text[DEATHMSG_TELEFRAG];
            else
            {
                w = source->player->readyweapon;
                if( inflictor )
                {
                    switch(inflictor->type) {
                    case MT_ROCKET   : w = wp_missile; break;
                    case MT_PLASMA   : w = wp_plasma;  break;
                    case MT_EXTRABFG :
                    case MT_BFG      : w = wp_bfg;     break;
                    default : break;
                    }
                }

                switch(w)
                {
                case wp_fist:
                    str = text[DEATHMSG_FIST];
                    break;
                case wp_pistol:
                    str = text[DEATHMSG_GUN];
                    break;
                case wp_shotgun:
                    str = text[DEATHMSG_SHOTGUN];
                    break;
                case wp_chaingun:
                    str = text[DEATHMSG_MACHGUN];
                    break;
                case wp_missile:
                    str = text[DEATHMSG_ROCKET];
                    if (target->health < -target->info->spawnhealth &&
                        target->info->xdeathstate)
                        str = text[DEATHMSG_GIBROCKET];
                    break;
                case wp_plasma:
                    str = text[DEATHMSG_PLASMA];
                    break;
                case wp_bfg:
                    str = text[DEATHMSG_BFGBALL];
                    break;
                case wp_chainsaw:
                    str = text[DEATHMSG_CHAINSAW];
                    break;
                case wp_supershotgun:
                    str = text[DEATHMSG_SUPSHOTGUN];
                    break;
                default:
                    str = text[DEATHMSG_PLAYUNKNOW];
                    break;
                }
            }
            if (cv_splitscreen.value)
            {
                char txt[512];
                sprintf(txt, str, player_names[target->player-players], 
                                  player_names[source->player-players]);
                CONS_Printf(txt);
                CONS_Printf("\4%s", txt);
            }
            else
                CONS_Printf(str,player_names[target->player-players],
                                player_names[source->player-players]);
        }
    }
    else
    {
        if (!source)
        {
            // environment kills
            w = target->player->specialsector;      //see p_spec.c

            if (w==5)
                str = text[DEATHMSG_HELLSLIME];
            else if (w==7)
                str = text[DEATHMSG_NUKE];
            else if (w==16 || w==4)
                str = text[DEATHMSG_SUPHELLSLIME];
            else
                str = text[DEATHMSG_SPECUNKNOW];
        }
        else
        {
            // check for lava,slime,water,crush,fall,monsters..
            if (source->type == MT_BARREL)
            {
                if (source->target->player)
                {
                    CONS_Printf(text[DEATHMSG_BARRELFRAG],
                                player_names[target->player-players],
                                player_names[source->target->player-players]);
                    return;
                }
                else
                    str = text[DEATHMSG_BARREL];
            }
            else
            switch (source->type)
            {
              case MT_POSSESSED: str = text[DEATHMSG_POSSESSED]; break;
              case MT_SHOTGUY:   str = text[DEATHMSG_SHOTGUY];   break;
              case MT_VILE:      str = text[DEATHMSG_VILE];      break;
              case MT_FATSO:     str = text[DEATHMSG_FATSO];     break;
              case MT_CHAINGUY:  str = text[DEATHMSG_CHAINGUY];  break;
              case MT_TROOP:     str = text[DEATHMSG_TROOP];     break;
              case MT_SERGEANT:  str = text[DEATHMSG_SERGEANT];  break;
              case MT_SHADOWS:   str = text[DEATHMSG_SHADOWS];   break;
              case MT_HEAD:      str = text[DEATHMSG_HEAD];      break;
              case MT_BRUISER:   str = text[DEATHMSG_BRUISER];   break;
              case MT_UNDEAD:    str = text[DEATHMSG_UNDEAD];    break;
              case MT_KNIGHT:    str = text[DEATHMSG_KNIGHT];    break;
              case MT_SKULL:     str = text[DEATHMSG_SKULL];     break;
              case MT_SPIDER:    str = text[DEATHMSG_SPIDER];    break;
              case MT_BABY:      str = text[DEATHMSG_BABY];      break;
              case MT_CYBORG:    str = text[DEATHMSG_CYBORG];    break;
              case MT_PAIN:      str = text[DEATHMSG_PAIN];      break;
              case MT_WOLFSS:    str = text[DEATHMSG_WOLFSS];    break;
              default:           str = text[DEATHMSG_DEAD];      break;
            }
        }
        CONS_Printf(str, player_names[target->player-players]);
    }
}

// WARNING : check cv_fraglimit>0 before call this function !
void P_CheckFragLimit(player_t *p)
{
    if(cv_teamplay.value)
    {
        int fragteam=0,i;

        for(i=0;i<MAXPLAYERS;i++)
            if(ST_SameTeam(p,&players[i]))
                fragteam += ST_PlayerFrags(i);

        if(cv_fraglimit.value<=fragteam)
            G_ExitLevel();
    }
    else
    {
        if(cv_fraglimit.value<=ST_PlayerFrags(p-players))
            G_ExitLevel();
    }
}


/************************************************************
 *
 *  Returns ammo count in current weapon
 *
 ************************************************************
 */
static int P_AmmoInWeapon(player_t *player)
{
    ammotype_t  ammo = player->weaponinfo[player->readyweapon].ammo;
    int         ammo_count = player->ammo[ammo];
    
    return ammo == am_noammo ? 0
        : ammo_count ? ammo_count : -1;
}


// P_KillMobj
//
//      source is the attacker,
//      target is the 'target' of the attack, target dies...
//                                          113
void P_KillMobj ( mobj_t*       target,
                  mobj_t*       inflictor,
                  mobj_t*       source )
{
    mobjtype_t  item = 0;
    mobj_t*     mo;
    int         drop_ammo_count = 0;

extern consvar_t cv_solidcorpse;
    // dead target is no more shootable
    if( !cv_solidcorpse.value )
        target->flags &= ~MF_SHOOTABLE;

    target->flags &= ~(MF_FLOAT|MF_SKULLFLY);

    if (target->type != MT_SKULL)
        target->flags &= ~MF_NOGRAVITY;

    // scream a corpse :)
    if( target->flags & MF_CORPSE )
    {
        // turn it to gibs
        P_SetMobjState (target, S_GIBS);

        target->flags &= ~MF_SOLID;
        target->height = 0;
        target->radius<<= 1;
        target->skin = 0;

        //added:22-02-98: lets have a neat 'crunch' sound!
        S_StartSound (target, sfx_slop);
        return;
    }

    //added:22-02-98: remember who exploded the barrel, so that the guy who
    //                shot the barrel which killed another guy, gets the frag!
    //                (source is passed from barrel to barrel also!)
    //                (only for multiplayer fun, does not remember monsters)
    if ((target->type == MT_BARREL || target->type == MT_POD) &&
        source &&
        source->player)
        target->target = source;

    if( demoversion < 131 )
    {
        // in version 131 and higer this is done later in a_fall 
        // (this fix the stepping monster)
        target->flags   |= MF_CORPSE|MF_DROPOFF;
        target->height >>= 2;
        if( demoversion>=112 )
            target->radius -= (target->radius>>4);      //for solid corpses
    }
    // show death messages, only if it concern the console player
    // (be it an attacker or a target)
    if (target->player && (target->player == &players[consoleplayer]) )
        P_DeathMessages (target, inflictor, source);
    else
    if (source && source->player && (source->player == &players[consoleplayer]) )
        P_DeathMessages (target, inflictor, source);
    else
    if (target->player && target->player->bot)	//added by AC for acbot
       P_DeathMessages (target, inflictor, source);



    // if killed by a player
    if (source && source->player)
    {
        // count for intermission
        if (target->flags & MF_COUNTKILL)
            source->player->killcount++;

        // count frags if player killed player
        if (target->player)
        {
            source->player->frags[target->player-players]++;
            if( gamemode == heretic )
            {
                if(source->player == &players[displayplayer] 
                || source->player == &players[secondarydisplayplayer] )
                    S_StartSound(NULL, sfx_gfrag);

                // Make a super chicken
                if(source->player->chickenTics)
                    P_GivePower(source->player, pw_weaponlevel2);
            }
            // check fraglimit cvar
            if (cv_fraglimit.value)
                P_CheckFragLimit(source->player);
        }
    }
    else if (!multiplayer && (target->flags & MF_COUNTKILL))
    {
        // count all monster deaths,
        // even those caused by other monsters
        players[0].killcount++;
    }

    // if a player avatar dies...
    if (target->player)
    {
        // count environment kills against you (you fragged yourself!)
        if (!source)
            target->player->frags[target->player-players]++;

        target->flags &= ~MF_SOLID;                     // does not block
        target->flags2 &= ~MF2_FLY;
        target->player->powers[pw_flight] = 0;
        target->player->powers[pw_weaponlevel2] = 0;
        target->player->playerstate = PST_DEAD;
        P_DropWeapon (target->player);                  // put weapon away
        if (target->player == &players[consoleplayer])
        {
            // don't die in auto map,
            // switch view prior to dying
            if (automapactive)
                AM_Stop ();

            //added:22-02-98: recenter view for next live...
            localaiming = 0;
        }
        if (target->player == &players[secondarydisplayplayer])
        {
            //added:22-02-98: recenter view for next live...
            localaiming2 = 0;
        }
/* HERETODO
        if(target->flags2&MF2_FIREDAMAGE)
        { // Player flame death
            P_SetMobjState(target, S_PLAY_FDTH1);
            //S_StartSound(target, sfx_hedat1); // Burn sound
            return;
        }
*/
    }

    if (( (gamemode != heretic && target->health < -target->info->spawnhealth)
        ||(gamemode == heretic && target->health < -(target->info->spawnhealth>>1)))
        && target->info->xdeathstate)
    {
        P_SetMobjState (target, target->info->xdeathstate);
    }
    else
        P_SetMobjState (target, target->info->deathstate);

    target->tics -= P_Random()&3;

    if (target->tics < 1)
        target->tics = 1;

    // Drop stuff.
    // This determines the kind of object spawned
    // during the death frame of a thing.

    // Frags Weapon Falling support
    if (target->player && cv_fragsweaponfalling.value )
    {
        drop_ammo_count = P_AmmoInWeapon(target->player);
        //if (!drop_ammo_count)
        //    return;
        
        if (gamemode == heretic)
        {
            switch (target->player->readyweapon)
            {
                case wp_crossbow:
                    item = MT_HMISC15;
                    break;

                case wp_blaster:
                    item = MT_RIPPER;
                    break;

                case wp_skullrod:
                    item = MT_WSKULLROD;
                    break;

                case wp_phoenixrod:
                    item = MT_WPHOENIXROD;
                    break;

                case wp_mace:
                    item = MT_WMACE;
                    break;

                default:
                    //CONS_Printf("Unknown weapon %d\n", target->player->readyweapon);
                    return;
            }
        }
        else
        {
            switch (target->player->readyweapon)
            {
                case wp_shotgun:
                    item = MT_SHOTGUN;
                    break;

                case wp_supershotgun:
                    item = MT_SUPERSHOTGUN;
                    break;

                case wp_chaingun:
                    item = MT_CHAINGUN;
                    break;

                case wp_missile:
                    item = MT_ROCKETLAUNCH;
                    break;

                case wp_plasma:
                    item = MT_PLASMAGUN;
                    break;

                case wp_bfg:
                    item = MT_BFG9000;
                    break;

                default:
                    //CONS_Printf("Unknown weapon %d\n", target->player->readyweapon);
                    return;
            }
        }
    }
    else
    {

        switch (target->type)
        {
            case MT_WOLFSS:
            case MT_POSSESSED:
                item = MT_CLIP;
                break;

            case MT_SHOTGUY:
                item = MT_SHOTGUN;
                break;

            case MT_CHAINGUY:
                item = MT_CHAINGUN;
                break;

            default:
                return;
        }
    }

    // SoM: Damnit! Why not use the target's floorz?
    mo = P_SpawnMobj (target->x, target->y, demoversion<132 ? ONFLOORZ : target->floorz, item);
    mo->flags |= MF_DROPPED;    // special versions of items

    if (!cv_fragsweaponfalling.value)
        drop_ammo_count = 0;    // Doom default ammo count

    mo->dropped_ammo_count = drop_ammo_count;
}


//---------------------------------------------------------------------------
//
// FUNC P_MinotaurSlam
//
//---------------------------------------------------------------------------

void P_MinotaurSlam(mobj_t *source, mobj_t *target)
{
    angle_t angle;
    fixed_t thrust;
    
    angle = R_PointToAngle2(source->x, source->y, target->x, target->y);
    angle >>= ANGLETOFINESHIFT;
    thrust = 16*FRACUNIT+(P_Random()<<10);
    target->momx += FixedMul(thrust, finecosine[angle]);
    target->momy += FixedMul(thrust, finesine[angle]);
    P_DamageMobj(target, NULL, NULL, HITDICE(6));
    if(target->player)
    {
        target->reactiontime = 14+(P_Random()&7);
    }
}

//---------------------------------------------------------------------------
//
// FUNC P_TouchWhirlwind
//
//---------------------------------------------------------------------------

boolean P_TouchWhirlwind(mobj_t *target)
{
    int randVal;
    
    target->angle += P_SignedRandom()<<20;
    target->momx += P_SignedRandom()<<10;
    target->momy += P_SignedRandom()<<10;
    if(leveltime&16 && !(target->flags2&MF2_BOSS))
    {
        randVal = P_Random();
        if(randVal > 160)
        {
            randVal = 160;
        }
        target->momz += randVal<<10;
        if(target->momz > 12*FRACUNIT)
        {
            target->momz = 12*FRACUNIT;
        }
    }
    if(!(leveltime&7))
    {
        return P_DamageMobj(target, NULL, NULL, 3);
    }
    return false;
}

//---------------------------------------------------------------------------
//
// FUNC P_ChickenMorphPlayer
//
// Returns true if the player gets turned into a chicken.
//
//---------------------------------------------------------------------------

boolean P_ChickenMorphPlayer(player_t *player)
{
    mobj_t *pmo;
    mobj_t *fog;
    mobj_t *chicken;
    fixed_t x;
    fixed_t y;
    fixed_t z;
    angle_t angle;
    int oldFlags2;
    
    if(player->chickenTics)
    {
        if((player->chickenTics < CHICKENTICS-TICRATE)
            && !player->powers[pw_weaponlevel2])
        { // Make a super chicken
            P_GivePower(player, pw_weaponlevel2);
        }
        return(false);
    }
    if(player->powers[pw_invulnerability])
    { // Immune when invulnerable
        return(false);
    }
    pmo = player->mo;
    x = pmo->x;
    y = pmo->y;
    z = pmo->z;
    angle = pmo->angle;
    oldFlags2 = pmo->flags2;
    P_SetMobjState(pmo, S_FREETARGMOBJ);
    fog = P_SpawnMobj(x, y, z+TELEFOGHEIGHT, MT_TFOG);
    S_StartSound(fog, sfx_telept);
    chicken = P_SpawnMobj(x, y, z, MT_CHICPLAYER);
    chicken->special1 = player->readyweapon;
    chicken->angle = angle;
    chicken->player = player;
    player->health = chicken->health = MAXCHICKENHEALTH;
    player->mo = chicken;
    player->armorpoints = player->armortype = 0;
    player->powers[pw_invisibility] = 0;
    player->powers[pw_weaponlevel2] = 0;
    player->weaponinfo = wpnlev1info;
    if(oldFlags2&MF2_FLY)
    {
        chicken->flags2 |= MF2_FLY;
    }
    player->chickenTics = CHICKENTICS;
    P_ActivateBeak(player);
    return(true);
}

//---------------------------------------------------------------------------
//
// FUNC P_ChickenMorph
//
//---------------------------------------------------------------------------

boolean P_ChickenMorph(mobj_t *actor)
{
    mobj_t *fog;
    mobj_t *chicken;
    mobj_t *target;
    mobjtype_t moType;
    fixed_t x;
    fixed_t y;
    fixed_t z;
    angle_t angle;
    int ghost;
    
    if(actor->player)
    {
        return(false);
    }
    moType = actor->type;
    switch(moType)
    {
        case MT_POD:
        case MT_CHICKEN:
        case MT_HHEAD:
        case MT_MINOTAUR:
        case MT_SORCERER1:
        case MT_SORCERER2:
            return(false);
        default:
            break;
    }
    x = actor->x;
    y = actor->y;
    z = actor->z;
    angle = actor->angle;
    ghost = actor->flags&MF_SHADOW;
    target = actor->target;
    P_SetMobjState(actor, S_FREETARGMOBJ);
    fog = P_SpawnMobj(x, y, z+TELEFOGHEIGHT, MT_TFOG);
    S_StartSound(fog, sfx_telept);
    chicken = P_SpawnMobj(x, y, z, MT_CHICKEN);
    chicken->special2 = moType;
    chicken->special1 = CHICKENTICS+P_Random();
    chicken->flags |= ghost;
    chicken->target = target;
    chicken->angle = angle;
    return(true);
}

//---------------------------------------------------------------------------
//
// FUNC P_AutoUseChaosDevice
//
//---------------------------------------------------------------------------

boolean P_AutoUseChaosDevice(player_t *player)
{
    int i;
    
    for(i = 0; i < player->inventorySlotNum; i++)
    {
        if(player->inventory[i].type == arti_teleport)
        {
            P_PlayerUseArtifact(player, arti_teleport);
            player->health = player->mo->health = (player->health+1)/2;
            return(true);
        }
    }
    return(false);
}

//---------------------------------------------------------------------------
//
// PROC P_AutoUseHealth
//
//---------------------------------------------------------------------------

void P_AutoUseHealth(player_t *player, int saveHealth)
{
    int i;
    int count;
    int normalCount;
    int normalSlot;
    int superCount;
    int superSlot;
    
    normalCount = superCount = 0;
    for(i = 0; i < player->inventorySlotNum; i++)
    {
        if(player->inventory[i].type == arti_health)
        {
            normalSlot = i;
            normalCount = player->inventory[i].count;
        }
        else if(player->inventory[i].type == arti_superhealth)
        {
            superSlot = i;
            superCount = player->inventory[i].count;
        }
    }
    if((gameskill == sk_baby) && (normalCount*25 >= saveHealth))
    { // Use quartz flasks
        count = (saveHealth+24)/25;
        for(i = 0; i < count; i++)
            P_PlayerUseArtifact( player, arti_health);
    }
    else if(superCount*100 >= saveHealth)
    { // Use mystic urns
        count = (saveHealth+99)/100;
        for(i = 0; i < count; i++)
            P_PlayerUseArtifact( player, arti_superhealth);
    }
    else if((gameskill == sk_baby)
        && (superCount*100+normalCount*25 >= saveHealth))
    { // Use mystic urns and quartz flasks
        count = (saveHealth+24)/25;
        for(i = 0; i < count; i++)
            P_PlayerUseArtifact( player, arti_health);

        saveHealth -= count*25;
        count = (saveHealth+99)/100;
        for(i = 0; i < count; i++)
            P_PlayerUseArtifact( player, arti_superhealth);
    }
    player->mo->health = player->health;
}


//
// P_DamageMobj
// Damages both enemies and players
// "inflictor" is the thing that caused the damage
//  creature or missile, can be NULL (slime, etc)
// "source" is the thing to target after taking damage
//  creature or NULL
// Source and inflictor are the same for melee attacks.
// Source can be NULL for slime, barrel explosions
// and other environmental stuff.
//
boolean P_DamageMobj ( mobj_t*   target,
                       mobj_t*   inflictor,
                       mobj_t*   source,
                       int       damage )
{
    unsigned    ang;
    int         saved;
    player_t*   player;
    fixed_t     thrust;
    boolean     takedamage;  // false on some case in teamplay

    if ( !(target->flags & MF_SHOOTABLE) )
        return false; // shouldn't happen...

    if (target->health <= 0)
        return false;

    if ( target->flags & MF_SKULLFLY )
    {
        // Minotaur is invulnerable during charge attack
        if(target->type == MT_MINOTAUR)
            return false;

        target->momx = target->momy = target->momz = 0;
    }

    player = target->player;
    if (player && gameskill == sk_baby)
        damage >>= 1;   // take half damage in trainer mode

    // Special damage types
    if(inflictor)
    {
        switch(inflictor->type)
        {
        case MT_EGGFX:
            if(player)
            {
                P_ChickenMorphPlayer(player);
            }
            else
            {
                P_ChickenMorph(target);
            }
            return false; // Always return
        case MT_WHIRLWIND:
            return P_TouchWhirlwind(target);
        case MT_MINOTAUR:
            if(inflictor->flags&MF_SKULLFLY)
            { // Slam only when in charge mode
                P_MinotaurSlam(inflictor, target);
                return true;
            }
            break;
        case MT_MACEFX4: // Death ball
            if((target->flags2&MF2_BOSS) || target->type == MT_HHEAD)
            { // Don't allow cheap boss kills
                break;
            }
            else if(target->player)
            { // Player specific checks
                if(target->player->powers[pw_invulnerability])
                { // Can't hurt invulnerable players
                    break;
                }
                if(P_AutoUseChaosDevice(target->player))
                { // Player was saved using chaos device
                    return false;
                }
            }
            damage = 10000; // Something's gonna die
            break;
        case MT_PHOENIXFX2: // Flame thrower
            if(target->player && P_Random() < 128)
            { // Freeze player for a bit
                target->reactiontime += 4;
            }
            break;
        case MT_RAINPLR1: // Rain missiles
        case MT_RAINPLR2:
        case MT_RAINPLR3:
        case MT_RAINPLR4:
            if(target->flags2&MF2_BOSS)
            { // Decrease damage for bosses
                damage = (P_Random()&7)+1;
            }
            break;
        case MT_HORNRODFX2:
        case MT_PHOENIXFX1:
            if(target->type == MT_SORCERER2 && P_Random() < 96)
            { // D'Sparil teleports away
                P_DSparilTeleport(target);
                return false;
            }
            break;
        case MT_BLASTERFX1:
        case MT_RIPPER:
            if(target->type == MT_HHEAD)
            { // Less damage to Ironlich bosses
                damage = P_Random()&1;
                if(!damage)
                    return false;
            }
            break;
        default:
            break;
        }
    }

    // Some close combat weapons should not
    // inflict thrust and push the victim out of reach,
    // thus kick away unless using the chainsaw.
    if (inflictor
        && !(target->flags & MF_NOCLIP)
        && !(inflictor->flags2&MF2_NODMGTHRUST)
        && (!source
            || !source->player
            || source->player->readyweapon != wp_chainsaw))
    {
        fixed_t            amomx, amomy, amomz=0;//SoM: 3/28/2000
        extern consvar_t   cv_allowrocketjump;

        ang = R_PointToAngle2 ( inflictor->x,
                                inflictor->y,
                                target->x,
                                target->y);

        if (gamemode == heretic )
            thrust = damage*(FRACUNIT>>3)*150/target->info->mass;
        else
            thrust = damage*(FRACUNIT>>3)*100/target->info->mass;

        // sometimes a target shot down might fall off a ledge forwards
        if ( damage < 40
             && damage > target->health
             && target->z - inflictor->z > 64*FRACUNIT
             && (P_Random ()&1) )
        {
            ang += ANG180;
            thrust *= 4;
        }

        ang >>= ANGLETOFINESHIFT;

        if(gamemode == heretic && source && source->player && (source == inflictor)
            && source->player->powers[pw_weaponlevel2]
            && source->player->readyweapon == wp_staff)
        {
            // Staff power level 2
            target->momx += FixedMul(10*FRACUNIT, finecosine[ang]);
            target->momy += FixedMul(10*FRACUNIT, finesine[ang]);
            if(!(target->flags&MF_NOGRAVITY))
            {
                target->momz += 5*FRACUNIT;
            }
        }
        else
        {
            amomx = FixedMul (thrust, finecosine[ang]);
            amomy = FixedMul (thrust, finesine[ang]);
            target->momx += amomx;
            target->momy += amomy;
            
            // added momz (do it better for missiles explotion)
            if (source && demoversion>=124 && (demoversion<129 || !cv_allowrocketjump.value))
            {
                int dist,z;
                
                if(source==target) // rocket in yourself (suicide)
                {
                    viewx=inflictor->x;
                    viewy=inflictor->y;
                    z=inflictor->z;
                }
                else
                {
                    viewx=source->x;
                    viewy=source->y;
                    z=source->z;
                }
                dist=R_PointToDist(target->x,target->y);
                
                viewx=0;
                viewy=z;
                ang = R_PointToAngle(dist,target->z);
                
                ang >>= ANGLETOFINESHIFT;
                amomz = FixedMul (thrust, finesine[ang]);
            }
            else //SoM: 2/28/2000: Added new function.
            if(demoversion >= 129 && cv_allowrocketjump.value)
            {
                fixed_t delta1 = abs(inflictor->z - target->z);
                fixed_t delta2 = abs(inflictor->z - (target->z + target->height));
                amomz = (abs(amomx) + abs(amomy))>>1;
                
                if(delta1 >= delta2 && inflictor->momz < 0)
                    amomz = -amomz;
            }
            target->momz += amomz;
#ifdef CLIENTPREDICTION2
            if( target->player && target->player->spirit )
            {
                target->player->spirit->momx += amomx;
                target->player->spirit->momy += amomy;
                target->player->spirit->momz += amomz;
            }
#endif  
        }
    }

    takedamage = false;
    // player specific
    if (player && (target->flags & MF_CORPSE)==0)
    {
        // end of game hell hack
        if (target->subsector->sector->special == 11
            && damage >= target->health)
        {
            damage = target->health - 1;
        }


        // Below certain threshold,
        // ignore damage in GOD mode, or with INVUL power.
        if ( damage < 1000
             && ( (player->cheats&CF_GODMODE)
                  || player->powers[pw_invulnerability] ) )
        {
            return false;
        }

        if (player->armortype)
        {
            if (player->armortype == 1)
                saved = gamemode == heretic ? damage>>1 : damage/3;
            else
                saved = gamemode == heretic ? (damage>>1)+(damage>>2) : damage/2;

            if (player->armorpoints <= saved)
            {
                // armor is used up
                saved = player->armorpoints;
                player->armortype = 0;
            }
            player->armorpoints -= saved;
            damage -= saved;
        }

        // added team play and teamdamage (view logboris at 13-8-98 to understand)
        if( demoversion < 125   || // support old demoversion
            cv_teamdamage.value ||
            damage>1000         || // telefrag
            source==target      ||
            !source             ||
            !source->player     ||
            (
             cv_deathmatch.value
             &&
             (!cv_teamplay.value ||
              !ST_SameTeam(source->player,player)
             )
            )
          )
        {
            if(damage >= player->health
                && ((gameskill == sk_baby) || cv_deathmatch.value)
                && !player->chickenTics)
            { // Try to use some inventory health
                P_AutoUseHealth(player, damage-player->health+1);
            }

            player->health -= damage;   // mirror mobj health here for Dave
            if (player->health < 0)
                player->health = 0;
            takedamage = true;

            player->damagecount += damage;  // add damage after armor / invuln

            if (player->damagecount > 100)
                player->damagecount = 100;  // teleport stomp does 10k points...

            //added:22-02-98: force feedback ??? electro-shock???
            if (player == &players[consoleplayer])
                I_Tactile (40,10,40+min(damage, 100)*2);
        }
        player->attacker = source;
    }
    else
        takedamage = true;

    if( takedamage )
    {
        // do the damage
        target->health -= damage;
        if (target->health <= 0)
        {
            target->special1 = damage;
            if(player && inflictor && !player->chickenTics)
            { // Check for flame death
                if((inflictor->flags2&MF2_FIREDAMAGE)
                    || ((inflictor->type == MT_PHOENIXFX1)
                    && (target->health > -50) && (damage > 25)))
                {
                    target->flags2 |= MF2_FIREDAMAGE;
                }
            }

            P_KillMobj ( target, inflictor, source );
            return true;
        }


        if ( (P_Random () < target->info->painchance)
             && !(target->flags&(MF_SKULLFLY|MF_CORPSE)) )
        {
            target->flags |= MF_JUSTHIT;    // fight back!

            P_SetMobjState (target, target->info->painstate);
        }

        target->reactiontime = 0;           // we're awake now...
    }

    if ( (!target->threshold || target->type == MT_VILE)
         && source && source != target
         && source->type != MT_VILE
         && !(source->flags2&MF2_BOSS)
         && !(target->type == MT_SORCERER2 && source->type == MT_WIZARD))
    {
        // if not intent on another player,
        // chase after this one
        target->target = source;
        target->threshold = BASETHRESHOLD;
        if (target->state == &states[target->info->spawnstate]
            && target->info->seestate != S_NULL)
            P_SetMobjState (target, target->info->seestate);
    }

    return takedamage;
}
