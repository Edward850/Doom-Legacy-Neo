// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: b_game.c,v 1.4 2003/06/11 04:04:50 ssntails Exp $
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
// $Log: b_game.c,v $
// Revision 1.4  2003/06/11 04:04:50  ssntails
// Rellik's Bot Code!
//
// Revision 1.3  2002/09/28 06:53:11  tonyd
// fixed CR problem, fixed game options crash
//
// Revision 1.2  2002/09/27 16:40:07  tonyd
// First commit of acbot
//

// Bot include
#include "b_bot.h"
#include "b_game.h"
#include "b_look.h"
#include "b_node.h"
// Doom include
#include "doomdef.h"
#include "doomstat.h"
//#include "r_defs.h"
#include "m_random.h"
#include "p_local.h"
#include "z_zone.h"

#include "command.h"
#include "r_state.h"
#include "v_video.h"
#include "m_argv.h"
#include "p_setup.h"
#include "r_main.h"
#include "r_things.h"
#include "g_game.h"
#include "d_net.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

boolean B_FindNextNode(player_t* p);

BOTINFOTYPE botinfo[MAXPLAYERS];
fixed_t botforwardmove[2] = {25/NEWTICRATERATIO, 50/NEWTICRATERATIO};
fixed_t botsidemove[2]    = {24/NEWTICRATERATIO, 40/NEWTICRATERATIO};
fixed_t botangleturn[4]   = {500, 1000, 2000, 4000};
extern consvar_t cv_skill;
extern thinker_t thinkercap;
extern mobj_t*	tmthing;

char* botnames[MAXPLAYERS] = {
	"Frag-God",
	"Thresh",
	"Reptile",
	"Archer",
	"Freak",
	"TF-Master",
	"Carmack",
	"Quaker",
	"FragMaster",
	"Punisher",
	"Romero",
	"Xoleras",
	"Hurdlerbot",
	"Meisterbot",
	"Borisbot",
	"Tailsbot",
	"crackbaby",
	"yo momma",
	"crusher",
	"aimbot",
	"crash",
	"akira",
	"meiko",
	"undead",
	"death",
	"TonyD-bot",
	"unit",
	"fodder",
	"2-vile",
	"nitemare",
	"nos482",
	"billy"
};

int botcolors[MAXSKINCOLORS] = 
{
   0, // = Green
   1, // = Indigo
   2, // = Blue
   3, // = Deep Red
   4, // = White
   5, // = Bright Brown
   6, // = Red
   7, // = Blue
   8, // = Dark Blue
   9, // = Yellow
   10 //= Bleached Bone
};

void B_InitBots()
{  
	boolean duplicateBot;
	int botNum, i, j;
	for (i=0; i< MAXPLAYERS; i++)
	{
		do
		{
			botNum = P_Random()%MAXPLAYERS;
			botinfo[i].name = botnames[botNum];
			duplicateBot = false;
			j = 0;
			while((j < i) && !duplicateBot)
			{
				if  ((j != botNum) && (botinfo[j].name == botinfo[botNum].name))
					duplicateBot = true;

				j++;
			}
		} while (duplicateBot);

		botinfo[i].colour = P_Random() % MAXSKINCOLORS;
	}
	botNodeArray = NULL;
}

//
// bot commands
//

void Command_AddBot(void)
{
	byte buf = 0;

	if (!server)
	{
		CONS_Printf("Only the server can add a bot\n");
		return;
	}
	while ((buf < MAXPLAYERS) && playeringame[buf])	//find free playerspot
		buf++;
	if (buf>=MAXPLAYERS) 
	{
		CONS_Printf ("You can only have %d players.\n", MAXPLAYERS);
		return; 
	}


	SendNetXCmd(XD_ADDBOT,&buf,1);
}

void B_AddCommands()
{
	COM_AddCommand ("addbot", Command_AddBot);
}

void B_AvoidMissile(player_t* p, mobj_t* missile)
{
	fixed_t		missileAngle = R_PointToAngle2 (p->mo->x,
								p->mo->y,
								missile->x,
								missile->y),

				delta = p->mo->angle - missileAngle;

	if (delta >= 0)
		p->cmd.sidemove = -botsidemove[1];
	else if (delta < 0)
		p->cmd.sidemove = botsidemove[1];
}

void B_ChangeWeapon (player_t* p)
{
	boolean		hasWeaponAndAmmo[NUMWEAPONS];
	byte		i,
				numWeapons = 0,
				weaponChance;

	for (i=0; i<NUMWEAPONS; i++)
	{
		switch (i)
		{
		case wp_fist:
			hasWeaponAndAmmo[i] = false;//true;
			break;
		case wp_pistol:
			hasWeaponAndAmmo[i] = p->ammo[am_clip];
			break;
		case wp_shotgun:
			hasWeaponAndAmmo[i] = (p->weaponowned[i] && p->ammo[am_shell]);
			break;
		case wp_chaingun:
			hasWeaponAndAmmo[i] = (p->weaponowned[i] && p->ammo[am_clip]);
			break;
		case wp_missile:
			hasWeaponAndAmmo[i] = (p->weaponowned[i] && p->ammo[am_misl]);
			break;
		case wp_plasma:
			hasWeaponAndAmmo[i] = (p->weaponowned[i] && p->ammo[am_cell]);
			break;
		case wp_bfg:
			hasWeaponAndAmmo[i] = (p->weaponowned[i] && (p->ammo[am_cell] >= 40));
			break;
		case wp_chainsaw:
			hasWeaponAndAmmo[i] = p->weaponowned[i];
			break;
		case wp_supershotgun:
			hasWeaponAndAmmo[i] = (p->weaponowned[i] && (p->ammo[am_shell] >= 2));
		}
		if (hasWeaponAndAmmo[i])// || ((i == wp_fist) && p->powers[pw_strength]))
			numWeapons++;
	}
																		//or I have just picked up a new weapon
	if (!p->bot->weaponchangetimer || !hasWeaponAndAmmo[p->readyweapon] || (numWeapons != p->bot->lastNumWeapons))
	{
		if ((hasWeaponAndAmmo[wp_shotgun] && (p->readyweapon != wp_shotgun))
			|| (hasWeaponAndAmmo[wp_chaingun] && (p->readyweapon != wp_chaingun))
			|| (hasWeaponAndAmmo[wp_missile] && (p->readyweapon != wp_missile))
			|| (hasWeaponAndAmmo[wp_plasma] && (p->readyweapon != wp_plasma))
			|| (hasWeaponAndAmmo[wp_bfg] && (p->readyweapon != wp_bfg))
			|| (hasWeaponAndAmmo[wp_supershotgun] && (p->readyweapon != wp_supershotgun)))
		{
			p->cmd.buttons &= ~BT_ATTACK;	//stop rocket from jamming;
			do
			{
				weaponChance = P_Random();
				if ((weaponChance < 30) && hasWeaponAndAmmo[wp_shotgun] && (p->readyweapon != wp_shotgun))//has shotgun and shells
					p->cmd.buttons |= (BT_CHANGE | (wp_shotgun<<BT_WEAPONSHIFT));
				else if ((weaponChance < 80) && hasWeaponAndAmmo[wp_chaingun] && (p->readyweapon != wp_chaingun))//has chaingun and bullets
					p->cmd.buttons |= (BT_CHANGE | (wp_chaingun<<BT_WEAPONSHIFT));
				else if ((weaponChance < 130) && hasWeaponAndAmmo[wp_missile] && (p->readyweapon != wp_missile))//has rlauncher and rocket
					p->cmd.buttons |= (BT_CHANGE | (wp_missile<<BT_WEAPONSHIFT));
				else if ((weaponChance < 180) && hasWeaponAndAmmo[wp_plasma] && (p->readyweapon != wp_plasma))//has plasma and cells
					p->cmd.buttons |= (BT_CHANGE | (wp_plasma<<BT_WEAPONSHIFT));
				else if ((weaponChance < 200) && hasWeaponAndAmmo[wp_bfg] && (p->readyweapon != wp_bfg))//has bfg and cells
					p->cmd.buttons |= (BT_CHANGE | (wp_bfg<<BT_WEAPONSHIFT));
				else if (hasWeaponAndAmmo[wp_supershotgun] && (p->readyweapon != wp_supershotgun))
					p->cmd.buttons |= (BT_CHANGE | BT_EXTRAWEAPON | (wp_shotgun<<BT_WEAPONSHIFT));
			} while (!(p->cmd.buttons & BT_CHANGE));
		}
		else if (hasWeaponAndAmmo[wp_pistol] && (p->readyweapon != wp_pistol))//has pistol and bullets
			p->cmd.buttons |= (BT_CHANGE | wp_pistol<<BT_WEAPONSHIFT);
		else if (p->weaponowned[wp_chainsaw] && !p->powers[pw_strength] && (p->readyweapon != wp_chainsaw))//has chainsaw, and not powered
			p->cmd.buttons |= (BT_CHANGE | BT_EXTRAWEAPON | (wp_fist<<BT_WEAPONSHIFT));
		else	//resort to fists, if have powered fists, better with fists then chainsaw
			p->cmd.buttons |= (BT_CHANGE | wp_fist<<BT_WEAPONSHIFT);

		p->bot->weaponchangetimer = (P_Random()<<7)+10000;	//how long until I next change my weapon
	}
	else if (p->bot->weaponchangetimer)
		p->bot->weaponchangetimer--;

	if (numWeapons != p->bot->lastNumWeapons)
		p->cmd.buttons &= ~BT_ATTACK;	//stop rocket from jamming;
	p->bot->lastNumWeapons = numWeapons;

	//CONS_Printf("p->bot->weaponchangetimer is %d\n", p->bot->weaponchangetimer);
}

#define ANG5 (ANG90/18)

// returns the difference between the angle mobj is facing,
// and the angle from mo to x,y

fixed_t B_AngleDiff(mobj_t* mo, fixed_t x, fixed_t y)
{
	return ((R_PointToAngle2 (mo->x, mo->y, x, y)) - mo->angle);
}

void B_TurnTowardsPoint(player_t* p, fixed_t x, fixed_t y)
{
	int			botspeed;
	fixed_t		angle = R_PointToAngle2 (p->mo->x,
								p->mo->y,
								x,
								y),

				delta = angle - p->mo->angle;

	if (abs(delta) < (ANG45>>2))
		botspeed = 0;
	else if (abs(delta) < ANG45)
		botspeed = 1;
	else
		botspeed = 1;

	if (abs(delta) < ANG5)
		p->cmd.angleturn = angle>>FRACBITS;	//perfect aim
    else if (delta > 0)
        p->cmd.angleturn += botangleturn[botspeed];
    else
        p->cmd.angleturn -= botangleturn[botspeed];
}

void B_AimWeapon(player_t* p)
{
	mobj_t		*dest = p->bot->closestEnemy,
				*source = p->mo;

	int			angle,
				botspeed = 0,
				delta,
				dist,
				missileSpeed,
				realAngle,
				time;

	fixed_t		px, py, pz;
	subsector_t	*sec;
	int			t;
	boolean		canHit;

	switch (p->readyweapon)	// changed so bot projectiles don't lead targets at lower skills
	{
		case wp_fist: case wp_chainsaw:			//must be close to hit with these
		case wp_pistol: case wp_shotgun: case wp_chaingun:	//instant hit weapons, aim directly at enemy
			missileSpeed = 0;
			break;
		case wp_missile:
			if (gameskill == sk_baby || gameskill == sk_easy || gameskill == sk_hard)
			{
				missileSpeed = 0;
				//CONS_Printf("rocketspeed zero\n");
				break;
			}
			else
			{
				missileSpeed = mobjinfo[MT_ROCKET].speed;
				//CONS_Printf("rocketspeed perfect\n");
			}
			break;
		case wp_plasma:
			if (gameskill == sk_baby || gameskill == sk_easy || gameskill == sk_hard)
			{
				missileSpeed = 0;
				//CONS_Printf("plasmaspeed = zero\n");
				break;
			}
			else
			{
				missileSpeed = mobjinfo[MT_PLASMA].speed;
				//CONS_Printf("plasmaspeed perfect\n");
				break;
			}
		case wp_bfg:
			if (gameskill == sk_baby || gameskill == sk_easy || gameskill == sk_hard)
			{
				missileSpeed = 0;
				//CONS_Printf("BFGspeed = zero\n");
				break;
			}
			else
			{
				missileSpeed = mobjinfo[MT_BFG].speed;
				//CONS_Printf("BFGspeed perfect\n");
				break;
			}
		default:
			missileSpeed = 0;
	}

	dist = P_AproxDistance (dest->x - source->x, dest->y - source->y);
	if ((p->readyweapon != wp_missile) || (dist > (100<<FRACBITS)))
	{
		if (missileSpeed)
		{
			time = dist/missileSpeed;
			time = P_AproxDistance (dest->x + dest->momx*time - source->x,
									dest->y + dest->momy*time - source->y)/missileSpeed;

			t = time + 4;
			do
			{
				t-=4;
				if (t < 0)
					t = 0;
				px = dest->x + dest->momx*t;
				py = dest->y + dest->momy*t;
				pz = dest->z + dest->momz*t;
				canHit = P_CheckSight2(source, dest, px, py, pz);
			} while (!canHit && (t > 0));

			sec = R_PointInSubsector(px, py);
			if (!sec)
				sec = dest->subsector;

			if (pz < sec->sector->floorheight)
				pz = sec->sector->floorheight;
			else if (pz > sec->sector->ceilingheight)
				pz = sec->sector->ceilingheight - dest->height;
		}
		else
		{
			px = dest->x;
			py = dest->y;
			pz = dest->z;
		}

		realAngle = angle = R_PointToAngle2 (source->x, source->y, px, py);
		p->cmd.aiming = ((int)((atan ((pz - source->z + (dest->height - source->height)/2) / (double)dist)) * ANGLE_180/M_PI))>>FRACBITS;

		if ((P_AproxDistance(dest->momx, dest->momy)>>FRACBITS) > 8)	//enemy is moving reasonably fast, so not perfectly acurate
		{
			if (dest->flags & MF_SHADOW)
				angle += P_SignedRandom()<<23;
			else if (!missileSpeed)
				angle += P_SignedRandom()<<22;
		}
		else
		{
			if (dest->flags & MF_SHADOW)
				angle += P_SignedRandom()<<22;
			else if (!missileSpeed)
				angle += P_SignedRandom()<<21;
		}

		delta = angle - source->angle;
		if (abs(delta) < (ANG45>>1))
			botspeed = 0;
		else if (abs(delta) < ANG45)
			botspeed = 1;
		else
			botspeed = 3;

		if (abs(delta) < ANG45)
		{
			if ((p->readyweapon == wp_chaingun) || (p->readyweapon == wp_plasma) || (p->readyweapon == wp_pistol))
				p->cmd.buttons |= BT_ATTACK;
			if (abs(delta) <= ANG5)
			{
				if (gameskill == sk_baby || gameskill == sk_easy || gameskill == sk_medium)  // check skill, if anything but nightmare bot aim is imperfect
					p->cmd.angleturn = angle>>FRACBITS;	// not so perfect aim
				else if (gameskill == sk_hard || gameskill == sk_nightmare) // check skill if nightmare then bot aim is perfect
					p->cmd.angleturn = realAngle>>FRACBITS; // perfect aim
				delta = 0;
					p->cmd.buttons |= BT_ATTACK;
			}
		}

		if (delta > 0)
			p->cmd.angleturn += botangleturn[botspeed];	//turn right
		else if (delta < 0)
			p->cmd.angleturn -= botangleturn[botspeed];//turn left
	}
} 

//
// MAIN BOT AI
//
void B_BuildTiccmd(player_t* p, ticcmd_t* netcmd)
{
	boolean		blocked,
				notUsed = true;

	int			botspeed = 1;
	int			x, y;
	fixed_t		cmomx, cmomy,	//what the extra momentum added from this tick will be
				px, py,			//coord of where I will be next tick
				forwardmove = 0,
				sidemove = 0,
				forwardAngle, sideAngle,
				targetDistance;	//how far away is my enemy, wanted thing

	ticcmd_t*	cmd = &p->cmd;

	if (cmd->buttons & BT_USE)	//needed so bot doesn't hold down use before reaching switch object
		notUsed = false;		//wouldn't be able to use switch

	memset (cmd,0,sizeof(*cmd));
	if (p->playerstate == PST_LIVE)
	{
		cmd->angleturn = p->mo->angle>>FRACBITS;
		cmd->aiming = 0;//p->aiming>>FRACBITS;

		B_LookForThings(p);
		B_ChangeWeapon(p);

		if (p->bot->avoidtimer)
		{
			p->bot->avoidtimer--;
			if (p->mo->eflags & MF_UNDERWATER)
			{
				forwardmove = botforwardmove[1];
				cmd->buttons |= BT_JUMP;
			}
			else
			{
				if (netcmd->forwardmove > 0)
					forwardmove = -botforwardmove[1];
				else
					forwardmove = botforwardmove[1];
				sidemove = botsidemove[1];
			}
		}
		else
		{
			if (p->bot->bestSeenItem)
			{
				targetDistance = P_AproxDistance (p->mo->x - p->bot->bestSeenItem->x, p->mo->y - p->bot->bestSeenItem->y)>>FRACBITS;
				if (targetDistance > 64)
					botspeed = 1;
				else
					botspeed = 0;
				B_TurnTowardsPoint(p, p->bot->bestSeenItem->x, p->bot->bestSeenItem->y);
				forwardmove = botforwardmove[botspeed];
				if ((((p->bot->bestSeenItem->floorz - p->mo->z)>>FRACBITS) > 24) && (targetDistance <= 100))
					cmd->buttons |= BT_JUMP;

				p->bot->bestItem = NULL;
			}	//if a target exists and is alive
			else if (p->bot->closestEnemy)// && (p->bot->closestEnemy->flags & ~MF_CORPSE))
			{
				//CONS_Printf("heading for an enemy\n");
				targetDistance = P_AproxDistance (p->mo->x - p->bot->closestEnemy->x, p->mo->y - p->bot->closestEnemy->y)>>FRACBITS;
				if ((targetDistance > 300) || (p->readyweapon == wp_fist) || (p->readyweapon == wp_chainsaw))
					forwardmove = botforwardmove[botspeed];
				if ((p->readyweapon == wp_missile) && (targetDistance < 400))
					forwardmove = -botforwardmove[botspeed];

				switch(gameskill) // gameskill setting determines likelyhood bot will start strafing
				{
					case sk_baby:
						if(targetDistance <=32)
							sidemove = botsidemove[botspeed];
						break;
					case sk_easy:
						if(targetDistance <=150)
							sidemove = botsidemove[botspeed];
						break;
					case sk_medium:
						if((targetDistance <= 150) || (p->bot->closestEnemy->player && ((p->bot->closestEnemy->player->readyweapon == wp_pistol) || (p->bot->closestEnemy->player->readyweapon == wp_shotgun) || (p->bot->closestEnemy->player->readyweapon == wp_chaingun))))
							sidemove = botsidemove[botspeed];
						break;
					case sk_hard:
						if((targetDistance <= 350) || (p->bot->closestEnemy->player && ((p->bot->closestEnemy->player->readyweapon == wp_pistol) || (p->bot->closestEnemy->player->readyweapon == wp_shotgun) || (p->bot->closestEnemy->player->readyweapon == wp_chaingun))))
							sidemove = botsidemove[botspeed];
						break;
					case sk_nightmare:
						if((targetDistance <= 650) || (p->bot->closestEnemy->player && ((p->bot->closestEnemy->player->readyweapon == wp_pistol) || (p->bot->closestEnemy->player->readyweapon == wp_shotgun) || (p->bot->closestEnemy->player->readyweapon == wp_chaingun))))
							sidemove = botsidemove[botspeed];
						break;
					default:
						break;
				}

				B_AimWeapon(p);
				p->bot->lastMobj = p->bot->closestEnemy;
				p->bot->lastMobjX = p->bot->closestEnemy->x;
				p->bot->lastMobjY = p->bot->closestEnemy->y;
			}
			else
			{
				cmd->aiming = 0;
				if (B_LookForSpecialLine(p, &x, &y) && B_ReachablePoint(p, p->mo->subsector->sector, x, y))	//look for an unactivated switch/door
				{
					//CONS_Printf("found a special line\n");
					B_TurnTowardsPoint(p, x, y);
					if (P_AproxDistance (p->mo->x - x, p->mo->y - y) <= USERANGE)
					{
						if (notUsed)
							cmd->buttons |= BT_USE;
					}
					else
						forwardmove = botforwardmove[1];
				}
				else if (p->bot->teammate)
				{
					targetDistance = P_AproxDistance (p->mo->x - p->bot->teammate->x, p->mo->y - p->bot->teammate->y)>>FRACBITS;
					if (targetDistance > 100)
					{
						B_TurnTowardsPoint(p, p->bot->teammate->x, p->bot->teammate->y);
						forwardmove = botforwardmove[botspeed];
					}

					p->bot->lastMobj = p->bot->teammate;
					p->bot->lastMobjX = p->bot->teammate->x;
					p->bot->lastMobjY = p->bot->teammate->y;
				}
				else if (p->bot->lastMobj && (p->bot->lastMobj->flags & MF_SOLID))// && B_ReachablePoint(p, R_PointInSubsector(p->bot->lastMobjX, p->bot->lastMobjY)->sector, p->bot->lastMobjX, p->bot->lastMobjY))	//since nothing else to do, go where last enemy/teamate was seen
				{
					if ((p->mo->momx == 0 && p->mo->momy == 0) || !B_NodeReachable(NULL, p->mo->x, p->mo->y, p->bot->lastMobjX, p->bot->lastMobjY))
						p->bot->lastMobj = NULL;	//just went through teleporter
					else
					{
						//CONS_Printf("heading towards last mobj\n");
						B_TurnTowardsPoint(p, p->bot->lastMobjX, p->bot->lastMobjY);
						forwardmove = botforwardmove[botspeed];
					}
				}
				else
				{
					p->bot->lastMobj = NULL;

					if (p->bot->bestItem)
					{
						SearchNode_t* temp = B_GetNodeAt(p->bot->bestItem->x, p->bot->bestItem->y);
						//CONS_Printf("found a best item at x:%d, y:%d\n", p->bot->bestItem->x>>FRACBITS, p->bot->bestItem->y>>FRACBITS);
						if (p->bot->destNode != temp)
							B_LLClear(p->bot->path);
						p->bot->destNode = temp;
					}
					else if (p->bot->closestUnseenTeammate)
					{
						SearchNode_t* temp = B_GetNodeAt(p->bot->closestUnseenTeammate->x, p->bot->closestUnseenTeammate->y);
						//CONS_Printf("found a best item at x:%d, y:%d\n", p->bot->bestItem->x>>FRACBITS, p->bot->bestItem->y>>FRACBITS);
						if (p->bot->destNode != temp)
							B_LLClear(p->bot->path);
						p->bot->destNode = temp;
					}
					else if (p->bot->closestUnseenEnemy)
					{
						SearchNode_t* temp = B_GetNodeAt(p->bot->closestUnseenEnemy->x, p->bot->closestUnseenEnemy->y);
						//CONS_Printf("found a best item at x:%d, y:%d\n", p->bot->bestItem->x>>FRACBITS, p->bot->bestItem->y>>FRACBITS);
						if (p->bot->destNode != temp)
							B_LLClear(p->bot->path);
						p->bot->destNode = temp;
					}
					else					
						p->bot->destNode = NULL;

					if (p->bot->destNode)
					{
						if (!B_LLIsEmpty(p->bot->path) && P_AproxDistance(p->mo->x - posX2x(p->bot->path->first->x), p->mo->y - posY2y(p->bot->path->first->y)) < (BOTNODEGRIDSIZE<<1))//BOTNODEGRIDSIZE>>1))
						{
#ifdef SHOWBOTPATH
							SearchNode_t* temp = B_LLRemoveFirstNode(p->bot->path);
							P_RemoveMobj(temp->mo);
							Z_Free(temp);
#else
							Z_Free(B_LLRemoveFirstNode(p->bot->path));
#endif
						}

						
						//CONS_Printf("at x%d, y%d\n", p->bot->wantedItemNode->x>>FRACBITS, p->bot->wantedItemNode->y>>FRACBITS);
						if (B_LLIsEmpty(p->bot->path) || !B_NodeReachable(NULL, p->mo->x, p->mo->y, posX2x(p->bot->path->first->x), posY2y(p->bot->path->first->y)))// > (BOTNODEGRIDSIZE<<2)))
						if (!B_FindNextNode(p))	//search for next node
						{
							//CONS_Printf("Bot stuck at x:%d y:%d could not find a path to x:%d y:%d\n",p->mo->x>>FRACBITS, p->mo->y>>FRACBITS, posX2x(p->bot->destNode->x)>>FRACBITS, posY2y(p->bot->destNode->y)>>FRACBITS);

							p->bot->destNode = NULL;	//can't get to it
						}

						if (!B_LLIsEmpty(p->bot->path))
						{
							//CONS_Printf("turning towards node at x%d, y%d\n", (p->bot->nextItemNode->x>>FRACBITS), (p->bot->nextItemNode->y>>FRACBITS));
							//CONS_Printf("it has a distance %d\n", (P_AproxDistance(p->mo->x - p->bot->nextItemNode->x, p->mo->y - p->bot->nextItemNode->y)>>FRACBITS));
							B_TurnTowardsPoint(p, posX2x(p->bot->path->first->x), posY2y(p->bot->path->first->y));
							forwardmove = botforwardmove[1];//botspeed];
						}
					}
				}
			}

			forwardAngle = (p->mo->angle) >> ANGLETOFINESHIFT;
			sideAngle = (p->mo->angle - ANG90) >> ANGLETOFINESHIFT;
			cmomx = FixedMul(forwardmove*2048, finecosine[forwardAngle]) + FixedMul(sidemove*2048, finecosine[sideAngle]);
			cmomy = FixedMul(forwardmove*2048, finesine[forwardAngle]) + FixedMul(sidemove*2048, finesine[sideAngle]);
			px = p->mo->x + p->mo->momx + cmomx;
			py = p->mo->y + p->mo->momy + cmomy;

			blocked = !P_CheckPosition (p->mo, px, py) || (((tmfloorz - p->mo->z)>>FRACBITS) > 24) || ((tmceilingz - tmfloorz) < p->mo->height);
			//if its time to change strafe directions, 
			if (sidemove && ((p->mo->flags & MF_JUSTHIT) || blocked))
			{
				p->bot->straferight = !p->bot->straferight;
				p->mo->flags &= ~MF_JUSTHIT;
			}

			if (blocked)
			{
				if ((++p->bot->blockedcount > 20) && ((P_AproxDistance(p->mo->momx, p->mo->momy) < (4<<FRACBITS)) || (tmthing && (tmthing->flags & MF_SOLID))))
					p->bot->avoidtimer = 20;

				if ((((tmfloorz - p->mo->z)>>FRACBITS) > 24) && ((((tmfloorz - p->mo->z)>>FRACBITS) <= 37) || ((((tmfloorz - p->mo->z)>>FRACBITS) <= 45) && (p->mo->subsector->sector->floortype != FLOOR_WATER))))
					cmd->buttons |= BT_JUMP;

				for (x=0; x<numspechit; x++)
				if (lines[spechit[x]].backsector)
				{
					if (!lines[spechit[x]].backsector->ceilingdata && !lines[spechit[x]].backsector->floordata && (lines[spechit[x]].special != 11))	//not the exit switch
						cmd->buttons |= BT_USE;
				}
			}
			else
				p->bot->blockedcount = 0;
		}

		if (sidemove)
		{
			if (p->bot->strafetimer)
				p->bot->strafetimer--;
			else
			{
				p->bot->straferight = !p->bot->straferight;
				p->bot->strafetimer = P_Random()/3;
			}
		}
		if (p->bot->weaponchangetimer)
			p->bot->weaponchangetimer--;

		p->cmd.forwardmove = forwardmove;
		p->cmd.sidemove = p->bot->straferight ? sidemove : -sidemove;
		if (p->bot->closestMissile)
			B_AvoidMissile(p, p->bot->closestMissile);
	}
	else
		cmd->buttons |= BT_USE;	//I want to respawn
	
	memcpy (netcmd, cmd, sizeof(*cmd));
} // end of BOT_Thinker

bot_t* B_CreateBot()
{
	bot_t* bot = Z_Malloc (sizeof(*bot), PU_STATIC, 0);

	bot->path = B_LLCreate();

	return bot;
}

void B_SpawnBot(bot_t* bot)
{
	bot->avoidtimer = 0;
	bot->blockedcount = 0;
	bot->weaponchangetimer = 0;

	bot->bestItem = NULL;
	bot->lastMobj = NULL;
	bot->destNode = NULL;

	B_LLClear(bot->path);
}
