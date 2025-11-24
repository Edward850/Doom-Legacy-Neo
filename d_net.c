// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: d_net.c,v 1.18 2003/05/04 02:29:14 sburke Exp $
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
// $Log: d_net.c,v $
// Revision 1.18  2003/05/04 02:29:14  sburke
// Fix for big-endian machines.
//
// Revision 1.17  2001/08/26 15:27:29  bpereira
// added fov for glide and fixed newcoronas code
//
// Revision 1.16  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.15  2001/02/10 12:27:13  bpereira
// no message
//
// Revision 1.14  2000/10/21 08:43:28  bpereira
// no message
//
// Revision 1.13  2000/10/16 20:02:29  bpereira
// no message
//
// Revision 1.12  2000/10/08 13:29:59  bpereira
// no message
//
// Revision 1.11  2000/09/28 20:57:14  bpereira
// no message
//
// Revision 1.10  2000/09/15 19:49:21  bpereira
// no message
//
// Revision 1.9  2000/09/10 10:38:18  metzgermeister
// *** empty log message ***
//
// Revision 1.8  2000/09/01 19:34:37  bpereira
// no message
//
// Revision 1.7  2000/09/01 18:23:42  hurdler
// fix some issues with latest network code changes
//
// Revision 1.6  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.5  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.4  2000/03/29 19:39:48  bpereira
// no message
//
// Revision 1.3  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      DOOM Network game communication and protocol,
//      all OS independend parts.
//
//      Implemente a Sliding window protocol without receiver window
//      (out of order reception)
//      This protocol use mix of "goback n" and "selective repeat" implementation
//      The NOTHING packet is send when connection is idle for acknowledge packets
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "g_game.h"
#include "i_net.h"
#include "i_system.h"
#include "m_argv.h"
#include "d_net.h"
#include "w_wad.h"
#include "d_netfil.h"
#include "d_clisrv.h"
#include "z_zone.h"
#include "i_tcp.h"

//
// NETWORKING
//
// gametic is the tic about to (or currently being) run
// server:
//   maketic is the tic that hasn't had control made for it yet
//   nettics : is the tic for eatch node
//   firsttictosend : is the lowest value of nettics
// client:
//   neededtic : is the tic needed by the client for run the game
//   firsttictosend : is used to optimize a condition
// normaly maketic>=gametic>0,

#define FORCECLOSE         0x8000
#define CONNECTIONTIMEOUT  (15*TICRATE)

doomcom_t*  doomcom;
doomdata_t* netbuffer;        // points inside doomcom

FILE*       debugfile=NULL;        // put some net info in a file
                              // during the game

#define     MAXREBOUND 8
static doomdata_t  reboundstore[MAXREBOUND];
static short       reboundsize[MAXREBOUND];
static int         rebound_head,rebound_tail;
int         net_bandwidth;
short       hardware_MAXPACKETLENGTH;

void    (*I_NetGet) (void);
void    (*I_NetSend) (void);
boolean (*I_NetCanSend) (void);
void    (*I_NetCloseSocket) (void);
void    (*I_NetFreeNodenum) (int nodenum);
int     (*I_NetMakeNode) (char *address);
boolean (*I_NetOpenSocket) (void);


// network stats
tic_t       statstarttic;
int         getbytes=0;
INT64       sendbytes=0;
int         retransmit=0   ,duppacket=0;
int         sendackpacket=0,getackpacket=0;
int         ticruned=0     ,ticmiss=0;

// globals
int    getbps,sendbps;
float  lostpercent,duppercent,gamelostpercent;
int    packetheaderlength;

boolean Net_GetNetStat(void)
{
    tic_t t=I_GetTime();
static int oldsendbyte=0;
    if( statstarttic+STATLENGTH<=t )
    {
        getbps=(getbytes*TICRATE)/(t-statstarttic);
        sendbps=((sendbytes-oldsendbyte)*TICRATE)/(t-statstarttic);
        if(sendackpacket)
            lostpercent=100.0*(float)retransmit/(float)sendackpacket;
        else
            lostpercent=0;
        if(getackpacket)
            duppercent=100.0*(float)duppacket/(float)getackpacket;
        else
            duppercent=0;
        if( ticruned )
            gamelostpercent=100.0*(float)ticmiss/(float)ticruned;
        else
            gamelostpercent=0;

        ticmiss=ticruned=0;
        oldsendbyte=sendbytes;
        getbytes=0;
        sendackpacket=getackpacket=duppacket=retransmit=0;
        statstarttic=t;

        return 1;
    }
    return 0;
}

// -----------------------------------------------------------------
//  Some stuct and function for acknowledgment of packets
// -----------------------------------------------------------------
#define MAXACKPACKETS    64 // minimum number of nodes
#define MAXACKTOSEND     64
#define URGENTFREESLOTENUM   6
#define ACKTOSENDTIMEOUT  (TICRATE/17)

typedef struct {
  byte   acknum;
  byte   nextacknum;
  byte   destinationnode;
  tic_t  senttime;
  USHORT length;
  USHORT resentnum; 
  char   pak[MAXPACKETLENGTH];
} ackpak_t;

typedef enum {
    CLOSE  = 1,    // flag is set when connection is closing
} node_flags_t;

// table of packet that was not acknowleged can be resend (the sender window)
static ackpak_t ackpak[MAXACKPACKETS];

typedef struct {
    // ack return to send (like slinding window protocol)
    byte  firstacktosend;

    // when no consecutive packet are received we keep in mind what packet 
    // we already received in a queu 
    byte  acktosend_head;
    byte  acktosend_tail;
    byte  acktosend[MAXACKTOSEND];

    // automaticaly send keep alive packet when not enought trafic
    tic_t lasttimeacktosend_sent;
    // detect connection lost
    tic_t lasttimepacketreceived;
    
    // flow control : do not sent to mush packet with ack 
    byte  remotefirstack;
    byte  nextacknum;

    
    byte   flags;
// jacobson tcp timeout evaluation algorithm (Karn variation)
    fixed_t ping;
    fixed_t varping;
    int     timeout;   // computed with ping and varping
} node_t;

static node_t nodes[MAXNETNODES];

#define  PINGDEFAULT     ((200*TICRATE*FRACUNIT)/1000)
#define  VARPINGDEFAULT  ( (50*TICRATE*FRACUNIT)/1000)
#define  TIMEOUT(p,v)    (p+4*v+FRACUNIT/2)>>FRACBITS;

// return <0 if a<b (mod 256)
//         0 if a=n (mod 256)
//        >0 if a>b (mod 256)
// mnemonic: to use it compare to 0 : cmpack(a,b)<0 is "a<b" ...
static int cmpack(byte a,byte b)
{
    register int d=a-b;

    if(d>=127 || d<-128)
        return -d;
    return d;
}

// return a free acknum and copy netbuffer in the ackpak table
static boolean GetFreeAcknum(byte *freeack, boolean lowtimer)
{
   node_t *node=&nodes[doomcom->remotenode];
   int i,numfreeslote=0;

   if(cmpack((node->remotefirstack+MAXACKTOSEND) % 256,node->nextacknum)<0)
   {
       DEBFILE(va("too fast %d %d\n",node->remotefirstack,node->nextacknum));
       return false;
   }

   for(i=0;i<MAXACKPACKETS;i++)
       if(ackpak[i].acknum==0)
       {
           // for low priority packet, make sure let freeslotes so urgents packets can be sent
           numfreeslote++;
           if( netbuffer->packettype >= PT_CANFAIL && numfreeslote<URGENTFREESLOTENUM)
               continue;

           ackpak[i].acknum=node->nextacknum;
           ackpak[i].nextacknum=node->nextacknum;
           node->nextacknum++;
           if( node->nextacknum==0 )
               node->nextacknum++;
           ackpak[i].destinationnode=node-nodes;
           ackpak[i].length=doomcom->datalength;
           if(lowtimer)
           {
               // lowtime mean can't be sent now so try it soon as possible
               ackpak[i].senttime=0;
               ackpak[i].resentnum = 1;
           }
           else
           {
               ackpak[i].senttime=I_GetTime();
               ackpak[i].resentnum = 0;
           }
           memcpy(ackpak[i].pak,netbuffer,ackpak[i].length);
           
           *freeack=ackpak[i].acknum;
           
           sendackpacket++; // for stat
           
           return true;
       }
#ifdef PARANOIA
   if( devparm )
       CONS_Printf("No more free ackpacket\n");
#endif
   if( netbuffer->packettype < PT_CANFAIL )
       I_Error("Connection lost\n");
   return false;
}

// Get a ack to send in the queu of this node
static byte GetAcktosend(int node)
{
    nodes[node].lasttimeacktosend_sent = I_GetTime();
    return nodes[node].firstacktosend;
}

static void Removeack(int i)
{
    int node=ackpak[i].destinationnode;
    fixed_t trueping=(I_GetTime()-ackpak[i].senttime)<<FRACBITS;
    if( ackpak[i].resentnum )
    {
        // +FRACUNIT/2 for round
        nodes[node].ping = (nodes[node].ping*7 + trueping)/8;
        nodes[node].varping = (nodes[node].varping*7 + abs(nodes[node].ping-trueping))/8;
        nodes[node].timeout = TIMEOUT(nodes[node].ping,nodes[node].varping);
    }
    DEBFILE(va("Remove ack %d trueping %d ping %f var %f timeout %d\n",ackpak[i].acknum,trueping>>FRACBITS,FIXED_TO_FLOAT(nodes[node].ping),FIXED_TO_FLOAT(nodes[node].varping),nodes[node].timeout));
    ackpak[i].acknum=0;
    if( nodes[node].flags & CLOSE )
        Net_CloseConnection( node );
}

// we have got a packet proceed the ack request and ack return
static boolean inline Processackpak()
{
   int i;
   boolean goodpacket=true;
   node_t *node=&nodes[doomcom->remotenode];

// received a ack return remove the ack in the list
   if(netbuffer->ackreturn && cmpack(node->remotefirstack,netbuffer->ackreturn)<0)
   {
       node->remotefirstack=netbuffer->ackreturn;
       // search the ackbuffer and free it
       for(i=0;i<MAXACKPACKETS;i++)
           if( ackpak[i].acknum &&
               ackpak[i].destinationnode==node-nodes &&
               cmpack(ackpak[i].acknum,netbuffer->ackreturn)<=0 )
               Removeack(i);
   }

// received a packet with ack put it in to queue for send the ack back
   if( netbuffer->ack )
   {
       byte ack=netbuffer->ack;
       getackpacket++;
       if( cmpack(ack,node->firstacktosend)<=0 )
       {
           DEBFILE(va("Discard(1) ack %d (duplicated)\n",ack));
           duppacket++;
           goodpacket=false; // discard packet (duplicat)
       }
       else
       {
           // check if it is not allready in the queue
           for(i =node->acktosend_tail;
               i!=node->acktosend_head;
               i =(i+1)%MAXACKTOSEND    )
               if(node->acktosend[i]==ack)
               {
                   DEBFILE(va("Discard(2) ack %d (duplicated)\n",ack));
                   duppacket++;
                   goodpacket=false; // discard packet (duplicat)
                   break;
               }
           if( goodpacket )
           {
               // is a good packet so increment the acknoledge number,then search a "hole" in the queue
               byte nextfirstack=node->firstacktosend+1;
               if(nextfirstack==0) nextfirstack=1;

               if(ack==nextfirstack)
               { 
                   byte hm1; // head-1
                   boolean change=true;

                   node->firstacktosend=nextfirstack++;
                   if(nextfirstack==0) nextfirstack=1;
                   hm1=(node->acktosend_head-1+MAXACKTOSEND)%MAXACKTOSEND;
                   while(change)
                   {
                       change=false;
                       for( i=node->acktosend_tail;i!=node->acktosend_head;i=(i+1)%MAXACKTOSEND)
                           if( cmpack(node->acktosend[i],nextfirstack)<=0 )
                           {
                               if( node->acktosend[i]==nextfirstack )
                               {
                                   node->firstacktosend=nextfirstack++;
                                   if(nextfirstack==0) nextfirstack=1;
                                   change=true;
                               }
                               if( i==node->acktosend_tail )
                               {
                                   node->acktosend[node->acktosend_tail] = 0;
                                   node->acktosend_tail = (i+1)%MAXACKTOSEND;
                               }
                               else
                                   if( i==hm1 )
                                   {
                                       node->acktosend[hm1] = 0;
                                       node->acktosend_head = hm1;
                                       hm1=(hm1-1+MAXACKTOSEND)%MAXACKTOSEND; 
                                   }
                                   
                           }
                   }
               }
               else
               { // out of order packet
                 // don't increment firsacktosend, put it in asktosend queue
                   // will be incremented when the nextfirstack come (code above)
                   byte newhead=(node->acktosend_head+1)%MAXACKTOSEND;
                   DEBFILE(va("out of order packet (%d expected)\n",nextfirstack));
                   if(newhead != node->acktosend_tail)
                   {
                       node->acktosend[node->acktosend_head]=ack;
                       node->acktosend_head = newhead;
                   }
                   else // buffer full discard packet, sender will resend it
                       // remark that we can admit the packet but we will not detect the duplication after :(
                   {
                       DEBFILE("no more freeackret\n");
                       goodpacket=false;
                   }
               }
           }
       }
   }
   return goodpacket;
}

// send special packet with only ack on it
extern void Net_SendAcks(int node)
{
    netbuffer->packettype = PT_NOTHING;
    memcpy(netbuffer->u.textcmd,nodes[node].acktosend,MAXACKTOSEND);
    HSendPacket(node,false,0,MAXACKTOSEND);
}

static void GotAcks(void)
{
    int i,j;

    for(j=0;j<MAXACKTOSEND;j++)
        if( netbuffer->u.textcmd[j] )
           for(i=0;i<MAXACKPACKETS;i++)
               if( ackpak[i].acknum && 
                   ackpak[i].destinationnode==doomcom->remotenode)
               {
                   if( ackpak[i].acknum==netbuffer->u.textcmd[j])
                       Removeack(i);
                   else
                   // nextacknum is first equal to acknum, then when receiving bigger ack
                   // there is big chance the packet is lost
                   // when resent, nextacknum=nodes[node].nextacknum this will redo the same but with differant value
                   if( cmpack(ackpak[i].nextacknum,netbuffer->u.textcmd[j])<=0 && ackpak[i].senttime>0)
                       ackpak[i].senttime--; // hurry up
               }
}

void Net_ConnectionTimeout( int node )
{
    // send a very special packet to self (hack the reboundstore queu)
    // main code will handle it
    reboundstore[rebound_head].packettype = PT_NODETIMEOUT;
    reboundstore[rebound_head].ack = 0;
    reboundstore[rebound_head].ackreturn = 0;
    reboundstore[rebound_head].u.textcmd[0] = node;
    reboundsize[rebound_head]=BASEPACKETSIZE+1;
    rebound_head=(rebound_head+1)%MAXREBOUND;

    // do not redo it quickly (if we do not close connection is for a good reason !)
    nodes[node].lasttimepacketreceived = I_GetTime();
}

// resend the data if needed
extern void Net_AckTicker(void)
{
    int i;

    for(i=0;i<MAXACKPACKETS;i++)
    {
        node_t *node=&nodes[ackpak[i].destinationnode];
        if(ackpak[i].acknum)
            if(ackpak[i].senttime+node->timeout<I_GetTime())
            {
                if( ackpak[i].resentnum > 10 && (node->flags & CLOSE) )
                {
                    DEBFILE(va("ack %d sent 20 time so connection is supposed lost : node %d\n",i,node-nodes));
                    Net_CloseConnection( (node-nodes) | FORCECLOSE);

                    ackpak[i].acknum = 0;
                    continue;
                }
                DEBFILE(va("Resend ack %d, %d<%d\n",ackpak[i].acknum,
                           ackpak[i].senttime,
                           node->timeout,
                           I_GetTime()));
                memcpy(netbuffer,ackpak[i].pak,ackpak[i].length);
                ackpak[i].senttime=I_GetTime();
                ackpak[i].resentnum++;
                ackpak[i].nextacknum=node->nextacknum;
                retransmit++; // for stat
                HSendPacket(node-nodes,false,ackpak[i].acknum,ackpak[i].length-BASEPACKETSIZE);
            }
    }

    for(i=1;i<MAXNETNODES;i++)
    {
        // this is something like node open flag
        if( nodes[i].firstacktosend )
        {
            // we haven't sent a packet since long time acknoledge packet if needed
            if( nodes[i].lasttimeacktosend_sent + ACKTOSENDTIMEOUT < I_GetTime() )
                Net_SendAcks(i);

            if( (nodes[i].flags & CLOSE) == 0 && 
                nodes[i].lasttimepacketreceived + CONNECTIONTIMEOUT < I_GetTime() )
                Net_ConnectionTimeout( i );
        }
    }
}

// remove last packet received ack before the resend the ackret
// (the higer layer don't have room, or something else ....)
extern void Net_UnAcknowledgPacket(int node)
{
    int hm1=(nodes[node].acktosend_head-1+MAXACKTOSEND)%MAXACKTOSEND;
    DEBFILE(va("UnAcknowledg node %d\n",node));
    if(!node)
        return;
    if( nodes[node].acktosend[hm1] == netbuffer->ack )
    {
        nodes[node].acktosend[hm1] = 0;
        nodes[node].acktosend_head = hm1;
    }
    else
    if( nodes[node].firstacktosend == netbuffer->ack )
    {
        nodes[node].firstacktosend--;
        if( nodes[node].firstacktosend==0 )
            nodes[node].firstacktosend--;
    }
    else
    {
        while (nodes[node].firstacktosend!=netbuffer->ack)
        {
            nodes[node].acktosend_tail = (nodes[node].acktosend_tail-1+MAXACKTOSEND)%MAXACKTOSEND;
            nodes[node].acktosend[nodes[node].acktosend_tail]=nodes[node].firstacktosend;

            nodes[node].firstacktosend--;
            if( nodes[node].firstacktosend==0 ) nodes[node].firstacktosend--;
        }
        nodes[node].firstacktosend++;
        if( nodes[node].firstacktosend==0 ) nodes[node].firstacktosend++;
    }
//    I_Error("can't Removing ackret\n");

}

extern boolean Net_AllAckReceived(void)
{
   int i;

   for(i=0;i<MAXACKPACKETS;i++)
      if(ackpak[i].acknum)
          return false;

   return true;
}

// wait the all ackreturn with timout in second
extern void Net_WaitAllAckReceived( ULONG timeout )
{
    tic_t tictac=I_GetTime();
    timeout=tictac+timeout*TICRATE;

    HGetPacket();
    while(timeout>I_GetTime() && !Net_AllAckReceived())
    {
        while(tictac==I_GetTime()) ;
        tictac=I_GetTime();
        HGetPacket();
        Net_AckTicker();
    }
}

static void InitNode( int node )
{
    nodes[node].acktosend_head  = 0;
    nodes[node].acktosend_tail  = 0;
    nodes[node].ping            = PINGDEFAULT;
    nodes[node].varping         = VARPINGDEFAULT;
    nodes[node].timeout         = TIMEOUT(nodes[node].ping,nodes[node].varping);
    nodes[node].firstacktosend  = 0;
    nodes[node].nextacknum      = 1;
    nodes[node].remotefirstack  = 0;
    nodes[node].flags           = 0;
}

static void InitAck()
{
   int i;

   for(i=0;i<MAXACKPACKETS;i++)
      ackpak[i].acknum=0;

   for(i=0;i<MAXNETNODES;i++)
       InitNode( i );
}

extern void Net_AbortPacketType(char packettype)
{
    int i;
    for( i=0;i<MAXACKPACKETS;i++ )
         if( ackpak[i].acknum && 
             (((doomdata_t *)ackpak[i].pak)->packettype==packettype || packettype==-1 ))
             ackpak[i].acknum=0;
}

// -----------------------------------------------------------------
//  end of acknowledge function
// -----------------------------------------------------------------


// remove a node, clear all ack from this node and reset askret
extern void Net_CloseConnection(int node)
{
    int i;
    boolean forceclose = (node & FORCECLOSE)!=0;
    node &= ~FORCECLOSE;

    if( !node )
        return;

    nodes[node].flags |= CLOSE;

    // try to Send ack back (two army problem)
    if( GetAcktosend(node) )
    {
        Net_SendAcks( node );
        Net_SendAcks( node );
    }
    // check if we wait ack from this node
    for(i=0;i<MAXACKPACKETS;i++)
        if( ackpak[i].acknum && ackpak[i].destinationnode==node)
        {
            if( !forceclose )
                return;     // connection will be closed when ack is returned
            else
                ackpak[i].acknum = 0;
        }

    InitNode(node);
    AbortSendFiles(node);
    I_NetFreeNodenum(node);
}

//
// Checksum
//
static unsigned NetbufferChecksum (void)
{
    unsigned    c;
    int         i,l;
    unsigned char   *buf;

    c = 0x1234567;

    l = doomcom->datalength - 4;
    buf = (unsigned char*)netbuffer+4;
    for (i=0 ; i<l ; i++,buf++)
        c += (*buf) * (i+1);

    return LONG(c);
}

#ifdef DEBUGFILE

static void fprintfstring(char *s,byte len)
{
    int i;
    int mode=0;

    for (i=0 ; i<len ; i++)
       if(s[i]<32)
           if(mode==0) {
               fprintf (debugfile,"[%d",(byte)s[i]);
               mode = 1;
           } else
               fprintf (debugfile,",%d",(byte)s[i]);
       else
       {
           if(mode==1) {
              fprintf (debugfile,"]");
              mode=0;
           }
           fprintf (debugfile,"%c",s[i]);
       }
    if(mode==1) fprintf (debugfile,"]");
    fprintf(debugfile,"\n");
}

static char *packettypename[NUMPACKETTYPE]={
    "NOTHING",
    "SERVERCFG",
    "CLIENTCMD",
    "CLIENTMIS",
    "CLIENT2CMD",
    "CLIENT2MIS",
    "NODEKEEPALIVE",
    "NODEKEEPALIVEMIS",
    "SERVERTICS",
    "SERVERREFUSE",
    "SERVERSHUTDOWN",
    "CLIENTQUIT",

    "ASKINFO",
    "SERVERINFO",
    "REQUESTFILE",

    "FILEFRAGMENT",
    "TEXTCMD",
    "TEXTCMD2",
    "CLIENTJOIN",
    "NODETIMEOUT",

};

static void DebugPrintpacket(char *header)
{
    fprintf (debugfile,"%-12s (node %d,ack %d,ackret %d,size %d) type(%d) : %s\n"
                      ,header
                      ,doomcom->remotenode
                      ,netbuffer->ack
                      ,netbuffer->ackreturn
                      ,doomcom->datalength
                      ,netbuffer->packettype,packettypename[netbuffer->packettype]);

    switch(netbuffer->packettype)
    {
       case PT_ASKINFO:
           fprintf(debugfile
                  ,"    time %u\n"
                  ,(unsigned int)netbuffer->u.askinfo.time);
           break;
       case PT_CLIENTJOIN:
           fprintf(debugfile
                  ,"    number %d mode %d\n"
                  ,netbuffer->u.clientcfg.localplayers
                  ,netbuffer->u.clientcfg.mode);
           break;
       case PT_SERVERTICS:
           fprintf(debugfile
                  ,"    firsttic %d ply %d tics %d ntxtcmd %d\n    "
                  ,ExpandTics (netbuffer->u.serverpak.starttic)
                  ,netbuffer->u.serverpak.numplayers
                  ,netbuffer->u.serverpak.numtics
                  ,(int)(&((char *)netbuffer)[doomcom->datalength] - (char *)&netbuffer->u.serverpak.cmds[netbuffer->u.serverpak.numplayers*netbuffer->u.serverpak.numtics]));
           fprintfstring(                                            (char *)&netbuffer->u.serverpak.cmds[netbuffer->u.serverpak.numplayers*netbuffer->u.serverpak.numtics]
                        ,&((char *)netbuffer)[doomcom->datalength] - (char *)&netbuffer->u.serverpak.cmds[netbuffer->u.serverpak.numplayers*netbuffer->u.serverpak.numtics]);
           break;
       case PT_CLIENTCMD:
       case PT_CLIENT2CMD:
       case PT_CLIENTMIS:
       case PT_CLIENT2MIS:
       case PT_NODEKEEPALIVE:
       case PT_NODEKEEPALIVEMIS:
           fprintf(debugfile
                  ,"    tic %4d resendfrom %d localtic %d\n"
                  ,ExpandTics (netbuffer->u.clientpak.client_tic)
                  ,ExpandTics (netbuffer->u.clientpak.resendfrom)
                  ,0 /*netbuffer->u.clientpak.cmd.localtic*/);
           break;
       case PT_TEXTCMD:
       case PT_TEXTCMD2:
           fprintf(debugfile
                  ,"    length %d\n    "
                  ,*(unsigned char*)netbuffer->u.textcmd);
           fprintfstring(netbuffer->u.textcmd+1,netbuffer->u.textcmd[0]);
           break;
       case PT_SERVERCFG:
           fprintf(debugfile
                  ,"    playermask %x players %d clientnode %d serverplayer %d gametic %li gamestate %d\n"
                  ,(unsigned int)netbuffer->u.servercfg.playerdetected
                  ,netbuffer->u.servercfg.totalplayernum
                  ,netbuffer->u.servercfg.clientnode
                  ,netbuffer->u.servercfg.serverplayer
                  ,netbuffer->u.servercfg.gametic
                  ,netbuffer->u.servercfg.gamestate);
           break;
       case PT_SERVERINFO :
           fprintf(debugfile
                  ,"    '%s' player %i/%i, map %s, filenum %d, time %u \n"
                  ,netbuffer->u.serverinfo.servername
                  ,netbuffer->u.serverinfo.numberofplayer
                  ,netbuffer->u.serverinfo.maxplayer
                  ,netbuffer->u.serverinfo.mapname
                  ,netbuffer->u.serverinfo.fileneedednum
                  ,(unsigned int)netbuffer->u.serverinfo.time);
           fprintfstring(netbuffer->u.serverinfo.fileneeded,(char *)netbuffer+doomcom->datalength-(char *)netbuffer->u.serverinfo.fileneeded);
           break;
       case PT_SERVERREFUSE :
           fprintf(debugfile
                  ,"    reason %s\n"
                  ,netbuffer->u.serverrefuse.reason);
           break;
       case PT_FILEFRAGMENT :
           fprintf(debugfile
                  ,"    fileid %d datasize %d position %li\n"
                  ,netbuffer->u.filetxpak.fileid
                  ,netbuffer->u.filetxpak.size
                  ,netbuffer->u.filetxpak.position);
           break;
       case PT_REQUESTFILE :
       default : // write as a raw packet
           fprintfstring(netbuffer->u.textcmd,(char *)netbuffer+doomcom->datalength-(char *)netbuffer->u.textcmd);
           break;

    }
}
#endif

//
// HSendPacket
//
extern boolean HSendPacket(int   node,boolean reliable ,byte acknum,int packetlength)
{

    doomcom->datalength = packetlength+BASEPACKETSIZE;
    if (!node)
    {
        if((rebound_head+1)%MAXREBOUND==rebound_tail)
        {
#ifdef PARANOIA
            CONS_Printf("No more rebound buf\n");
#endif
            return false;
        }
        memcpy(&reboundstore[rebound_head],netbuffer,doomcom->datalength);
        reboundsize[rebound_head]=doomcom->datalength;
        rebound_head=(rebound_head+1)%MAXREBOUND;
#ifdef DEBUGFILE
        if (debugfile)
        {
            doomcom->remotenode = node;
            DebugPrintpacket("SENDLOCAL");
        }
#endif
        return true;
    }

//    if (demoplayback)
//        return true;

    if (!netgame)
        I_Error ("Tried to transmit to another node");

    // do this before GetFreeAcknum because this function
    // backup the current paket
    doomcom->remotenode = node;
    if(doomcom->datalength<=0)
    {
        DEBFILE("HSendPacket : nothing to send\n");
#ifdef DEBUGFILE
        if (debugfile)
            DebugPrintpacket("TRISEND");
#endif
        return false;
    }

    if(node<MAXNETNODES) // can be a broadcast
        netbuffer->ackreturn=GetAcktosend(node);
    else
        netbuffer->ackreturn=0;
    if(reliable)
    {
        if( I_NetCanSend && !I_NetCanSend() )
        {
            if( netbuffer->packettype < PT_CANFAIL )
                GetFreeAcknum(&netbuffer->ack,true) ;

            DEBFILE("HSendPacket : Out of bandwidth\n");
            return false;
        }
        else
            if( !GetFreeAcknum(&netbuffer->ack,false) )
                return false;
    }
    else
        netbuffer->ack=acknum;

    netbuffer->checksum=NetbufferChecksum ();
    sendbytes+=(packetheaderlength+doomcom->datalength); // for stat

    // simulate internet :)
    if( true || rand()<RAND_MAX/5 )
    {
#ifdef DEBUGFILE
        if (debugfile)
            DebugPrintpacket("SEND");
#endif
        I_NetSend();
    }
#ifdef DEBUGFILE
    else
        if (debugfile)
            DebugPrintpacket("NOTSEND");
#endif
    return true;
}

//
// HGetPacket
// Returns false if no packet is waiting
// Check Datalength and checksum
//
extern boolean HGetPacket (void)
{
    // get a packet from my
    if (rebound_tail!=rebound_head)
    {
        memcpy(netbuffer,&reboundstore[rebound_tail],reboundsize[rebound_tail]);
        doomcom->datalength = reboundsize[rebound_tail];
        if( netbuffer->packettype == PT_NODETIMEOUT )
            doomcom->remotenode = netbuffer->u.textcmd[0];
        else
            doomcom->remotenode = 0;

        rebound_tail=(rebound_tail+1)%MAXREBOUND;
#ifdef DEBUGFILE
        if (debugfile)
           DebugPrintpacket("GETLOCAL");
#endif
        return true;
    }

    if (!netgame)
        return false;

    I_NetGet();

    if (doomcom->remotenode == -1)
        return false;

    getbytes+=(packetheaderlength+doomcom->datalength); // for stat

    if (doomcom->remotenode >= MAXNETNODES)
    {
        DEBFILE(va("receive packet from node %d !\n", doomcom->remotenode));
        return false;
    }

    nodes[doomcom->remotenode].lasttimepacketreceived = I_GetTime();

    if (netbuffer->checksum != NetbufferChecksum ())
    {
        DEBFILE("Bad packet checksum\n");
        return false;
    }

#ifdef DEBUGFILE
    if (debugfile)
        DebugPrintpacket("GET");
#endif

    // proceed the ack and ackreturn field
    if(!Processackpak())
        return false;    // discated (duplicated)

    // a packet with just ackreturn
    if( netbuffer->packettype == PT_NOTHING)
    {
        GotAcks();
        return false;
    }

    return true;
}

void Internal_Get(void)
{
    doomcom->remotenode = -1;
    // I_Error("Get without netgame\n");
}

void Internal_Send(void)
{
     I_Error("Send without netgame\n");
}

void Internal_FreeNodenum(int nodenum)
{
}

//
// D_CheckNetGame
// Works out player numbers among the net participants
//
extern boolean D_CheckNetGame (void)
{
    boolean ret=false;

    InitAck();
    rebound_tail=0;
    rebound_head=0;

    statstarttic=I_GetTime();

    I_NetGet           = Internal_Get;
    I_NetSend          = Internal_Send;
    I_NetCanSend       = NULL;
    I_NetCloseSocket   = NULL;
    I_NetFreeNodenum   = Internal_FreeNodenum;
    I_NetMakeNode      = NULL;

    hardware_MAXPACKETLENGTH = MAXPACKETLENGTH;
    net_bandwidth = 3000;
    // I_InitNetwork sets doomcom and netgame
    // check and initialize the network driver
    multiplayer = false;

    // only dos version with external driver will return true
    netgame = I_InitNetwork ();
    if( !netgame )
    {
        doomcom=Z_Malloc(sizeof(doomcom_t),PU_STATIC,NULL);
        memset(doomcom,0,sizeof(doomcom_t));
        doomcom->id = DOOMCOM_ID;        
        doomcom->numplayers = doomcom->numnodes = 1;
        doomcom->deathmatch = false;
        doomcom->consoleplayer = 0;
        doomcom->extratics = 0;

        netgame = I_InitTcpNetwork();
    }
    if( netgame )
        ret = true;
    if( !server && netgame )
        netgame = false;
    server = true; // WTF? server always true???
                   // BP: no ! The deault mode is server. Client is set elsewhere
                   //     when the client execute connect command, i think
    doomcom->ticdup = 1;
    
    if (M_CheckParm ("-extratic"))
    {
        if( M_IsNextParm() )
            doomcom->extratics = atoi(M_GetNextParm());
        else
            doomcom->extratics = 1;
        CONS_Printf("Set extratic to %d\n",doomcom->extratics);
    }

    if(M_CheckParm ("-bandwidth"))
    {
        if(M_IsNextParm())
        {
            net_bandwidth = atoi(M_GetNextParm());
            if( net_bandwidth<1000 ) 
                net_bandwidth=1000;
            if( net_bandwidth>100000 )  
                hardware_MAXPACKETLENGTH = MAXPACKETLENGTH;
            CONS_Printf("Network bandwidth set to %d\n",net_bandwidth);
        }
        else
            I_Error("usage : -bandwidth <byte_per_sec>");
    }

    software_MAXPACKETLENGTH=hardware_MAXPACKETLENGTH;
    if(M_CheckParm ("-packetsize"))
    {
        int p=atoi(M_GetNextParm());
        if(p<75)
           p=75;
        if(p>hardware_MAXPACKETLENGTH)
           p=hardware_MAXPACKETLENGTH;
        software_MAXPACKETLENGTH=p;
    }

    if( netgame )
        multiplayer = true;

    if (doomcom->id != DOOMCOM_ID)
        I_Error ("Doomcom buffer invalid!");
    if (doomcom->numnodes>MAXNETNODES)
        I_Error ("To many nodes (%d), max:%d",doomcom->numnodes,MAXNETNODES);

    netbuffer = (doomdata_t *)&doomcom->data;

#ifdef DEBUGFILE
    if (M_CheckParm ("-debugfile"))
    {
        char    filename[20];
        int     k=doomcom->consoleplayer-1;
        if( M_IsNextParm() )
            k = atoi(M_GetNextParm())-1;
        while (!debugfile && k<MAXPLAYERS)
        {
            k++;
            sprintf (filename,"debug%i.txt",k);
            debugfile = fopen (filename,"w");
        }
        if( debugfile )
            CONS_Printf ("debug output to: %s\n",filename);
        else
            CONS_Printf ("\2cannot debug output to file !\n",filename);
    }
#endif

    D_ClientServerInit();

    return ret;
}


extern void D_CloseConnection( void )
{
    int i;

    if( netgame )
    {
        // wait the ackreturn with timout of 5 Sec
        Net_WaitAllAckReceived(5);

        // close all connection
        for( i=0;i<MAXNETNODES;i++ )
            Net_CloseConnection(i | FORCECLOSE);

        InitAck();

        if( I_NetCloseSocket )
            I_NetCloseSocket();
        
        I_NetGet           = Internal_Get;
        I_NetSend          = Internal_Send;
        I_NetCanSend       = NULL;
        I_NetCloseSocket   = NULL;
        I_NetFreeNodenum   = Internal_FreeNodenum;
        I_NetMakeNode      = NULL;
        netgame = false;
    }
}
