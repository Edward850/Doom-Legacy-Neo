// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hu_stuff.c,v 1.18 2003/11/22 00:22:09 darkwolf95 Exp $
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
// $Log: hu_stuff.c,v $
// Revision 1.18  2003/11/22 00:22:09  darkwolf95
// get rid of FS hud pics on level exit and new game, also added exl's fix for clearing hub variables on new game
//
// Revision 1.17  2003/07/14 12:39:12  darkwolf95
// Made rankings screen smaller for splitscreen.  Ditched the graphical title and limited max rankings to four.
//
// Revision 1.16  2002/07/23 15:07:10  mysterial
// Messages to second player appear on his half of the screen
//
// Revision 1.15  2001/12/15 18:41:35  hurdler
// small commit, mainly splitscreen fix
//
// Revision 1.14  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.13  2001/07/16 22:35:40  bpereira
// - fixed crash of e3m8 in heretic
// - fixed crosshair not drawed bug
//
// Revision 1.12  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.11  2001/04/01 17:35:06  bpereira
// no message
//
// Revision 1.10  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.9  2001/02/19 17:40:34  hurdler
// Fix a bug with "chat on" in hw mode
//
// Revision 1.8  2001/01/25 22:15:42  bpereira
// added heretic support
//
// Revision 1.7  2000/11/04 16:23:43  bpereira
// no message
//
// Revision 1.6  2000/11/02 17:50:06  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.5  2000/09/28 20:57:15  bpereira
// no message
//
// Revision 1.4  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.3  2000/08/03 17:57:42  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      heads up displays, cleaned up (hasta la vista hu_lib)
//      because a lot of code was unnecessary now
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "hu_stuff.h"

#include "d_netcmd.h"
#include "d_clisrv.h"

#include "g_game.h"
#include "g_input.h"

#include "i_video.h"

// Data.
#include "dstrings.h"
#include "st_stuff.h"   //added:05-02-98: ST_HEIGHT
#include "r_local.h"
#include "wi_stuff.h"  // for drawranckings
#include "p_info.h"

#include "keys.h"
#include "v_video.h"

#include "w_wad.h"
#include "z_zone.h"

#include "console.h"
#include "am_map.h"
#include "d_main.h"

#ifdef HWRENDER
#include "hardware/hw_main.h"
#endif


// coords are scaled
#define HU_INPUTX       0
#define HU_INPUTY       0

//-------------------------------------------
//              heads up font
//-------------------------------------------
patch_t*                hu_font[HU_FONTSIZE];


static player_t*        plr;
boolean                 chat_on;

static char             w_chat[HU_MAXMSGLEN];

static boolean          headsupactive = false;

boolean                 hu_showscores;        // draw deathmatch rankings

static char             hu_tick;

//-------------------------------------------
//              misc vars
//-------------------------------------------

consvar_t*   chat_macros[10];

//added:16-02-98: crosshair 0=off, 1=cross, 2=angle, 3=point, see m_menu.c
patch_t*           crosshair[3];     //3 precached crosshair graphics


// -------
// protos.
// -------
void   HU_drawDeathmatchRankings (void);
void   HU_drawCrosshair (void);
static void HU_DrawTip();


//======================================================================
//                 KEYBOARD LAYOUTS FOR ENTERING TEXT
//======================================================================

char*     shiftxform;

char french_shiftxform[] =
{
    0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31,
    ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '?', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    '0', // shift-0
    '1', // shift-1
    '2', // shift-2
    '3', // shift-3
    '4', // shift-4
    '5', // shift-5
    '6', // shift-6
    '7', // shift-7
    '8', // shift-8
    '9', // shift-9
    '/',
    '.', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127

};

char english_shiftxform[] =
{

    0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31,
    ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '<', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    ')', // shift-0
    '!', // shift-1
    '@', // shift-2
    '#', // shift-3
    '$', // shift-4
    '%', // shift-5
    '^', // shift-6
    '&', // shift-7
    '*', // shift-8
    '(', // shift-9
    ':',
    ':', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127
};


char frenchKeyMap[128]=
{
    0,
    1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,
    31,
    ' ','!','"','#','$','%','&','%','(',')','*','+',';','-',':','!',
    '0','1','2','3','4','5','6','7','8','9',':','M','<','=','>','?',
    '@','Q','B','C','D','E','F','G','H','I','J','K','L',',','N','O',
    'P','A','R','S','T','U','V','Z','X','Y','W','^','\\','$','^','_',
    '@','Q','B','C','D','E','F','G','H','I','J','K','L',',','N','O',
    'P','A','R','S','T','U','V','Z','X','Y','W','^','\\','$','^',127
};


char ForeignTranslation(unsigned char ch)
{
    return ch < 128 ? frenchKeyMap[ch] : ch;
}


//======================================================================
//                          HEADS UP INIT
//======================================================================

// just after
void Command_Say_f (void);
void Command_Sayto_f (void);
void Command_Sayteam_f (void);
void Got_Saycmd(char **p,int playernum);

// Initialise Heads up
// once at game startup.
//
void HU_Init(void)
{

    int         i;
    int         j;
    char        buffer[9];

    if(dedicated)
	return;
    
    COM_AddCommand ("say"    , Command_Say_f);
    COM_AddCommand ("sayto"  , Command_Sayto_f);
    COM_AddCommand ("sayteam", Command_Sayteam_f);
    RegisterNetXCmd(XD_SAY,Got_Saycmd);

    // set shift translation table
    if (language==french)
        shiftxform = french_shiftxform;
    else
        shiftxform = english_shiftxform;

    // cache the heads-up font for entire game execution
    j = gamemode == heretic ? 1 : HU_FONTSTART;
    for (i=0;i<HU_FONTSIZE;i++)
    {
        if( raven )
            sprintf(buffer, "FONTA%.2d", j>59 ? 59 : j);
        else
            sprintf(buffer, "STCFN%.3d", j);
        j++;
        hu_font[i] = (patch_t *) W_CachePatchName(buffer, PU_STATIC);
    }

    // cache the crosshairs, dont bother to know which one is being used,
    // just cache them 3 all, they're so small anyway.
    for(i=0;i<HU_CROSSHAIRS;i++)
    {
       sprintf(buffer, "CROSHAI%c", '1'+i);
       crosshair[i] = (patch_t *) W_CachePatchName(buffer, PU_STATIC);
    }
}


void HU_Stop(void)
{
    headsupactive = false;
}

// 
// Reset Heads up when consoleplayer spawns
//
void HU_Start(void)
{
    if (headsupactive)
        HU_Stop();

    plr = &players[consoleplayer];
    chat_on = false;

    headsupactive = true;
}



//======================================================================
//                            EXECUTION
//======================================================================

void TeamPlay_OnChange(void)
{
    int i;
    char s[50];

    // Change the name of the teams

    if(cv_teamplay.value==1)
    {
        // color
        for(i=0;i<MAXSKINCOLORS;i++)
        {
            sprintf(s,"%s team",Color_Names[i]);
            strcpy(team_names[i],s);
        }
    }
    else
    if(cv_teamplay.value==2)
    {
        // skins

        for(i=0;i<numskins;i++)
        {
            sprintf(s,"%s team",skins[i].name);
            strcpy(team_names[i],s);
        }
    }
}

void Command_Say_f (void)
{
    char buf[255];
    int i,j;

    if((j=COM_Argc())<2)
    {
        CONS_Printf ("say <message> : send a message\n");
        return;
    }

    buf[0]=0;
    strcpy(&buf[1],COM_Argv(1));
    for(i=2;i<j;i++)
    {
        strcat(&buf[1]," ");
        strcat(&buf[1],COM_Argv(i));
    }
    SendNetXCmd(XD_SAY,buf,strlen(buf+1)+2); // +2 because 1 for buf[0] and the other for null terminated string
}

void Command_Sayto_f (void)
{
    char buf[255];
    int i,j;

    if((j=COM_Argc())<3)
    {
        CONS_Printf ("sayto <playername|playernum> <message> : send a message to a player\n");
        return;
    }

    buf[0]=nametonum(COM_Argv(1));
    if(buf[0]==-1)
        return;
    strcpy(&buf[1],COM_Argv(2));
    for(i=3;i<j;i++)
    {
        strcat(&buf[1]," ");
        strcat(&buf[1],COM_Argv(i));
    }
    SendNetXCmd(XD_SAY,buf,strlen(buf+1)+2);
}

void Command_Sayteam_f (void)
{
    char buf[255];
    int i,j;

    if((j=COM_Argc())<2)
    {
        CONS_Printf ("sayteam <message> : send a message to your team\n");
        return;
    }

    buf[0]=-consoleplayer;
    strcpy(&buf[1],COM_Argv(1));
    for(i=2;i<j;i++)
    {
        strcat(&buf[1]," ");
        strcat(&buf[1],COM_Argv(i));
    }
    SendNetXCmd(XD_SAY,buf,strlen(buf+1)+2); // +2 because 1 for buf[0] and the other for null terminated string
}

// netsyntax : to : byte  1->32  player 1 to 32
//                        0      all
//                       -1->-32 say team -numplayer of the sender

void Got_Saycmd(char **p,int playernum)
{
    char to;
    to=*(*p)++;

    if(to==0 || to==consoleplayer || consoleplayer==playernum
       || (to<0 && ST_SameTeam(&players[consoleplayer],&players[-to])) )
         CONS_Printf("\3%s: %s\n", player_names[playernum], *p);

    *p+=strlen(*p)+1;
}

//  Handles key input and string input
//
boolean HU_keyInChatString (char *s, char ch)
{
    int         l;

    if (ch >= ' ' && ch <= '_')
    {
        l = strlen(s);
        if (l<HU_MAXMSGLEN-1)
        {
            s[l++]=ch;
            s[l]=0;
            return true;
        }
        return false;
    }
    else
        if (ch == KEY_BACKSPACE)
        {
            l = strlen(s);
            if (l)
                s[--l]=0;
            else
                return false;
        }
        else
            if (ch != KEY_ENTER)
                return false; // did not eat key

    return true; // ate the key
}


//
//
void HU_Ticker(void)
{
    player_t    *pl;

    if(dedicated)
	return;
    
    hu_tick++;
    hu_tick &= 7;        //currently only to blink chat input cursor

    // display message if necessary
    // (display the viewplayer's messages)
    pl = &players[displayplayer];

    if (cv_showmessages.value && pl->message)
    {
        CONS_Printf ("%s\n",pl->message);
        pl->message = 0;
    }

    // In splitscreen, display second player's messages
    if (cv_splitscreen.value)
    {
        pl = &players[secondarydisplayplayer];
        if (cv_showmessages.value && pl->message)
        {
            CONS_Printf ("\4%s\n",pl->message);
            pl->message = 0;
        }
    }
    
    //deathmatch rankings overlay if press key or while in death view
    if (cv_deathmatch.value)
    {
        if (gamekeydown[gamecontrol[gc_scores][0]] ||
            gamekeydown[gamecontrol[gc_scores][1]] )
            hu_showscores = !chat_on;
        else
            hu_showscores = playerdeadview; //hack from P_DeathThink()
    }
    else
        hu_showscores = false;
}




#define QUEUESIZE               128

static char     chatchars[QUEUESIZE];
static int      head = 0;
static int      tail = 0;

//
//
char HU_dequeueChatChar(void)
{
    char c;

    if (head != tail)
    {
        c = chatchars[tail];
        tail = (tail + 1) & (QUEUESIZE-1);
    }
    else
    {
        c = 0;
    }

    return c;
}

//
//
void HU_queueChatChar(char c)
{
    if (((head + 1) & (QUEUESIZE-1)) == tail)
    {
        plr->message = HUSTR_MSGU;      //message not send
    }
    else
    {
        if (c == KEY_BACKSPACE)
        {
            if(tail!=head)
                head = (head - 1) & (QUEUESIZE-1);
        }
        else
        {
            chatchars[head] = c;
            head = (head + 1) & (QUEUESIZE-1);
        }
    }

    // send automaticly the message (no more chat char)
    if(c==KEY_ENTER)
    {
        char buf[255],c;
        int i=0;

        do {
            c=HU_dequeueChatChar();
            buf[i++]=c;
        } while(c);
        if(i>3)
            COM_BufInsertText (va("say %s",buf));
    }
}

extern int     con_keymap;

//
//  Returns true if key eaten
//
boolean HU_Responder (event_t *ev)
{
static boolean        shiftdown = false;
static boolean        altdown   = false;

    boolean             eatkey = false;
    char*               macromessage;
    unsigned char       c;


    if (ev->data1 == KEY_SHIFT)
    {
        shiftdown = (ev->type == ev_keydown);
        return false;
    }
    else if (ev->data1 == KEY_ALT)
    {
        altdown = (ev->type == ev_keydown);
        return false;
    }

    if (ev->type != ev_keydown)
        return false;

   // only KeyDown events now...

    if (!chat_on)
    {
       // enter chat mode
        if (ev->data1==gamecontrol[gc_talkkey][0]
         || ev->data1==gamecontrol[gc_talkkey][1])
        {
            eatkey = chat_on = true;
            w_chat[0] = 0;
            HU_queueChatChar(HU_BROADCAST);
        }
    }
    else
    {
        c = ev->data1;

        // use console translations
        if (con_keymap==french)
            c = ForeignTranslation(c);
        if (shiftdown)
            c = shiftxform[c];

        // send a macro
        if (altdown)
        {
            c = c - '0';
            if (c > 9)
                return false;

            macromessage = chat_macros[c]->string;

            // kill last message with a '\n'
            HU_queueChatChar(KEY_ENTER); // DEBUG!!!

            // send the macro message
            while (*macromessage)
                HU_queueChatChar(*macromessage++);
            HU_queueChatChar(KEY_ENTER);

            // leave chat mode and notify that it was sent
            chat_on = false;
            eatkey = true;
        }
        else
        {
            if (language==french)
                c = ForeignTranslation(c);
            if (shiftdown || (c >= 'a' && c <= 'z'))
                c = shiftxform[c];
            eatkey = HU_keyInChatString(w_chat,c);
            if (eatkey)
            {
                // static unsigned char buf[20]; // DEBUG
                HU_queueChatChar(c);

                // sprintf(buf, "KEY: %d => %d", ev->data1, c);
                //      plr->message = buf;
            }
            if (c == KEY_ENTER)
            {
                chat_on = false;
            }
            else if (c == KEY_ESCAPE)
                chat_on = false;
        }
    }

    return eatkey;
}


//======================================================================
//                         HEADS UP DRAWING
//======================================================================

//  Draw chat input
//
static void HU_DrawChat (void)
{
    int  i,c,y;

    c=0;
    i=0;
    y=HU_INPUTY;
    while (w_chat[i])
    {
        //Hurdler: isn't it better like that?
        V_DrawCharacter( HU_INPUTX + (c<<3), y, w_chat[i++] | 0x80 |V_NOSCALEPATCH|V_NOSCALESTART);

        c++;
        if (c>=(vid.width>>3))
        {
            c = 0;
            y+=8;
        }

    }

    if (hu_tick<4)
        V_DrawCharacter( HU_INPUTX + (c<<3), y, '_' | 0x80 |V_NOSCALEPATCH|V_NOSCALESTART);
}


extern consvar_t cv_chasecam;
//  Heads up displays drawer, call each frame
//
void HU_Drawer(void)
{
    // draw chat string plus cursor
    if (chat_on)
        HU_DrawChat ();

    // draw deathmatch rankings
    if (hu_showscores)
        HU_drawDeathmatchRankings ();

    // draw the crosshair, not when viewing demos nor with chasecam
    if (!automapactive && cv_crosshair.value && !demoplayback && !cv_chasecam.value)
        HU_drawCrosshair ();

    HU_DrawTip();
    HU_DrawFSPics();
}

//======================================================================
//                          PLAYER TIPS
//======================================================================
#define MAXTIPLINES 20
char    *tiplines[MAXTIPLINES];
int     numtiplines = 0;
int     tiptime = 0;
int     largestline = 0;



void HU_SetTip(char *tip, int displaytics)
{
  int    i;
  char   *rover, *ctipline, *ctipline_p;


  for(i = 0; i < numtiplines; i++)
    Z_Free(tiplines[i]);


  numtiplines = 0;

  rover = tip;
  ctipline = ctipline_p = Z_Malloc(128, PU_STATIC, NULL);
  *ctipline = 0;
  largestline = 0;

  while(*rover)
  {
    if(*rover == '\n' || strlen(ctipline) + 2 >= 128 || V_StringWidth(ctipline) + 16 >= BASEVIDWIDTH)
    {
      if(numtiplines > MAXTIPLINES)
        break;
      if(V_StringWidth(ctipline) > largestline)
        largestline = V_StringWidth(ctipline);

      tiplines[numtiplines] = ctipline;
      ctipline = ctipline_p = Z_Malloc(128, PU_STATIC, NULL);
      *ctipline = 0;
      numtiplines ++;
    }
    else
    {
      *ctipline_p = *rover;
      ctipline_p++;
      *ctipline_p = 0;
    }
    rover++;

    if(!*rover)
    {
      if(V_StringWidth(ctipline) > largestline)
        largestline = V_StringWidth(ctipline);
      tiplines[numtiplines] = ctipline;
      numtiplines ++;
    }
  }

  tiptime = displaytics;
}




static void HU_DrawTip()
{
  int    i;
  if(!numtiplines) return;
  if(!tiptime)
  {
    for(i = 0; i < numtiplines; i++)
      Z_Free(tiplines[i]);
    numtiplines = 0;
    return;
  }
  tiptime--;


  for(i = 0; i < numtiplines; i++)
  {
    V_DrawString((BASEVIDWIDTH - largestline) / 2,
                 ((BASEVIDHEIGHT - (numtiplines * 8)) / 2) + ((i + 1) * 8),
                 0,
                 tiplines[i]);
  }
}


void HU_ClearTips()
{
  int    i;

  for(i = 0; i < numtiplines; i++)
    Z_Free(tiplines[i]);
  numtiplines = 0;

  tiptime = 0;
}


//======================================================================
//                           FS HUD Grapics!
//======================================================================
typedef struct
{
  int       lumpnum;
  int       xpos;
  int       ypos;
  patch_t   *data;
  boolean   draw;
} fspic_t;

fspic_t*   piclist = NULL;
int        maxpicsize = 0;


//
// HU_InitFSPics
// This function is called when Doom starts and every time the piclist needs
// to be expanded.
void HU_InitFSPics()
{
  int  newstart, newend, i;

  if(!maxpicsize)
  {
    newstart = 0;
    newend = maxpicsize = 128;
  }
  else
  {
    newstart = maxpicsize;
    newend = maxpicsize = (maxpicsize * 2);
  }

  piclist = realloc(piclist, sizeof(fspic_t) * maxpicsize);
  for(i = newstart; i < newend; i++)
  {
    piclist[i].lumpnum = -1;
    piclist[i].data = NULL;
  }
}


int  HU_GetFSPic(int lumpnum, int xpos, int ypos)
{
  int      i;

  if(!maxpicsize)
    HU_InitFSPics();

  getpic:
  for(i = 0; i < maxpicsize; i++)
  {
    if(piclist[i].lumpnum != -1)
      continue;

    piclist[i].lumpnum = lumpnum;
    piclist[i].xpos = xpos;
    piclist[i].ypos = ypos;
    piclist[i].draw = false;
    return i;
  }

  HU_InitFSPics();
  goto getpic;
}


int   HU_DeleteFSPic(int handle)
{
  if(handle < 0 || handle > maxpicsize)
    return -1;

  piclist[handle].lumpnum = -1;
  piclist[handle].data = NULL;
  return 0;
}


int   HU_ModifyFSPic(int handle, int lumpnum, int xpos, int ypos)
{
  if(handle < 0 || handle > maxpicsize)
    return -1;

  if(piclist[handle].lumpnum == -1)
    return -1;

  piclist[handle].lumpnum = lumpnum;
  piclist[handle].xpos = xpos;
  piclist[handle].ypos = ypos;
  piclist[handle].data = NULL;
  return 0;
}


int   HU_FSDisplay(int handle, boolean newval)
{
  if(handle < 0 || handle > maxpicsize)
    return -1;
  if(piclist[handle].lumpnum == -1)
    return -1;

  piclist[handle].draw = newval;
  return 0;
}


void HU_DrawFSPics()
{
  int       i;

  for(i = 0; i < maxpicsize; i++)
  {
    if(piclist[i].lumpnum == -1 || piclist[i].draw == false)
      continue;
    if(piclist[i].xpos >= vid.width || piclist[i].ypos >= vid.height)
      continue;

    if(!piclist[i].data)
      piclist[i].data = (patch_t *) W_CachePatchNum(piclist[i].lumpnum, PU_STATIC);

    if((piclist[i].xpos + piclist[i].data->width) < 0 || (piclist[i].ypos + piclist[i].data->height) < 0)
      continue;

    V_DrawScaledPatch(piclist[i].xpos, piclist[i].ypos, 0, piclist[i].data);
  }
}

void HU_ClearFSPics()
{
	piclist = NULL;
	maxpicsize = 0;

	HU_InitFSPics();
}

//======================================================================
//                 HUD MESSAGES CLEARING FROM SCREEN
//======================================================================

//  Clear old messages from the borders around the view window
//  (only for reduced view, refresh the borders when needed)
//
//  startline  : y coord to start clear,
//  clearlines : how many lines to clear.
//
static int     oldclearlines;

void HU_Erase (void)
{
    int topline;
    int bottomline;
    int y,yoffset;

    //faB: clear hud msgs on double buffer (Glide mode)
    boolean secondframe;
    static  int     secondframelines;

    if (con_clearlines==oldclearlines && !con_hudupdate && !chat_on)
        return;

    // clear the other frame in double-buffer modes
    secondframe = (con_clearlines!=oldclearlines);
    if (secondframe)
        secondframelines = oldclearlines;

    // clear the message lines that go away, so use _oldclearlines_
    bottomline = oldclearlines;
    oldclearlines = con_clearlines;
    if( chat_on )
        if( bottomline < 8 )
            bottomline=8;

    if (automapactive || viewwindowx==0)   // hud msgs don't need to be cleared
        return;

    // software mode copies view border pattern & beveled edges from the backbuffer
    if (rendermode==render_soft)
    {
        topline = 0;
        for (y=topline,yoffset=y*vid.width; y<bottomline ; y++,yoffset+=vid.width)
        {
            if (y < viewwindowy || y >= viewwindowy + viewheight)
                R_VideoErase(yoffset, vid.width); // erase entire line
            else
            {
                R_VideoErase(yoffset, viewwindowx); // erase left border
                // erase right border
                R_VideoErase(yoffset + viewwindowx + viewwidth, viewwindowx);
            }
        }
        con_hudupdate = false;      // if it was set..
    }
#ifdef HWRENDER 
    else {
        // refresh just what is needed from the view borders
        HWR_DrawViewBorder (secondframelines);
        con_hudupdate = secondframe;
    }
#endif
}



//======================================================================
//                   IN-LEVEL DEATHMATCH RANKINGS
//======================================================================

// count frags for each team
int HU_CreateTeamFragTbl(fragsort_t *fragtab,int dmtotals[],int fragtbl[MAXPLAYERS][MAXPLAYERS])
{
    int i,j,k,scorelines,team;

    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            if(cv_teamplay.value==1)
                team=players[i].skincolor;
            else
                team=players[i].skin;

            for(j=0;j<scorelines;j++)
                if (fragtab[j].num == team)
                { // found there team
                     if(fragtbl)
                     {
                         for(k=0;k<MAXPLAYERS;k++)
                             if(playeringame[k])
                             {
                                 if(cv_teamplay.value==1)
                                     fragtbl[team][players[k].skincolor] +=
                                                        players[i].frags[k];
                                 else
                                     fragtbl[team][players[k].skin] +=
                                                        players[i].frags[k];
                     }
                     }

                     fragtab[j].count += ST_PlayerFrags(i);
                     if(dmtotals)
                         dmtotals[team]=fragtab[j].count;
                     break;
                }
            if (j==scorelines)
            {   // team not found add it

                if(fragtbl)
                    for(k=0;k<MAXPLAYERS;k++)
                        fragtbl[team][k] = 0;

                fragtab[scorelines].count = ST_PlayerFrags(i);
                fragtab[scorelines].num   = team;
                fragtab[scorelines].color = players[i].skincolor;
                fragtab[scorelines].name  = team_names[team];

                if(fragtbl)
                {
                    for(k=0;k<MAXPLAYERS;k++)
                        if(playeringame[k])
                        {
                            if(cv_teamplay.value==1)
                                fragtbl[team][players[k].skincolor] +=
                                                   players[i].frags[k];
                            else
                                fragtbl[team][players[k].skin] +=
                                                   players[i].frags[k];
                }
                }

                if(dmtotals)
                    dmtotals[team]=fragtab[scorelines].count;

                scorelines++;
            }
        }
    }
    return scorelines;
}


//
//  draw Deathmatch Rankings
//
void HU_drawDeathmatchRankings (void)
{
    patch_t*     p;
    fragsort_t   fragtab[MAXPLAYERS];
    int          i;
    int          scorelines;
    int          whiteplayer;
	int          y;
	char*		 title;
	boolean		 large;

    // draw the ranking title panel
	if(!cv_splitscreen.value)
	{
		p = W_CachePatchName("RANKINGS",PU_CACHE);
		V_DrawScaledPatch ((BASEVIDWIDTH-p->width)/2,5,0,p);
	}

    // count frags for each present player
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            fragtab[scorelines].count = ST_PlayerFrags(i);
            fragtab[scorelines].num   = i;
            fragtab[scorelines].color = players[i].skincolor;
            fragtab[scorelines].name  = player_names[i];
            scorelines++;
        }
    }

    //Fab:25-04-98: when you play, you quickly see your frags because your
    //  name is displayed white, when playback demo, you quicly see who's the
    //  view.
    whiteplayer = demoplayback ? displayplayer : consoleplayer;

    if (scorelines>9)
        scorelines = 9; //dont draw past bottom of screen, show the best only
	else if (cv_splitscreen.value && scorelines > 4)
		scorelines = 4;

	if(cv_splitscreen.value)
	{
		y = (100 - (12 * (scorelines + 1) / 2)) + 15;
		title = "Rankings";
		large = false;
	}
	else
	{
		y = 70;
		title = NULL;
		large = true;
	}

    if(cv_teamplay.value==0)
        WI_drawRancking(title,80, y,fragtab,scorelines, large,whiteplayer);
    else
    {
        // draw the frag to the right
//        WI_drawRancking("Individual",170,70,fragtab,scorelines,true,whiteplayer);

        scorelines = HU_CreateTeamFragTbl(fragtab,NULL,NULL);

        // and the team frag to the left
        WI_drawRancking("Teams",80, y,fragtab,scorelines,large,players[whiteplayer].skincolor);
    }
}


// draw the Crosshair, at the exact center of the view.
//
// Crosshairs are pre-cached at HU_Init
#ifdef HWRENDER // not win32 only 19990829 by Kin
    extern float gr_basewindowcentery;
    extern float gr_viewheight;
#endif
void HU_drawCrosshair (void)
{
    int     i;
    int     y;

    i = cv_crosshair.value & 3;
    if (!i)
        return;

#ifdef HWRENDER
    if ( rendermode != render_soft ) 
        y = gr_basewindowcentery;
    else
#endif
        y = viewwindowy+(viewheight>>1);

/*	if (cv_crosshairscale.value)
		V_DrawTranslucentPatch (vid.width>>1, y, 0, crosshair[i-1]);
	else*/
		V_DrawTranslucentPatch (vid.width>>1, y, V_NOSCALESTART, crosshair[i-1]);

    if( cv_splitscreen.value )
    {
#ifdef HWRENDER
        if ( rendermode != render_soft )
            y += gr_viewheight;
        else
#endif
            y += viewheight;

/*		if (cv_crosshairscale.value)
			V_DrawTranslucentPatch (vid.width>>1, y, 0, crosshair[i-1]);
		else*/
			V_DrawTranslucentPatch (vid.width>>1, y, V_NOSCALESTART, crosshair[i-1]);
    }
}


//======================================================================
//                    CHAT MACROS COMMAND & VARS
//======================================================================

// better do HackChatmacros() because the strings are NULL !!

consvar_t cv_chatmacro1 = {"_chatmacro1", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro2 = {"_chatmacro2", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro3 = {"_chatmacro3", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro4 = {"_chatmacro4", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro5 = {"_chatmacro5", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro6 = {"_chatmacro6", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro7 = {"_chatmacro7", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro8 = {"_chatmacro8", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro9 = {"_chatmacro9", NULL, CV_SAVE,NULL};
consvar_t cv_chatmacro0 = {"_chatmacro0", NULL, CV_SAVE,NULL};


// set the chatmacros original text, before config is executed
// if a dehacked patch was loaded, it will set the hacked texts,
// but the config.cfg will override it.
//
void HU_HackChatmacros (void)
{
    int    i;

    // this is either the original text, or dehacked ones
    cv_chatmacro0.defaultvalue = HUSTR_CHATMACRO0;
    cv_chatmacro1.defaultvalue = HUSTR_CHATMACRO1;
    cv_chatmacro2.defaultvalue = HUSTR_CHATMACRO2;
    cv_chatmacro3.defaultvalue = HUSTR_CHATMACRO3;
    cv_chatmacro4.defaultvalue = HUSTR_CHATMACRO4;
    cv_chatmacro5.defaultvalue = HUSTR_CHATMACRO5;
    cv_chatmacro6.defaultvalue = HUSTR_CHATMACRO6;
    cv_chatmacro7.defaultvalue = HUSTR_CHATMACRO7;
    cv_chatmacro8.defaultvalue = HUSTR_CHATMACRO8;
    cv_chatmacro9.defaultvalue = HUSTR_CHATMACRO9;

    // link chatmacros to cvars
    chat_macros[0] = &cv_chatmacro0;
    chat_macros[1] = &cv_chatmacro1;
    chat_macros[2] = &cv_chatmacro2;
    chat_macros[3] = &cv_chatmacro3;
    chat_macros[4] = &cv_chatmacro4;
    chat_macros[5] = &cv_chatmacro5;
    chat_macros[6] = &cv_chatmacro6;
    chat_macros[7] = &cv_chatmacro7;
    chat_macros[8] = &cv_chatmacro8;
    chat_macros[9] = &cv_chatmacro9;

    // register chatmacro vars ready for config.cfg
    for (i=0;i<10;i++)
       CV_RegisterVar (chat_macros[i]);
}


//  chatmacro <0-9> "chat message"
//
void Command_Chatmacro_f (void)
{
    int    i;

    if (COM_Argc()<2)
    {
        CONS_Printf ("chatmacro <0-9> : view chatmacro\n"
                     "chatmacro <0-9> \"chat message\" : change chatmacro\n");
        return;
    }

    i = atoi(COM_Argv(1)) % 10;

    if (COM_Argc()==2)
    {
        CONS_Printf("chatmacro %d is \"%s\"\n",i,chat_macros[i]->string);
        return;
    }

    // change a chatmacro
    CV_Set (chat_macros[i], COM_Argv(2));
}
