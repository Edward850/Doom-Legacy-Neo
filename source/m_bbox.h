// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: m_bbox.h,v 1.3 2000/04/23 16:19:52 bpereira Exp $
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
// $Log: m_bbox.h,v $
// Revision 1.3  2000/04/23 16:19:52  bpereira
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
//    bounding boxes.
//    
//-----------------------------------------------------------------------------


#ifndef __M_BBOX__
#define __M_BBOX__

#include "m_fixed.h"

// Bounding box coordinate storage.
enum
{
    BOXTOP,
    BOXBOTTOM,
    BOXLEFT,
    BOXRIGHT
};      // bbox coordinates

// Bounding box functions.
void M_ClearBox (fixed_t*       box);

void M_AddToBox (fixed_t*       box, fixed_t    x, fixed_t      y );
boolean M_PointInBox ( fixed_t*      box,
                       fixed_t       x,
                       fixed_t       y );

boolean M_CircleTouchBox(fixed_t* box, fixed_t circlex, fixed_t circley, fixed_t circleradius) ;


#endif
