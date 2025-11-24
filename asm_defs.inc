// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: asm_defs.inc,v 1.4 2001/02/24 13:35:19 bpereira Exp $
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
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
// $Log: asm_defs.inc,v $
// Revision 1.4  2001/02/24 13:35:19  bpereira
// no message
//
// Revision 1.3  2001/02/10 15:24:19  hurdler
// Apply Rob's patch for Linux version
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      must match the C structures !!!
//
//-----------------------------------------------------------------------------


#ifndef __ASM_DEFS__
#define __ASM_DEFS__

 // this makes variables more noticable,
// and make the label match with C code


// Linux, unlike DOS, has no "_" 19990119 by Kin
// and nasm needs .data code segs under linux 20010210 by metzgermeister
// FIXME: nasm ignores these settings, so I put the macros into the makefile
#ifdef LINUX
#define C(label) label
#define CODE_SEG .data
#else
#define C(label) _##label
#define CODE_SEG .text                         
#endif

/* This is a more readable way to access the arguments passed from C code   */
/* PLEASE NOTE: it is supposed that all arguments passed from C code are    */
/*              32bit integer (int, long, and most *pointers)               */
#define ARG1      8(%ebp)
#define ARG2      12(%ebp)
#define ARG3      16(%ebp)
#define ARG4      20(%ebp)
#define ARG5      24(%ebp)
#define ARG6      28(%ebp)
#define ARG7      32(%ebp)
#define ARG8      36(%ebp)
#define ARG9      40(%ebp)      //(c)tm ... Allegro by Shawn Hargreaves.


#endif
