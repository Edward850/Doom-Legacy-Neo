// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: d_clisrv.c,v 1.45 2003/11/22 00:22:08 darkwolf95 Exp $
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
// $Log: d_clisrv.c,v $
// Revision 1.45  2003/11/22 00:22:08  darkwolf95
// get rid of FS hud pics on level exit and new game, also added exl's fix for clearing hub variables on new game
//
// Revision 1.44  2003/05/04 04:30:30  sburke
// Ensure that big-endian machines encode/decode network packets as little-endian.
//
// Revision 1.43  2003/03/22 22:35:59  hurdler
// Fix CR+LF issue
//
// Revision 1.42  2002/09/27 16:40:08  tonyd
// First commit of acbot
//
// Revision 1.41  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.40  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.39  2001/05/16 17:12:52  crashrl
// Added md5-sum support, removed recursiv wad search
//
// Revision 1.38  2001/05/14 19:02:57  metzgermeister
//   * Fixed floor not moving up with player on E3M1
//   * Fixed crash due to oversized string in screen message ... bad bug!
//   * Corrected some typos
//   * fixed sound bug in SDL
//
// Revision 1.37  2001/04/27 13:32:13  bpereira
// no message
//
// Revision 1.36  2001/04/01 17:35:06  bpereira
// no message
//
// Revision 1.35  2001/03/30 17:12:49  bpereira
// no message
//
// Revision 1.34  2001/03/03 06:17:33  bpereira
// no message
//
// Revision 1.33  2001/02/24 13:35:19  bpereira
// no message
//
// Revision 1.32  2001/02/10 12:27:13  bpereira
// no message
//
// Revision 1.31  2001/01/25 22:15:41  bpereira
// added heretic support
//
// Revision 1.30  2000/11/11 13:59:45  bpereira
// no message
//
// Revision 1.29  2000/11/02 17:50:06  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.28  2000/10/22 00:20:53  hurdler
// Updated for the latest master server code
//
// Revision 1.27  2000/10/21 23:21:56  hurdler
// Minor updates
//
// Revision 1.26  2000/10/21 08:43:28  bpereira
// no message
//
// Revision 1.25  2000/10/17 10:09:27  hurdler
// Update master server code for easy connect from menu
//
// Revision 1.24  2000/10/16 20:02:28  bpereira
// no message
//
// Revision 1.23  2000/10/08 13:29:59  bpereira
// no message
//
// Revision 1.22  2000/10/01 10:18:16  bpereira
// no message
//
// Revision 1.21  2000/09/28 20:57:14  bpereira
// no message
//
// Revision 1.20  2000/09/15 19:49:21  bpereira
// no message
//
// Revision 1.19  2000/09/10 10:37:28  metzgermeister
// *** empty log message ***
//
// Revision 1.18  2000/09/01 19:34:37  bpereira
// no message
//
// Revision 1.17  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.16  2000/08/21 11:06:43  hurdler
// Add ping and some fixes
//
// Revision 1.15  2000/08/16 15:44:18  hurdler
// update master server code
//
// Revision 1.14  2000/08/16 14:10:01  hurdler
// add master server code
//
// Revision 1.13  2000/08/11 19:10:13  metzgermeister
// *** empty log message ***
//
// Revision 1.12  2000/08/11 12:25:23  hurdler
// latest changes for v1.30
//
// Revision 1.11  2000/08/03 17:57:41  bpereira
// no message
//
// Revision 1.10  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.9  2000/04/24 20:24:38  bpereira
// no message
//
// Revision 1.8  2000/04/16 18:38:06  bpereira
// no message
//
// Revision 1.7  2000/04/04 00:32:45  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.6  2000/03/29 19:39:48  bpereira
// no message
//
// Revision 1.5  2000/03/08 17:02:42  hurdler
// fix the joiningame problem under Linux
//
// Revision 1.4  2000/03/06 16:51:08  hurdler
// hack for OpenGL / Open Entry problem
//
// Revision 1.3  2000/02/27 16:30:28  hurdler
// dead player bug fix + add allowmlook <yes|no>
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      DOOM Network game communication and protocol,
//      all OS independend parts.
//
//-----------------------------------------------------------------------------


#include <time.h>
#if defined (__DJGPP__) || defined (LINUX)
#include <unistd.h>
#endif

#include "doomdef.h"
#include "command.h"
#include "i_net.h"
#include "i_system.h"
#include "i_video.h"
#include "d_net.h"
#include "d_netcmd.h"
#include "d_main.h"
#include "g_game.h"
#include "hu_stuff.h"
#include "keys.h"
#include "doomstat.h"
#include "m_argv.h"
#include "m_menu.h"
#include "console.h"
#include "d_netfil.h"
#include "byteptr.h"

#include "p_saveg.h"
#include "p_setup.h"
#include "z_zone.h"
#include "p_tick.h"
#include "p_local.h"
#include "m_misc.h"
#include "am_map.h"
#include "m_random.h"
#include "d_clisrv.h"
#include "mserv.h"
#include "t_vari.h"

#include "b_game.h"	//added by AC for acbot

#include "crtfunc.h"

//
// NETWORKING
//
// gametic is the tic about to (or currently being) run
// maketic is the tic that hasn't had control made for it yet
// server:
//   nettics is the tic for eatch node
//   firstticstosend is the lowest value of nettics
// client:
//   neededtic is the tic needed by the client for run the game
//   firstticstosend is used to optimize a condition
// normaly maketic>=gametic>0,

#define PREDICTIONQUEUE         BACKUPTICS
#define PREDICTIONMASK          (PREDICTIONQUEUE-1)

boolean         server = true;           // true or false but !server=client
boolean         serverrunning = false;
char            serverplayer;

// server specific vars
static byte     playernode[MAXPLAYERS];
static tic_t    cl_maketic[MAXNETNODES];
static char     nodetoplayer[MAXNETNODES];
static char     nodetoplayer2[MAXNETNODES]; // say the numplayer for this node if any (splitscreen)
static byte     playerpernode[MAXNETNODES]; // used specialy for scplitscreen
static boolean  nodeingame[MAXNETNODES];  // set false as nodes leave game
static tic_t    nettics[MAXNETNODES];     // what tic the client have received
static tic_t    supposedtics[MAXNETNODES];// nettics prevision for smaller packet
static byte     nodewaiting[MAXNETNODES];
static tic_t    firstticstosend;          // min of the nettics
static short    consistancy[BACKUPTICS];
static tic_t    tictoclear=0;             // optimize d_clearticcmd
static tic_t    maketic;
#ifdef CLIENTPREDICTION2
tic_t localgametic;
#endif

// client specific
static ticcmd_t localcmds;
static ticcmd_t localcmds2;
static boolean  cl_packetmissed;
boolean         drone;
// here it is for the secondary local player (splitscreen)
static byte     mynode;        // my address pointofview server


static byte     localtextcmd[MAXTEXTCMD];
static byte     localtextcmd2[MAXTEXTCMD]; // splitscreen
static tic_t    neededtic;
char            servernode;       // the number of the server node

// engine
ticcmd_t        netcmds[BACKUPTICS][MAXPLAYERS];
static byte     textcmds[BACKUPTICS][MAXPLAYERS][MAXTEXTCMD];

consvar_t cv_playdemospeed  = {"playdemospeed","0",0,CV_Unsigned};


// some software don't support largest packet
// (original sersetup, not exactely, but the probabylity of sending a packet
// of 512 octet is like 0.1)
USHORT software_MAXPACKETLENGTH;

int ExpandTics (int low)
{
    int delta;

    delta = low - (maketic&0xff);

    if (delta >= -64 && delta <= 64)
        return (maketic&~0xff) + low;
    if (delta > 64)
        return (maketic&~0xff) - 256 + low;
    if (delta < -64)
        return (maketic&~0xff) + 256 + low;
#ifdef PARANOIA
    I_Error ("ExpandTics: strange value %i at maketic %i",low,maketic);
#endif
    return 0;
}

// -----------------------------------------------------------------
//  Some extra data function for handle textcmd buffer
// -----------------------------------------------------------------

static void (*listnetxcmd[MAXNETXCMD])(char **p,int playernum);

void RegisterNetXCmd(netxcmd_t id,void (*cmd_f) (char **p,int playernum))
{
#ifdef PARANOIA
   if(id>=MAXNETXCMD)
      I_Error("command id %d too big",id);
   if(listnetxcmd[id]!=0)
      I_Error("Command id %d already used",id);
#endif
   listnetxcmd[id]=cmd_f;
}

void SendNetXCmd(byte id,void *param,int nparam)
{
   if(demoplayback)
       return;

   if(localtextcmd[0]+1+nparam>MAXTEXTCMD)
   {
#ifdef PARANOIA
       I_Error("No more place in the buffer for netcmd %d\n",id);
#else
       CONS_Printf("\2Net Command fail\n");
#endif
       return;
   }
   localtextcmd[0]++;
   localtextcmd[localtextcmd[0]]=id;
   if(param && nparam)
   {
       memcpy(&localtextcmd[localtextcmd[0]+1],param,nparam);
       localtextcmd[0]+=nparam;
   }
}

// splitscreen player
void SendNetXCmd2(byte id,void *param,int nparam)
{
   if(demoplayback)
       return;

   if(localtextcmd2[0]+1+nparam>MAXTEXTCMD)
   {
#ifdef PARANOIA
       I_Error("No more place in the buffer for netcmd %d\n",id);
#else
       CONS_Printf("\2Net Command fail\n");
#endif
       return;
   }
   localtextcmd2[0]++;
   localtextcmd2[localtextcmd2[0]]=id;
   if(param && nparam)
   {
       memcpy(&localtextcmd2[localtextcmd2[0]+1],param,nparam);
       localtextcmd2[0]+=nparam;
   }
}


static void ExtraDataTicker(void)
{
    int  i,tic;
    byte *curpos,*bufferend;

    tic=gametic % BACKUPTICS;

    for(i=0;i<MAXPLAYERS;i++)
        if((playeringame[i]) || (i==0))
        {
            curpos=(byte *)&(textcmds[tic][i]);
            bufferend=&curpos[curpos[0]+1];
            curpos++;
            while(curpos<bufferend)
            {
                if(*curpos < MAXNETXCMD && listnetxcmd[*curpos])
                {
                    byte id=*curpos;
                    curpos++;
                    DEBFILE(va("executing x_cmd %d ply %d ",id,i));
                    (listnetxcmd[id])((char **)&curpos,i);
                    DEBFILE("done\n");
                }
                else
                    I_Error("Got unknown net command [%d]=%d (max %d)\n"
                           ,curpos-(byte *)&(textcmds[tic][i])
                           ,*curpos,textcmds[tic][i][0]);
            }
        }
}

static void D_Clearticcmd(int tic)
{
    int i;

    for(i=0;i<MAXPLAYERS;i++)
    {
        textcmds[tic%BACKUPTICS][i][0]=0;
        netcmds[tic%BACKUPTICS][i].angleturn = 0; //&= ~TICCMD_RECEIVED;
    }
    DEBFILE(va("clear tic %5d (%2d)\n",tic,tic%BACKUPTICS));
}

// -----------------------------------------------------------------
//  end of extra data function
// -----------------------------------------------------------------

// -----------------------------------------------------------------
//  extra data function for lmps
// -----------------------------------------------------------------

// desciption of extradate byte of LEGACY 1.12 not the same of the 1.20
// 1.20 don't have the extradata bits fields but a byte for eatch command
// see XD_xxx in d_netcmd.h
//
// if extradatabit is set, after the ziped tic you find this :
//
//   type   |  description
// ---------+--------------
//   byte   | size of the extradata
//   byte   | the extradata (xd) bits : see XD_...
//            with this byte you know what parameter folow
// if(xd & XDNAMEANDCOLOR)
//   byte   | color
//   char[MAXPLAYERNAME] | name of the player
// endif
// if(xd & XD_WEAPON_PREF)
//   byte   | original weapon switch : boolean, true if use the old
//          | weapon switch methode
//   char[NUMWEAPONS] | the weapon switch priority
//   byte   | autoaim : true if use the old autoaim system
// endif
boolean AddLmpExtradata(byte **demo_point,int playernum)
{
    int  tic;

    tic=gametic % BACKUPTICS;
    if(textcmds[tic][playernum][0]==0)
        return false;

    memcpy(*demo_point,textcmds[tic][playernum],textcmds[tic][playernum][0]+1);
    *demo_point += textcmds[tic][playernum][0]+1;
    return true;
}

void ReadLmpExtraData(byte **demo_pointer,int playernum)
{
    unsigned char nextra,ex;

    if(!demo_pointer)
    {
        textcmds[gametic%BACKUPTICS][playernum][0]=0;
        return;
    }
    nextra=**demo_pointer;
    if(demoversion==112) // support old demos v1.12
    {
        int    size=0;
        char   *p=*demo_pointer+1; // skip nextra

        textcmds[gametic%BACKUPTICS][playernum][0]=0;

        ex=*p++;
        if(ex & 1)
        {
            size=textcmds[gametic%BACKUPTICS][playernum][0];
            textcmds[gametic%BACKUPTICS][playernum][size+1]=XD_NAMEANDCOLOR;
            memcpy(&textcmds[gametic%BACKUPTICS][playernum][size+2],
                   p,
                   MAXPLAYERNAME+1);
            p+=MAXPLAYERNAME+1;
            textcmds[gametic%BACKUPTICS][playernum][0]+=MAXPLAYERNAME+1+1;
        }
        if(ex & 2)
        {
            size=textcmds[gametic%BACKUPTICS][playernum][0];
            textcmds[gametic%BACKUPTICS][playernum][size+1]=XD_WEAPONPREF;
            memcpy(&textcmds[gametic%BACKUPTICS][playernum][size+2],
                   p,
                   NUMWEAPONS+2);
            p+=NUMWEAPONS+2;
            textcmds[gametic%BACKUPTICS][playernum][0]+=NUMWEAPONS+2+1;
        }
        nextra--;
    }
    else
        memcpy(textcmds[gametic%BACKUPTICS][playernum],*demo_pointer,nextra+1);
    // increment demo pointer
    *demo_pointer +=nextra+1;
}

// -----------------------------------------------------------------
//  end extra data function for lmps
// -----------------------------------------------------------------

static short Consistancy(void);

typedef enum {
   cl_searching,
   cl_downloadfiles,
   cl_askjoin,
   cl_waitjoinresponce,
   cl_downloadsavegame,
   cl_connected
} cl_mode_t;

static void GetPackets(void);
void SV_ResetServer( void );

cl_mode_t cl_mode=cl_searching;

//
// SendClJoin
//
// send a special packet for declare how many player in local
// used only in arbitratrenetstart()
static boolean CL_SendJoin()
{
    CONS_Printf("Send join request...\n");
    netbuffer->packettype=PT_CLIENTJOIN;

    if (drone)
        netbuffer->u.clientcfg.localplayers=0;
    else
    if (cv_splitscreen.value)
        netbuffer->u.clientcfg.localplayers=2;
    else
        netbuffer->u.clientcfg.localplayers=1;
    netbuffer->u.clientcfg.version = VERSION;
    netbuffer->u.clientcfg.subversion = LONG(SUBVERSION);

    return HSendPacket(servernode,true,0,sizeof(clientconfig_pak));
}


static void SV_SendServerInfo(int node, tic_t time)
{
    byte  *p;

    netbuffer->packettype=PT_SERVERINFO;
    netbuffer->u.serverinfo.version = VERSION;
    netbuffer->u.serverinfo.subversion = LONG(SUBVERSION);
    // return back the time value so client can compute there ping
    netbuffer->u.serverinfo.time = LONG(time);
    netbuffer->u.serverinfo.numberofplayer = doomcom->numplayers;
    netbuffer->u.serverinfo.maxplayer = cv_maxplayers.value;
    netbuffer->u.serverinfo.load = 0;        // unused for the moment
    netbuffer->u.serverinfo.deathmatch = cv_deathmatch.value;
    strncpy(netbuffer->u.serverinfo.servername, cv_servername.string, MAXSERVERNAME);
    if(gamemapname[0])
        strcpy(netbuffer->u.serverinfo.mapname,gamemapname);
    else
        strcpy(netbuffer->u.serverinfo.mapname,G_BuildMapName(gameepisode,gamemap));

    p=PutFileNeeded();

    HSendPacket(node,false,0, p-((byte *)&netbuffer->u));
}


static boolean SV_SendServerConfig(int node)
{
    int   i,playermask=0;
    byte  *p;

    netbuffer->packettype=PT_SERVERCFG;
    for(i=0;i<MAXPLAYERS;i++)
         if(playeringame[i])
              playermask|=1<<i;

    netbuffer->u.servercfg.version         = VERSION;
    netbuffer->u.servercfg.subversion      = LONG(SUBVERSION);

    netbuffer->u.servercfg.serverplayer    = serverplayer;
    netbuffer->u.servercfg.totalplayernum  = doomcom->numplayers;
    netbuffer->u.servercfg.playerdetected  = LONG(playermask);
    netbuffer->u.servercfg.gametic         = LONG(gametic);
    netbuffer->u.servercfg.clientnode      = node;
    netbuffer->u.servercfg.gamestate       = gamestate;
    p = netbuffer->u.servercfg.netcvarstates;
    CV_SaveNetVars((char**)&p);

    return HSendPacket(node,true,0,sizeof(serverconfig_pak)+(p-netbuffer->u.servercfg.netcvarstates));
}

#define JOININGAME
#ifdef JOININGAME
#define SAVEGAMESIZE    (128*1024)

static void SV_SendSaveGame(int node)
{
    int length;
    byte*   savebuffer;

    // first save it in a malloced buffer
    save_p = savebuffer = (byte *)malloc(SAVEGAMESIZE);
    if(!save_p)
    {
        CONS_Printf ("No More free memory for savegame\n");
        return;
    }

    P_SaveGame();

    length = save_p - savebuffer;
    if (length > SAVEGAMESIZE)
        I_Error ("Savegame buffer overrun");

    // then send it !
    SendRam(node, savebuffer, length, SF_RAM, 0);
}

static const char *tmpsave="$$$.sav";

static void CL_LoadReceivedSavegame(void)
{
    byte*   savebuffer;
    int length = FIL_ReadFile(tmpsave,&savebuffer);


    CONS_Printf("loading savegame length %d\n",length);
    if (!length)
    {
        I_Error ("Can't read savegame sent");
        return;
    }

    G_Downgrade (VERSION);
    save_p = savebuffer;

    paused        = false;
    demoplayback  = false;
    automapactive = false;

    // load a base level
    playerdeadview = false;

    if( !P_LoadGame() )
    {
        CONS_Printf("Can't load the level !!!\n");
        return;
    }

    // done
    Z_Free (savebuffer);
    unlink(tmpsave);
    consistancy[gametic%BACKUPTICS]=Consistancy();
    CON_ToggleOff ();
}


#endif

void SendAskInfo( int node )
{
    netbuffer->packettype = PT_ASKINFO;
    netbuffer->u.askinfo.version = VERSION;
    netbuffer->u.askinfo.time = LONG(I_GetTime());
    HSendPacket(node,false,0,sizeof(askinfo_pak));
}

serverelem_t serverlist[MAXSERVERLIST];
int serverlistcount=0;

void SL_ClearServerList( int connectedserver )
{
    int i;
    for( i=0; i<serverlistcount; i++ )
        if( connectedserver != serverlist[i].node )
        {
            Net_CloseConnection(serverlist[i].node);
            serverlist[i].node = 0;
        }
    serverlistcount = 0;
}

int SL_SearchServer( int node )
{
    int i;
    for( i=0; i<serverlistcount; i++ )
        if( serverlist[i].node == node )
            return i;

    return -1;
}

void SL_InsertServer( serverinfo_pak *info, int node)
{
    int i;
    boolean moved;

    // search if not allready on it
    i = SL_SearchServer( node );
    if( i==-1 )
    {
        // not found add it
        if( serverlistcount >= MAXSERVERLIST )
            return; // list full
        i=serverlistcount++;
    }

    serverlist[i].info = *info;
    serverlist[i].node = node;

    // list is sorted by time (ping)
    // so move the entry until it is sorted
    do {
        moved = false;
        if( i>0 && serverlist[i].info.time < serverlist[i-1].info.time )
        {
            serverelem_t s;
            s = serverlist[i];
            serverlist[i] =  serverlist[i-1];
            serverlist[i-1] = s;
            i--;
            moved = true;
        }
        else
        if( i<serverlistcount-1 && serverlist[i].info.time > serverlist[i+1].info.time )
        {
            serverelem_t s;
            s = serverlist[i];
            serverlist[i] =  serverlist[i+1];
            serverlist[i+1] = s;
            i++;
            moved = true;
        }
    } while(moved);
}

void CL_UpdateServerList( boolean internetsearch )
{
    SL_ClearServerList(0);

    if( !netgame )
    {
        I_NetOpenSocket();
        netgame = true;
        multiplayer = true;
    }
    // search for local servers
    SendAskInfo( BROADCASTADDR );

    if( internetsearch )
    {
        msg_server_t *server_list;
        int          i;

        if( (server_list = GetShortServersList()) )
        {
            for (i=0; server_list[i].header[0]; i++)
            {
                int  node;
                char addr_str[24];

                // insert ip (and optionaly port) in node list
                sprintf(addr_str, "%s:%s", server_list[i].ip, server_list[i].port);
                node = I_NetMakeNode(addr_str);
                if( node == -1 )
                    break; // no more node free
                SendAskInfo( node );
            }
        }
    }
}

// use addaptive send using net_bandwidth and stat.sendbytes
static void CL_ConnectToServer()
{
    int     numnodes,nodewaited=doomcom->numnodes,i;
    boolean waitmore;
    tic_t   asksent;
    tic_t   oldtic;

    cl_mode=cl_searching;

    CONS_Printf("Press ESC to abort\n");
    if( servernode<0 || servernode>=MAXNETNODES )
        CONS_Printf("Searching the server...\n");
    else
        CONS_Printf("Contacting the server...\n");
    DEBFILE(va("waiting %d nodes\n",doomcom->numnodes));
    gamestate = wipegamestate = GS_WAITINGPLAYERS;

    numnodes=1;
    oldtic=I_GetTime()-1;
    asksent=-TICRATE/2;
    SL_ClearServerList(servernode);
    do {
        switch(cl_mode) {
            case cl_searching :
                // serverlist is updated by GetPacket function
                if( serverlistcount>0 )
                {
                    // this can be a responce to our broadcast request
                    if( servernode==-1 || servernode>=MAXNETNODES)
                    {
                        i = 0;
                        servernode = serverlist[i].node;
                        CONS_Printf("Found, ");
                    }
                    else
                    {
                        i=SL_SearchServer(servernode);
                        if (i<0)
                            break; // the case
                    }
                    D_ParseFileneeded(serverlist[i].info.fileneedednum,
                                      serverlist[i].info.fileneeded    );
                    CONS_Printf("Checking files...\n");
                    i = CL_CheckFiles();
                    if( i==2 ) // cannot join for some reason
                    {
                        CL_Reset();
                        D_StartTitle();
                        return;
                    }
                    else if( i==1 )
                        cl_mode=cl_askjoin;
                    else
                    {   // must download something
                        // no problem if can't send packet, we will retry later
                        if( SendRequestFile() )
                            cl_mode=cl_downloadfiles;
                    }
                    break;
                }
                // ask the info to the server (askinfo packet)
                if(asksent+TICRATE/2<I_GetTime())
                {
                    SendAskInfo(servernode);
                    asksent=I_GetTime();
                }
                break;
            case cl_downloadfiles :
                waitmore=false;
                for(i=0;i<fileneedednum;i++)
                    if(fileneeded[i].status==FS_DOWNLOADING || fileneeded[i].status==FS_REQUESTED)
                    {
                        waitmore=true;
                        break;
                    }
                if(waitmore==false)
                    cl_mode=cl_askjoin; //don't break case continue to cljoin request now
                else
                    break; // exit the case
            case cl_askjoin :
                CL_LoadServerFiles();
#ifdef JOININGAME
                // prepare structures to save the file
                // WARNING: this can be useless in case of server not in GS_LEVEL
                // but since the network layer don't provide ordered packet ...
                CL_PrepareDownloadSaveGame(tmpsave);
#endif
                if( CL_SendJoin() )
                    cl_mode=cl_waitjoinresponce;
                break;
#ifdef JOININGAME
            case cl_downloadsavegame :
                if( fileneeded[0].status==FS_FOUND )
                {
                    CL_LoadReceivedSavegame();
                    gamestate = GS_LEVEL;
                    cl_mode=cl_connected;
                }           //don't break case continue to cl_connected
                else
                    break;
#endif
            case cl_waitjoinresponce :
            case cl_connected :
                break;
        }

        GetPackets();
        // connection closed by cancel or timeout
        if( !server && !netgame )
        {
            cl_mode = cl_searching;
            return;
        }
        Net_AckTicker();

        // call it only one by tic
        if( oldtic!=I_GetTime() )
        {
            int key;

            I_OsPolling();
            key = I_GetKey();
            if (key==KEY_ESCAPE)
            {
                M_StartMessage ("Network game synchronization aborted.\n\nPress ESC\n", NULL, MM_NOTHING);
                CL_Reset();
                D_StartTitle();
                return;
            }
            if( key=='s' && server) 
                doomcom->numnodes=numnodes;

            FiletxTicker();
            oldtic=I_GetTime();

            CON_Drawer ();
            I_FinishUpdate ();              // page flip or blit buffer
        }

        if(server)
        {
            numnodes=0;
            for(i=0;i<MAXNETNODES;i++)
                if(nodeingame[i]) numnodes++;

        }
    }  while (!( (cl_mode == cl_connected) &&
                 ( (!server) || (server && (nodewaited<=numnodes)) )));

    DEBFILE(va("Synchronisation Finished\n"));

    consoleplayer&= ~DRONE;
    displayplayer = consoleplayer;
}

void Command_connect(void)
{
    if( COM_Argc()<2 )
    {
        CONS_Printf ("connect <serveraddress> : connect to a server\n"
                     "connect ANY : connect to the first lan server found\n"
                     "connect SELF: connect to self server\n");
        return;
    }
    server = false;

    if( I_stricmp(COM_Argv(1),"self")==0 )
    {
        servernode = 0;
        server = true;
        // should be but...
        //SV_SpawnServer();
    }
    else
    {
        // used in menu to connect to a server in the list
        if( netgame && I_stricmp(COM_Argv(1),"node")==0 )
            servernode = atoi(COM_Argv(2));
        else
        if( netgame )
        {
            CONS_Printf("You cannot connect while in netgame\n"
                        "Leave this game first\n");
            return;
        }
        else
        {
            I_NetOpenSocket();
            netgame = true;
            multiplayer = true;
        
            if( I_stricmp(COM_Argv(1),"any")==0 )
                servernode = BROADCASTADDR;
            else
            if( I_NetMakeNode )
                servernode = I_NetMakeNode(COM_Argv(1));
            else
            {
                CONS_Printf("There is no server identification with this network driver\n");
                D_CloseConnection();
                return;
            }
        }
    }
    CL_ConnectToServer();
}

static void ResetNode(int node);

static void CL_RemovePlayer(int playernum)
{
    int i;
    if( server && !demoplayback )
    {
        int node = playernode[playernum];
        playerpernode[node]--;
        if( playerpernode[node]<=0 )
        {
            nodeingame[playernode[playernum]] = false;
            Net_CloseConnection(playernode[playernum]);
            ResetNode(node);
        }
    }

    // we should use a reset player but there is not such function
    for(i=0;i<MAXPLAYERS;i++)
    {
        players[i].addfrags += players[i].frags[playernum];
        players[i].frags[playernum] = 0;
        players[playernum].frags[i] = 0;
    }
    players[playernum].addfrags = 0;

    // remove avatar of player
    if( players[playernum].mo )
    {
        players[playernum].mo->player = NULL;
        P_RemoveMobj (players[playernum].mo);
    }
    players[playernum].mo = NULL;
    playeringame[playernum] = false;
    playernode[playernum] = -1;
    while(playeringame[doomcom->numplayers-1]==0 && doomcom->numplayers>1) doomcom->numplayers--;
}

void CL_Reset (void)
{
    if (demorecording)
        G_CheckDemoStatus ();

    // reset client/server code
    DEBFILE(va("\n-=-=-=-=-=-=-= Client reset =-=-=-=-=-=-=-\n\n"));

    if( servernode>0 && servernode<MAXNETNODES)
    {
        nodeingame[(byte)servernode]=false;
        Net_CloseConnection(servernode);
    }
    D_CloseConnection();         // netgame=false
    multiplayer = false;
    servernode=0;
    server=true;
    doomcom->numnodes=1;
    doomcom->numplayers=1;
    SV_StopServer();
    SV_ResetServer();

	T_ClearHubScript();	//DarkWolf95: Originally implemented by Exl
	HU_ClearFSPics();


    // reset game engine
    //D_StartTitle ();
}

void Command_GetPlayerNum(void)
{
    int i;

    for(i=0;i<MAXPLAYERS;i++)
        if(playeringame[i])
        {
            if(serverplayer==i)
                CONS_Printf("\2num:%2d  node:%2d  %s\n",i,playernode[i],player_names[i]);
            else
                CONS_Printf("num:%2d  node:%2d  %s\n",i,playernode[i],player_names[i]);
        }
}

int nametonum(char *name)
{
    int playernum,i;

    if( strcmp(name,"0")==0 )
        return 0;

    playernum=atoi(name);

    if(playernum)
    {
        if(playeringame[playernum])
            return playernum;
        else 
            return -1;
    }

    for(i=0;i<MAXPLAYERS;i++)
        if(playeringame[i] && I_stricmp(player_names[i],name)==0)
            return i;
    
    CONS_Printf("there is no player named\"%s\"\n",name);

    return -1;
}

#define KICK_MSG_GO_AWAY     1
#define KICK_MSG_CON_FAIL    2
#define KICK_MSG_PLAYER_QUIT 3
#define KICK_MSG_TIMEOUT     4

void Command_Kick(void)
{
    char  buf[3];

    if (COM_Argc() != 2)
    {
        CONS_Printf ("kick <playername> or <playernum> : kick a player\n");
        return;
    }

    if(server)
    {
        buf[0]=nametonum(COM_Argv(1));
        if(buf[0]==-1)
           return;
        buf[1]=KICK_MSG_GO_AWAY;
        SendNetXCmd(XD_KICK,&buf,2);
    }
    else
        CONS_Printf("You are not the server\n");

}

void Got_KickCmd(char **p,int playernum)
{
    int pnum=READBYTE(*p);
    int msg =READBYTE(*p);

    CONS_Printf("\2%s ",player_names[pnum]);

    switch(msg)
    {
       case KICK_MSG_GO_AWAY:
               CONS_Printf("has been kicked (Go away)\n");
               break;
       case KICK_MSG_CON_FAIL:
               CONS_Printf("has been kicked (Consistency failure)\n");
               break;
       case KICK_MSG_TIMEOUT:
               CONS_Printf("left the game (Connection timeout)\n");
               break;
       case KICK_MSG_PLAYER_QUIT:
               CONS_Printf("left the game\n");
               break;
    }
    if( pnum==consoleplayer )
    {
        CL_Reset();
        D_StartTitle();
        M_StartMessage("You have been kicked by the server\n\nPress ESC\n",NULL,MM_NOTHING);
    }
    else
        CL_RemovePlayer(pnum);
}

CV_PossibleValue_t maxplayers_cons_t[]={{1,"MIN"},{32,"MAX"},{0,NULL}};

consvar_t cv_allownewplayer = {"sv_allownewplayers","1",0,CV_OnOff};
consvar_t cv_maxplayers     = {"sv_maxplayers","32",CV_NETVAR,maxplayers_cons_t,NULL,32};

void Got_AddPlayer(char **p,int playernum);
void Got_AddBot(char **p,int playernum);	//added by AC for acbot

// called one time at init
void D_ClientServerInit (void)
{
    DEBFILE(va("- - -== Doom LEGACY v%i.%i.%i"VERSIONSTRING" debugfile ==- - -\n",VERSION/100,VERSION%100,SUBVERSION));

    drone = false;

    if(M_CheckParm("-left"))
    {
        drone = true;
        viewangleoffset = ANG90;
    }
    if(M_CheckParm("-right"))
    {
        drone = true;
        viewangleoffset = -ANG90;
    }

    // for dedicated server
    dedicated=M_CheckParm("-dedicated")!=0;
    
    COM_AddCommand("getplayernum",Command_GetPlayerNum);
    COM_AddCommand("kick",Command_Kick);
    COM_AddCommand("connect",Command_connect);

    RegisterNetXCmd(XD_KICK,Got_KickCmd);
    RegisterNetXCmd(XD_ADDPLAYER,Got_AddPlayer);
    RegisterNetXCmd(XD_ADDBOT,Got_AddBot);	//added by AC for acbot
    CV_RegisterVar (&cv_allownewplayer);
    CV_RegisterVar (&cv_maxplayers);

    gametic = 0;
    localgametic = 0;

    // do not send anything before the real begin
    SV_StopServer();

    SV_ResetServer();

    if(dedicated)
	SV_SpawnServer();
}

static void ResetNode(int node)
{
    nodeingame[node] = false;
    nodetoplayer[node]=-1;
    nodetoplayer2[node]=-1;
    nettics[node]=gametic;
    supposedtics[node]=gametic;
    cl_maketic[node]=maketic;
    nodewaiting[node]=0;
    playerpernode[node]=0;
}

void SV_ResetServer( void )
{
    int    i;

    // +1 because this command will be executed in com_executebuffer in
    // tryruntic so gametic will be incremented, anyway maketic > gametic 
    // is not a issue

    maketic=gametic+1;
    neededtic=maketic;
#ifdef CLIENTPREDICTION2
    localgametic = gametic;
#endif
    tictoclear=maketic;

    for (i=0 ; i<MAXNETNODES ; i++)
        ResetNode(i);

    for (i=0 ; i<MAXPLAYERS ; i++)
    {
        playeringame[i]=false;
        playernode[i]=-1;
    }

    mynode=0;
    cl_packetmissed=false;
    viewangleoffset=0;

    if( dedicated )
    {
        nodeingame[0]=true;
        serverplayer=-1;
    }
    else
        serverplayer=consoleplayer;

    if(server)
        servernode=0;

    doomcom->numplayers=0;

    DEBFILE(va("\n-=-=-=-=-=-=-= Server Reset =-=-=-=-=-=-=-\n\n"));
}

//
// D_QuitNetGame
// Called before quitting to leave a net game
// without hanging the other players
//
void D_QuitNetGame (void)
{
    if (!netgame)
        return;

    DEBFILE("===========================================================================\n"
            "                  Quiting Game, closing connection\n"
            "===========================================================================\n");

    // abort send/receive of files
    CloseNetFile();

    if( server )
    {
        int i;

        netbuffer->packettype=PT_SERVERSHUTDOWN;
        for(i=0;i<MAXNETNODES;i++)
            if( nodeingame[i] )
                HSendPacket(i,true,0,0);
        if ( serverrunning && cv_internetserver.value )
             UnregisterServer(); 
    }
    else
    if(servernode>0 && servernode<MAXNETNODES && nodeingame[(byte)servernode]!=0)
    {
        netbuffer->packettype=PT_CLIENTQUIT;
        HSendPacket(servernode,true,0,0);
    }

    D_CloseConnection();

    DEBFILE("===========================================================================\n"
            "                         Log finish\n"
            "===========================================================================\n");
#ifdef DEBUGFILE
    if (debugfile)
    {
        fclose (debugfile);
        debugfile = NULL;
    }
#endif
}

// add a node to the game (player will follow at map change or at savegame....)
void SV_AddNode(int node)
{
    nettics[node]       = gametic;
    supposedtics[node]  = gametic;
    cl_maketic[node]    = maketic;
    // little hack because the server connect to itself and put
    // nodeingame when connected not here
    if(node)
       nodeingame[node]=true;
}

// Xcmd XD_ADDPLAYER
void Got_AddPlayer(char **p,int playernum)
{
    int node=READBYTE(*p);
    int newplayernum=READBYTE(*p);
    boolean splitscreenplayer = newplayernum&0x80;
    static ULONG sendconfigtic=0xffffffff;

    newplayernum&=~0x80;

    playeringame[newplayernum]=true;
    G_AddPlayer(newplayernum);
    if( newplayernum+1>doomcom->numplayers )
        doomcom->numplayers=newplayernum+1;
    CONS_Printf("Player %d is in the game (node %d)\n",newplayernum,node);

    // the server is creating my player
    if(node==mynode)
    {
        playernode[newplayernum]=0;  // for information only
        if(!splitscreenplayer)
        {
            consoleplayer=newplayernum;
            displayplayer=newplayernum;
            secondarydisplayplayer=newplayernum;
            DEBFILE("spawning me\n");
        }
        else
        {
            secondarydisplayplayer=newplayernum;
            DEBFILE("spawning my brother\n");
        }
    }

    // the new player send there config
    // and the old player send there config to the new one
    // WARNING : this can cause a bottleneck in the txtcmd
    //           this can also produse consistancy failure if packet get lost
    //           because everibody know the actualconfig except the joiner
    //    TODO : fixthis

    //  don't send more than once the config par tic (more than one player join)
    if( sendconfigtic!=gametic )
    {
        sendconfigtic=gametic;
        D_SendPlayerConfig();
    }
}

// Xcmd XD_ADDBOT
void Got_AddBot(char **p,int playernum)	//added by AC for acbot
{
    int newplayernum=READBYTE(*p);
	//int node = 0;
	//int i = 0;
	newplayernum&=~0x80;
	playeringame[newplayernum]=true;
	strcpy(player_names[newplayernum], botinfo[newplayernum].name);
	players[newplayernum].skincolor = botinfo[newplayernum].colour;
	G_AddPlayer(newplayernum);
	players[newplayernum].bot = B_CreateBot();
	if( newplayernum+1>doomcom->numplayers )
		doomcom->numplayers=newplayernum+1;

	multiplayer=1;

	CONS_Printf ("Bot %s has entered the game\n", player_names[newplayernum]);
}

boolean SV_AddWaitingPlayers(void)
{
    int  node,n;
    int  newplayer=false;
    byte buf[2],newplayernum;

    newplayernum=0;
    for(node=0;node<MAXNETNODES;node++)
    {
        // splitscreen can allow 2 player in one node
        for(;nodewaiting[node]>0;nodewaiting[node]--)
        {
            newplayer=true;

            // search for a free playernum
            // we can't use playeringame since it is not updated here
            //while(newplayernum<MAXPLAYERS && playeringame[newplayernum])
            //    newplayernum++;
            for ( ;newplayernum<MAXPLAYERS;newplayernum++)
            {
                for(n=0;n<MAXNETNODES;n++)
                    if( nodetoplayer[n]  == newplayernum ||
                        nodetoplayer2[n] == newplayernum )
                        break;
                if( n == MAXNETNODES )
                    break;
            }
            
#ifdef PARANOIA
            // should never happens since we check the playernum before accept 
            // the join
            if(newplayernum==MAXPLAYERS)
                I_Error("SV_AddWaitingPlayers : j=MAXPLAYERS\n");
#endif
            playernode[newplayernum]=node;

            buf[0]=node;
            buf[1]=newplayernum;
            if( playerpernode[node]<1 )
                nodetoplayer[node]  = newplayernum;
            else
            {
                nodetoplayer2[node] = newplayernum;
                buf[1]|=0x80;
            }
            playerpernode[node]++;

            SendNetXCmd(XD_ADDPLAYER,&buf,2);
            if( doomcom->numplayers==0 )
                doomcom->numplayers++;  //we must send the change to other players
            
            DEBFILE(va("Server added player %d node %d\n", newplayernum, node));
            // use the next free slote (we can't put playeringame[j]=true here)            
            newplayernum++; 
        }
    }

    return newplayer;
}

void CL_AddSplitscreenPlayer( void )
{
    if( cl_mode == cl_connected )
        CL_SendJoin();
}

void CL_RemoveSplitscreenPlayer( void )
{
    char  buf[2];
    
    if( cl_mode != cl_connected )
        return;

    buf[0]=secondarydisplayplayer;
    buf[1]=KICK_MSG_PLAYER_QUIT;
    SendNetXCmd(XD_KICK,&buf,2);
}

// is there a game running
boolean Playing( void )
{
    return (server && serverrunning) || (!server && cl_mode==cl_connected);
}

boolean SV_SpawnServer( void )
{
    if( demoplayback )
        G_StopDemo(); // reset engine parameter

    if( serverrunning == false )
    {
        CONS_Printf("Starting Server....\n");
        serverrunning = true;
        SV_ResetServer();
        if( netgame )
        {
            I_NetOpenSocket();
            if( cv_internetserver.value )
                RegisterServer(0, 0);
        }

        // server just connect to itself
        if( !dedicated )
            CL_ConnectToServer();
    }

    return SV_AddWaitingPlayers();
}

void SV_StopServer( void )
{
    int i;

    gamestate = wipegamestate = GS_NULL;

    localtextcmd[0]=0;
    localtextcmd2[0]=0;

    for(i=0;i<BACKUPTICS;i++)
        D_Clearticcmd(i);

    consoleplayer=0;
    cl_mode = cl_searching;
    maketic=gametic+1;
    neededtic=maketic;
    serverrunning = false;
}

// called at singleplayer start and stopdemo
void SV_StartSinglePlayerServer(void)
{
    server        = true;
    netgame       = false;
    multiplayer   = false;

    // no more tic the game with this settings !
    SV_StopServer();

    if( cv_splitscreen.value )
        multiplayer    = true;
}

static void SV_SendRefuse(int node,char *reason)
{
    strcpy(netbuffer->u.serverrefuse.reason, reason);

    netbuffer->packettype=PT_SERVERREFUSE;
    HSendPacket(node,true,0,strlen(netbuffer->u.serverrefuse.reason)+1);
    Net_CloseConnection(node);
}

// used at txtcmds received to check packetsize bound
static int TotalTextCmdPerTic(int tic)
{
    int i,total=1; // num of textcmds in the tic (ntextcmd byte)

    tic %= BACKUPTICS;

    for(i=0;i<MAXPLAYERS;i++)
        if( ((i==0) || playeringame[i]) && textcmds[tic][i][0] )
            total += 2 + textcmds[tic][i][0]; // "+2" for size and playernum

    return total;
}

// Copy an array of ticcmd_t, swapping between host and network byte order.
//
static void TicCmdCopy(ticcmd_t * dst, ticcmd_t * src, int n)
{
    int i;
    for (i = 0; i < n; src++, dst++, i++)
    {
#ifdef CLIENTPREDICTION2
	dst->x = readlong(&src->x);
	dst->y = readlong(&src->y);
#endif
	dst->forwardmove = src->forwardmove;
	dst->sidemove    = src->sidemove;
	dst->angleturn   = readshort(&src->angleturn);
	dst->aiming      = readshort(&src->aiming);
	dst->buttons     = src->buttons;
    }
}

//
// GetPackets
//
// TODO : break this 300 line function to mutliple functions
static void GetPackets (void)
{
    int         netconsole;
    tic_t       realend,realstart;
    int         p=maketic%BACKUPTICS;
    ticcmd_t    *pak;
    byte        *txtpak,numtxtpak;
    int         node;

    while ( HGetPacket() )
    {
        node = doomcom->remotenode;
        if( netbuffer->packettype == PT_CLIENTJOIN && server )
        {
            if(         netbuffer->u.clientcfg.version     != VERSION
                || LONG(netbuffer->u.clientcfg.subversion) != SUBVERSION)
                SV_SendRefuse(node,va("Different DOOM versions cannot play a net game! (server version %d.%d.%d)",VERSION/100,VERSION%100,SUBVERSION));
            else
            if(!cv_allownewplayer.value && node!=0 )
                SV_SendRefuse(node,"The server is not accepting people for the moment");
            else
            // TODO; compute it using nodewaiting and playeringame
            if(doomcom->numplayers+1>cv_maxplayers.value)
                SV_SendRefuse(node,va("Maximum of player reached (max:%d)",cv_maxplayers.value));
            else
            {
                boolean newnode=false;
                // client autorised to join
                nodewaiting[node]=netbuffer->u.clientcfg.localplayers-playerpernode[node];
                if(!nodeingame[node])
                {
                    newnode = true;
                    SV_AddNode(node);
                    if(!SV_SendServerConfig(node))
                    {
                        // TODO : fix this !!!
                        CONS_Printf("Internal Error 5 : client lost\n");
                        continue; // the while
                    }
                    DEBFILE("new node joined\n");
                }
#ifdef JOININGAME
                if( nodewaiting[node] )
                {
                    if( gamestate == GS_LEVEL && newnode)
                    {
                        SV_SendSaveGame(node);
                        CONS_Printf("send savegame\n");
                    }
                    SV_AddWaitingPlayers();
                }
#endif
            }
            continue;
        } // if(netbuffer->packettype == PT_CLIENTJOIN)
        if( netbuffer->packettype == PT_SERVERSHUTDOWN && node==servernode && 
            !server && cl_mode != cl_searching)
        {
            M_StartMessage("Server has Shutdown\n\nPress Esc",NULL,MM_NOTHING);
            CL_Reset();
            D_StartTitle();
            continue;
        }
        if( netbuffer->packettype == PT_NODETIMEOUT && node==servernode && 
            !server && cl_mode != cl_searching)
        {
            M_StartMessage("Server Timeout\n\nPress Esc",NULL,MM_NOTHING);
            CL_Reset();
            D_StartTitle();
            continue;
        }

        if( netbuffer->packettype == PT_SERVERINFO )
        {
            // compute ping in ms
            netbuffer->u.serverinfo.time = (I_GetTime() - LONG(netbuffer->u.serverinfo.time))*1000/TICRATE; 
            netbuffer->u.serverinfo.servername[MAXSERVERNAME-1]=0;

            SL_InsertServer( &netbuffer->u.serverinfo, node);
            continue;
        }

        if(!nodeingame[node])
        {
            if( node!=servernode )
                DEBFILE(va("Received packet from unknown host %d\n",node));

            // anyone trying to join !
            switch(netbuffer->packettype) {
                case PT_ASKINFO:
                    if(server && serverrunning)
                    {
                        SV_SendServerInfo(node, LONG(netbuffer->u.askinfo.time));
                        Net_CloseConnection(node);
                    }
                    break;
                case PT_SERVERREFUSE : // negative responce of client join request
                    if( cl_mode==cl_waitjoinresponce )
                    {
                        M_StartMessage(va("Server refuses connection\n\nReason :\n%s"
                                          ,netbuffer->u.serverrefuse.reason), NULL, MM_NOTHING);
                        CL_Reset();
                        D_StartTitle();
                    }
                    break;
                case PT_SERVERCFG :    // positive responce of client join request
                {
                    int j;
                    byte *p;

                    if( cl_mode!=cl_waitjoinresponce )
                        break;

                    if(!server)
                        maketic = gametic = neededtic = LONG(netbuffer->u.servercfg.gametic);
#ifdef CLIENTPREDICTION2
                    localgametic = gametic;
#endif
                    nodeingame[(byte)servernode]=true;
                    serverplayer = netbuffer->u.servercfg.serverplayer;
                    doomcom->numplayers = netbuffer->u.servercfg.totalplayernum;
                    mynode = netbuffer->u.servercfg.clientnode;
                    if (serverplayer>=0)
                        playernode[(byte)serverplayer]=servernode;

                    CONS_Printf("Join accepted, wait next map change...\n");
                    DEBFILE(va("Server accept join gametic=%d mynode=%d\n",gametic,mynode));

                    for(j=0;j<MAXPLAYERS;j++)
                        playeringame[j]=(LONG(netbuffer->u.servercfg.playerdetected) & (1<<j))!=0;

                    p = netbuffer->u.servercfg.netcvarstates;
                    CV_LoadNetVars( (char**)&p );
#ifdef JOININGAME
                    if( netbuffer->u.servercfg.gamestate == GS_LEVEL )
                        cl_mode = cl_downloadsavegame;
                    else
#endif
                        cl_mode = cl_connected;

                    break;
                }
                // handled in d_netfil.c
                case PT_FILEFRAGMENT :
                    if( !server )
                        Got_Filetxpak();
                    break;
                case PT_REQUESTFILE :
                    if( server )
                        Got_RequestFilePak(node);
                    break;
                case PT_NODETIMEOUT:
                case PT_CLIENTQUIT:
                    if( server )
                        Net_CloseConnection(node);
                    break;
                case PT_SERVERTICS:
                    // do not remove my own server (we have just get a out of order packet)
                    if( node == servernode )
                        break;
                default:
                    DEBFILE(va("unknow packet received (%d) from unknow host !\n",netbuffer->packettype));
                    Net_CloseConnection(node);
                    break; // ignore it
            } // switch
            continue; //while
        }
        netconsole = nodetoplayer[node];
#ifdef PARANOIA
        if(!(netconsole & DRONE) && netconsole>=MAXPLAYERS)
            I_Error("bad table nodetoplayer : node %d player %d"
                    ,doomcom->remotenode,netconsole);
#endif


        switch(netbuffer->packettype) {
// -------------------------------------------- SERVER RECEIVE ----------
            case PT_CLIENTCMD  :
            case PT_CLIENT2CMD :
            case PT_CLIENTMIS  :
            case PT_CLIENT2MIS :
            case PT_NODEKEEPALIVE :
            case PT_NODEKEEPALIVEMIS :
                if(!server)
                    break;

                // to save bytes, only the low byte of tic numbers are sent
                // Figure out what the rest of the bytes are
                realstart  = ExpandTics (netbuffer->u.clientpak.client_tic);
                realend = ExpandTics (netbuffer->u.clientpak.resendfrom);

                if(  netbuffer->packettype==PT_CLIENTMIS
                  || netbuffer->packettype==PT_CLIENT2MIS
                  || netbuffer->packettype==PT_NODEKEEPALIVEMIS 
                  || supposedtics[node] < realend)
                {
                    supposedtics[node] = realend;
                }
                // discard out of order packet
                if( nettics[node] > realend )
                {
                    DEBFILE(va("out of order ticcmd discarded nettics = %d\n",nettics[node]));
                    break;
                }

                // update the nettics
                nettics[node] = realend;

                // don't do anything for drones just update their nettics
                if((netconsole & DRONE)!=0 || netconsole==-1 || netbuffer->packettype==PT_NODEKEEPALIVE || netbuffer->packettype==PT_NODEKEEPALIVEMIS)
                     break;

                // check consistancy
                if((realstart <= gametic) && (realstart > gametic-BACKUPTICS+1) &&
                   (consistancy[realstart%BACKUPTICS] != SHORT(netbuffer->u.clientpak.consistancy))) 
                {
#if 1
                    char buf[3];

                    buf[0]=netconsole;
                    buf[1]=KICK_MSG_CON_FAIL;
                    SendNetXCmd(XD_KICK,&buf,2);
#else
                    CONS_Printf("\2player %d kicked [%d] consistancy failure\n",netconsole,realstart);
#endif
                    DEBFILE(va("player %d kicked [%d] %d!=%d\n",netconsole,realstart,consistancy[realstart%BACKUPTICS],SHORT(netbuffer->u.clientpak.consistancy)));

                }

                // copy ticcmd
                TicCmdCopy(&netcmds[maketic%BACKUPTICS][netconsole], &netbuffer->u.clientpak.cmd, 1);

                if( netbuffer->packettype==PT_CLIENT2CMD && nodetoplayer2[node]>=0)
                {
                    TicCmdCopy(&netcmds[maketic%BACKUPTICS][(byte)nodetoplayer2[node]], 
                               &netbuffer->u.client2pak.cmd2, 1);
                }

                break;
            case PT_TEXTCMD2 : // splitscreen special
                netconsole=nodetoplayer2[node];
            case PT_TEXTCMD :
                if(!server)
                    break;

                if( netconsole<0 || netconsole>=MAXPLAYERS )
                    Net_UnAcknowledgPacket(node);
                else
                {
                    int j;
                    tic_t tic=maketic;

                    // check if tic that we are making isn't too large else we cannot send it :(
                    // doomcom->numplayers+1 "+1" since doomcom->numplayers can change within this time and sent time
                    j=software_MAXPACKETLENGTH-(netbuffer->u.textcmd[0]+2+BASESERVERTICSSIZE+(doomcom->numplayers+1)*sizeof(ticcmd_t));

                    // search a tic that have enougth space in the ticcmd
                    while((TotalTextCmdPerTic(tic)>j || netbuffer->u.textcmd[0]+textcmds[tic % BACKUPTICS][netconsole][0]>MAXTEXTCMD) && tic<firstticstosend+BACKUPTICS)
                        tic++;

                    if(tic>=firstticstosend+BACKUPTICS)
                    {
                        DEBFILE(va("GetPacket: Textcmd too long (max %d, used %d, mak %d,tosend %d,node %d, player %d)\n",j,TotalTextCmdPerTic(maketic),maketic,firstticstosend,node,netconsole));
                        Net_UnAcknowledgPacket(node);
                        break;
                    }
                    DEBFILE(va("textcmd put in tic %d at position %d (player %d) ftts %d mk %d\n",tic,textcmds[p][netconsole][0]+1,netconsole,firstticstosend,maketic));
                    p=tic % BACKUPTICS;
                    memcpy(&textcmds[p][netconsole][textcmds[p][netconsole][0]+1]
                          ,netbuffer->u.textcmd+1
                          ,netbuffer->u.textcmd[0]);
                    textcmds[p][netconsole][0]+=netbuffer->u.textcmd[0];
                }
                break;
            case PT_NODETIMEOUT:
            case PT_CLIENTQUIT:
                if(!server)
                    break;

                // nodeingame will be put false in the execution of kick command
                // this allow to send some packets to the quiting client to have there ack back
                nodewaiting[node]= 0;
                if(netconsole!=-1 && playeringame[netconsole])
                {
                    char  buf[2];
                    buf[0]=netconsole;
                    if( netbuffer->packettype == PT_NODETIMEOUT )
                        buf[1]=KICK_MSG_TIMEOUT;
                    else
                        buf[1]=KICK_MSG_PLAYER_QUIT;
                    SendNetXCmd(XD_KICK,&buf,2);
                    nodetoplayer[node]=-1;
                    if(nodetoplayer2[node]!=-1 && nodetoplayer2[node]>=0 && playeringame[(byte)nodetoplayer2[node]])
                    {
                        buf[0]=nodetoplayer2[node];
                        SendNetXCmd(XD_KICK,&buf,2);
                        nodetoplayer2[node]=-1;
                    }
                }
                Net_CloseConnection(node);
                nodeingame[node]=false;
                break;
// -------------------------------------------- CLIENT RECEIVE ----------
            case PT_SERVERTICS :
                realstart  = ExpandTics (netbuffer->u.serverpak.starttic);
                realend    = realstart+netbuffer->u.serverpak.numtics;

                txtpak=(char *)&netbuffer->u.serverpak.cmds[netbuffer->u.serverpak.numplayers*netbuffer->u.serverpak.numtics];

                if( realend>gametic+BACKUPTICS )
                    realend=gametic+BACKUPTICS;
                cl_packetmissed=realstart>neededtic;

                if(realstart<=neededtic && realend>neededtic)
                {
                    tic_t i,j;
		    pak=netbuffer->u.serverpak.cmds;

                    for(i=realstart;i<realend;i++)
                    {
                    // clear first
                        D_Clearticcmd(i);

                    // copy the tics
                        TicCmdCopy(netcmds[i%BACKUPTICS], pak, netbuffer->u.serverpak.numplayers);
                        pak+=netbuffer->u.serverpak.numplayers;

                    // copy the textcmds
                        numtxtpak=*txtpak++;
                        for(j=0;j<numtxtpak;j++)
                        {
                            int k=*txtpak++; // playernum

                            memcpy(textcmds[i%BACKUPTICS][k]
                                  ,txtpak
                                  ,txtpak[0]+1);
                            txtpak+=txtpak[0]+1;
                        }
                    }

                    neededtic=realend;
                }
                else
                    DEBFILE(va("frame not in bound : %u\n", neededtic));
                break;
            case PT_SERVERCFG :
                   break;
            case PT_FILEFRAGMENT :
                if( !server )
                    Got_Filetxpak();
                break;
            default:
                DEBFILE(va("UNKNOWN PACKET TYPE RECEIVED %f from host %d\n",netbuffer->packettype,node));
        } // end switch
    } // end while
}

//
// NetUpdate
// Builds ticcmds for console player,
// sends out a packet
//
// no more use random generator, because at very first tic isn't yet synchronized
static short Consistancy(void)
{
    short ret=0;
    int   i;

    DEBFILE(va("TIC %d ",gametic));
    for(i=0;i<MAXPLAYERS;i++)
        if( playeringame[i] && players[i].mo )
        {
            DEBFILE(va("p[%d].x = %f ",i,FIXED_TO_FLOAT(players[i].mo->x)));
            ret+=players[i].mo->x;
        }
    DEBFILE(va("pos = %d, rnd %d\n",ret,P_GetRandIndex()));
    ret+=P_GetRandIndex();

    return ret;
}

// send the client packet to the server
static void CL_SendClientCmd (void)
{
/* oops can do that until i have implemented a real dead reckoning
    static ticcmd_t lastcmdssent;
    static int      lastsenttime=-TICRATE;

    if( memcmp(&localcmds,&lastcmdssent,sizeof(ticcmd_t))!=0 || lastsenttime+TICRATE/3<I_GetTime())
    {
        lastsenttime=I_GetTime();
*/
    int packetsize=0;

    netbuffer->packettype=PT_CLIENTCMD;

    if (cl_packetmissed)
        netbuffer->packettype++;
    netbuffer->u.clientpak.resendfrom = neededtic;
    netbuffer->u.clientpak.client_tic = gametic;

    if(gamestate==GS_WAITINGPLAYERS)
    {
        // send NODEKEEPALIVE packet
        netbuffer->packettype+=4;
        packetsize = sizeof(clientcmd_pak)-sizeof(ticcmd_t)-sizeof(short);
        HSendPacket (servernode,false,0,packetsize);
    }
    else
    if( gamestate!=GS_NULL )
    {
        TicCmdCopy(&netbuffer->u.clientpak.cmd, &localcmds, 1);
        netbuffer->u.clientpak.consistancy = SHORT(consistancy[gametic%BACKUPTICS]);

        // send a special packet with 2 cmd for splitscreen
        if (cv_splitscreen.value)
        {
            netbuffer->packettype+=2;
            TicCmdCopy(&netbuffer->u.client2pak.cmd2, &localcmds2, 1);
            packetsize = sizeof(client2cmd_pak);
        }
        else
            packetsize = sizeof(clientcmd_pak);
        
        HSendPacket (servernode,false,0,packetsize);
    }

    if( cl_mode == cl_connected )
    {
        // send extra data if needed
        if (localtextcmd[0])
        {
            netbuffer->packettype=PT_TEXTCMD;
            memcpy(netbuffer->u.textcmd,localtextcmd,localtextcmd[0]+1);
            // all extra data have been sended
            if( HSendPacket(servernode,true,0,localtextcmd[0]+1) ) // send can fail for some reasons...
                localtextcmd[0]=0;
        }
        
        // send extra data if needed for player 2 (splitscreen)
        if (localtextcmd2[0])
        {
            netbuffer->packettype=PT_TEXTCMD2;
            memcpy(netbuffer->u.textcmd,localtextcmd2,localtextcmd2[0]+1);
            // all extra data have been sended
            if( HSendPacket(servernode,true,0,localtextcmd2[0]+1) ) // send can fail for some reasons...
                localtextcmd2[0]=0;
        }
    }

}


// send the server packet
// send tic from firstticstosend to maketic-1
static void SV_SendTics (void)
{
    tic_t realfirsttic,lasttictosend,i;
    ULONG n;
    int  j,packsize;
    char *bufpos;
    char *ntextcmd;

    // send to all client but not to me
    // for each node create a packet with x tics and send it
    // x is computed using supposedtics[n], max packet size and maketic
    for(n=1;n<MAXNETNODES;n++)
        if( nodeingame[n] )
        {
            lasttictosend=maketic;

            // assert supposedtics[n]>=nettics[n]
            realfirsttic = supposedtics[n];
            if(realfirsttic>=maketic)
            {
                // well we have sent all tics we will so use extrabandwidth
                // to resent packet that are supposed lost (this is necessary since lost 
                // packet detection work when we have received packet with firsttic>neededtic 
                // (getpacket servertics case)
                DEBFILE(va("Nothing to send node %d mak=%u sup=%u net=%u \n", 
                           n, maketic, supposedtics[n], nettics[n]));
                realfirsttic = nettics[n];
                if( realfirsttic >= maketic || (I_GetTime()+n)&3)
                    // all tic are ok 
                    continue;
                DEBFILE(va("Sent %d anyway\n", realfirsttic));
            }
            if( realfirsttic<firstticstosend )
                realfirsttic=firstticstosend;

            // compute the lenght of the packet and cut it if too large
            packsize=BASESERVERTICSSIZE;
            for(i=realfirsttic;i<lasttictosend;i++)
            {
                packsize+=sizeof(ticcmd_t)*doomcom->numplayers;
                packsize+=TotalTextCmdPerTic(i);

                if(packsize>software_MAXPACKETLENGTH)
                {
                    DEBFILE(va("packet too large (%d) at tic %d (should be from %d to %d)\n", 
                               packsize, i, realfirsttic, lasttictosend));
                    lasttictosend=i;

                    // too bad : too much player have send extradata and there is too
                    //           mutch data in one tic. 
                    //           Too avoid it put the data on the next tic. (see getpacket 
                    //           textcmd case) but when numplayer chang the computation can be different
                    if(lasttictosend==realfirsttic)
                    {
                        if( packsize>MAXPACKETLENGTH )
                            I_Error("Too many players:can't send %d data for %d players to node %d\n"
                                    "Well sorry nobody is perfect....\n",
                                    packsize, doomcom->numplayers, n);
                        else
                        {
                            lasttictosend++;  // send it anyway !
                            DEBFILE("sending it anyway\n");
                        }
                    }
                    break;
                }
            }
            
            // Send the tics

            netbuffer->packettype=PT_SERVERTICS;
            netbuffer->u.serverpak.starttic=realfirsttic;
            netbuffer->u.serverpak.numtics=lasttictosend-realfirsttic;
            netbuffer->u.serverpak.numplayers=SHORT(doomcom->numplayers);
            bufpos=(char *)&netbuffer->u.serverpak.cmds;
            
            for(i=realfirsttic;i<lasttictosend;i++)
            {
                TicCmdCopy((ticcmd_t*) bufpos, netcmds[i%BACKUPTICS], doomcom->numplayers);
                bufpos+=doomcom->numplayers*sizeof(ticcmd_t);
            }
            
            // add textcmds
            for(i=realfirsttic;i<lasttictosend;i++)
            {
                ntextcmd=bufpos++;
                *ntextcmd=0;
                for(j=0;j<MAXPLAYERS;j++)
                {
                    int size=textcmds[i%BACKUPTICS][j][0];
                    
                    if(((j==0) || playeringame[j]) && size)
                    {
                        (*ntextcmd)++;
                        *bufpos++ = j;
                        memcpy(bufpos,textcmds[i%BACKUPTICS][j],size+1);
                        bufpos += size+1;
                    }
                }
            }
            packsize = bufpos - (char *)&(netbuffer->u);

            HSendPacket(n,false,0,packsize);
            // when tic are too large, only one tic is sent so don't go backward !
            if( lasttictosend-doomcom->extratics > realfirsttic )
                supposedtics[n] = lasttictosend-doomcom->extratics;
            else
                supposedtics[n] = lasttictosend;
            if( supposedtics[n] < nettics[n] ) supposedtics[n] = nettics[n];
        }
    // node 0 is me !
    supposedtics[0] = maketic;
}

//
// TryRunTics
//
static void Local_Maketic(int realtics)
{
    if(dedicated)
	return;
    
    I_OsPolling();       // i_getevent
    D_ProcessEvents ();  // menu responder ???!!!
                         // Cons responder
                         // game responder call :
                         //    HU_responder,St_responder, Am_responder
                         //    F_responder (final)
                         //    and G_MapEventsToControls

    rendergametic=gametic;
    // translate inputs (keyboard/mouse/joystick) into game controls
    G_BuildTiccmd (&localcmds,realtics);
    if (cv_splitscreen.value)
        G_BuildTiccmd2(&localcmds2,realtics);

#ifdef CLIENTPREDICTION2
    if( !paused && localgametic<gametic+BACKUPTICS)
    {
        P_MoveSpirit ( &players[consoleplayer], &localcmds, realtics );
        localgametic+=realtics;
    }
#endif
    localcmds.angleturn |= TICCMD_RECEIVED;
}

void SV_SpawnPlayer(int playernum, int x, int y, angle_t angle)
{
    // for futur copytic use the good x,y and angle!
    if( server )
    {
#ifdef CLIENTPREDICTION2
        netcmds[maketic%BACKUPTICS][playernum].x=x;
        netcmds[maketic%BACKUPTICS][playernum].y=y;
#endif
        netcmds[maketic%BACKUPTICS][playernum].angleturn=(angle>>16) | TICCMD_RECEIVED;
    }
}

// create missed tic
void SV_Maketic(void)
{
    int j;

    for(j=0;j<MAXNETNODES;j++)
       if(playerpernode[j])
       {
           int player=nodetoplayer[j];
           if((netcmds[maketic%BACKUPTICS][player].angleturn & TICCMD_RECEIVED) == 0)   // we don't received this tic
           {
               int i;

               DEBFILE(va("MISS tic%4u for node %d\n",maketic,j));
#ifdef PARANOIA
               if( devparm )
                   CONS_Printf("\2Client Misstic %d\n",maketic);
#endif
               // copy the old tic
               for(i=0;i<playerpernode[j];i++,player=nodetoplayer2[j])
               {
                   netcmds[maketic%BACKUPTICS][player] = netcmds[(maketic-1)%BACKUPTICS][player];
                   netcmds[maketic%BACKUPTICS][player].angleturn &=~TICCMD_RECEIVED;
               }
           }
       }

    // all tic are now proceed make the next
    maketic++;
}

extern  boolean advancedemo;
static  int     load;

void TryRunTics (tic_t realtics)
{
    // the machine have laged but is not so bad
    if(realtics>TICRATE/7) // FIXME: consistency failure!!
    {
        if(server)
            realtics=1;
        else
            realtics=TICRATE/7;
    }

    if(singletics)
        realtics = 1;

    if( realtics>= 1)
        COM_BufExecute();            

    NetUpdate();

    if(demoplayback)
    {
        neededtic = gametic + realtics + cv_playdemospeed.value;
        // start a game after a demo
        maketic+=realtics;
        firstticstosend=maketic;
        tictoclear=firstticstosend;
    }

#ifdef DEBUGFILE
    if(realtics==0)
        if(load) load--;
#endif
    GetPackets ();

#ifdef DEBUGFILE
    if (debugfile && (realtics || neededtic>gametic))
    {
        //SoM: 3/30/2000: Need long int in the format string for args 4 & 5.
        //Shut up stupid warning!
        fprintf (debugfile,
                 "------------ Tryruntic : REAL:%li NEED:%li GAME:%li LOAD: %i\n",
                 realtics, neededtic, gametic, load);
        load=100000;
    }
#endif

    if (neededtic > gametic)
    {
        if (advancedemo)
            D_DoAdvanceDemo ();
        else
        // run the count * tics
        while (neededtic > gametic)
        {
            DEBFILE(va("============ Runing tic %u (local %d)\n",gametic, localgametic));

            G_Ticker ();
            ExtraDataTicker();
            gametic++;
            // skip paused tic in a demo
            if(demoplayback)
            {   if(paused) neededtic++; }
            else
                consistancy[gametic%BACKUPTICS]=Consistancy();
        }
    }
}

void NetUpdate(void)
{
    static tic_t gametime=0;
    tic_t        nowtime;
    int          i;
    int          realtics;

    nowtime  = I_GetTime ();
    realtics = nowtime - gametime;

    if( realtics <= 0 )   // nothing new to update
        return;
    if( realtics>5 )
    {
        if( server )
            realtics=1;
        else
            realtics=5;
    }

    gametime = nowtime;

    if( !server )
        maketic = neededtic;

    Local_Maketic (realtics);    // make local tic, and call menu ?!
    if( server && !demoplayback && !dedicated)
        CL_SendClientCmd ();     // send it
    GetPackets ();               // get packet from client or from server

    // client send the command after a receive of the server
    // the server send before because in single player is beter

    if( !server )
        CL_SendClientCmd ();   // send tic cmd
    else
    {
        //Hurdler: added for acking the master server
        if( cv_internetserver.value )
            SendPingToMasterServer();

        if(!demoplayback)
        {
            int counts;

            firstticstosend=gametic;
            for(i=0;i<MAXNETNODES;i++)
                if(nodeingame[i] && nettics[i]<firstticstosend)
                    firstticstosend=nettics[i];

            // Don't erace tics not acknowledged
            counts=realtics;

            if( maketic+counts>=firstticstosend+BACKUPTICS )
                counts=firstticstosend+BACKUPTICS-maketic-1;

            for(i=0;i<counts;i++)
                SV_Maketic();       // create missed tics and increment maketic

            for(;tictoclear<firstticstosend;tictoclear++) // clear only when acknoledged
                D_Clearticcmd(tictoclear);                // clear the maketic the new tic

            SV_SendTics();

            neededtic=maketic; // the server is a client too
        }
    }
    Net_AckTicker();
    M_Ticker ();
    CON_Ticker();
    FiletxTicker();
}
