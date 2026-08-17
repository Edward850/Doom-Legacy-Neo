// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: d_net.h,v 1.5 2001/02/10 12:27:13 bpereira Exp $
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
// $Log: d_net.h,v $
// Revision 1.5  2001/02/10 12:27:13  bpereira
// no message
//
// Revision 1.4  2000/09/15 19:49:22  bpereira
// no message
//
// Revision 1.3  2000/08/31 14:30:55  bpereira
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
//      Networking stuff.
//      part of layer 4 (transport) (tp4) of the osi model
//      assure the reception of packet and proceed a checksums
//
//-----------------------------------------------------------------------------


#ifndef __D_NET__
#define __D_NET__

//
// Network play related stuff.
// There is a data struct that stores network
//  communication related stuff, and another
//  one that defines the actual packets to
//  be transmitted.
//

// Max computers in a game.
#define MAXNETNODES             32
#define BROADCASTADDR           MAXNETNODES // added xx/5/99: can use broadcast now
#define MAXSPLITSCREENPLAYERS   2    // maximum number of players on a single computer

#define STATLENGTH  (TICRATE*2)

// stat of net
extern  int    getbps,sendbps;
extern  float  lostpercent,duppercent,gamelostpercent;
extern  int    packetheaderlength;
boolean Net_GetNetStat(void);
extern  int    getbytes;
extern  INT64  sendbytes;        // realtime updated 

void    Net_AckTicker(void);
boolean Net_AllAckReceived(void);

// if reliable return true if packet sent, 0 else
boolean HSendPacket(int   node, boolean reliable, byte acknum, int packetlength);
boolean HGetPacket (void);
boolean D_CheckNetGame (void);
void    D_CloseConnection( void );
void    Net_UnAcknowledgPacket(int node);
void    Net_CloseConnection(int node);
void    Net_AbortPacketType(char packettype);
void    Net_SendAcks(int node);
void    Net_WaitAllAckReceived( ULONG timeout );
#endif
