// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: m_bbox.c,v 1.3 2000/04/23 16:19:52 bpereira Exp $
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
// $Log: m_bbox.c,v $
// Revision 1.3  2000/04/23 16:19:52  bpereira
// no message
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      bounding boxes
//
//-----------------------------------------------------------------------------


#include "doomtype.h"
#include "m_bbox.h"

// faB: getting sick of windows includes errors,
//     I'm supposed to clean that up later.. sure
#ifdef __WIN32__
#define MAXINT    ((int)0x7fffffff)
#define MININT    ((int)0x80000000)
#endif


void M_ClearBox (fixed_t *box)
{
    box[BOXTOP] = box[BOXRIGHT] = MININT;
    box[BOXBOTTOM] = box[BOXLEFT] = MAXINT;
}

void M_AddToBox ( fixed_t*      box,
                  fixed_t       x,
                  fixed_t       y )
{
    if (x<box[BOXLEFT  ])   box[BOXLEFT  ] = x;
    if (x>box[BOXRIGHT ])   box[BOXRIGHT ] = x;

    if (y<box[BOXBOTTOM])   box[BOXBOTTOM] = y;
    if (y>box[BOXTOP   ])   box[BOXTOP   ] = y;
}

boolean M_PointInBox ( fixed_t*      box,
                       fixed_t       x,
                       fixed_t       y )
{
    if (x<box[BOXLEFT]  ) return false;
    if (x>box[BOXRIGHT] ) return false;
    if (y<box[BOXBOTTOM]) return false;
    if (y>box[BOXTOP]   ) return false;
    
    return true;
}

boolean M_CircleTouchBox(fixed_t* box, fixed_t circlex, fixed_t circley, fixed_t circleradius)
{
    if( box[BOXLEFT  ]-circleradius > circlex ) return false;
    if( box[BOXRIGHT ]+circleradius < circlex ) return false;
    if( box[BOXBOTTOM]-circleradius > circley ) return false;
    if( box[BOXTOP   ]+circleradius < circley ) return false;
    return true;
}
