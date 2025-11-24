// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: I_net.c,v 1.3 2001/03/03 19:29:44 ydario Exp $
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
// $Log: I_net.c,v $
// Revision 1.3  2001/03/03 19:29:44  ydario
// code clean up
//
//
// DESCRIPTION:
//      network interface
//
//-----------------------------------------------------------------------------

#include "../doomdef.h"
#include "../m_argv.h"

//
// NETWORKING
//

//
// I_InitNetwork
//
boolean I_InitNetwork (void)
{
    if( M_CheckParm ("-net") )
    {
      I_Error("The OS/2 version of Legacy don't work with external driver like ipxsetup, sersetup, or doomatic\n"
              "Read the documentation about \"-server\" and \"-connect\" parameters\n");
    }

    return false;
}
