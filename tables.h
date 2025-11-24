// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: tables.h,v 1.2 2000/02/27 00:42:11 hurdler Exp $
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
// $Log: tables.h,v $
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Lookup tables.
//      Do not try to look them up :-).
//      In the order of appearance: 
//
//      int finetangent[4096]   - Tangens LUT.
//       Should work with BAM fairly well (12 of 16bit,
//      effectively, by shifting).
//
//      int finesine[10240]             - Sine lookup.
//
//      int tantoangle[2049]    - ArcTan LUT,
//        maps tan(angle) to angle fast. Gotta search.
//    
//-----------------------------------------------------------------------------


#ifndef __TABLES__
#define __TABLES__

#ifdef LINUX
#include <math.h>
#else
//#define PI                              3.141592657
#endif

#include "m_fixed.h"

#define FINEANGLES              8192
#define FINEMASK                (FINEANGLES-1)
#define ANGLETOFINESHIFT        19      // 0x100000000 to 0x2000


// Effective size is 10240.
extern  fixed_t         finesine[5*FINEANGLES/4];

// Re-use data, is just PI/2 phase shift.
extern  fixed_t*        finecosine;


// Effective size is 4096.
extern fixed_t          finetangent[FINEANGLES/2];

#define ANG45           0x20000000
#define ANG90           0x40000000
#define ANG180          0x80000000
#define ANG270          0xc0000000

#define ANGLE_45    0x20000000
#define ANGLE_90    0x40000000
#define ANGLE_180   0x80000000
#define ANGLE_MAX   0xffffffff
#define ANGLE_1     (ANGLE_45/45)
#define ANGLE_60    (ANGLE_180/3)

typedef unsigned angle_t;


// to get a global angle from cartesian coordinates, the coordinates are
// flipped until they are in the first octant of the coordinate system, then
// the y (<=x) is scaled and divided by x to get a tangent (slope) value
// which is looked up in the tantoangle[] table.
#define SLOPERANGE  2048
#define SLOPEBITS   11
#define DBITS       (FRACBITS-SLOPEBITS)

// The +1 size is to handle the case when x==y without additional checking.
extern  angle_t     tantoangle[SLOPERANGE+1];

// Utility function, called by R_PointToAngle.
int SlopeDiv ( unsigned      num,
               unsigned      den);


#endif
