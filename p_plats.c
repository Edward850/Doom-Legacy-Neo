// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_plats.c,v 1.5 2001/01/25 22:15:43 bpereira Exp $
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
// $Log: p_plats.c,v $
// Revision 1.5  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.4  2000/10/21 08:43:30  bpereira
// no message
//
// Revision 1.3  2000/04/04 00:32:47  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Plats (i.e. elevator platforms) code, raising/lowering.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "doomstat.h"
#include "p_local.h"
#include "s_sound.h"
#include "r_state.h"
#include "z_zone.h"
#include "m_random.h"

//SoM: 3/7/2000: Use boom's limitless format.
platlist_t      *activeplats;


//
// Move a plat up and down
//
void T_PlatRaise(plat_t* plat)
{
    result_e    res;

    switch(plat->status)
    {
      case up:
        res = T_MovePlane(plat->sector,
                          plat->speed,
                          plat->high,
                          plat->crush,0,1);

        if ( gamemode == heretic && !(leveltime % (32*NEWTICRATERATIO)))
            S_StartSound ((mobj_t *) & plat->sector->soundorg,
                          sfx_stnmov);

        if (plat->type == raiseAndChange
            || plat->type == raiseToNearestAndChange)
        {
            if (!(leveltime % (8*NEWTICRATERATIO)))
                S_StartSound((mobj_t *)&plat->sector->soundorg,
                             sfx_stnmov);
        }


        if (res == crushed && (!plat->crush))
        {
            plat->count = plat->wait;
            plat->status = down;
            S_StartSound((mobj_t *)&plat->sector->soundorg,
                         sfx_pstart);
        }
        else
        {
            if (res == pastdest)
            {
                //SoM: 3/7/2000: Moved this little baby over.
                // if not an instant toggle type, wait, make plat stop sound
                if (plat->type!=toggleUpDn)
                {
                  plat->count = plat->wait;
                  plat->status = waiting;
                  S_StartSound((mobj_t *)&plat->sector->soundorg, sfx_pstop);
                }
                else // else go into stasis awaiting next toggle activation
                {
                  plat->oldstatus = plat->status;//jff 3/14/98 after action wait  
                  plat->status = in_stasis;      //for reactivation of toggle
                }

                switch(plat->type)
                {
                  case blazeDWUS:
                  case downWaitUpStay:
                  case raiseAndChange:
                  case raiseToNearestAndChange:
                  case genLift:
                    P_RemoveActivePlat(plat); //SoM: 3/7/2000: Much cleaner boom code.
                  default:
                    break;
                }
            }
        }
        break;

      case      down:
        res = T_MovePlane(plat->sector,plat->speed,plat->low,false,0,-1);

        if (res == pastdest)
        {
            //SoM: 3/7/2000: if not an instant toggle, start waiting, make plat stop sound
            if (plat->type!=toggleUpDn) 
            {                           
              plat->count = plat->wait;
              plat->status = waiting;
              S_StartSound((mobj_t *)&plat->sector->soundorg,sfx_pstop);
            }
            else //SoM: 3/7/2000: instant toggles go into stasis awaiting next activation
            {
              plat->oldstatus = plat->status;  
              plat->status = in_stasis;      
            }

            //jff 1/26/98 remove the plat if it bounced so it can be tried again
            //only affects plats that raise and bounce
    
            if (boomsupport)
            {
              switch(plat->type)
              {
                case raiseAndChange:
                case raiseToNearestAndChange:
                  P_RemoveActivePlat(plat);
                default:
                  break;
              }
            }
        }
        else
            if (gamemode == heretic && !(leveltime & 31))
                S_StartSound ((mobj_t *) & plat->sector->soundorg,
                    sfx_stnmov);

        break;

      case      waiting:
        if (!--plat->count)
        {
            if (plat->sector->floorheight == plat->low)
                plat->status = up;
            else
                plat->status = down;
            S_StartSound((mobj_t *)&plat->sector->soundorg,sfx_pstart);
        }
      case      in_stasis:
        break;
    }
}


//
// Do Platforms
//  "amount" is only used for SOME platforms.
//
int
EV_DoPlat
( line_t*       line,
  plattype_e    type,
  int           amount )
{
    plat_t*     plat;
    int         secnum;
    int         rtn;
    sector_t*   sec;

    secnum = -1;
    rtn = 0;


    //  Activate all <type> plats that are in_stasis
    switch(type)
    {
      case perpetualRaise:
        P_ActivateInStasis(line->tag);
        break;

      case toggleUpDn:
        P_ActivateInStasis(line->tag);
        rtn=1;
        break;

      default:
        break;
    }

    while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
    {
        sec = &sectors[secnum];

        if (P_SectorActive(floor_special,sec)) //SoM: 3/7/2000: 
            continue;

        // Find lowest & highest floors around sector
        rtn = 1;
        plat = Z_Malloc( sizeof(*plat), PU_LEVSPEC, 0);
        P_AddThinker(&plat->thinker);

        plat->type = type;
        plat->sector = sec;
        plat->sector->floordata = plat; //SoM: 3/7/2000
        plat->thinker.function.acp1 = (actionf_p1) T_PlatRaise;
        plat->crush = false;
        plat->tag = line->tag;

        //jff 1/26/98 Avoid raise plat bouncing a head off a ceiling and then
        //going down forever -- default low to plat height when triggered
        plat->low = sec->floorheight;

        switch(type)
        {
          case raiseToNearestAndChange:
            plat->speed = PLATSPEED/2;
            sec->floorpic = sides[line->sidenum[0]].sector->floorpic;
            plat->high = P_FindNextHighestFloor(sec,sec->floorheight);
            plat->wait = 0;
            plat->status = up;
            // NO MORE DAMAGE, IF APPLICABLE
            sec->special = 0;
            sec->oldspecial = 0; //SoM: 3/7/2000: Clear old field.

            S_StartSound((mobj_t *)&sec->soundorg,sfx_stnmov);
            break;

          case raiseAndChange:
            plat->speed = PLATSPEED/2;
            sec->floorpic = sides[line->sidenum[0]].sector->floorpic;
            plat->high = sec->floorheight + amount*FRACUNIT;
            plat->wait = 0;
            plat->status = up;

            S_StartSound((mobj_t *)&sec->soundorg,sfx_stnmov);
            break;

          case downWaitUpStay:
            plat->speed = PLATSPEED * 4;
            plat->low = P_FindLowestFloorSurrounding(sec);

            if (plat->low > sec->floorheight)
                plat->low = sec->floorheight;

            plat->high = sec->floorheight;
            plat->wait = 35*PLATWAIT;
            plat->status = down;
            S_StartSound((mobj_t *)&sec->soundorg,sfx_pstart);
            break;

          case blazeDWUS:
            plat->speed = PLATSPEED * 8;
            plat->low = P_FindLowestFloorSurrounding(sec);

            if (plat->low > sec->floorheight)
                plat->low = sec->floorheight;

            plat->high = sec->floorheight;
            plat->wait = 35*PLATWAIT;
            plat->status = down;
            S_StartSound((mobj_t *)&sec->soundorg,sfx_pstart);
            break;

          case perpetualRaise:
            plat->speed = PLATSPEED;
            plat->low = P_FindLowestFloorSurrounding(sec);

            if (plat->low > sec->floorheight)
                plat->low = sec->floorheight;

            plat->high = P_FindHighestFloorSurrounding(sec);

            if (plat->high < sec->floorheight)
                plat->high = sec->floorheight;

            plat->wait = 35*PLATWAIT;
            plat->status = P_Random()&1;

            S_StartSound((mobj_t *)&sec->soundorg,sfx_pstart);
            break;

          case toggleUpDn: //SoM: 3/7/2000: Instant toggle.
            plat->speed = PLATSPEED;
            plat->wait = 35*PLATWAIT;
            plat->crush = true;

            // set up toggling between ceiling, floor inclusive
            plat->low = sec->ceilingheight;
            plat->high = sec->floorheight;
            plat->status =  down;
            break;

          default:
            break;
        }
        P_AddActivePlat(plat);
    }
    return rtn;
}


//SoM: 3/7/2000: Use boom limit removal
void P_ActivateInStasis(int tag)
{
  platlist_t *pl;
  for (pl=activeplats; pl; pl=pl->next)
  {
    plat_t *plat = pl->plat;
    if (plat->tag == tag && plat->status == in_stasis) 
    {
      if (plat->type==toggleUpDn)
        plat->status = plat->oldstatus==up? down : up;
      else
        plat->status = plat->oldstatus;
      plat->thinker.function.acp1 = (actionf_p1) T_PlatRaise;
    }
  }
}
//SoM: 3/7/2000: use Boom code insted.
int EV_StopPlat(line_t* line)
{
  platlist_t *pl;
  for (pl=activeplats; pl; pl=pl->next)
  {
    plat_t *plat = pl->plat;
    if (plat->status != in_stasis && plat->tag == line->tag)
    {
      plat->oldstatus = plat->status;
      plat->status = in_stasis;
      plat->thinker.function.acv = (actionf_v)NULL;
    }
  }
  return 1;
}

//SoM: 3/7/2000: No more limits!
void P_AddActivePlat(plat_t* plat)
{
  platlist_t *list = malloc(sizeof *list);
  list->plat = plat;
  plat->list = list;
  if ((list->next = activeplats))
    list->next->prev = &list->next;
  list->prev = &activeplats;
  activeplats = list;
}

//SoM: 3/7/2000: No more limits!
void P_RemoveActivePlat(plat_t* plat)
{
  platlist_t *list = plat->list;
  plat->sector->floordata = NULL; //jff 2/23/98 multiple thinkers
  P_RemoveThinker(&plat->thinker);
  if ((*list->prev = list->next))
    list->next->prev = list->prev;
  free(list);
}


//SoM: 3/7/2000: Removes all active plats.
void P_RemoveAllActivePlats(void)
{
  while (activeplats)
  {  
    platlist_t *next = activeplats->next;
    free(activeplats);
    activeplats = next;
  }
}
