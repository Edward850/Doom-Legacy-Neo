// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: p_info.c,v 1.15 2001/12/26 17:24:46 hurdler Exp $
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
// $Log: p_info.c,v $
// Revision 1.15  2001/12/26 17:24:46  hurdler
// Update Linux version
//
// Revision 1.14  2001/08/14 00:36:26  hurdler
// Small update
//
// Revision 1.13  2001/08/13 22:53:39  stroggonmeth
// Small commit
//
// Revision 1.12  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.11  2001/08/02 19:15:59  bpereira
// fix player reset in secret level of doom2
//
// Revision 1.10  2001/06/30 15:06:01  bpereira
// fixed wrong next level name in intermission
//
// Revision 1.9  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.8  2001/05/07 20:27:16  stroggonmeth
// no message
//
// Revision 1.7  2001/03/21 18:24:38  stroggonmeth
// Misc changes and fixes. Code cleanup
//
// Revision 1.6  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.5  2000/11/11 13:59:45  bpereira
// no message
//
// Revision 1.4  2000/11/09 17:56:20  stroggonmeth
// Hopefully fixed a few bugs and did a few optimizations.
//
// Revision 1.3  2000/11/06 20:52:16  bpereira
// no message
//
// Revision 1.2  2000/11/03 11:48:39  hurdler
// Fix compiling problem under win32 with 3D-Floors and FragglScript (to verify!)
//
// Revision 1.1  2000/11/03 02:00:44  stroggonmeth
// Added p_info.c and p_info.h
//
//
//--------------------------------------------------------------------------
//
// Level info.
//
// Under smmu, level info is stored in the level marker: ie. "mapxx"
// or "exmx" lump. This contains new info such as: the level name, music
// lump to be played, par time etc.
//
// By Simon Howard
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "doomstat.h"
#include "doomdef.h"
#include "command.h"
#include "dehacked.h"
#include "dstrings.h"
#include "p_setup.h"
#include "p_info.h"
#include "p_mobj.h"
#include "t_script.h"
#include "w_wad.h"
#include "z_zone.h"
#include "p_local.h"

//----------------------------------------------------------------------------
//
// Helper functions
//

void P_LowerCase(char *line)
{
  char *temp;

  for(temp=line; *temp; temp++)
    *temp = tolower(*temp);
}

void P_StripSpaces(char *line)
{
  char *temp;

  temp = line+strlen(line)-1;

  while(*temp == ' ')
    {
      *temp = '\0';
      temp--;
    }
}

static void P_RemoveComments(char *line)
{
  char *temp = line;

  while(*temp)
    {
      if(*temp=='/' && *(temp+1)=='/')
        {
          *temp = '\0'; return;
        }
      temp++;
    }
}

static void P_RemoveEqualses(char *line)
{
  char *temp;

  temp = line;

  while(*temp)
    {
      if(*temp == '=')
        {
          *temp = ' ';
        }
      temp++;
    }
}

//----------------------------------------------------------------------------
//
//  Level vars: level variables in the [level info] section.
//
//  Takes the form:
//     [variable name] = [value]
//
//  '=' sign is optional: all equals signs are internally turned to spaces
//

char *info_interpic;
char *info_levelname;
int info_partime;
char *info_music;
char *info_skyname;
char *info_creator;
char *info_levelpic;
char *info_nextlevel;
char *info_nextsecret;
char *info_intertext = NULL;
char *info_backdrop;
char *info_weapons;
int info_scripts;       // has the current level got scripts?
int gravity;

enum
{
  IVT_STRING,
  IVT_INT,
  IVT_CONSOLECMD, // SoM: W00t I RULE
  IVT_END
};

typedef struct
{
  int type;
  char *name;
  void *variable;
} levelvar_t;

levelvar_t levelvars[]=
{
  {IVT_STRING,    "levelpic",     &info_levelpic},
  {IVT_STRING,    "levelname",    &info_levelname},
  {IVT_INT,       "partime",      &info_partime},
  {IVT_STRING,    "music",        &info_music},
  {IVT_STRING,    "skyname",      &info_skyname},
  {IVT_STRING,    "creator",      &info_creator},
  {IVT_STRING,    "interpic",     &info_interpic},
  {IVT_STRING,    "nextlevel",    &info_nextlevel},
  {IVT_STRING,    "nextsecret",   &info_nextsecret},
  {IVT_INT,       "gravity",      &gravity},
  {IVT_STRING,    "inter-backdrop",&info_backdrop},
  {IVT_STRING,    "defaultweapons",&info_weapons},
  {IVT_CONSOLECMD,"consolecmd",    NULL},
  {IVT_END,       0,              0}
};

void P_ParseLevelVar(char *cmd)
{
  char varname[50];
  char *equals;
  levelvar_t* current;

  if(!*cmd) return;

  P_RemoveEqualses(cmd);

  // right, first find the variable name

  sscanf(cmd, "%s", varname);

  // find what it equals
  equals = cmd+strlen(varname);
  while(*equals == ' ') equals++; // cut off the leading spaces

  current = levelvars;

  while(current->type != IVT_END)
    {
      if(!strcasecmp(current->name, varname))
        {
          switch(current->type)
            {
            case IVT_STRING:
              *(char**)current->variable         // +5 for safety
                = Z_Malloc(strlen(equals)+5, PU_LEVEL, NULL);
              strcpy(*(char**)current->variable, equals);
              break;

            case IVT_INT:
              *(int*)current->variable = atoi(equals);
                     break;
            case IVT_CONSOLECMD:
              {
                char t[256];
                sprintf(t, "%s\n", equals);
                COM_BufAddText(t);
              }
              break;
            }
        }
      current++;
    }
}


// clear all the level variables so that none are left over from a
// previous level

int isExMy(char *name)
{
  if(strlen(name) != 4)
    return 0;

  if(toupper(name[0]) != 'E' || toupper(name[2]) != 'M' || !isnumchar(name[1]) || !isnumchar(name[3]) || name[4] != '\0')
    return 0;
  return 1;
}

int isMAPxy(char *name)
{
  if(strlen(name) != 5)
    return 0;

  if(toupper(name[0]) != 'M' || toupper(name[1]) != 'A' || toupper(name[2]) != 'P' || !isnumchar(name[3]) || !isnumchar(name[4]) || name[5] != '\0')
    return 0;
  return 1;
}

void P_ClearLevelVars()
{
  info_levelname = info_skyname = info_levelpic = info_interpic = "";
  info_music = "";
  info_creator = "unknown";
  info_partime = -1;

  if(gamemode == commercial && isExMy(levelmapname))
  {
    static char nextlevel[10];
    info_nextlevel = nextlevel;

    // set the next episode
    strcpy(nextlevel, levelmapname);
    nextlevel[3] ++;
    if(nextlevel[3] > '9')  // next episode
    {
      nextlevel[3] = '1';
      nextlevel[1] ++;
    }

    info_music = levelmapname;
  }
  else
    info_nextlevel = "";

  info_nextsecret = "";

  info_weapons = "";
  gravity = FRACUNIT;     // default gravity

  if(info_intertext)
  {
    Z_Free(info_intertext);
    info_intertext = NULL;
  }

  info_backdrop = NULL;

  T_ClearScripts();
  info_scripts = false;
}

//----------------------------------------------------------------------------
//
// P_ParseScriptLine
//
// FraggleScript: if we are reading in script lines, we add the new lines
// into the levelscript
//

//SoM: Dynamic limit set by lumpsize...
int   maxscriptsize = -1;

void P_ParseScriptLine(char *line)
{
  if(levelscript.data[0] == 0)
  {
    Z_Free(levelscript.data);
    levelscript.data = Z_Malloc(maxscriptsize, PU_LEVEL, 0);
    levelscript.data[0] = '\0';
  }

  if( (int)(strlen(levelscript.data)+strlen(line)) > maxscriptsize)
    I_Error("Script larger than script lump???\n");

  // add the new line to the current data using sprintf (ugh)
  sprintf(levelscript.data, "%s%s\n", levelscript.data, line);
}

//-------------------------------------------------------------------------
//
// P_ParseInterText
//
// Add line to the custom intertext
//

void P_ParseInterText(char *line)
{
  while(*line==' ') line++;
  if(!*line) return;

  if(info_intertext)
  {
    int textlen = strlen(info_intertext);

    if(textlen + 1 > maxscriptsize)
      I_Error("Intermission text bigger than LUMP?\n");

    // newline
    info_intertext[textlen] = '\n';

    // add line to end
    sprintf(info_intertext, "%s\n%s", info_intertext, line);
  }
  else
  {
    info_intertext = Z_Malloc(maxscriptsize, PU_STATIC, 0);
    strcpy(info_intertext, line);
  }
}

//---------------------------------------------------------------------------
//
// Setup/Misc. Functions
//

boolean default_weaponowned[NUMWEAPONS];

void P_InitWeapons()
{
  char *s;

  memset(default_weaponowned, 0, sizeof(default_weaponowned));

  s = info_weapons;

  while(*s)
    {
      switch(*s)
        {
        case '3': default_weaponowned[wp_shotgun] = true; break;
        case '4': default_weaponowned[wp_chaingun] = true; break;
        case '5': default_weaponowned[wp_missile] = true; break;
        case '6': default_weaponowned[wp_plasma] = true; break;
        case '7': default_weaponowned[wp_bfg] = true; break;
        case '8': default_weaponowned[wp_supershotgun] = true; break;
        default: break;
        }
      s++;
    }
}

//SoM: Moved from hu_stuff.c
#define HU_TITLE  (text[HUSTR_E1M1_NUM + (gameepisode-1)*9+gamemap-1])
#define HU_TITLE2 (text[HUSTR_1_NUM + gamemap-1])
#define HU_TITLEP (text[PHUSTR_1_NUM + gamemap-1])
#define HU_TITLET (text[THUSTR_1_NUM + gamemap-1])
#define HU_TITLEH (text[HERETIC_E1M1_NUM + (gameepisode-1)*9+gamemap-1])

unsigned char *levelname;

void P_FindLevelName()
{
  extern char *maplumpname;

  // determine the level name
  // there are a number of sources from which it can come from,
  // getting the right one is the tricky bit =)
  // info level name from level lump (p_info.c) ?

  if(*info_levelname) levelname = info_levelname;
        // not a new level or dehacked level names ?
  else if(!newlevel || deh_loaded)
    {
      if( gamemode == heretic )
        levelname = HU_TITLEH;
      else
      if(isMAPxy(maplumpname))
        levelname = gamemission == pack_tnt ? HU_TITLET :
        gamemission == pack_plut ? HU_TITLEP : HU_TITLE2;
      else if(isExMy(maplumpname))
        levelname = HU_TITLE;
      else
        levelname = maplumpname;
    }
  else        //  otherwise just put "new level"
    {
      static char newlevelstr[50];

      sprintf(newlevelstr, "%s: new level", maplumpname);
      levelname = newlevelstr;
    }
}

//-------------------------------------------------------------------------
//
// P_ParseInfoCmd
//
// We call the relevant function to deal with the line we are given,
// based on readtype. If we get a section divider ([] bracketed) we
// change readtype.
//

enum
{
  RT_LEVELINFO,
  RT_SCRIPT,
  RT_OTHER,
  RT_INTERTEXT
} readtype;


void P_ParseInfoCmd(char *line)
{
  if(!*line)
    return;

  if(readtype != RT_SCRIPT)       // not for scripts
    {
      //      P_LowerCase(line);
      while(*line == ' ') line++;
      if(!*line) return;
      if((line[0] == '/' && line[1] == '/') ||     // comment
         line[0] == '#' || line[0] == ';') return;
    }

  if(*line == '[')                // a new section seperator
    {
      line++;
      if(!strncasecmp(line, "level info", 10))
        readtype = RT_LEVELINFO;
      if(!strncasecmp(line, "scripts", 7))
        {
          readtype = RT_SCRIPT;
          info_scripts = true;    // has scripts
        }
      if(!strncasecmp(line, "intertext", 9))
        readtype = RT_INTERTEXT;
      return;
    }

  switch(readtype)
    {
    case RT_LEVELINFO:
      P_ParseLevelVar(line);
      break;

    case RT_SCRIPT:
      P_ParseScriptLine(line);
      break;

    case RT_INTERTEXT:
      P_ParseInterText(line);
      break;

    case RT_OTHER:
      break;
    }
}

//-------------------------------------------------------------------------
//
// P_LoadLevelInfo
//
// Load the info lump for a level. Call P_ParseInfoCmd for each
// line of the lump.

void P_LoadLevelInfo(int lumpnum)
{
  char      *lump;
  char      *readline;
  int       lumpsize;

  readtype = RT_OTHER;
  P_ClearLevelVars();

  lumpsize = maxscriptsize = W_LumpLength(lumpnum);
  readline = Z_Malloc(lumpsize + 1, PU_STATIC, 0);
  readline[0] = '\0';

  if(lumpsize > 0)
  {
    rover = lump = W_CacheLumpNum(lumpnum, PU_STATIC);
    while(rover < lump + lumpsize)
      {
        if(*rover == '\n') // end of line
          {
          P_ParseInfoCmd(readline);  // parse line
          readline[0] = '\0';
          }
        else
        // add to line if valid char
        if(isprint(*rover) || *rover == '{' || *rover == '}')
          {
            // add char
            readline[strlen(readline)+1] = '\0';
            readline[strlen(readline)] = *rover;
          }

      rover++;
    }

    // parse last line
    P_ParseInfoCmd(readline);
    Z_Free(lump);
  }
  Z_Free(readline);

  P_InitWeapons();
  P_FindLevelName();

  //Set the gravity for the level!
  if( cv_gravity.value != gravity )
  {
      if(gravity != FRACUNIT)
          COM_BufAddText(va("gravity %f\n", ((double)gravity) / 100));
      else
          COM_BufAddText(va("gravity %f\n", ((double)gravity) / FRACUNIT));
  }

  COM_BufExecute(); //Hurdler: flush the command buffer
  return;
}

//-------------------------------------------------------------------------
//
// Console Commands
//

void COM_Creator_f(void) {CONS_Printf("%s\n", info_creator);}
void COM_Levelname_f(void) {CONS_Printf("%s\n", levelname);}


void P_Info_AddCommands()
{
  COM_AddCommand("creator",    COM_Creator_f);
  COM_AddCommand("levelname",  COM_Levelname_f);
}



char *P_LevelName()
{
  return levelname;
}

// todo : make this use mapinfo lump
char *P_LevelNameByNum( int episode, int map )
{
    switch(gamemode) 
    {
       case  shareware :
       case  registered :
       case  retail :
                                return text[HUSTR_E1M1_NUM + (episode-1)*9+map-1];
       case  commercial :
           switch(gamemission) {
               case pack_tnt  : return text[THUSTR_1_NUM + map-1];
               case pack_plut : return text[PHUSTR_1_NUM + map-1];
               default :        return text[HUSTR_1_NUM + map-1];
           }
       case  heretic:           return text[HERETIC_E1M1_NUM + (episode-1)*9+map-1];
       case  hexen: break;
       default: break;
    }
    return "New map";
}
