// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: m_cheat.h,v 1.3 2001/02/10 12:27:14 bpereira Exp $
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
// $Log: m_cheat.h,v $
// Revision 1.3  2001/02/10 12:27:14  bpereira
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
//      Cheat code checking.
//
//-----------------------------------------------------------------------------


#ifndef __M_CHEAT__
#define __M_CHEAT__

#include "d_event.h"


//
// CHEAT SEQUENCE PACKAGE
//

#define SCRAMBLE(a) \
((((a)&1)<<7) + (((a)&2)<<5) + ((a)&4) + (((a)&8)<<1) \
 + (((a)&16)>>1) + ((a)&32) + (((a)&64)>>5) + (((a)&128)>>7))

typedef struct
{
    byte*      sequence;
    byte*      p;

} cheatseq_t;

int cht_CheckCheat ( cheatseq_t*           cht,
                     char                  key );


void cht_GetParam ( cheatseq_t*           cht,
                    char*                 buffer );

boolean cht_Responder (event_t* ev);
void cht_Init();

void Command_CheatNoClip_f (void);
void Command_CheatGod_f (void);
void Command_CheatGimme_f (void);

#endif
