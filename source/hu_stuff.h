// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hu_stuff.h,v 1.5 2003/11/22 00:22:09 darkwolf95 Exp $
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
// $Log: hu_stuff.h,v $
// Revision 1.5  2003/11/22 00:22:09  darkwolf95
// get rid of FS hud pics on level exit and new game, also added exl's fix for clearing hub variables on new game
//
// Revision 1.4  2000/11/03 03:27:17  stroggonmeth
// Again with the bug fixing...
//
// Revision 1.3  2000/11/02 17:50:06  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:  
//      Head up display
//
//-----------------------------------------------------------------------------

#ifndef __HU_STUFF_H__
#define __HU_STUFF_H__


#include "d_event.h"
#include "w_wad.h"
#include "wi_stuff.h"
#include "r_defs.h"

//------------------------------------
//           heads up font
//------------------------------------
#define HU_FONTSTART    '!'     // the first font characters
#define HU_FONTEND      '_'     // the last font characters

#define HU_FONTSIZE     (HU_FONTEND - HU_FONTSTART + 1)


#define HU_CROSSHAIRS   3       // maximum 9 see HU_Init();

extern char*   shiftxform;   // french/english translation shift table

extern char    english_shiftxform[];
extern char    french_shiftxform[];

//------------------------------------
//           chat stuff
//------------------------------------
#define HU_BROADCAST    5       // first char in chat message

#define HU_MAXMSGLEN    80

extern patch_t*       hu_font[HU_FONTSIZE];

//set true by hu_ when entering a chat message
extern boolean chat_on; 

// P_DeathThink set this true to show scores while dead, in dmatch
extern boolean hu_showscores;
extern boolean playerdeadview;


// init heads up data at game startup.
void    HU_Init(void);

// reset heads up when consoleplayer respawns.
void    HU_Start(void);

//
boolean HU_Responder(event_t* ev);

//
void    HU_Ticker(void);
void    HU_Drawer(void);
char    HU_dequeueChatChar(void);
void    HU_Erase(void);

// used by console input
char ForeignTranslation(unsigned char ch);

// set chatmacros cvars point the original or dehacked texts, before
// config.cfg is executed !!
void HU_HackChatmacros (void);

// chatmacro <0-9> "message" console command
void Command_Chatmacro_f (void);

int HU_CreateTeamFragTbl(fragsort_t *fragtab,
                         int dmtotals[],
                         int fragtbl[MAXPLAYERS][MAXPLAYERS]);



void HU_SetTip(char *tip, int displaytics);
void HU_ClearTips();
int  HU_GetFSPic(int lumpnum, int xpos, int ypos);
int  HU_DeleteFSPic(int handle);
int  HU_ModifyFSPic(int handle, int lumpnum, int xpos, int ypos);
void HU_DrawFSPics();
void HU_ClearFSPics();
#endif
