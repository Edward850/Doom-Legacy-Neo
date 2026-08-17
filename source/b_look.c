// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: b_look.c,v 1.5 2003/06/11 04:20:45 ssntails Exp $
//
// Copyright (C) 2002 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
//
// $Log: b_look.c,v $
// Revision 1.5  2003/06/11 04:20:45  ssntails
// Keep stupid bots from trying to get to items on 3d Floors.
//
// Revision 1.4  2003/06/11 04:04:50  ssntails
// Rellik's Bot Code!
//
// Revision 1.3  2002/09/28 06:53:11  tonyd
// fixed CR problem, fixed game options crash
//
// Revision 1.2  2002/09/27 16:40:08  tonyd
// First commit of acbot
//

#include "b_bot.h"
#include "b_game.h"
#include "b_look.h"
//#include "bot_move.h"
#include "b_node.h"
//#include "bot_ctf.h"

#include "g_game.h"
#include "r_defs.h"
#include "p_local.h"
#include "m_random.h"
#include "r_main.h"
#include "z_zone.h"
           
#define MAX_TRAVERSE_DIST 100000000 //10 meters, used within b_func.c

extern int maxsoul;
extern int max_armor;
extern thinker_t thinkercap;

//Used with Reachable().
static mobj_t	*looker,
				*destMobj;
static sector_t *last_s;

static boolean PTR_QuickReachable (intercept_t *in)
{
	fixed_t floorheight, ceilingheight;
	line_t *line;
	mobj_t* thing;
	sector_t *s;

	if (in->isaline)
	{
		line = in->d.line;

		if (!(line->flags & ML_TWOSIDED) || (line->flags & ML_BLOCKING))
			return false; //Cannot continue.
		else
		{
			//Determine if going to use backsector/frontsector.
			s = (line->backsector == last_s) ? line->frontsector : line->backsector;
			ceilingheight = s->ceilingheight;
			floorheight = s->floorheight;

			if ((((floorheight <= (last_s->floorheight+(37<<FRACBITS))) || (((floorheight <= (last_s->floorheight+(45<<FRACBITS))) && (last_s->floortype != FLOOR_WATER))))
				&& (((ceilingheight == floorheight) && line->special)
					|| ((ceilingheight - floorheight) >= looker->height)))) //Does it fit?
			{
				last_s = s;
				return true;
			}
			else
				return false;
		}
	}
	else
	{
		thing = in->d.thing;
		if ((thing != looker) && (thing != destMobj) && (thing->flags & MF_SOLID))
			return false;
	}

	return true;
}

boolean B_Reachable(player_t* p, mobj_t* mo)
{
	looker = p->mo;
	destMobj = mo;
	last_s = p->mo->subsector->sector;

#ifdef FIXROVERBUGS
// Bots shouldn't try to get stuff that's on a 3dfloor they can't get to. SSNTails 06-10-2003
    if(p->mo->subsector == mo->subsector && p->mo->subsector->sector->ffloors)
    {
      ffloor_t*  rover;

      for(rover = mo->subsector->sector->ffloors; rover; rover = rover->next)
      {
        if(!(rover->flags & FF_SOLID) || !(rover->flags & FF_EXISTS)) continue;

		if(*rover->topheight <= p->mo->z && mo->z < *rover->topheight)
			return false;

		if(*rover->bottomheight >= p->mo->z + p->mo->height
			&& mo->z > *rover->bottomheight)
			return false;
      }
    }
#endif

	return P_PathTraverse (p->mo->x, p->mo->y, mo->x, mo->y, PT_ADDLINES|PT_ADDTHINGS, PTR_QuickReachable);
}

//Checks TRUE reachability from
//one actor to another. First mobj (actor) is looker.
boolean B_ReachablePoint (player_t *p, sector_t* destSector, fixed_t x, fixed_t y)
{
/*	if ((destSector->ceilingheight -
		 destSector->floorheight)
		< p->mo->height) //Where target is, looker can't be.
		return false;*/

	//if (p->mo->subsector->sector == destSector)
	//	return true;    

	looker = p->mo;
	destMobj = NULL;
	last_s = p->mo->subsector->sector;

	return P_PathTraverse (p->mo->x, p->mo->y, x, y, PT_ADDLINES|PT_ADDTHINGS, PTR_QuickReachable);
}
//
// B_LookForSpecialLine
//
// This function looks through the sector the bot is in and, one sector level outwards,
// very inefficient cause searches of sectors are done multiple times
// when a sector has many linedefs between a single sector-sector boundary
//  must fix this, perhaps use the visited boolean
// maybe should do search through thes switches array instead
//

boolean B_LookForSpecialLine(player_t* p, fixed_t* x, fixed_t* y)
{
	int				i,
					j;
	sector_t		*specialsector,
					*insector,
					*sector;
	line_t			*edge;
	msecnode_t		*insectornode;

	insectornode = p->mo->touching_sectorlist;
	while (insectornode)
	{
		insector = insectornode->m_sector;
		for (i = 0; i < insector->linecount; i++)
		{
			edge = insector->lines[i];
			specialsector = (insector == edge->frontsector) ? edge->backsector : edge->frontsector;
			if (//edge->special && !(edge->special & ML_REPEAT_SPECIAL))//P_CheckTag(edge) && (!specialsector || !specialsector->ceilingdata))//line!(line->flags & ML_TWOSIDED) || (line->flags & ML_BLOCKING))
				//((edge->special & TriggerType) >> TriggerTypeShift) == SwitchOnce) || (((edge->special & TriggerType) >> TriggerTypeShift) == PushOnce))
				(edge->special == 31) /*(edge->special == 1) || ||*/
				|| (edge->special == 23) || (edge->special == 102) || (edge->special == 103) || (edge->special == 71))//switches*/
			{
				*x = (edge->v1->x + edge->v2->x)/2;
				*y = (edge->v1->y + edge->v2->y)/2;

				return true;
			}
			else if (edge->sidenum[1] >= 0)	//if its a double sided sector
			{
				 if (edge->frontsector == insector)
					 sector = edge->backsector;
				 else
					 sector = edge->frontsector;

				for (j = 0; j < sector->linecount; j++)
				{
					edge = sector->lines[j];
					specialsector = (sector == edge->frontsector) ? edge->backsector : edge->frontsector;
					if (//edge->special && !(edge->special & ML_REPEAT_SPECIAL))//P_CheckTag(edge) && (!specialsector || !specialsector->ceilingdata))//line!(line->flags & ML_TWOSIDED) || (line->flags & ML_BLOCKING))
						//(((edge->special & TriggerType) >> TriggerTypeShift) == SwitchOnce) || (((edge->special & TriggerType) >> TriggerTypeShift) == PushOnce))
						//(edge->frontsector == sector)	//if its a pressable switch
						 (edge->special == 31)	//doors
						 ||(edge->special == 23) || (edge->special == 102) || (edge->special == 103) || (edge->special == 71))//switches*/
					{
						*x = (edge->v1->x + edge->v2->x)/2;
						*y = (edge->v1->y + edge->v2->y)/2;

						return true;
					}
				}
			}
		}
		insectornode = insectornode->m_snext;
	}

	return false;
}


//
// B_LookForThings
//
void B_LookForThings (player_t* p)
{
	boolean			enemyFound = false;

	fixed_t			bestItemDistance = 0,
					bestSeenItemDistance = 0,
					closestEnemyDistance = 0,
					closestMissileDistance = 0,
					closestUnseenEnemyDistance = 0,
					closestUnseenTeammateDistance = 0,
					furthestTeammateDistance = 0,
					thingDistance = 0;

	double			bestItemWeight = 0.0,	//used to determine best object to get
					bestSeenItemWeight = 0.0,
					itemWeight = 0.0;

	mobj_t			*bestSeenItem = NULL,
					*bestItem = NULL,
					*mo;
    thinker_t*		currentthinker;

	p->bot->closestEnemy = NULL;
	p->bot->closestMissile = NULL;
	p->bot->closestUnseenEnemy = NULL;
	p->bot->closestUnseenTeammate = NULL;
	p->bot->teammate = NULL;
	p->bot->bestSeenItem = NULL;
	p->bot->bestItem = NULL;

    currentthinker = thinkercap.next;
    while (currentthinker != &thinkercap)	//search through the list of all thinkers
    {
        if (currentthinker->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			itemWeight = 0;		//initalise to no weight, best items have greatest weight
			mo = (mobj_t *)currentthinker;
			thingDistance = P_AproxDistance (p->mo->x - mo->x, p->mo->y - mo->y);

			if (((mo->flags & MF_COUNTKILL) || (mo->type == MT_SKULL) || (mo->type == MT_BARREL)) && (mo->flags & MF_SOLID))//its a monster thats not dead
				enemyFound = true;
			else if (mo->player)
			{
				if ((p != mo->player) && (mo->flags & MF_SOLID))
				{
					if (cv_deathmatch.value)
						enemyFound = true;
					else
					{
						if (B_Reachable(p, mo))	//i can reach this teammate
						{
							if ((thingDistance > furthestTeammateDistance) && (!p->bot->teammate && !mo->player->bot))
							{
								furthestTeammateDistance = thingDistance;
								p->bot->teammate = mo;
								//CONS_Printf("found a teammate\n");
							}
						}
						else	//i can not reach this teammate
						{
							SearchNode_t* tempNode = B_GetNodeAt(mo->x, mo->y);
							if (tempNode
								&& (!closestUnseenTeammateDistance
									|| ((thingDistance < closestUnseenTeammateDistance)
										&& (!p->bot->teammate
											|| (!mo->player->bot && p->bot->teammate->player->bot)))))
							{
								closestUnseenTeammateDistance = thingDistance;
								p->bot->closestUnseenTeammate = mo;
								//CONS_Printf("found a teammate\n");
							}
						}

					}
				}
			}
			else if (mo->flags & MF_MISSILE)	//is it a threatening missile
			{
				if (mo->target != p->mo)	//if its an enemies missile I had better avoid it
				{							//important: a missiles "target" is actually its owner...
					// see if the missile is heading my way, if the missile will be closer to me, next tick
					//then its heading at least somewhat towards me, so better dodge it
					if (P_AproxDistance (p->mo->x+p->mo->momx - (mo->x+mo->momx), p->mo->y+p->mo->momy - (mo->y+mo->momy)) < thingDistance)
					{
						//if its the closest missile and its reasonably close I should try and avoid it
						if (thingDistance && (!closestMissileDistance || (thingDistance < closestMissileDistance))
						&& ((thingDistance>>FRACBITS) <= 300))
						{
							closestMissileDistance = thingDistance;
							p->bot->closestMissile = mo;
						}
					}
					thingDistance = 0;
				}
			}
			else if (((mo->flags & MF_SPECIAL) || (mo->flags & MF_DROPPED)))	//most likely a pickup
			{
				if(gamemode == heretic)
				{
					switch (mo->type)
					{
//  HERETIC??? --> ///////// bonuses/powerups ////////////////////////
					case MT_ARTIINVULNERABILITY:	//invulnrability always run to get it
						if (cv_deathmatch.value || !p->powers[pw_invulnerability])
							itemWeight = 10;
						break;
					case MT_ARTIINVISIBILITY:	//invisability
						if (cv_deathmatch.value || !p->powers[pw_invisibility])
							itemWeight = 9;
						break;
					case MT_ARTISUPERHEAL:	//soul sphere
						if (cv_deathmatch.value || p->health < maxsoul)
							itemWeight = 8;
						break;
					case MT_ITEMSHIELD2:	//blue armour, if we have >= maxarmour, its impossible to get
						if (p->armorpoints < max_armor)
							itemWeight = 8;
						break;
					case MT_ITEMSHIELD1:	//green armour
						if (p->armorpoints < (max_armor/2))
							itemWeight = 5;
						break;
					case SPR_MEDI: case SPR_STIM: //medication
						if (p->health < 50)
							itemWeight = 6;
						else if (p->health < 60)
							itemWeight = 5;
						else if (p->health < 80)
							itemWeight = 4;
						else if (p->health < 100)
							itemWeight = 3;
						break;
					case SPR_BON1:	//health potion
						if (p->mo->health < maxsoul)
							itemWeight = 1;
						break;
					case SPR_BON2:	//armour bonus
						if (p->armorpoints < max_armor)
							itemWeight = 1;
						break;

	/////////////// weapons ////////////////////////////				
					case SPR_SHOT:
						if (!p->weaponowned[wp_shotgun])
						{
							if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
								|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
								|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
								|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
								itemWeight = 4;
							else
								itemWeight = 6;
						}
						else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_shell] < p->maxammo[am_shell]))
							itemWeight = 3;
						break;
					case SPR_MGUN:
						if (!p->weaponowned[wp_chaingun])
						{
							if ((p->weaponowned[wp_missile] && p->ammo[am_misl])
								|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
								|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
								itemWeight = 5;
							else
								itemWeight = 6;
						}
						else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_clip] < p->maxammo[am_clip]))
							itemWeight = 3;
						break;
					case SPR_LAUN:
						if (!p->weaponowned[wp_missile])
						{
							if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
								|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
								|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
								itemWeight = 5;
							else
								itemWeight = 7;
						}
						else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_misl] < p->maxammo[am_misl]))
							itemWeight = 3;
						break;
					case SPR_PLAS:
						if (!p->weaponowned[wp_plasma])
						{
							if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
								|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
								|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
								itemWeight = 5;
							else
								itemWeight = 7;
						}
						else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_cell] < p->maxammo[am_cell]))
							itemWeight = 3;
						break;
					case SPR_BFUG:
						if (!p->weaponowned[wp_bfg])
						{
							if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
								|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
								|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
								|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
								itemWeight = 5;
							else
								itemWeight = 7;
						}
						else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_cell] < p->maxammo[am_cell]))
							itemWeight = 3;
						break;
					case SPR_SGN2:
						if (!p->weaponowned[wp_supershotgun])
						{
							if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
								|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
								|| (p->weaponowned[wp_plasma] && p->ammo[am_cell]))
								itemWeight = 5;
							else
								itemWeight = 7;
						}
						else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_shell] < p->maxammo[am_shell]))
							itemWeight = 3;
						break;

	/////////////////////ammo
					case SPR_CLIP: case SPR_AMMO:
						if (!p->ammo[am_clip] && ((p->readyweapon == wp_fist)
							|| (p->readyweapon == wp_chainsaw)))
							itemWeight = 6;
						else
							itemWeight = (p->ammo[am_clip] < p->maxammo[am_clip]) ? 3 : 0;
						break;
					case SPR_SHEL: case SPR_SBOX:
						if ((p->weaponowned[wp_shotgun] || p->weaponowned[wp_supershotgun]) && !p->ammo[am_shell]
							&& ((p->readyweapon == wp_fist) || (p->readyweapon == wp_chainsaw)))
							itemWeight = 6;
						else
							itemWeight = (p->ammo[am_shell] < p->maxammo[am_shell]) ? 3 : 0;
						break;
					case SPR_ROCK: case SPR_BROK:
						if (p->weaponowned[wp_missile] && !p->ammo[am_misl] && ((p->readyweapon == wp_fist)
							|| (p->readyweapon == wp_chainsaw)))
							itemWeight = 6;
						else
							itemWeight = (p->ammo[am_misl] < p->maxammo[am_misl]) ? 3 : 0;
						break;
					case SPR_CELL: case SPR_CELP:
						if (p->weaponowned[wp_plasma] && !p->ammo[am_cell] && ((p->readyweapon == wp_fist)
							|| (p->readyweapon == wp_chainsaw)))
							itemWeight = 6;
						else
							itemWeight = (p->ammo[am_cell] < p->maxammo[am_cell]) ? 3 : 0;
						break;

	///////////////////////keys
					case SPR_BKEY:
						if (!(p->cards & it_bluecard))
							itemWeight = 5;
						break;
					case SPR_BSKU:
						if (!(p->cards & it_blueskull))
							itemWeight = 5;
						break;
					case SPR_RKEY:
						if (!(p->cards & it_redcard))
							itemWeight = 5;
						break;
					case SPR_RSKU:
						if (!(p->cards & it_redskull))
							itemWeight = 5;
						break;
					case SPR_YKEY:
						if (!(p->cards & it_yellowcard))
							itemWeight = 5;
						break;
					case SPR_YSKU:
						if (!(p->cards & it_yellowskull))
							itemWeight = 5;
						break;
					default:
						itemWeight = 0;	//dont want it
						break;
					}
				}
				else switch (mo->sprite)
				{
//NON-HERETIC???////////// bonuses/powerups now checks for skill level
				case SPR_PINV:	//invulnrability always run to get it
					if (gameskill == sk_baby)
					{
						if (cv_deathmatch.value || !p->powers[pw_invulnerability])
							itemWeight = 2;
					}
					else if (gameskill == sk_easy)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invulnerability]) 
							itemWeight = 5; 
					}
					else if (gameskill == sk_medium)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invulnerability]) 
							itemWeight = 6; 
					}
					else if (gameskill == sk_hard)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invulnerability]) 
							itemWeight = 8; 
					}
					else if (gameskill == sk_nightmare)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invulnerability]) 
							itemWeight = 10; 
					}
					break;
				case SPR_MEGA: //megasphere
					if (gameskill == sk_baby)
					{ 
						if (cv_deathmatch.value || (p->health < maxsoul || p->armorpoints < max_armor)) 
							itemWeight = 2; 
					}
					else if (gameskill == sk_easy)
					{ 
						if (cv_deathmatch.value || (p->health < maxsoul || p->armorpoints < max_armor)) 
							itemWeight = 4; 
					}
					else if (gameskill == sk_medium)
					{ 
						if (cv_deathmatch.value || (p->health < maxsoul || p->armorpoints < max_armor)) 
							itemWeight = 5; 
					}
					else if (gameskill == sk_hard)
					{ 
						if (cv_deathmatch.value || (p->health < maxsoul || p->armorpoints < max_armor)) 
							itemWeight = 7; 
					}
					else if (gameskill == sk_nightmare)
					{ 
						if (cv_deathmatch.value || (p->health < maxsoul || p->armorpoints < max_armor)) 
							itemWeight = 9; 
					}
					break;
				case SPR_PINS:	//invisibility
					if (gameskill == sk_baby)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invisibility]) 
							itemWeight = 2; 
					}
					else if (gameskill == sk_easy)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invisibility]) 
							itemWeight = 3; 
					}
					else if (gameskill == sk_medium)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invisibility]) 
							itemWeight = 5; 
					}
					else if (gameskill == sk_hard)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invisibility]) 
							itemWeight = 7; 
					}
					else if (gameskill == sk_baby)
					{ 
						if (cv_deathmatch.value || !p->powers[pw_invisibility]) 
							itemWeight = 9; 
					}
					break;
				case SPR_SOUL:	//soul sphere
					if (gameskill == sk_baby)
					{ 
						if (cv_deathmatch.value || p->health < maxsoul) 
							itemWeight = 1; 
					}
					else if (gameskill == sk_easy)
					{ 
						if (cv_deathmatch.value || p->health < maxsoul) 
							itemWeight = 2; 
					}
					else if (gameskill == sk_medium)
					{ 
						if (cv_deathmatch.value || p->health < maxsoul) 
							itemWeight = 4; 
					}
					else if (gameskill == sk_hard)
					{ 
						if (cv_deathmatch.value || p->health < maxsoul) 
							itemWeight = 6; 
					}
					else if (gameskill == sk_nightmare)
					{ 
						if (cv_deathmatch.value || p->health < maxsoul) 
							itemWeight = 8; 
					}
					break;
				case SPR_ARM2:	//blue armour, if we have >= maxarmour, its impossible to get
					if (gameskill == sk_baby)
					{
						if (p->armorpoints < max_armor)
						itemWeight = 1;
					}
					else if (gameskill == sk_easy)
					{
						if (p->armorpoints < max_armor)
						itemWeight = 2;
					}
					else if (gameskill == sk_medium)
					{
						if (p->armorpoints < max_armor)
						itemWeight = 4;
					}
					else if (gameskill == sk_hard)
					{
						if (p->armorpoints < max_armor)
						itemWeight = 6;
					}
					else if (gameskill == sk_nightmare)
					{
						if (p->armorpoints < max_armor)
						itemWeight = 8;
					}
					break;
				case SPR_PSTR:	//berserk pack 
					if (gameskill == sk_baby)
					{
						if (p->health < 40) itemWeight = 9; 
						else if (p->health < 50) itemWeight = 9; 
						else if (p->health < 60) itemWeight = 9;         
						else if (p->health < 80) itemWeight = 8; 
						else if (p->health < 100) itemWeight = 7;
						else if (!p->powers[pw_strength]) itemWeight = 2;
					}
					else if (gameskill == sk_easy)
					{
						if (p->health < 40) itemWeight = 9; 
						else if (p->health < 50) itemWeight = 9; 
						else if (p->health < 60) itemWeight = 8;         
						else if (p->health < 80) itemWeight = 7; 
						else if (p->health < 100) itemWeight = 6;
						else if (!p->powers[pw_strength]) itemWeight = 2;
					}
					else if (gameskill == sk_medium)
					{
						if (p->health < 40) itemWeight = 9; 
						else if (p->health < 50) itemWeight = 8; 
						else if (p->health < 60) itemWeight = 7;         
						else if (p->health < 80) itemWeight = 6; 
						else if (p->health < 100) itemWeight = 5;
						else if (!p->powers[pw_strength]) itemWeight = 2;
					}
					else if (gameskill == sk_hard)
					{
						if (p->health < 40) itemWeight = 9; 
						else if (p->health < 50) itemWeight = 8; 
						else if (p->health < 60) itemWeight = 7;         
						else if (p->health < 80) itemWeight = 5; 
						else if (p->health < 100) itemWeight = 4;
						else if (!p->powers[pw_strength]) itemWeight = 2;
					}
					else if (gameskill == sk_nightmare)
					{
						if (p->health < 40) itemWeight = 7; 
						else if (p->health < 50) itemWeight = 6; 
						else if (p->health < 60) itemWeight = 5;         
						else if (p->health < 80) itemWeight = 4; 
						else if (p->health < 100) itemWeight = 3;
						else if (!p->powers[pw_strength]) itemWeight = 2;
					}
					break;

				case SPR_ARM1:	//green armour
					if (gameskill == sk_baby)
					{
						if (p->armorpoints < (max_armor/2))
						itemWeight = 1;
					}
					if (gameskill == sk_easy)
					{
						if (p->armorpoints < (max_armor/2))
						itemWeight = 2;
					}
					if (gameskill == sk_medium)
					{
						if (p->armorpoints < (max_armor/2))
						itemWeight = 3;
					}
					if (gameskill == sk_hard)
					{
						if (p->armorpoints < (max_armor/2))
						itemWeight = 4;
					}
					if (gameskill == sk_nightmare)
					{
						if (p->armorpoints < (max_armor/2))
						itemWeight = 5;
					}
					break;

				case SPR_MEDI: case SPR_STIM: //medication  MEDIKIT or STIMPACK
					if (gameskill == sk_baby)
					{
						if (p->health < 50) itemWeight = 2; 
						else if (p->health < 60) itemWeight = 1; 
						else if (p->health < 80) itemWeight = 1; 
						else if (p->health < 100) itemWeight = 1;
					}
					if (gameskill == sk_easy)
					{
						if (p->health < 50) itemWeight = 3; 
						else if (p->health < 60) itemWeight = 2; 
						else if (p->health < 80) itemWeight = 1; 
						else if (p->health < 100) itemWeight = 1;
					}
					if (gameskill == sk_medium)
					{
						if (p->health < 50) itemWeight = 4; 
						else if (p->health < 60) itemWeight = 3; 
						else if (p->health < 80) itemWeight = 2; 
						else if (p->health < 100) itemWeight = 1;
					}
					if (gameskill == sk_hard)
					{
						if (p->health < 50) itemWeight = 5; 
						else if (p->health < 60) itemWeight = 4; 
						else if (p->health < 80) itemWeight = 3; 
						else if (p->health < 100) itemWeight = 2;
					}
					if (gameskill == sk_nightmare)
					{
						if (p->health < 50) itemWeight = 6; 
						else if (p->health < 60) itemWeight = 5; 
						else if (p->health < 80) itemWeight = 4; 
						else if (p->health < 100) itemWeight = 3;
					}
					break;

				case SPR_BON1:	//health potion
					if (p->mo->health < maxsoul)
						itemWeight = 1;
					break;
				case SPR_BON2:	//armour bonus
					if (p->armorpoints < max_armor)
						itemWeight = 1;
					break;

/////////////// weapons ////////////////////////////				
				case SPR_SHOT:
					if (!p->weaponowned[wp_shotgun])
					{
						if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
							|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
							|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
							|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
							itemWeight = 4;
						else
							itemWeight = 6;
					}
					else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_shell] < p->maxammo[am_shell]))
						itemWeight = 3;
					break;
				case SPR_MGUN:
					if (!p->weaponowned[wp_chaingun])
					{
						if ((p->weaponowned[wp_missile] && p->ammo[am_misl])
							|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
							|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
							itemWeight = 5;
						else
							itemWeight = 6;
					}
					else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_clip] < p->maxammo[am_clip]))
						itemWeight = 3;
					break;
				case SPR_LAUN:
					if (!p->weaponowned[wp_missile])
					{
						if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
							|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
							|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
							itemWeight = 5;
						else
							itemWeight = 7;
					}
					else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_misl] < p->maxammo[am_misl]))
						itemWeight = 3;
					break;
				case SPR_PLAS:
					if (!p->weaponowned[wp_plasma])
					{
						if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
							|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
							|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
							itemWeight = 5;
						else
							itemWeight = 7;
					}
					else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_cell] < p->maxammo[am_cell]))
						itemWeight = 3;
					break;
				case SPR_BFUG:
					if (!p->weaponowned[wp_bfg])
					{
						if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
							|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
							|| (p->weaponowned[wp_plasma] && p->ammo[am_cell])
							|| (p->weaponowned[wp_supershotgun] && p->ammo[am_shell] >= 2))
							itemWeight = 5;
						else
							itemWeight = 7;
					}
					else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_cell] < p->maxammo[am_cell]))
						itemWeight = 3;
					break;
				case SPR_SGN2:
					if (!p->weaponowned[wp_supershotgun])
					{
						if ((p->weaponowned[wp_chaingun] && p->ammo[am_clip])
							|| (p->weaponowned[wp_missile] && p->ammo[am_misl])
							|| (p->weaponowned[wp_plasma] && p->ammo[am_cell]))
							itemWeight = 5;
						else
							itemWeight = 7;
					}
					else if (((cv_deathmatch.value == 2) || (mo->flags & MF_DROPPED)) && (p->ammo[am_shell] < p->maxammo[am_shell]))
						itemWeight = 3;
					break;

/////////////////////ammo
				case SPR_CLIP: case SPR_AMMO:
					if (!p->ammo[am_clip] && ((p->readyweapon == wp_fist)
						|| (p->readyweapon == wp_chainsaw)))
						itemWeight = 6;
					else
						itemWeight = (p->ammo[am_clip] < p->maxammo[am_clip]) ? 3 : 0;
					break;
				case SPR_SHEL: case SPR_SBOX:
					if ((p->weaponowned[wp_shotgun] || p->weaponowned[wp_supershotgun]) && !p->ammo[am_shell]
						&& ((p->readyweapon == wp_fist) || (p->readyweapon == wp_chainsaw)))
						itemWeight = 6;
					else
						itemWeight = (p->ammo[am_shell] < p->maxammo[am_shell]) ? 3 : 0;
					break;
				case SPR_ROCK: case SPR_BROK:
					if (p->weaponowned[wp_missile] && !p->ammo[am_misl] && ((p->readyweapon == wp_fist)
						|| (p->readyweapon == wp_chainsaw)))
						itemWeight = 6;
					else
						itemWeight = (p->ammo[am_misl] < p->maxammo[am_misl]) ? 3 : 0;
					break;
				case SPR_CELL: case SPR_CELP:
					if (p->weaponowned[wp_plasma] && !p->ammo[am_cell] && ((p->readyweapon == wp_fist)
						|| (p->readyweapon == wp_chainsaw)))
						itemWeight = 6;
					else
						itemWeight = (p->ammo[am_cell] < p->maxammo[am_cell]) ? 3 : 0;
					break;

///////////////////////keys
				case SPR_BKEY:
					if (!(p->cards & it_bluecard))
						itemWeight = 5;
					break;
				case SPR_BSKU:
					if (!(p->cards & it_blueskull))
						itemWeight = 5;
					break;
				case SPR_RKEY:
					if (!(p->cards & it_redcard))
						itemWeight = 5;
					break;
				case SPR_RSKU:
					if (!(p->cards & it_redskull))
						itemWeight = 5;
					break;
				case SPR_YKEY:
					if (!(p->cards & it_yellowcard))
						itemWeight = 5;
					break;
				case SPR_YSKU:
					if (!(p->cards & it_yellowskull))
						itemWeight = 5;
					break;
				default:
					itemWeight = 0;	//don't want it
					break;
				}

				if (P_CheckSight(p->mo, mo) && B_Reachable(p, mo))
				{
					if (((itemWeight > bestSeenItemWeight) || ((itemWeight == bestSeenItemWeight) && (thingDistance < bestSeenItemDistance))))
					{
						bestSeenItem = mo;
						bestSeenItemDistance = thingDistance;
						bestSeenItemWeight = itemWeight;
					}
				}
				else	//this item is not getable atm, may use a search later to find a path to it
				{
					SearchNode_t* tempNode = B_GetNodeAt(mo->x, mo->y);
					//if there is a node near the item wanted, and its the best item
					if (tempNode// && ((P_AproxDistance(posX2x(tempNode->x) - mo->x, posY2y(tempNode->y) - mo->y) < (BOTNODEGRIDSIZE<<1))
						&& ((((itemWeight > bestItemWeight)
						|| ((itemWeight == bestItemWeight) && (thingDistance < bestItemDistance))))))
					{
						bestItem = mo;
						bestItemDistance = thingDistance;
						bestItemWeight = itemWeight;
						//CONS_Printf("best item set to x:%d y:%d for type:%d\n", mo->x>>FRACBITS, mo->y>>FRACBITS, mo->type);
					}

					//if (!tempNode)
					//	CONS_Printf("could not find a node here x:%d y:%d for type:%d\n", mo->x>>FRACBITS, mo->y>>FRACBITS, mo->type);
				}
			}

			if (enemyFound)
			{
				if (P_CheckSight(p->mo, mo))
				{
					//if I have seen an enemy, if its deathmatch, players have priority, so closet player targeted
					//otherwise make closest target the closest monster
					if (thingDistance && (!closestEnemyDistance || (thingDistance < closestEnemyDistance) 
						|| (mo->player && !p->bot->closestEnemy->player)))
					{
						closestEnemyDistance = thingDistance;
						p->bot->closestEnemy = mo;
					}
				}
				else
				{
					SearchNode_t* tempNode = B_GetNodeAt(mo->x, mo->y);
					if (tempNode
						&& ((!closestUnseenEnemyDistance || (thingDistance < closestUnseenEnemyDistance) 
						|| (mo->player && !p->bot->closestUnseenEnemy->player))))
					{
						closestUnseenEnemyDistance = thingDistance;
						p->bot->closestUnseenEnemy = mo;
					}
				}

				enemyFound = false;
				thingDistance = 0;
			}
		}
		currentthinker = currentthinker->next;
	}

	//if a item has a good weight, get it no matter what.  Else only if we have no target/enemy get it
	if ((bestSeenItemWeight > 5) || (bestSeenItemWeight && !p->bot->closestEnemy))
		p->bot->bestSeenItem = bestSeenItem;
	else
		p->bot->bestSeenItem = NULL;;

	if (bestItemWeight)
		p->bot->bestItem = bestItem;
	else
		p->bot->bestItem = NULL;;
}
