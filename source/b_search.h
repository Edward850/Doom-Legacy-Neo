// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: b_search.h,v 1.3 2002/09/28 06:53:11 tonyd Exp $
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
// $Log: b_search.h,v $
// Revision 1.3  2002/09/28 06:53:11  tonyd
// fixed CR problem, fixed game options crash
//
// Revision 1.2  2002/09/27 16:40:08  tonyd
// First commit of acbot
//

#include "m_fixed.h"
#include "b_node.h"

#ifndef __BOTSEARCH_H__
#define __BOTSEARCH_H__

typedef struct
{
	fixed_t		currentSize;	// size of array
	fixed_t		maxSize;	// number of items in array

	SearchNode_t**	heapArray;

} PriorityQ_t;

typedef struct
{
	fixed_t		currentSize;	// size of list

	SearchNode_t	*first,
					*last;
} LinkedList_t;

LinkedList_t* B_LLCreate();
void B_LLDelete(LinkedList_t* list);
void B_LLClear(LinkedList_t* list);
void B_LLInsertFirstNode(LinkedList_t* list, SearchNode_t* tempNode);
boolean B_LLIsEmpty(LinkedList_t* list);
SearchNode_t* B_LLRemoveFirstNode(LinkedList_t* list);
SearchNode_t* B_LLRemoveLastNode(LinkedList_t* list);

void B_NodePushSuccessors(PriorityQ_t* open, SearchNode_t* parent_node, SearchNode_t* dest);

PriorityQ_t* B_PQCreatePQ(int mx);
void B_PQInsertNode(PriorityQ_t* open, SearchNode_t* new_node);
SearchNode_t* B_PQFindNode(PriorityQ_t* open, SearchNode_t* node);
SearchNode_t* B_PQRemoveNode(PriorityQ_t* open, SearchNode_t* node);
SearchNode_t* B_PQRemoveFirstNode(PriorityQ_t* open);
void B_PQTrickleDown(PriorityQ_t* pq, int index);
void B_PQTrickleUp(PriorityQ_t* pq, int index);

#endif
