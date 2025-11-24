// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_tcp.h,v 1.4 2000/10/16 20:02:29 bpereira Exp $
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
// $Log: i_tcp.h,v $
// Revision 1.4  2000/10/16 20:02:29  bpereira
// no message
//
// Revision 1.3  2000/08/16 14:10:01  hurdler
// add master server code
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

extern int sock_port;


int I_InitTcpNetwork(void);
//Hurdler: temporar addition for master server
void I_InitTcpDriver(void);
void I_ShutdownTcpDriver(void);
