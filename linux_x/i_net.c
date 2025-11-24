// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_net.c,v 1.4 2000/09/10 10:49:48 metzgermeister Exp $
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
// $Log: i_net.c,v $
// Revision 1.4  2000/09/10 10:49:48  metzgermeister
// make it work again
//
// Revision 1.3  2000/09/01 19:34:37  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      network interface
//      
//-----------------------------------------------------------------------------

#include "../doomdef.h"
#include "../m_argv.h"

//
// I_InitNetwork
// Only required for DOS, so this is more a dummy
//
boolean I_InitNetwork (void)
{
    if( M_CheckParm ("-net") )
    {
        I_Error("-net not supported, use -server and -connect\n"
            "see docs for more\n");
    }
    return false;
}
