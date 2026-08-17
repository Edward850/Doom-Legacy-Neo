// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: m_argv.h,v 1.3 2000/03/29 19:39:48 bpereira Exp $
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
// $Log: m_argv.h,v $
// Revision 1.3  2000/03/29 19:39:48  bpereira
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
//  
//    
//-----------------------------------------------------------------------------


#ifndef __M_ARGV__
#define __M_ARGV__

//
// MISC
//
extern  int     myargc;
extern  char**  myargv;

// Returns the position of the given parameter
// in the arg list (0 if not found).
int  M_CheckParm (char* check);


// push all parameters bigining by a +, ex : +map map01
void M_PushSpecialParameters( void );

// return true if there is available parameters
// use it befor M_GetNext 
boolean M_IsNextParm(void);

// return the next parameter after a M_CheckParm
// NULL if not found use M_IsNext to find if there is a parameter
char *M_GetNextParm(void);

// Find a Response File
void M_FindResponseFile (void);

#endif //__M_ARGV__
