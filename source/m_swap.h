// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: m_swap.h,v 1.5 2003/05/07 03:03:03 sburke Exp $
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
// $Log: m_swap.h,v $
// Revision 1.5  2003/05/07 03:03:03  sburke
// Make the SHORT and LONG macros cast their value to short and long.
//
// Revision 1.4  2001/04/17 22:26:07  calumr
// Initial Mac add
//
// Revision 1.3  2001/02/24 13:35:20  bpereira
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
//      Endianess handling, swapping 16bit and 32bit.
//
//-----------------------------------------------------------------------------


#pragma once

#ifdef __BIG_ENDIAN__
inline short LE_SWAP_SHORT(int x)
{
    return static_cast<short>(
        (((unsigned short)(x) & 0x00ffU) << 8) |
        (((unsigned short)(x) & 0xff00U) >> 8));
}

inline int LE_SWAP_LONG(int x)
{
    return
        (((unsigned int)(x) & 0x000000ffUL) << 24) |
        (((unsigned int)(x) & 0x0000ff00UL) << 8) |
        (((unsigned int)(x) & 0x00ff0000UL) >> 8) |
        (((unsigned int)(x) & 0xff000000UL) >> 24);
}
#else
inline short LE_SWAP_SHORT(int x) { return static_cast<short>(x); }
inline long LE_SWAP_LONG(long x) { return static_cast<long>(x); }
#endif
