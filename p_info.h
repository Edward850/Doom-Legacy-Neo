// Emacs style mode select -*- C++ -*-
//---------------------------------------------------------------------------
//
// $Id: p_info.h,v 1.5 2001/08/06 23:57:09 stroggonmeth Exp $
//
// Copyright(C) 2000 Simon Howard
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Log: p_info.h,v $
// Revision 1.5  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.4  2001/08/02 19:15:59  bpereira
// fix player reset in secret level of doom2
//
// Revision 1.3  2001/06/30 15:06:01  bpereira
// fixed wrong next level name in intermission
//
// Revision 1.2  2000/11/03 03:27:17  stroggonmeth
// Again with the bug fixing...
//
// Revision 1.1  2000/11/03 02:00:44  stroggonmeth
// Added p_info.c and p_info.h
//
//
//--------------------------------------------------------------------------
#ifndef __P_INFO_H__
#define __P_INFO_H__

#include "command.h"

void P_LoadLevelInfo(int lumpnum);

void P_CleanLine(char *line);

extern char *info_interpic;
extern char *info_levelname;
extern char *info_levelpic;
extern char *info_music;
extern int info_partime;
extern char *info_levelcmd[128];
extern char *info_skyname;
extern char *info_creator;
extern char *info_nextlevel;
extern char *info_nextsecret;
extern char *info_intertext;
extern char *info_backdrop;
extern int info_scripts;        // whether the current level has scripts

extern boolean default_weaponowned[NUMWEAPONS];

// level menu
// level authors can include a menu in their level to
// activate special features

typedef struct
{
  char *description;
  int scriptnum;
} levelmenuitem_t;

#define isnumchar(c) ( (c) >= '0' && (c) <= '9')
int isExMy(char *name);
int isMAPxy(char *name);
/*#define isExMy(s) ( (tolower((s)[0]) == 'e') && \
                    (isnumchar((s)[1])) &&      \
                    (tolower((s)[2]) == 'm') && \
                    (isnumchar((s)[3])) &&      \
                    ((s)[4] == '\0') )
#define isMAPxy(s) ( (strlen(s) == 5) && \
                     (tolower((s)[0]) == 'm') && \
                     (tolower((s)[1]) == 'a') && \
                     (tolower((s)[2]) == 'p') && \
                     (isnumchar((s)[3])) &&      \
                     (isnumchar((s)[4])) &&      \
                     ((s)[5] == '\0'))*/

void P_Info_AddCommands();
char *P_LevelName();
char *P_LevelNameByNum( int episode, int map );

#endif
