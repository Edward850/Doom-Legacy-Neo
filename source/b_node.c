// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: b_node.c,v 1.3 2002/09/28 06:53:11 tonyd Exp $
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
// $Log: b_node.c,v $
// Revision 1.3  2002/09/28 06:53:11  tonyd
// fixed CR problem, fixed game options crash
//
// Revision 1.2  2002/09/27 16:40:08  tonyd
// First commit of acbot
//


#include "b_bot.h"
#include "b_node.h"
#include "z_zone.h"
#include "g_game.h"
#include "r_state.h"
#include "p_maputl.h"
#include "r_main.h"
#include "p_local.h"
#include "p_setup.h"

#define x2ClosestPosX(a) ((fixed_t)((float)(a)/(float)BOTNODEGRIDSIZE + 0.5) - xOffset)
#define y2ClosestPosY(a) ((fixed_t)((float)(a)/(float)BOTNODEGRIDSIZE + 0.5) - yOffset)
#define x2PosX(a) ((fixed_t)((float)(a)/(float)BOTNODEGRIDSIZE) - xOffset)
#define y2PosY(a) ((fixed_t)((float)(a)/(float)BOTNODEGRIDSIZE) - yOffset)

sector_t *oksector = NULL;

boolean				botdoorfound = false,
							botteledestfound = false;
int					botteledestx,
					botteledesty,
					botteletype,
					numbotnodes,
					xOffset,
					xSize,
					yOffset,
					ySize;
SearchNode_t		***botNodeArray;

static sector_t *last_s;

SearchNode_t* B_FindClosestNode(fixed_t x, fixed_t y)
{
	int				i, j,
					depth = 0;

	botdirtype_t	dir = BDI_SOUTH;
	SearchNode_t	*closestNode = NULL;

	i = x = x2ClosestPosX(x);
	j = y = y2ClosestPosY(y);

	while (!closestNode && (depth < 50))
	{
		if ((i >= 0) && (i < xSize) && (j >= 0) && (j < ySize))
		{
			if (botNodeArray[i][j])
				closestNode = botNodeArray[i][j];
		}

		switch (dir)
		{
		case (BDI_EAST):
			if (++i > x + depth)
			{
				i--;	//change it back
				dir = BDI_NORTH;	//go in the new direction
			}
			break;
		case (BDI_NORTH):
			if (++j > y + depth)
			{
				j--;	//change it back
				dir = BDI_WEST;
			}
			break;
		case (BDI_WEST):
			if (--i < x - depth)
			{
				i++;	//change it back
				dir = BDI_SOUTH;
			}
			break;
		case (BDI_SOUTH):
			if (--j < y - depth)
			{
				j++;	//change it back
				dir = BDI_EAST;
				depth++;
			}
			break;
		default:	//shouldn't ever happen
			break;
		}
	}

	return closestNode;
}

SearchNode_t* B_GetClosestReachableNode(fixed_t x, fixed_t y)
{
	int	i,j, nx, ny;
	SearchNode_t	*tempNode = NULL;

	nx = x2PosX(x);
	ny = y2PosY(y);

	if ((nx >= 0) && (nx < xSize) && (ny >= 0) && (ny < ySize))
	{
		tempNode = botNodeArray[nx][ny];
		if (tempNode && !B_NodeReachable(NULL, x, y, posX2x(tempNode->x), posY2y(tempNode->y)))
			tempNode = NULL;
	}

	for (i = nx-1; !tempNode && (i <= nx+1); i++)
	for (j = ny-1; !tempNode && (j <= ny+1); j++)
		if ((i >= 0) && (i < xSize) && (j >= 0) && (j < ySize))
		{
			tempNode = botNodeArray[i][j];

			if (tempNode)
			{
				if (!B_NodeReachable(NULL, x, y, posX2x(tempNode->x), posY2y(tempNode->y)))
					tempNode = NULL;
			}
		}

	if (!tempNode)
		tempNode = B_FindClosestNode(x, y);
	return tempNode;
}

SearchNode_t* B_GetNodeAt(fixed_t x, fixed_t y)
{
	int	i,j, nx, ny;
	SearchNode_t	*tempNode = NULL;

	nx = x2PosX(x);
	ny = y2PosY(y);

	if ((nx >= 0) && (nx < xSize) && (ny >= 0) && (ny < ySize))
	{
		tempNode = botNodeArray[nx][ny];
		if (tempNode && !B_NodeReachable(NULL, posX2x(tempNode->x), posY2y(tempNode->y), x, y))
			tempNode = NULL;
	}

	for (i = nx-1; !tempNode && (i <= nx+1); i++)
	for (j = ny-1; !tempNode && (j <= ny+1); j++)
		if ((i >= 0) && (i < xSize) && (j >= 0) && (j < ySize) && ((i != nx) && (j != ny)))
		{
			tempNode = botNodeArray[i][j];

			if (tempNode)
			{
				if (!B_NodeReachable(NULL, posX2x(tempNode->x), posY2y(tempNode->y), x, y))
					tempNode = NULL;
			}
		}

	return tempNode;
}

boolean B_PTRPathTraverse (intercept_t *in)
{
	fixed_t floorheight, ceilingheight;
	line_t *line;
	sector_t *s;


	if (in->isaline)
	{
		line = in->d.line;

		if (!(line->flags & ML_TWOSIDED) || (line->flags & ML_BLOCKING))
			return false;
		else
		{
			int			i;
			mobj_t*	m;
			thinker_t* th;

			switch(line->special)
			{
				case 1:           // Vertical Door
				case 26:          // Blue Door/Locked
				case 27:          // Yellow Door /Locked
				case 28:          // Red Door /Locked

				case 31:          // Manual door open
				case 32:          // Blue locked door open
				case 33:          // Red locked door open
				case 34:          // Yellow locked door open

				case 62:			//SR slow lift
				case 123:			//SR blazing lift

				case 117:         // Blazing door raise
				case 118:         // Blazing door open
					//Determine if looking at backsector/frontsector.
					oksector = (line->backsector == last_s) ? line->frontsector : line->backsector;

					botdoorfound = true;
					break;
			  //case 39:      // TELEPORT TRIGGER	useful only once anyway so forget it
				case 97:      // TELEPORT RETRIGGER
				case 208:     //boom Silent thing teleporters
				case 207:
				for (i = -1; (i = P_FindSectorFromLineTag(line, i)) >= 0;)
					for (th = thinkercap.next; th != &thinkercap; th = th->next)
						if (th->function.acp1 == (actionf_p1) P_MobjThinker &&
							(m = (mobj_t *) th)->type == MT_TELEPORTMAN  &&
							m->subsector->sector-sectors == i)
						{
							botteledestfound = true;
							botteledestx = m->x;
							botteledesty = m->y;
							botteletype = line->special;

							//CONS_Printf("found a teleporter line going to x:%d, y:%d\n", botteledestx, botteledesty);
						}
				break;
			  // boom linedef types.
			  case 243:     //Same as below but trigger once.
			  case 244:     //Silent line to line teleporter
			  case 262:     //Same as 243 but reversed
			  case 263:     //Same as 244 but reversed
				if (!botteledestfound)
				for (i = -1; (i = P_FindLineFromLineTag(line, i)) >= 0;)
				if (&lines[i] != line)
				{
					botteledestfound = true;
					botteledestx = (lines[i].v1->x+lines[i].v2->x)/2 - ((line->v1->x+line->v2->x)/2 - botteledestx);
					botteledesty = (lines[i].v1->y+lines[i].v2->y)/2 - ((line->v1->y+line->v2->y)/2 - botteledesty);
					botteletype = line->special;

							//CONS_Printf("found a teleporter line going to x:%d, y:%d\n", botteledestx, botteledesty);
				}
				break;
			  default:	//not a special type

				botteledestfound = false;
				//Determine if looking at backsector/frontsector.
				s = (line->backsector == last_s) ? line->frontsector : line->backsector;
				ceilingheight = s->ceilingheight;
				floorheight = s->floorheight;
				if (s != oksector)
				{
					if (((ceilingheight - floorheight) < mobjinfo[MT_PLAYER].height) && !s->tag)
						return false;	//can't fit
					if (((floorheight > (last_s->floorheight+(45<<FRACBITS))) || (((floorheight > (last_s->floorheight+(37<<FRACBITS))) && (last_s->floortype == FLOOR_WATER)))))
						return false;	//i can't jump or reach there
				}
			}

			return true;
		}
	}
	else if ((in->d.thing->flags & MF_SOLID) && !(in->d.thing->flags & MF_SHOOTABLE))
		return false;

	return true;
}

boolean PIT_NodeReachable (line_t* ld)
{
    if (tmbbox[BOXRIGHT] <= ld->bbox[BOXLEFT]
        || tmbbox[BOXLEFT] >= ld->bbox[BOXRIGHT]
        || tmbbox[BOXTOP] <= ld->bbox[BOXBOTTOM]
        || tmbbox[BOXBOTTOM] >= ld->bbox[BOXTOP] )
        return true;

	if (P_BoxOnLineSide (tmbbox, ld) != -1)
        return true;

	if (ld->flags & ML_BLOCKING)
		return false;

	return true;
}

boolean B_CheckNodePosition(mobj_t* thing, fixed_t x, fixed_t y)
{
    int                 xl;
    int                 xh;
    int                 yl;
    int                 yh;
    int                 bx;
    int                 by;
//    subsector_t*        newsubsec;

    tmbbox[BOXTOP] = y + thing->radius;
    tmbbox[BOXBOTTOM] = y - thing->radius;
    tmbbox[BOXRIGHT] = x + thing->radius;
    tmbbox[BOXLEFT] = x - thing->radius;

    validcount++;
    numspechit = 0;
	
	/*	xl = (tmbbox[BOXLEFT] - bmaporgx - MAXRADIUS)>>MAPBLOCKSHIFT;
	xh = (tmbbox[BOXRIGHT] - bmaporgx + MAXRADIUS)>>MAPBLOCKSHIFT;
	yl = (tmbbox[BOXBOTTOM] - bmaporgy - MAXRADIUS)>>MAPBLOCKSHIFT;
	yh = (tmbbox[BOXTOP] - bmaporgy + MAXRADIUS)>>MAPBLOCKSHIFT;

	for (bx=xl ; bx<=xh ; bx++)
		for (by=yl ; by<=yh ; by++)
			if (!P_BlockThingsIterator(bx,by,PIT_CheckThing))
				return false;
*/
    // check lines
    xl = (tmbbox[BOXLEFT] - bmaporgx)>>MAPBLOCKSHIFT;
    xh = (tmbbox[BOXRIGHT] - bmaporgx)>>MAPBLOCKSHIFT;
    yl = (tmbbox[BOXBOTTOM] - bmaporgy)>>MAPBLOCKSHIFT;
    yh = (tmbbox[BOXTOP] - bmaporgy)>>MAPBLOCKSHIFT;

    for (bx=xl ; bx<=xh ; bx++)
        for (by=yl ; by<=yh ; by++)
            if (!P_BlockLinesIterator (bx,by,PIT_NodeReachable))
                return false;
			
	return true;
}

boolean B_NodeReachable(mobj_t* mo, fixed_t x, fixed_t y, fixed_t destx, fixed_t desty)
{
	fixed_t		nx = x2PosX(destx),
				ny = y2PosY(desty);

	botdoorfound = false;
	botteledestfound = false;

	if ((nx >= 0) && (nx < xSize) && (ny >= 0) && (ny < ySize))
	{	
		botteledestx = destx;
		botteledesty = desty;
		last_s = R_PointInSubsector(x, y)->sector;
		if (mo)
		{
			if (B_CheckNodePosition(mo, destx, desty)
				&& P_PathTraverse (x, y, destx - 1, desty + 1, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x, y, destx + 1, desty + 1, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x, y, destx - 1, desty - 1, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x, y, destx + 1, desty - 1, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x - 1, y + 1, destx, desty, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x + 1, y + 1, destx, desty, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x - 1, y - 1, destx, desty, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x + 1, y - 1, destx, desty, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse)
				&& P_PathTraverse (x, y, destx, desty, PT_ADDLINES|PT_ADDTHINGS, B_PTRPathTraverse))
				return true;
			else
			{
				botteledestfound = false;
				return false;
			}
		}
		else
			return P_PathTraverse (x, y, destx, desty, PT_ADDLINES, B_PTRPathTraverse);
	}

	return false;
}

int B_GetNodeCost(SearchNode_t* node)
{
	int				cost = 10000;
	sector_t*		sector;
	if (node)
	{
		sector = R_PointInSubsector(node->x, node->y)->sector;
		if (sector->floortype == FLOOR_LAVA)
			cost = 50000;
		else if (sector->floortype == FLOOR_LAVA)
			cost = 40000;
	}

	return cost;
}

SearchNode_t* B_CreateNode(fixed_t x, fixed_t y)
{
	SearchNode_t* newnode = Z_Malloc(sizeof(SearchNode_t), PU_LEVEL, 0);

	newnode->cost = 0;
	newnode->f = 0;
	newnode->heuristic = 0;
	newnode->vnext = NULL;
	newnode->pprevious = NULL;
	newnode->pnext = NULL;
	newnode->visited = false;
	newnode->x = x;
	newnode->y = y;

	//CONS_Printf("Created node at x:%d, y:%d.\n", (x>>FRACBITS), (y>>FRACBITS));
	botNodeArray[x][y] = newnode;
	/*newnode->mo = P_SpawnMobj(posX2x(newnode->x), posY2y(newnode->y), R_PointInSubsector(posX2x(newnode->x), posY2y(newnode->y))->sector->floorheight, MT_MISC49);*/

	return newnode;
}

void B_DeleteNode(SearchNode_t* node)
{
	Z_Free(node);

//	CONS_Printf("deleted a node\n");

}

void B_SetNodeTeleDest(SearchNode_t* node)
{
	fixed_t		x = x2ClosestPosX(botteledestx),
				y = y2ClosestPosY(botteledesty);

	//CONS_Printf("trying to make a tele node at x:%d, y:%d\n", botteledestx>>FRACBITS, botteledesty>>FRACBITS);
	
		if (!botNodeArray[x][y])
		{
			node->dir[BDI_TELEPORT] = B_CreateNode(x, y);
			//CONS_Printf("created teleporter node at x:%d, y:%d\n", botteledestx>>FRACBITS, botteledesty>>FRACBITS);
			numbotnodes++;
			B_BuildNodes(node->dir[BDI_TELEPORT]);
		}
		else
			node->dir[BDI_TELEPORT] = botNodeArray[x][y];
		node->costDir[BDI_TELEPORT] = 20000;//B_GetNodeCost(node->dir[TELEPORT]);
}

void B_BuildNodes(SearchNode_t* node)
{
	fixed_t		angle,
				nx, ny,
				extraCost;

	LinkedList_t *queue = B_LLCreate();
	B_LLInsertFirstNode(queue, node);
	while (!B_LLIsEmpty(queue))
	{
		node = B_LLRemoveLastNode(queue);
		node->dir[BDI_TELEPORT] = NULL;

		for (angle = BDI_EAST; angle <= BDI_SOUTHEAST; angle++)
		{
			extraCost = 0;
			switch(angle)
			{
			case (BDI_EAST):
				nx = node->x + 1;
				ny = node->y;
				break;
			case (BDI_NORTHEAST):
				nx = node->x + 1;
				ny = node->y + 1;
				extraCost = 5000;	//because diaginal
				break;
			case (BDI_NORTH):
				nx = node->x;
				ny = node->y + 1;
				break;
			case (BDI_NORTHWEST):
				nx = node->x - 1;
				ny = node->y + 1;
				extraCost = 5000;	//because diaginal
				break;
			case (BDI_WEST):
				nx = node->x - 1;
				ny = node->y;
				break;
			case (BDI_SOUTHWEST):
				nx = node->x - 1;
				ny = node->y - 1;
				extraCost = 5000;	//because diaginal
				break;
			case (BDI_SOUTH):
				nx = node->x;
				ny = node->y - 1;
				break;
			case (BDI_SOUTHEAST):
				nx = node->x + 1;
				ny = node->y - 1;
				extraCost = 5000;	//because diaginal
				break;
			default:	//shouldn't ever happen
				nx = ny = node->x;	//shut compiler up
				break;
			}

			if (B_NodeReachable(players[0].mo, posX2x(node->x), posY2y(node->y), posX2x(nx), posY2y(ny)))
			{
				if (!botNodeArray[nx][ny])
				{
					node->dir[angle] = B_CreateNode(nx, ny);
					numbotnodes++;
					B_LLInsertFirstNode(queue, node->dir[angle]);
				}
				else
					node->dir[angle] = botNodeArray[nx][ny];
				node->costDir[angle] = B_GetNodeCost(node->dir[angle]) + extraCost;

				if (botteledestfound)
					B_SetNodeTeleDest(node->dir[angle]);
			}
			else
				node->dir[angle] = NULL;
		}
	}

	B_LLDelete(queue);
}

void B_InitNodes()
{
	int		i, j, px, py,
			xMax = -1000000000,
			xMin = 1000000000,
			yMax = -1000000000,
			yMin = 1000000000;

	SearchNode_t* tempNode;

	for (i=0; i<numvertexes; i++)
	{
		if (vertexes[i].x < xMin)
			xMin = vertexes[i].x;
		else if (vertexes[i].x > xMax)
			xMax = vertexes[i].x;

		if (vertexes[i].y < yMin)
			yMin = vertexes[i].y;
		else if (vertexes[i].y > yMax)
			yMax = vertexes[i].y;
	}
//	xMin-=BOTNODEGRIDSIZE; xMax+=2*BOTNODEGRIDSIZE;
//	yMin-=BOTNODEGRIDSIZE; yMax+=2*BOTNODEGRIDSIZE;

	xOffset = xMin/BOTNODEGRIDSIZE;
	xSize = ceil( (double)(xMax - xMin)/(double)BOTNODEGRIDSIZE) + 1;
	yOffset = yMin/BOTNODEGRIDSIZE;	
	ySize = ceil( (double)(yMax - yMin)/(double)BOTNODEGRIDSIZE) + 1;

	botNodeArray = (SearchNode_t***)Z_Malloc(xSize*sizeof(SearchNode_t*),PU_LEVEL,0);
	for (i=0; i<xSize; i++)
	{
		botNodeArray[i] = (SearchNode_t**)Z_Malloc(ySize*sizeof(SearchNode_t*),PU_LEVEL,0);

		for (j=0; j<ySize; j++)
			botNodeArray[i][j] = NULL;
	}

	numbotnodes = 0;
	CONS_Printf("Building nodes for acbot.....\n");
	for (i = 0; i < MAXPLAYERS; i++)
	if (playerstarts[i])
	{
		px = playerstarts[i]->x/(BOTNODEGRIDSIZE>>FRACBITS) - xOffset;
		py = playerstarts[i]->y/(BOTNODEGRIDSIZE>>FRACBITS) - yOffset;
		if (((px >= 0) && (px < xSize) && (py >= 0) && (py < ySize)) && (!botNodeArray[px][py]))
		{
			tempNode = B_CreateNode(px, py);
			numbotnodes++;
			B_BuildNodes(tempNode);
		}
	}

	for (i = 0; i < MAX_DM_STARTS; i++)
	if (deathmatchstarts[i])
	{
		px = deathmatchstarts[i]->x/(BOTNODEGRIDSIZE>>FRACBITS) - xOffset;
		py = deathmatchstarts[i]->y/(BOTNODEGRIDSIZE>>FRACBITS) - yOffset;
		if (((px >= 0) && (px < xSize) && (py >= 0) && (py < ySize)) && (!botNodeArray[px][py]))
		{
			tempNode = B_CreateNode(px, py);
			numbotnodes++;
			B_BuildNodes(tempNode);
		}
	}

	CONS_Printf("Completed building %d nodes.\n", numbotnodes);
}

