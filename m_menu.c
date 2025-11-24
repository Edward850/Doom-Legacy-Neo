// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: m_menu.c,v 1.52 2003/08/11 13:50:01 hurdler Exp $
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
// $Log: m_menu.c,v $
// Revision 1.52  2003/08/11 13:50:01  hurdler
// go final + translucent HUD + fix spawn in net game
//
// Revision 1.51  2003/03/22 22:35:59  hurdler
// Fix CR+LF issue
//
// Revision 1.50  2002/09/27 16:40:08  tonyd
// First commit of acbot
//
// Revision 1.49  2002/09/12 20:10:50  hurdler
// Added some cvars
//
// Revision 1.48  2002/08/24 22:42:03  hurdler
// Apply Robert Hogberg patches
//
// Revision 1.47  2001/12/31 13:47:46  hurdler
// Add setcorona FS command and prepare the code for beta 4
//
// Revision 1.46  2001/12/26 17:24:46  hurdler
// Update Linux version
//
// Revision 1.45  2001/12/15 18:41:35  hurdler
// small commit, mainly splitscreen fix
//
// Revision 1.44  2001/11/02 23:29:13  judgecutor
// Fixed "secondary player controls" bug
//
// Revision 1.43  2001/11/02 21:44:05  judgecutor
// Added Frag's weapon falling
//
// Revision 1.42  2001/08/20 21:37:34  hurdler
// fix palette in splitscreen + hardware mode
//
// Revision 1.41  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.40  2001/08/08 20:34:43  hurdler
// Big TANDL update
//
// Revision 1.39  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.38  2001/05/27 13:42:47  bpereira
// no message
//
// Revision 1.37  2001/05/16 22:00:10  hurdler
// fix compiling problem
//
// Revision 1.36  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.35  2001/05/14 19:02:58  metzgermeister
//   * Fixed floor not moving up with player on E3M1
//   * Fixed crash due to oversized string in screen message ... bad bug!
//   * Corrected some typos
//   * fixed sound bug in SDL
//
// Revision 1.34  2001/04/29 14:25:26  hurdler
// small fix
//
// Revision 1.33  2001/04/01 17:35:06  bpereira
// no message
//
// Revision 1.32  2001/03/03 06:17:33  bpereira
// no message
//
// Revision 1.31  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.30  2001/02/10 12:27:14  bpereira
// no message
//
// Revision 1.29  2001/01/25 22:15:42  bpereira
// added heretic support
//
// Revision 1.28  2000/11/26 20:36:14  hurdler
// Adding autorun2
//
// Revision 1.27  2000/10/21 08:43:29  bpereira
// no message
//
// Revision 1.26  2000/10/17 10:09:27  hurdler
// Update master server code for easy connect from menu
//
// Revision 1.25  2000/10/16 20:02:29  bpereira
// no message
//
// Revision 1.24  2000/10/08 13:30:01  bpereira
// no message
//
// Revision 1.23  2000/10/02 18:25:45  bpereira
// no message
//
// Revision 1.22  2000/10/01 15:20:23  hurdler
// Add private server
//
// Revision 1.21  2000/10/01 10:18:17  bpereira
// no message
//
// Revision 1.20  2000/10/01 09:09:36  hurdler
// Put the md2 code in #ifdef TANDL
//
// Revision 1.19  2000/09/15 19:49:22  bpereira
// no message
//
// Revision 1.18  2000/09/08 22:28:30  hurdler
// merge masterserver_ip/port in one cvar, add -private
//
// Revision 1.17  2000/09/02 15:38:24  hurdler
// Add master server to menus (temporaray)
//
// Revision 1.16  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.15  2000/04/24 15:10:56  hurdler
// Support colormap for text
//
// Revision 1.14  2000/04/23 00:29:28  hurdler
// fix a small bug in skin color
//
// Revision 1.13  2000/04/23 00:25:20  hurdler
// fix a small bug in skin color
//
// Revision 1.12  2000/04/22 21:12:15  hurdler
// I like it better like that
//
// Revision 1.11  2000/04/22 20:27:35  metzgermeister
// support for immediate fullscreen switching
//
// Revision 1.10  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.9  2000/04/13 16:26:41  hurdler
// looks better like that
//
// Revision 1.8  2000/04/12 19:31:37  metzgermeister
// added use_mouse to menu
//
// Revision 1.7  2000/04/08 17:29:24  stroggonmeth
// no message
//
// Revision 1.6  2000/04/07 23:11:17  metzgermeister
// added mouse move
//
// Revision 1.5  2000/04/04 10:44:00  hurdler
// Remove a warning message in Dos/Windows
//
// Revision 1.4  2000/04/04 00:32:46  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.3  2000/03/23 22:54:00  metzgermeister
// added support for HOME/.legacy under Linux
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      DOOM selection menu, options, episode etc.
//      Sliders and icons. Kinda widget stuff.
//
// NOTE:
//      All V_DrawPatchDirect () has been replaced by V_DrawScaledPatch ()
//      so that the menu is scaled to the screen size. The scaling is always
//      an integer multiple of the original size, so that the graphics look
//      good.
//
//-----------------------------------------------------------------------------

#ifndef __WIN32__
#include <unistd.h>
#endif
#include <fcntl.h>

#include "am_map.h"

#include "doomdef.h"
#include "dstrings.h"
#include "d_main.h"

#include "console.h"

#include "r_local.h"
#include "hu_stuff.h"
#include "g_game.h"
#include "g_input.h"

#include "m_argv.h"

// Data.
#include "sounds.h"
#include "s_sound.h"
#include "i_system.h"

#include "m_menu.h"
#include "v_video.h"
#include "i_video.h"

#include "keys.h"
#include "z_zone.h"
#include "w_wad.h"
#include "p_local.h"
#include "p_fab.h"

#ifdef HWRENDER
#include "hardware/hw_main.h"
#endif

#include "d_net.h"
#include "mserv.h"
#include "p_inter.h"

// -1 = no quicksave slot picked!
int                     quickSaveSlot;
boolean                 menuactive;

#define SKULLXOFF       -32
#define LINEHEIGHT       16
#define STRINGHEIGHT     10
#define FONTBHEIGHT      20
#define SMALLLINEHEIGHT   8
#define SLIDER_RANGE     10
#define SLIDER_WIDTH    (8*SLIDER_RANGE+6)
#define MAXSTRINGLENGTH  32

// we are going to be entering a savegame string
int     saveStringEnter;
int     saveSlot;       // which slot to save in
int     saveCharIndex;  // which char we're editing
// old save description before edit
char    saveOldString[SAVESTRINGSIZE];

char    savegamestrings[10][SAVESTRINGSIZE];

// flags for items in the menu
// menu handle (what we do when key is pressed
#define  IT_TYPE             14     // (2+4+8)
#define  IT_CALL              0     // call the function
#define  IT_ARROWS            2     // call function with 0 for left arrow and 1 for right arrow in param
#define  IT_KEYHANDLER        4     // call with the key in param
#define  IT_SUBMENU           6     // go to sub menu
#define  IT_CVAR              8     // hangdle as a cvar
#define  IT_SPACE            10     // no handling
#define  IT_MSGHANDLER       12     // same as key but with event and sometime can handle y/n key (special for message

#define  IT_DISPLAY  (48+64+128)    // 16+32+64
#define  IT_NOTHING           0     // space
#define  IT_PATCH            16     // a patch or a string with big font
#define  IT_STRING           32     // little string (spaced with 10)
#define  IT_WHITESTRING      48     // little string in white
#define  IT_DYBIGSPACE       64     // same as noting
#define  IT_DYLITLSPACE  (16+64)    // little space
#define  IT_STRING2      (32+64)    // a simple string
#define  IT_GRAYPATCH    (16+32+64) // grayed patch or big font string
#define  IT_BIGSLIDER     (128)     // volume sound use this

//consvar specific
#define  IT_CVARTYPE   (256+512+1024)
#define  IT_CV_NORMAL         0
#define  IT_CV_SLIDER       256
#define  IT_CV_STRING       512
#define  IT_CV_NOPRINT (256+512)
#define  IT_CV_NOMOD       1024

// in short for some common use
#define  IT_BIGSPACE    (IT_SPACE  +IT_DYBIGSPACE)
#define  IT_LITLSPACE   (IT_SPACE  +IT_DYLITLSPACE)
#define  IT_CONTROL     (IT_STRING2+IT_CALL)
#define  IT_CVARMAX     (IT_CVAR   +IT_CV_NOMOD)
#define  IT_DISABLED    (IT_SPACE  +IT_GRAYPATCH)

typedef union
{
    struct menu_s     *submenu;               // IT_SUBMENU
    consvar_t         *cvar;                  // IT_CVAR
    void             (*routine)(int choice);  // IT_CALL, IT_KEYHANDLER, IT_ARROWS
} itemaction_t;

//
// MENU TYPEDEFS
//
typedef struct menuitem_s
{
    // show IT_xxx
    short     status;

    char      *patch;
    char      *text;  // used when FONTBxx lump is found

    // FIXME: should be itemaction_t !!!
    void      *itemaction;

    // hotkey in menu
    // or y of the item 
    byte      alphaKey;
} menuitem_t;

typedef struct menu_s
{
    char            *menutitlepic;
    char            *menutitle;             // title as string for display with fontb if present
    short           numitems;               // # of menu items
    struct menu_s*  prevMenu;               // previous menu
    menuitem_t*     menuitems;              // menu items
    void            (*drawroutine)(void);   // draw routine
    short           x;
    short           y;                      // x,y of menu
    short           lastOn;                 // last item user was on in menu
    boolean         (*quitroutine)(void);   // called before quit a menu return true if we can
} menu_t;

// current menudef
menu_t*   currentMenu;
short     itemOn;                       // menu item skull is on
short     skullAnimCounter;             // skull animation counter
short     whichSkull;                   // which skull to draw
int       SkullBaseLump;

// graphic name of skulls
char      skullName[2][9] = {"M_SKULL1","M_SKULL2"};

//
// PROTOTYPES
//
void M_DrawSaveLoadBorder(int x,int y);
void M_SetupNextMenu(menu_t *menudef);

void M_DrawTextBox (int x, int y, int width, int lines);     //added:06-02-98:
void M_DrawThermo(int x,int y,consvar_t *cv);
void M_DrawEmptyCell(menu_t *menu,int item);
void M_DrawSelCell(menu_t *menu,int item);
void M_DrawSlider (int x, int y, int range);
void M_CentreText(int y, char* string);        //added:30-01-98:writetext centered

void M_StartControlPanel(void);
void M_StopMessage(int choice);
void M_ClearMenus (boolean callexitmenufunc);
int  M_StringHeight(char* string);
void M_GameOption(int choice);
void M_NetOption(int choice);
//28/08/99: added by Hurdler
void M_OpenGLOption(int choice);

menu_t MainDef,SinglePlayerDef,MultiPlayerDef,SetupMultiPlayerDef,
       EpiDef,NewDef,OptionsDef,VidModeDef,ControlDef,SoundDef,
       ReadDef2,ReadDef1,SaveDef,LoadDef,ControlDef2,GameOptionDef,
       NetOptionDef,VideoOptionsDef,MouseOptionsDef,ServerOptionsDef;

const char *ALLREADYPLAYING="You are already playing\n\nLeave this game first\n";

//===========================================================================
//Generic Stuffs (more easy to create menus :))
//===========================================================================

void M_DrawMenuTitle(void)
{
    if( FontBBaseLump && currentMenu->menutitle )
    {
        int xtitle = (BASEVIDWIDTH-V_TextBWidth(currentMenu->menutitle))/2;
        int ytitle = (currentMenu->y-V_TextBHeight(currentMenu->menutitle))/2;
        if(xtitle<0) xtitle=0;
        if(ytitle<0) ytitle=0;

        V_DrawTextB(currentMenu->menutitle, xtitle, ytitle);
    }
    else
    if( currentMenu->menutitlepic )
    {
        patch_t* p = W_CachePatchName(currentMenu->menutitlepic,PU_CACHE);
#if 1
        //SoM: 4/7/2000: Old code was causing problems with large graphics.
//        int xtitle = (vid.width / 2) - (p->width / 2);
//        int ytitle = (y-p->height)/2;
        int xtitle = 94;
        int ytitle = 2;
#else
        int xtitle = (BASEVIDWIDTH-p->width)/2;
        int ytitle = (currentMenu->y-p->height)/2;
#endif

        if(xtitle<0) xtitle=0;
        if(ytitle<0) ytitle=0;
        V_DrawScaledPatch (xtitle,ytitle,0,p);
    }
}

void M_DrawGenericMenu(void)
{
    int x,y,i,cursory=0;

    // DRAW MENU
    x = currentMenu->x;
    y = currentMenu->y;

    // draw title (or big pic)
    M_DrawMenuTitle();

    for (i=0;i<currentMenu->numitems;i++)
    {
        if (i==itemOn)
            cursory=y;
        switch (currentMenu->menuitems[i].status & IT_DISPLAY) {
           case IT_PATCH  :
               if( FontBBaseLump && currentMenu->menuitems[i].text )
               {
                   V_DrawTextB(currentMenu->menuitems[i].text, x, y);
                   y += FONTBHEIGHT-LINEHEIGHT;
               }
               else 
               if( currentMenu->menuitems[i].patch &&
                   currentMenu->menuitems[i].patch[0] )
                   V_DrawScaledPatch (x,y,0,
                                      W_CachePatchName(currentMenu->menuitems[i].patch ,PU_CACHE));
           case IT_NOTHING:
           case IT_DYBIGSPACE:
               y += LINEHEIGHT;
               break;
           case IT_BIGSLIDER :
               M_DrawThermo( x, y, (consvar_t *)currentMenu->menuitems[i].itemaction);
               y += LINEHEIGHT;
               break;
           case IT_STRING :
           case IT_WHITESTRING :
               if( currentMenu->menuitems[i].alphaKey )
                   y = currentMenu->y+currentMenu->menuitems[i].alphaKey;
               if (i==itemOn)
                   cursory=y;

               if( (currentMenu->menuitems[i].status & IT_DISPLAY)==IT_STRING ) 
                   V_DrawString(x,y,0,currentMenu->menuitems[i].text);
               else
                   V_DrawString(x,y,V_WHITEMAP,currentMenu->menuitems[i].text);

               // Cvar specific handling
               switch (currentMenu->menuitems[i].status & IT_TYPE)
                   case IT_CVAR:
                   {
                    consvar_t *cv=(consvar_t *)currentMenu->menuitems[i].itemaction;
                    switch (currentMenu->menuitems[i].status & IT_CVARTYPE) {
                       case IT_CV_SLIDER :
                           M_DrawSlider (BASEVIDWIDTH-x-SLIDER_WIDTH,
                                         y,
                                         ( (cv->value - cv->PossibleValue[0].value) * 100 /
                                         (cv->PossibleValue[1].value - cv->PossibleValue[0].value)));
                       case IT_CV_NOPRINT: // color use this 
                           break;
                       case IT_CV_STRING:
                           M_DrawTextBox(x,y+4,MAXSTRINGLENGTH,1);
                           V_DrawString (x+8,y+12,0,cv->string);
                           if( skullAnimCounter<4 && i==itemOn )
                               V_DrawCharacter( x+8+V_StringWidth(cv->string),
                                                y+12,
                                                '_' | 0x80);
                           y+=16;
                           break;
                       default:
                           V_DrawString(BASEVIDWIDTH-x-V_StringWidth (cv->string),
                                        y, V_WHITEMAP, 
                                        cv->string);
                           break;
                   }
                   break;
               }
               y+=STRINGHEIGHT;
               break;
           case IT_STRING2:
               V_DrawString (x,y,0,currentMenu->menuitems[i].text);
           case IT_DYLITLSPACE:
               y+=SMALLLINEHEIGHT;
               break;
           case IT_GRAYPATCH:
               if( FontBBaseLump && currentMenu->menuitems[i].text )
               {
                   V_DrawTextBGray(currentMenu->menuitems[i].text, x, y);
                   y += FONTBHEIGHT-LINEHEIGHT;
               }
               else 
               if( currentMenu->menuitems[i].patch &&
                   currentMenu->menuitems[i].patch[0] )
                   V_DrawMappedPatch (x,y,0,
                                      W_CachePatchName(currentMenu->menuitems[i].patch ,PU_CACHE),
                                      graymap);
               y += LINEHEIGHT;
               break;

        }
    }

    // DRAW THE SKULL CURSOR
    if (((currentMenu->menuitems[itemOn].status & IT_DISPLAY)==IT_PATCH)
      ||((currentMenu->menuitems[itemOn].status & IT_DISPLAY)==IT_NOTHING) )
    {
        V_DrawScaledPatch(currentMenu->x + SKULLXOFF,
                          cursory-5,
                          0,
                          W_CachePatchName(skullName[whichSkull],PU_CACHE) );
    }
    else
    if (skullAnimCounter<4 * NEWTICRATERATIO)  //blink cursor
    {
        V_DrawCharacter(currentMenu->x - 10,
                        cursory,
                        '*' | 0x80);
    }

}

//===========================================================================
//MAIN MENU
//===========================================================================

void M_QuitDOOM(int choice);

enum
{
    singleplr = 0,
    multiplr,
    options,
    readthis,
    quitdoom,
    main_end
} main_e;

menuitem_t MainMenu[]=
{
    {IT_SUBMENU | IT_PATCH,"M_SINGLE","SINGLE PLAYER",&SinglePlayerDef ,'s'},
    {IT_SUBMENU | IT_PATCH,"M_MULTI" ,"MULTIPLAYER",&MultiPlayerDef  ,'m'},
    {IT_SUBMENU | IT_PATCH,"M_OPTION","OPTIONS",&OptionsDef      ,'o'},
    {IT_SUBMENU | IT_PATCH,"M_RDTHIS","INFO",&ReadDef1        ,'r'},  // Another hickup with Special edition.
    {IT_CALL    | IT_PATCH,"M_QUITG" ,"QUIT GAME",M_QuitDOOM       ,'q'}
};

void HereticMainMenuDrawer(void)
{
    int frame = (I_GetTime()/3)%18;

    V_DrawScaledPatch(40, 10, 0, W_CachePatchNum(SkullBaseLump+(17-frame), PU_CACHE));
    V_DrawScaledPatch(232, 10, 0, W_CachePatchNum(SkullBaseLump+frame, PU_CACHE));
    M_DrawGenericMenu();
}

menu_t  MainDef =
{
    "M_DOOM",
    NULL,
    main_end,
    NULL,
    MainMenu,
    M_DrawGenericMenu,
    97,64,
    0
};

//===========================================================================
//SINGLE PLAYER MENU
//===========================================================================

void M_NewGame(int choice);
void M_LoadGame(int choice);
void M_SaveGame(int choice);
void M_EndGame(int choice);

enum
{
    newgame = 0,
    loadgame,
    savegame,
    endgame,
    single_end
} single_e;

menuitem_t SinglePlayerMenu[] =
{
    {IT_CALL | IT_PATCH,"M_NGAME" ,"NEW GAME" ,M_NewGame ,'n'},
    {IT_CALL | IT_PATCH,"M_LOADG" ,"LOAD GAME",M_LoadGame,'l'},
    {IT_CALL | IT_PATCH,"M_SAVEG" ,"SAVE GAME",M_SaveGame,'s'},
    {IT_CALL | IT_PATCH,"M_ENDGAM","END GAME" ,M_EndGame ,'e'}
};

menu_t  SinglePlayerDef =
{
    "M_SINGLE",
    "Single Player",
    single_end,
    &MainDef,
    SinglePlayerMenu,
    M_DrawGenericMenu,
    97,64,
    0
};

//===========================================================================
// Connect Menu
//===========================================================================

CV_PossibleValue_t serversearch_cons_t[] = {{0,"Local Lan"}
                                           ,{1,"Internet"}
                                           ,{0,NULL}};


consvar_t cv_serversearch = {"serversearch"    ,"0",CV_HIDEN,serversearch_cons_t};

#define FIRSTSERVERLINE 3

void M_Connect( int choise )
{
    // do not call menuexitfunc 
    M_ClearMenus(false);

    COM_BufAddText(va("connect node %d\n", serverlist[choise-FIRSTSERVERLINE].node));
}

static int localservercount;

void M_Refresh( int choise )
{
    CL_UpdateServerList( cv_serversearch.value );
}

menuitem_t  ConnectMenu[] =
{
    {IT_STRING | IT_CVAR ,0,"Search On"       ,&cv_serversearch       ,0},
    {IT_STRING | IT_CALL ,0,"Refresh"         ,M_Refresh              ,0},
    {IT_WHITESTRING | IT_SPACE,0,"Server Name                      ping plys dm" ,0 ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
    {IT_STRING | IT_SPACE,0,""             ,M_Connect              ,0},
};

void M_DrawConnectMenu( void )
{
    int i;
    char *p;

    for( i=FIRSTSERVERLINE; i<localservercount+FIRSTSERVERLINE; i++ )
        ConnectMenu[i].status = IT_STRING | IT_SPACE;

    if( serverlistcount <= 0 )
        V_DrawString (currentMenu->x,currentMenu->y+FIRSTSERVERLINE*STRINGHEIGHT,0,"No server found");
    else
    for( i=0;i<serverlistcount && i+FIRSTSERVERLINE<sizeof(ConnectMenu)/sizeof(menuitem_t);i++ )
    {
        V_DrawString (currentMenu->x,currentMenu->y+(FIRSTSERVERLINE+i)*STRINGHEIGHT,0,serverlist[i].info.servername);
        p = va("%d", serverlist[i].info.time);
        V_DrawString (currentMenu->x+200-V_StringWidth(p),currentMenu->y+(FIRSTSERVERLINE+i)*STRINGHEIGHT,0,p);
        p = va("%d/%d  %d", serverlist[i].info.numberofplayer,
                            serverlist[i].info.maxplayer,
                            serverlist[i].info.deathmatch);
        V_DrawString (currentMenu->x+250-V_StringWidth(p),currentMenu->y+(FIRSTSERVERLINE+i)*STRINGHEIGHT,0,p);

        ConnectMenu[i+FIRSTSERVERLINE].status = IT_STRING | IT_CALL;
    }
    localservercount = serverlistcount;

    M_DrawGenericMenu();
}

boolean M_CancelConnect(void)
{
    D_CloseConnection();
    return true;
}

menu_t  Connectdef =
{
    "M_CONNEC",
    "Connect Server",
    sizeof(ConnectMenu)/sizeof(menuitem_t),
    &MultiPlayerDef,
    ConnectMenu,
    M_DrawConnectMenu,
    27,40,
    0,
    M_CancelConnect
};

void M_ConnectMenu(int choise)
{
    if( Playing() )
    {
        M_StartMessage(ALLREADYPLAYING,NULL,MM_NOTHING);
        return;
    }

    M_SetupNextMenu(&Connectdef);
    M_Refresh(0);
}

//===========================================================================
// Start Server Menu
//===========================================================================

CV_PossibleValue_t skill_cons_t[] = {{1,"I'm too young to die"}
                                    ,{2,"Hey, not too rough"}
                                    ,{3,"Hurt me plenty"}
                                    ,{4,"Ultra violence"}
                                    ,{5,"Nightmare!" }
                                    ,{0,NULL}};

CV_PossibleValue_t map_cons_t[] = {{ 1,"map01"} ,{ 2,"map02"} ,{ 3,"map03"}
                                  ,{ 4,"map04"} ,{ 5,"map05"} ,{ 6,"map06"}
                                  ,{ 7,"map07"} ,{ 8,"map08"} ,{ 9,"map09"}
                                  ,{10,"map10"} ,{11,"map11"} ,{12,"map12"}
                                  ,{13,"map13"} ,{14,"map14"} ,{15,"map15"}
                                  ,{16,"map16"} ,{17,"map17"} ,{18,"map18"}
                                  ,{19,"map19"} ,{20,"map20"} ,{21,"map21"}
                                  ,{22,"map22"} ,{23,"map23"} ,{24,"map24"}
                                  ,{25,"map25"} ,{26,"map26"} ,{27,"map27"}
                                  ,{28,"map28"} ,{29,"map29"} ,{30,"map30"}
                                  ,{31,"map31"} ,{32,"map32"} ,{0,NULL}};

CV_PossibleValue_t exmy_cons_t[] ={{11,"e1m1"} ,{12,"e1m2"} ,{13,"e1m3"}
                                  ,{14,"e1m4"} ,{15,"e1m5"} ,{16,"e1m6"}
                                  ,{17,"e1m7"} ,{18,"e1m8"} ,{19,"e1m9"}
                                  ,{21,"e2m1"} ,{22,"e2m2"} ,{23,"e2m3"}
                                  ,{24,"e2m4"} ,{25,"e2m5"} ,{26,"e2m6"}
                                  ,{27,"e2m7"} ,{28,"e2m8"} ,{29,"e2m9"}
                                  ,{31,"e3m1"} ,{32,"e3m2"} ,{33,"e3m3"}
                                  ,{34,"e3m4"} ,{35,"e3m5"} ,{36,"e3m6"}
                                  ,{37,"e3m7"} ,{38,"e3m8"} ,{39,"e3m9"}
                                  ,{41,"e4m1"} ,{42,"e4m2"} ,{43,"e4m3"}
                                  ,{44,"e4m4"} ,{45,"e4m5"} ,{46,"e4m6"}
                                  ,{47,"e4m7"} ,{48,"e4m8"} ,{49,"e4m9"}
                                  ,{41,"e5m1"} ,{42,"e5m2"} ,{43,"e5m3"}
                                  ,{44,"e5m4"} ,{45,"e5m5"} ,{46,"e5m6"}
                                  ,{47,"e5m7"} ,{48,"e5m8"} ,{49,"e5m9"}
                                  ,{0,NULL}};

consvar_t cv_skill    = {"skill"    ,"4",CV_HIDEN,skill_cons_t};
consvar_t cv_monsters = {"monsters" ,"0",CV_HIDEN,CV_YesNo};
consvar_t cv_nextmap  = {"nextmap"  ,"1",CV_HIDEN,map_cons_t};
extern CV_PossibleValue_t deathmatch_cons_t[];
consvar_t cv_newdeathmatch  = {"newdeathmatch"  ,"3",CV_HIDEN,deathmatch_cons_t};
static boolean StartSplitScreenGame;

void M_StartServer( int choise )
{
    netgame = true;
    multiplayer = true;
    COM_BufAddText(va("stopdemo;splitscreen %d;deathmatch %d;map \"%s\" -monsters %d skill %d\n", 
                      StartSplitScreenGame, cv_newdeathmatch.value, 
                      cv_nextmap.string, cv_monsters.value, cv_skill.value));
    M_ClearMenus(true);
}

menuitem_t  ServerMenu[] =
{
    {IT_STRING | IT_CVAR,0,"Map"             ,&cv_nextmap          ,0},
    {IT_STRING | IT_CVAR,0,"Skill"           ,&cv_skill            ,0},
    {IT_STRING | IT_CVAR,0,"Monsters"        ,&cv_monsters         ,0},
    {IT_STRING | IT_CVAR,0,"Deathmatch Type" ,&cv_newdeathmatch    ,0},
                         
    {IT_STRING | IT_CVAR,0,"Internet Server" ,&cv_internetserver   ,0},
    {IT_STRING | IT_CVAR
     | IT_CV_STRING     ,0,"Server Name"     ,&cv_servername       ,0},
    {IT_WHITESTRING 
               | IT_CALL,0,"Start"           ,M_StartServer        ,120}
};

menu_t  Serverdef =
{
    "M_STSERV",
    "Start Server",
    sizeof(ServerMenu)/sizeof(menuitem_t),
    &MultiPlayerDef,
    ServerMenu,
    M_DrawGenericMenu,
    27,40,
    0,
};

void M_StartServerMenu(int choise)
{
    if( Playing() )
    {
        M_StartMessage(ALLREADYPLAYING,NULL,MM_NOTHING);
        return;
    }

    StartSplitScreenGame = (choise != 0);
    M_SetupNextMenu(&Serverdef);
}

//===========================================================================
//                            MULTI PLAYER MENU
//===========================================================================
void M_SetupMultiPlayer (int choice);
void M_SetupMultiPlayerBis (int choice);
void M_Splitscreen(int choise);

enum {
    startserver=0,
    connectmultiplayermenu,
    startsplitscreengame,
    setupplayer1,
    setupplayer2,
    multiplayeroptions,
    end_game,
    multiplayer_end
} multiplayer_e;

menuitem_t MultiPlayerMenu[] =
{
    {IT_CALL | IT_PATCH,"M_STSERV","CREATE SERVER",M_StartServerMenu ,'a'},
    {IT_CALL | IT_PATCH,"M_CONNEC","CONNECT SERVER",M_ConnectMenu ,'c'},
    {IT_CALL | IT_PATCH,"M_2PLAYR","TWO PLAYER GAME",M_Splitscreen ,'n'},
    {IT_CALL | IT_PATCH,"M_SETUPA","SETUP PLAYER 1",M_SetupMultiPlayer ,'s'},
    {IT_CALL | IT_PATCH,"M_SETUPB","SETUP PLAYER 2",M_SetupMultiPlayerBis ,'t'},
    {IT_CALL | IT_PATCH,"M_OPTION","OPTIONS",M_NetOption ,'o'},
    {IT_CALL | IT_PATCH,"M_ENDGAM","END GAME",M_EndGame ,'e'}
};

menu_t  MultiPlayerDef =
{
    "M_MULTI",
    "Multiplayer",
    multiplayer_end,
    &MainDef,
    MultiPlayerMenu,
    M_DrawGenericMenu,
    85,40,
    0
};


void M_Splitscreen(int choise)
{
    M_StartServerMenu(1);
}

//===========================================================================
// Seconde mouse config for the splitscreen player
//===========================================================================

menuitem_t  SecondMouseCfgMenu[] =
{
    {IT_STRING | IT_CVAR,0,"Seconde Mouse Serial Port",&cv_mouse2port,0},
    {IT_STRING | IT_CVAR,0,"Use Mouse 2"     ,&cv_usemouse2        ,0},
    {IT_STRING | IT_CVAR
     | IT_CV_SLIDER     ,0,"Mouse2 Speed"    ,&cv_mousesens2       ,0},
    {IT_STRING | IT_CVAR,0,"Always MouseLook",&cv_alwaysfreelook2  ,0},
    {IT_STRING | IT_CVAR,0,"Mouse Move",      &cv_mousemove2       ,0},
    {IT_STRING | IT_CVAR,0,"Invert Mouse2"   ,&cv_invertmouse2     ,0},
    {IT_STRING | IT_CVAR
     | IT_CV_SLIDER     ,0,"Mlook Speed"     ,&cv_mlooksens2       ,0},
};

menu_t  SecondMouseCfgdef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(SecondMouseCfgMenu)/sizeof(menuitem_t),
    &SetupMultiPlayerDef,
    SecondMouseCfgMenu,
    M_DrawGenericMenu,
    27,40,
    0,
};

//===========================================================================
// Second options for the splitscreen player
//===========================================================================

menuitem_t  SecondOptionsMenu[] =
{
    //Hurdler: for now, only autorun is implemented 
    //         others should be implemented as well if we want to be complete
//    {IT_STRING | IT_CVAR,"Messages:"       ,&cv_showmessages2    ,0},
    {IT_STRING | IT_CVAR,0,"Always Run"      ,&cv_autorun2         ,0},
//    {IT_STRING | IT_CVAR,"Crosshair"       ,&cv_crosshair2       ,0},
//    {IT_STRING | IT_CVAR,"Autoaim"         ,&cv_autoaim2         ,0},
//    {IT_STRING | IT_CVAR,"Control per key" ,&cv_controlperkey2   ,0},
};

menu_t  SecondOptionsdef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(SecondOptionsMenu)/sizeof(menuitem_t),
    &SetupMultiPlayerDef,
    SecondOptionsMenu,
    M_DrawGenericMenu,
    27,40,
    0,
};

//===========================================================================
//MULTI PLAYER SETUP MENU
//===========================================================================
void M_DrawSetupMultiPlayerMenu(void);
void M_HandleSetupMultiPlayer(int choice);
void M_SetupControlsMenu(int choice);
boolean M_QuitMultiPlayerMenu(void);

menuitem_t SetupMultiPlayerMenu[] =
{
    {IT_KEYHANDLER | IT_STRING          ,0,"Your name" ,M_HandleSetupMultiPlayer,0},
    {IT_CVAR | IT_STRING | IT_CV_NOPRINT,0,"Your color",&cv_playercolor         ,16},
    {IT_KEYHANDLER | IT_STRING          ,0,"Your skin" ,M_HandleSetupMultiPlayer,96},
    /* this line calls the setup controls for secondary player, only if numitems is > 3 */
    //Hurdler: uncomment this line when other options are available
    //{IT_SUBMENU | IT_WHITESTRING, 0,"Second Player config...", &SecondOptionsdef, 110},
    //... and remove this one
    {IT_STRING | IT_CVAR,0,"Always Run"      ,&cv_autorun2         ,110},
    {IT_CALL | IT_WHITESTRING, 0,"Setup Controls...", M_SetupControlsMenu, 120},
    {IT_SUBMENU | IT_WHITESTRING, 0,"Second Mouse config...", &SecondMouseCfgdef, 130}
};

enum {
    setupmultiplayer_name = 0,
    setupmultiplayer_color,
    setupmultiplayer_skin,
    setupmultiplayer_options2,
    setupmultiplayer_controls,
    setupmultiplayer_mouse2,
    setupmulti_end
};

menu_t  SetupMultiPlayerDef =
{
    "M_MULTI",
    "Multiplayer",
    sizeof(SetupMultiPlayerMenu)/sizeof(menuitem_t),
    &MultiPlayerDef,
    SetupMultiPlayerMenu,
    M_DrawSetupMultiPlayerMenu,
    27,40,
    0,
    M_QuitMultiPlayerMenu
};


#define PLBOXW    8
#define PLBOXH    9

static  int       multi_tics;
static  state_t*  multi_state;

// this is set before entering the MultiPlayer setup menu,
// for either player 1 or 2
static  char       setupm_name[MAXPLAYERNAME+1];
static  player_t*  setupm_player;
static  consvar_t* setupm_cvskin;
static  consvar_t* setupm_cvcolor;
static  consvar_t* setupm_cvname;

void M_SetupMultiPlayer (int choice)
{
    multi_state = &states[mobjinfo[MT_PLAYER].seestate];
    multi_tics = multi_state->tics;
    strcpy(setupm_name, cv_playername.string);

    SetupMultiPlayerDef.numitems = setupmultiplayer_skin +1;      //remove player2 setup controls and mouse2 

    // set for player 1
    SetupMultiPlayerMenu[setupmultiplayer_color].itemaction = &cv_playercolor;
    setupm_player = &players[consoleplayer];
    setupm_cvskin = &cv_skin;
    setupm_cvcolor = &cv_playercolor;
    setupm_cvname = &cv_playername;
    M_SetupNextMenu (&SetupMultiPlayerDef);
}

// start the multiplayer setup menu, for secondary player (splitscreen mode)
void M_SetupMultiPlayerBis (int choice)
{
    multi_state = &states[mobjinfo[MT_PLAYER].seestate];
    multi_tics = multi_state->tics;
    strcpy (setupm_name, cv_playername2.string);
    SetupMultiPlayerDef.numitems = setupmulti_end;          //activate the setup controls for player 2

    // set for splitscreen secondary player
    SetupMultiPlayerMenu[setupmultiplayer_color].itemaction = &cv_playercolor2;
    setupm_player = &players[secondarydisplayplayer];
    setupm_cvskin = &cv_skin2;
    setupm_cvcolor = &cv_playercolor2;
    setupm_cvname = &cv_playername2;
    M_SetupNextMenu (&SetupMultiPlayerDef);
}


// called at splitscreen changes
void M_SwitchSplitscreen(void)
{
// activate setup for player 2
    if (cv_splitscreen.value)
        MultiPlayerMenu[setupplayer2].status = IT_CALL | IT_PATCH;
    else
        MultiPlayerMenu[setupplayer2].status = IT_DISABLED;

    if( MultiPlayerDef.lastOn==setupplayer2)
        MultiPlayerDef.lastOn=setupplayer1; 
}


//
//  Draw the multi player setup menu, had some fun with player anim
//
void M_DrawSetupMultiPlayerMenu(void)
{
    int             mx,my;
    spritedef_t*    sprdef;
    spriteframe_t*  sprframe;
    int             lump;
    patch_t*        patch;
    int             st;
    byte*           colormap;

    mx = SetupMultiPlayerDef.x;
    my = SetupMultiPlayerDef.y;

    // use generic drawer for cursor, items and title
    M_DrawGenericMenu();

    // draw name string
    M_DrawTextBox(mx+90,my-8,MAXPLAYERNAME,1);
    V_DrawString (mx+98,my,0,setupm_name);

    // draw skin string
    V_DrawString (mx+90, my+96,0, setupm_cvskin->string);

    // draw text cursor for name
    if (itemOn==0 &&
        skullAnimCounter<4)   //blink cursor
        V_DrawCharacter(mx+98+V_StringWidth(setupm_name),my,'_' | 0x80);

    // anim the player in the box
    if (--multi_tics<=0)
    {
        st = multi_state->nextstate;
        if (st!=S_NULL)
            multi_state = &states[st];
        multi_tics = multi_state->tics;
        if (multi_tics==-1)
            multi_tics=15;
    }

    // skin 0 is default player sprite
    sprdef    = &skins[R_SkinAvailable(setupm_cvskin->string)].spritedef;
    sprframe  = &sprdef->spriteframes[ multi_state->frame & FF_FRAMEMASK];
    lump  = sprframe->lumppat[0];
    patch = W_CachePatchNum (lump, PU_CACHE);

    // draw box around guy
    M_DrawTextBox(mx+90,my+8, PLBOXW, PLBOXH);

    if (setupm_cvcolor->value==0)
        colormap = colormaps;
    else
        colormap = (byte *) translationtables - 256 + (setupm_cvcolor->value<<8);
    // draw player sprite
    V_DrawMappedPatch (mx+98+(PLBOXW*8/2),my+16+(PLBOXH*8)-8,0,patch,colormap);
}


//
// Handle Setup MultiPlayer Menu
//
void M_HandleSetupMultiPlayer (int choice)
{
    int      l;
    boolean  exitmenu = false;  // exit to previous menu and send name change
    int      myskin;

    myskin  = setupm_cvskin->value;

    switch( choice )
    {
      case KEY_DOWNARROW:
        S_StartSound(NULL,sfx_pstop);
        if (itemOn+1 >= SetupMultiPlayerDef.numitems)
            itemOn = 0;
        else itemOn++;
        break;

      case KEY_UPARROW:
        S_StartSound(NULL,sfx_pstop);
        if (!itemOn)
            itemOn = SetupMultiPlayerDef.numitems-1;
        else itemOn--;
        break;

      case KEY_LEFTARROW:
        if (itemOn==2)       //player skin
        {
            S_StartSound(NULL,sfx_stnmov);
            myskin--;
        }
        break;

      case KEY_RIGHTARROW:
        if (itemOn==2)       //player skin
        {
            S_StartSound(NULL,sfx_stnmov);
            myskin++;
        }
        break;

      case KEY_ENTER:
        S_StartSound(NULL,sfx_stnmov);
        exitmenu = true;
        break;

      case KEY_ESCAPE:
        S_StartSound(NULL,sfx_swtchx);
        exitmenu = true;
        break;

      case KEY_BACKSPACE:
        if ( (l=strlen(setupm_name))!=0 && itemOn==0)
        {
            S_StartSound(NULL,sfx_stnmov);
            setupm_name[l-1]=0;
        }
        break;

      default:
        if (choice < 32 || choice > 127 || itemOn!=0)
            break;
        l = strlen(setupm_name);
        if (l<MAXPLAYERNAME-1)
        {
            S_StartSound(NULL,sfx_stnmov);
            setupm_name[l]=choice;
            setupm_name[l+1]=0;
        }
        break;
    }

    // check skin
    if (myskin <0)
        myskin = numskins-1;
    if (myskin >numskins-1)
        myskin = 0;

    // check skin change
    if (myskin != setupm_player->skin)
        COM_BufAddText ( va("%s \"%s\"",setupm_cvskin->name ,skins[myskin].name));

    if (exitmenu)
    {
        if (currentMenu->prevMenu)
            M_SetupNextMenu (currentMenu->prevMenu);
        else
            M_ClearMenus (true);
    }
}

boolean M_QuitMultiPlayerMenu(void)
{
    int      l;
    // send name if changed
    if (strcmp(setupm_name, setupm_cvname->string))
    {
        // remove trailing whitespaces
        for (l= strlen(setupm_name)-1;
             l>=0 && setupm_name[l]==' '; l--)
            setupm_name[l]=0;
        COM_BufAddText ( va("%s \"%s\"",setupm_cvname->name ,setupm_name));
        
    }
    return true;
}


//===========================================================================
//                              EPISODE SELECT
//===========================================================================

void M_Episode(int choice);

enum
{
    ep1,
    ep2,
    ep3,
    ep4,
    ep5,
    ep_end
} episodes_e;

menuitem_t EpisodeMenu[]=
{
    {IT_CALL | IT_PATCH,"M_EPI1","Knee-Deep in the Dead", M_Episode,'k'},
    {IT_CALL | IT_PATCH,"M_EPI2","The Shores of Hell"   , M_Episode,'t'},
    {IT_CALL | IT_PATCH,"M_EPI3","Inferno"              , M_Episode,'i'},
    {IT_CALL | IT_PATCH,"M_EPI4","Thy Flesh consumed"   , M_Episode,'t'},
    {IT_CALL | IT_PATCH,"M_EPI5","Episode 5"            , M_Episode,'t'},
};

menu_t  EpiDef =
{
    "M_EPISOD",
    "Which Episode?",
    ep_end,             // # of menu items
    &MainDef,           // previous menu
    EpisodeMenu,        // menuitem_t ->
    M_DrawGenericMenu,  // drawing routine ->
    48,63,              // x,y
    ep1                 // lastOn, flags
};

//
//      M_Episode
//
int     epi;

void M_Episode(int choice)
{
    if ( (gamemode == shareware)
         && choice)
    {
        M_SetupNextMenu(&ReadDef1);
        M_StartMessage(SWSTRING,NULL,MM_NOTHING);
        return;
    }

    // Yet another hack...
    if ( (gamemode == registered)
         && (choice > 2))
    {
        I_Error("M_Episode: 4th episode requires UltimateDOOM\n");
        choice = 0;
    }

    epi = choice;
    M_SetupNextMenu(&NewDef);
}


//===========================================================================
//                           NEW GAME FOR SINGLE PLAYER
//===========================================================================
void M_DrawNewGame(void);

void M_ChooseSkill(int choice);

enum
{
    killthings,
    toorough,
    hurtme,
    violence,
    nightmare,
    newg_end
} newgame_e;

menuitem_t NewGameMenu[]=
{
    {IT_CALL | IT_PATCH,"M_JKILL","I'm too young to die.",M_ChooseSkill, 'i'},
    {IT_CALL | IT_PATCH,"M_ROUGH","Hey, not too rough."  ,M_ChooseSkill, 'h'},
    {IT_CALL | IT_PATCH,"M_HURT" ,"Hurt me plenty."      ,M_ChooseSkill, 'h'},
    {IT_CALL | IT_PATCH,"M_ULTRA","Ultra-Violence"       ,M_ChooseSkill, 'u'},
    {IT_CALL | IT_PATCH,"M_NMARE","Nightmare!"           ,M_ChooseSkill, 'n'}
};

menu_t  NewDef =
{
    "M_NEWG",
    "NEW GAME",
    newg_end,           // # of menu items
    &EpiDef,            // previous menu
    NewGameMenu,        // menuitem_t ->
    M_DrawNewGame,      // drawing routine ->
    48,63,              // x,y
    violence            // lastOn
};

void M_DrawNewGame(void)
{
    patch_t* p;

    //faB: testing with glide
    p = W_CachePatchName("M_SKILL",PU_CACHE);
    V_DrawScaledPatch ((BASEVIDWIDTH-p->width)/2,38,0,p);

    //    V_DrawScaledPatch (54,38,0,W_CachePatchName("M_SKILL",PU_CACHE));
    M_DrawGenericMenu();
}

void M_NewGame(int choice)
{
    if( Playing() )
    {
        M_StartMessage(ALLREADYPLAYING,NULL,MM_NOTHING);
        return;
    }

    if (netgame)
    {
        M_StartMessage(NEWGAME,NULL,MM_NOTHING);
        return;
    }

    if ( gamemode == commercial )
        M_SetupNextMenu(&NewDef);
    else
        M_SetupNextMenu(&EpiDef);

    StartSplitScreenGame=false;
}

void M_VerifyNightmare(int ch);

void M_ChooseSkill(int choice)
{
    if (choice == nightmare)
    {
        M_StartMessage(NIGHTMARE,M_VerifyNightmare,MM_YESNO);
        return;
    }

    G_DeferedInitNew(choice, G_BuildMapName(epi+1,1),StartSplitScreenGame);
    M_ClearMenus (true);
}

void M_VerifyNightmare(int ch)
{
    if (ch != 'y')
        return;

    G_DeferedInitNew (nightmare, G_BuildMapName(epi+1,1),StartSplitScreenGame);
    M_ClearMenus (true);
}

//===========================================================================
//                             OPTIONS MENU
//===========================================================================
//
// M_Options
//

//added:10-02-98: note: alphaKey member is the y offset
menuitem_t OptionsMenu[]=
{
    {IT_STRING | IT_CVAR,0,"Messages:"       ,&cv_showmessages    ,0},
    {IT_STRING | IT_CVAR,0,"Always Run"      ,&cv_autorun         ,0},
    {IT_STRING | IT_CVAR,0,"Crosshair"       ,&cv_crosshair       ,0},
//    {IT_STRING | IT_CVAR,0,"Crosshair scale" ,&cv_crosshairscale  ,0},
    {IT_STRING | IT_CVAR,0,"Autoaim"         ,&cv_autoaim         ,0},
    {IT_STRING | IT_CVAR,0,"Control per key" ,&cv_controlperkey   ,0},
    {IT_STRING | IT_CVAR,0,"Random sound pitch",&cv_rndsoundpitch ,0},

    {IT_SUBMENU | IT_WHITESTRING,0,"Server options...",&ServerOptionsDef  ,70},
    {IT_CALL    | IT_WHITESTRING,0,"Game Options..."  ,M_GameOption       ,0},
    {IT_SUBMENU | IT_WHITESTRING,0,"Sound Volume..."  ,&SoundDef          ,0},
    {IT_SUBMENU | IT_WHITESTRING,0,"Video Options..." ,&VideoOptionsDef   ,0},
    {IT_SUBMENU | IT_WHITESTRING,0,"Mouse Options..." ,&MouseOptionsDef   ,0},
    {IT_CALL    | IT_WHITESTRING,0,"Setup Controls...",M_SetupControlsMenu,0}
};

menu_t  OptionsDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(OptionsMenu)/sizeof(menuitem_t),
    &MainDef,
    OptionsMenu,
    M_DrawGenericMenu,
    60,40,
    0
};

//
//  A smaller 'Thermo', with range given as percents (0-100)
//
void M_DrawSlider (int x, int y, int range)
{
    int i;

    if (range < 0)
        range = 0;
    if (range > 100)
        range = 100;

    V_DrawScaledPatch (x-8, y, 0, W_CachePatchName( "M_SLIDEL" ,PU_CACHE) );

    for (i=0 ; i<SLIDER_RANGE ; i++)
        V_DrawScaledPatch (x+i*8, y, 0,
                           W_CachePatchName( "M_SLIDEM" ,PU_CACHE) );

    V_DrawScaledPatch (x+SLIDER_RANGE*8, y, 0,
                       W_CachePatchName( "M_SLIDER" ,PU_CACHE) );

    // draw the slider cursor
    V_DrawMappedPatch (x + ((SLIDER_RANGE-1)*8*range)/100, y, 0,
                       W_CachePatchName( "M_SLIDEC" ,PU_CACHE),
                       whitemap);
}

//===========================================================================
//                        Video OPTIONS MENU
//===========================================================================

//added:10-02-98: note: alphaKey member is the y offset
menuitem_t VideoOptionsMenu[]=
{
    {IT_STRING | IT_SUBMENU,0, "Video Modes..."   , &VidModeDef       , 0},
#ifndef __DJGPP__
    {IT_STRING | IT_CVAR,0,    "Fullscreen"       , &cv_fullscreen    , 0},
#endif
    {IT_STRING | IT_CVAR
     | IT_CV_SLIDER     ,0,    "Brightness"       , &cv_usegamma      , 0},
    {IT_STRING | IT_CVAR
     | IT_CV_SLIDER     ,0,    "Screen Size"      , &cv_viewsize      , 0},
    {IT_STRING | IT_CVAR,0,    "Scale Status Bar" , &cv_scalestatusbar, 0},
    {IT_STRING | IT_CVAR,0,    "Translucency"     , &cv_translucency  , 0},
    {IT_STRING | IT_CVAR,0,    "Splats"           , &cv_splats        , 0},
    {IT_STRING | IT_CVAR,0,    "Max splats"       , &cv_maxsplats     , 0},
    {IT_STRING | IT_CVAR,0,    "Screens Link"     , &cv_screenslink   , 0},
    {IT_STRING | IT_CVAR,0,    "Wait Retrace"     , &cv_vidwait       , 0},
#ifdef HWRENDER
    //17/10/99: added by Hurdler
    {IT_CALL|IT_WHITESTRING,0, "3D Card Options...", M_OpenGLOption    ,100},
#endif
};

menu_t  VideoOptionsDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(VideoOptionsMenu)/sizeof(menuitem_t),
    &OptionsDef,
    VideoOptionsMenu,
    M_DrawGenericMenu,
    60,40,
    0
};

//===========================================================================
//                        Mouse OPTIONS MENU
//===========================================================================

//added:24-03-00: note: alphaKey member is the y offset
menuitem_t MouseOptionsMenu[]=
{
    {IT_STRING | IT_CVAR,0,"Use Mouse",        &cv_usemouse        ,0},
    {IT_STRING | IT_CVAR,0,"Always MouseLook", &cv_alwaysfreelook  ,0},
    {IT_STRING | IT_CVAR,0,"Mouse Move"      , &cv_mousemove       ,0},
    {IT_STRING | IT_CVAR,0,"Invert Mouse"    , &cv_invertmouse     ,0},
    {IT_STRING | IT_CVAR
     | IT_CV_SLIDER     ,0,"Mouse Speed"     , &cv_mousesens       ,0},
    {IT_STRING | IT_CVAR
     | IT_CV_SLIDER     ,0,"Mlook Speed"     , &cv_mlooksens       ,0}
#ifdef __MACOS__
        ,{IT_CALL   | IT_WHITESTRING,0,"Configure Input Sprocket..."  ,macConfigureInput     ,60}
#endif
};

menu_t  MouseOptionsDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(MouseOptionsMenu)/sizeof(menuitem_t),
    &OptionsDef,
    MouseOptionsMenu,
    M_DrawGenericMenu,
    60,40,
    0
};

//===========================================================================
//                        Game OPTIONS MENU
//===========================================================================

menuitem_t GameOptionsMenu[]=
{
    {IT_STRING | IT_CVAR,0,"Item Respawn"        ,&cv_itemrespawn        ,0},
    {IT_STRING | IT_CVAR,0,"Item Respawn time"   ,&cv_itemrespawntime    ,0},
    {IT_STRING | IT_CVAR,0,"Monster Respawn"     ,&cv_respawnmonsters    ,0},
    {IT_STRING | IT_CVAR,0,"Monster Respawn time",&cv_respawnmonsterstime,0},
    {IT_STRING | IT_CVAR,0,"Fast Monsters"       ,&cv_fastmonsters       ,0},
		{IT_STRING | IT_CVAR,0,"Predicting Monsters" ,&cv_predictingmonsters ,0},	//added by AC for predmonsters
    {IT_STRING | IT_CVAR,0,"Gravity"             ,&cv_gravity            ,0},
    {IT_STRING | IT_CVAR,0,"Solid corpse"        ,&cv_solidcorpse        ,0},
    {IT_STRING | IT_CVAR,0,"BloodTime"           ,&cv_bloodtime          ,0},
    {IT_CALL   | IT_WHITESTRING,0,"Network Options..."  ,M_NetOption     ,110}
};

menu_t  GameOptionDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(GameOptionsMenu)/sizeof(menuitem_t),
    &OptionsDef,
    GameOptionsMenu,
    M_DrawGenericMenu,
    60,40,
    0
};

void M_GameOption(int choice)
{
    if(!server)
    {
        M_StartMessage("You are not the server\nYou can't change the options\n",NULL,MM_NOTHING);
        return;
    }
    M_SetupNextMenu(&GameOptionDef);
}

//===========================================================================
//                        Network OPTIONS MENU
//===========================================================================

menuitem_t NetOptionsMenu[]=
{
    {IT_STRING | IT_CVAR,0,"Allow Jump"      ,&cv_allowjump       ,0},
    //SoM: 3/28/2000
    {IT_STRING | IT_CVAR,0,"Allow Rocket Jump",&cv_allowrocketjump,0},
    {IT_STRING | IT_CVAR,0,"Allow autoaim"   ,&cv_allowautoaim    ,0},
    {IT_STRING | IT_CVAR,0,"Allow turbo"     ,&cv_allowturbo      ,0},
    {IT_STRING | IT_CVAR,0,"Allow exitlevel" ,&cv_allowexitlevel  ,0},
    {IT_STRING | IT_CVAR,0,"Allow join player",&cv_allownewplayer ,0},
    {IT_STRING | IT_CVAR,0,"Teamplay"        ,&cv_teamplay        ,0},
    {IT_STRING | IT_CVAR,0,"TeamDamage"      ,&cv_teamdamage      ,0},
    {IT_STRING | IT_CVAR,0,"Fraglimit"       ,&cv_fraglimit       ,0},
    {IT_STRING | IT_CVAR,0,"Timelimit"       ,&cv_timelimit       ,0},
    {IT_STRING | IT_CVAR,0,"Deathmatch Type" ,&cv_deathmatch      ,0},
    {IT_STRING | IT_CVAR,0,"Frag's Weapon Falling", &cv_fragsweaponfalling, 0},
    {IT_STRING | IT_CVAR,0,"Maxplayers"      ,&cv_maxplayers      ,0},
    {IT_CALL   | IT_WHITESTRING,0,"Games Options..." ,M_GameOption,0},
};

menu_t  NetOptionDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(NetOptionsMenu)/sizeof(menuitem_t),
    &MultiPlayerDef,
    NetOptionsMenu,
    M_DrawGenericMenu,
    60,40,
    0
};

void M_NetOption(int choice)
{
    if(!server)
    {
        M_StartMessage("You are not the server\nYou can't change the options\n",NULL,MM_NOTHING);
        return;
    }
    M_SetupNextMenu(&NetOptionDef);
}

//===========================================================================
//                        Server OPTIONS MENU
//===========================================================================
menuitem_t ServerOptionsMenu[]=
{
    {IT_STRING | IT_CVAR,0, "Internet server",     &cv_internetserver   ,  0},
    {IT_STRING | IT_CVAR
        | IT_CV_STRING  ,0, "Master server",       &cv_masterserver     ,  0},
    {IT_STRING | IT_CVAR
        | IT_CV_STRING  ,0, "Server name",         &cv_servername       ,  0},
};

menu_t  ServerOptionsDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(ServerOptionsMenu)/sizeof(menuitem_t),
    &OptionsDef,
    ServerOptionsMenu,
    M_DrawGenericMenu,
    28,40,
    0
};

//===========================================================================
//                          Read This! MENU 1
//===========================================================================

void M_DrawReadThis1(void);
void M_DrawReadThis2(void);

enum
{
    rdthsempty1,
    read1_end
} read_e;

menuitem_t ReadMenu1[] =
{
    {IT_SUBMENU | IT_NOTHING,0,"",&ReadDef2,0}
};

menu_t  ReadDef1 =
{
    NULL,
    NULL,
    read1_end,
    &MainDef,
    ReadMenu1,
    M_DrawReadThis1,
    280,185,
    0
};

//
// Read This Menus
// Had a "quick hack to fix romero bug"
//
void M_DrawReadThis1(void)
{
    switch ( gamemode )
    {
      case commercial:
        V_DrawScaledPatch (0,0,0,W_CachePatchName("HELP",PU_CACHE));
        break;
      case shareware:
      case registered:
      case retail:
        V_DrawScaledPatch (0,0,0,W_CachePatchName("HELP1",PU_CACHE));
        break;
      case heretic:
        V_DrawRawScreen(0,0,W_GetNumForName("HELP1"), 320, 200);
        break;
      default:
        break;
    }
    return;
}

//===========================================================================
//                          Read This! MENU 2
//===========================================================================

enum
{
    rdthsempty2,
    read2_end
} read_e2;

menuitem_t ReadMenu2[]=
{
    {IT_SUBMENU | IT_NOTHING,0,"",&MainDef,0}
};

menu_t  ReadDef2 =
{
    NULL,
    NULL,
    read2_end,
    &ReadDef1,
    ReadMenu2,
    M_DrawReadThis2,
    330,175,
    0
};


//
// Read This Menus - optional second page.
//
void M_DrawReadThis2(void)
{
    switch ( gamemode )
    {
      case retail:
      case commercial:
        // This hack keeps us from having to change menus.
        V_DrawScaledPatch (0,0,0,W_CachePatchName("CREDIT",PU_CACHE));
        break;
      case shareware:
      case registered:
        V_DrawScaledPatch (0,0,0,W_CachePatchName("HELP2",PU_CACHE));
        break;
      case heretic :
        V_DrawRawScreen(0,0,W_GetNumForName("HELP2"), 320, 200);
      default:
        break;
    }
    return;
}

//===========================================================================
//                        SOUND VOLUME MENU
//===========================================================================
void M_DrawSound(void);

void M_SfxVol(int choice);
void M_MusicVol(int choice);
void M_CDAudioVol (int choice);

enum
{
    sfx_vol,
    sfx_empty1,
    music_vol,
    sfx_empty2,
    //cdaudio_vol,
    //sfx_empty3,
    sound_end
} sound_e;

menuitem_t SoundMenu[]=
{
    {IT_CVARMAX   | IT_PATCH ,"M_SFXVOL","Sound Volume",&cv_soundvolume  ,'s'},
    {IT_BIGSLIDER | IT_SPACE ,NULL      ,NULL          ,&cv_soundvolume      },
    {IT_CVARMAX   | IT_PATCH ,"M_MUSVOL","Music Volume",&cv_musicvolume  ,'m'},
    {IT_BIGSLIDER | IT_SPACE ,NULL      ,NULL          ,&cv_musicvolume      },
    //{IT_CVARMAX   | IT_PATCH ,"M_CDVOL" ,"CD Volume"   ,&cd_volume       ,'c'},
    //{IT_BIGSLIDER | IT_SPACE ,NULL      ,NULL          ,&cd_volume           },
};

menu_t  SoundDef =
{
    "M_SVOL",
    "Sound Volume",
    sound_end,
    &OptionsDef,
    SoundMenu,
    M_DrawGenericMenu,
    80,50,
    0
};


//===========================================================================
//                          CONTROLS MENU
//===========================================================================
void M_DrawControl(void);               // added 3-1-98
void M_ChangeControl(int choice);

//
// this is the same for all control pages
//
menuitem_t ControlMenu[]=
{
    {IT_CALL | IT_STRING2,0,"Fire"        ,M_ChangeControl,gc_fire       },
    {IT_CALL | IT_STRING2,0,"Use/Open"    ,M_ChangeControl,gc_use        },
    {IT_CALL | IT_STRING2,0,"Jump"        ,M_ChangeControl,gc_jump       },
    {IT_CALL | IT_STRING2,0,"Forward"     ,M_ChangeControl,gc_forward    },
    {IT_CALL | IT_STRING2,0,"Backpedal"   ,M_ChangeControl,gc_backward   },
    {IT_CALL | IT_STRING2,0,"Turn Left"   ,M_ChangeControl,gc_turnleft   },
    {IT_CALL | IT_STRING2,0,"Turn Right"  ,M_ChangeControl,gc_turnright  },
    {IT_CALL | IT_STRING2,0,"Run"         ,M_ChangeControl,gc_speed      },
    {IT_CALL | IT_STRING2,0,"Strafe On"   ,M_ChangeControl,gc_strafe     },
    {IT_CALL | IT_STRING2,0,"Strafe Left" ,M_ChangeControl,gc_strafeleft },
    {IT_CALL | IT_STRING2,0,"Strafe Right",M_ChangeControl,gc_straferight},
    {IT_CALL | IT_STRING2,0,"Look Up"     ,M_ChangeControl,gc_lookup     },
    {IT_CALL | IT_STRING2,0,"Look Down"   ,M_ChangeControl,gc_lookdown   },
    {IT_CALL | IT_STRING2,0,"Center View" ,M_ChangeControl,gc_centerview },
    {IT_CALL | IT_STRING2,0,"Mouselook"   ,M_ChangeControl,gc_mouseaiming},

    {IT_SUBMENU | IT_WHITESTRING,0,"next" ,&ControlDef2,128}
};

menu_t  ControlDef =
{
    "M_CONTRO",
    "Setup Controles",
    sizeof(ControlMenu)/sizeof(menuitem_t),
    &OptionsDef,
    ControlMenu,
    M_DrawControl,
    50,40,
    0
};

//
//  Controls page 1
//
menuitem_t ControlMenu2[]=
{
  {IT_CALL | IT_STRING2,0,"Fist/Chainsaw"  ,M_ChangeControl,gc_weapon1},
  {IT_CALL | IT_STRING2,0,"Pistol"         ,M_ChangeControl,gc_weapon2},
  {IT_CALL | IT_STRING2,0,"Shotgun/Double" ,M_ChangeControl,gc_weapon3},
  {IT_CALL | IT_STRING2,0,"Chaingun"       ,M_ChangeControl,gc_weapon4},
  {IT_CALL | IT_STRING2,0,"Rocket Launcher",M_ChangeControl,gc_weapon5},
  {IT_CALL | IT_STRING2,0,"Plasma rifle"   ,M_ChangeControl,gc_weapon6},
  {IT_CALL | IT_STRING2,0,"BFG"            ,M_ChangeControl,gc_weapon7},
  {IT_CALL | IT_STRING2,0,"Chainsaw"       ,M_ChangeControl,gc_weapon8},
  {IT_CALL | IT_STRING2,0,"Previous Weapon",M_ChangeControl,gc_prevweapon},
  {IT_CALL | IT_STRING2,0,"Next Weapon"    ,M_ChangeControl,gc_nextweapon},
  {IT_CALL | IT_STRING2,0,"Best Weapon"    ,M_ChangeControl,gc_bestweapon},
  {IT_CALL | IT_STRING2,0,"Talk key"       ,M_ChangeControl,gc_talkkey},
  {IT_CALL | IT_STRING2,0,"Rankings/Scores",M_ChangeControl,gc_scores },
  {IT_CALL | IT_STRING2,0,"Console"        ,M_ChangeControl,gc_console},
  {IT_CALL | IT_STRING2,0,"Inventory Left" ,M_ChangeControl,gc_invprev},  
  {IT_CALL | IT_STRING2,0,"Inventory Right",M_ChangeControl,gc_invnext},
  {IT_CALL | IT_STRING2,0,"Inventory Use"  ,M_ChangeControl,gc_invuse },
  {IT_CALL | IT_STRING2,0,"Fly down"       ,M_ChangeControl,gc_flydown},
                        
  {IT_SUBMENU | IT_WHITESTRING,0,"next"    ,&ControlDef,140}
};

menu_t  ControlDef2 =
{
    "M_CONTRO",
    "Setup Controles",
    sizeof(ControlMenu2)/sizeof(menuitem_t),
    &OptionsDef,
    ControlMenu2,
    M_DrawControl,
    50,40,
    0
};


//
// Start the controls menu, setting it up for either the console player,
// or the secondary splitscreen player
//
static  boolean setupcontrols_secondaryplayer;
static  int   (*setupcontrols)[2];  // pointer to the gamecontrols of the player being edited
void M_SetupControlsMenu (int choice)
{
    // set the gamecontrols to be edited
    if (choice == setupmultiplayer_controls) {
        setupcontrols_secondaryplayer = true;
        setupcontrols = gamecontrolbis;     // was called from secondary player's multiplayer setup menu
    }
    else {
        setupcontrols_secondaryplayer = false;
        setupcontrols = gamecontrol;        // was called from main Options (for console player, then)
    }
    currentMenu->lastOn = itemOn;
    M_SetupNextMenu(&ControlDef);
}


//
//  Draws the Customise Controls menu
//
void M_DrawControl(void)
{
    char     tmp[50];
    int      i;
    int      keys[2];

    // draw title, strings and submenu
    M_DrawGenericMenu();

    M_CentreText (ControlDef.y-12,
        (setupcontrols_secondaryplayer ? "SET CONTROLS FOR SECONDARY PLAYER" :
                                         "PRESS ENTER TO CHANGE, BACKSPACE TO CLEAR") );

    for(i=0;i<currentMenu->numitems;i++)
    {
        if (currentMenu->menuitems[i].status!=IT_CONTROL)
            continue;

        keys[0] = setupcontrols[currentMenu->menuitems[i].alphaKey][0];
        keys[1] = setupcontrols[currentMenu->menuitems[i].alphaKey][1];

        tmp[0]='\0';
        if (keys[0] == KEY_NULL && keys[1] == KEY_NULL)
        {
            strcpy(tmp, "---");
        }
        else
        {
            if( keys[0] != KEY_NULL )
                strcat (tmp, G_KeynumToString (keys[0]));

            if( keys[0] != KEY_NULL && keys[1] != KEY_NULL )
                strcat(tmp," or ");

            if( keys[1] != KEY_NULL )
                strcat (tmp, G_KeynumToString (keys[1]));


        }
        V_DrawString(ControlDef.x+220-V_StringWidth(tmp), ControlDef.y + i*8,V_WHITEMAP, tmp);
    }

}

static int controltochange;

void M_ChangecontrolResponse(event_t* ev)
{
    int        control;
    int        found;
    int        ch=ev->data1;

    // ESCAPE cancels
    if (ch!=KEY_ESCAPE && ch!=KEY_PAUSE)
    {

        switch (ev->type)
        {
          // ignore mouse/joy movements, just get buttons
          case ev_mouse:
               ch = KEY_NULL;      // no key
            break;
          case ev_joystick:
               ch = KEY_NULL;      // no key
            break;

          // keypad arrows are converted for the menu in cursor arrows
          // so use the event instead of ch
          case ev_keydown:
            ch = ev->data1;
            break;

          default:
            break;
        }

        control = controltochange;

        // check if we already entered this key
        found = -1;
        if (setupcontrols[control][0]==ch)
            found = 0;
        else
        if (setupcontrols[control][1]==ch)
            found = 1;
        if (found>=0)
        {
            // replace mouse and joy clicks by double clicks
            if (ch>=KEY_MOUSE1 && ch<=KEY_MOUSE1+MOUSEBUTTONS)
                setupcontrols[control][found] = ch-KEY_MOUSE1+KEY_DBLMOUSE1;
            else
              if (ch>=KEY_JOY1 && ch<=KEY_JOY1+JOYBUTTONS)
                setupcontrols[control][found] = ch-KEY_JOY1+KEY_DBLJOY1;
        }
        else
        {
            // check if change key1 or key2, or replace the two by the new
            found = 0;
            if (setupcontrols[control][0] == KEY_NULL)
                found++;
            if (setupcontrols[control][1] == KEY_NULL)
                found++;
            if (found==2)
            {
                found = 0;
                setupcontrols[control][1] = KEY_NULL;  //replace key 1 ,clear key2
            }
            G_CheckDoubleUsage(ch);
            setupcontrols[control][found] = ch;
        }

    }

    M_StopMessage(0);
}

void M_ChangeControl(int choice)
{
    static char tmp[55];

    controltochange = currentMenu->menuitems[choice].alphaKey;
    sprintf (tmp,"Hit the new key for\n%s\nESC for Cancel",currentMenu->menuitems[choice].text);

    M_StartMessage (tmp,M_ChangecontrolResponse,MM_EVENTHANDLER);
}

//===========================================================================
//                        VIDEO MODE MENU
//===========================================================================
void M_DrawVideoMode(void);             //added:30-01-98:

void M_HandleVideoMode (int ch);

menuitem_t VideoModeMenu[]=
{
    {IT_KEYHANDLER | IT_NOTHING, 0, "", M_HandleVideoMode, '\0'},     // dummy menuitem for the control func
};


menu_t  VidModeDef =
{
    "M_VIDEO",
    "Video Mode",
    1,                  // # of menu items
    //sizeof(VideoModeMenu)/sizeof(menuitem_t),
    &OptionsDef,        // previous menu
    VideoModeMenu,      // menuitem_t ->
    M_DrawVideoMode,    // drawing routine ->
    48,36,              // x,y
    0                   // lastOn
};

//added:30-01-98:
#define MAXCOLUMNMODES   10     //max modes displayed in one column
#define MAXMODEDESCS     (MAXCOLUMNMODES*3)

// shhh... what am I doing... nooooo!
static int vidm_testingmode=0;
static int vidm_previousmode;
static int vidm_current=0;
static int vidm_nummodes;
static int vidm_column_size;

typedef struct
{
    int     modenum;    //video mode number in the vidmodes list
    char    *desc;      //XXXxYYY
    int     iscur;      //1 if it is the current active mode
} modedesc_t;

static modedesc_t   modedescs[MAXMODEDESCS];


//
// Draw the video modes list, a-la-Quake
//
void M_DrawVideoMode(void)
{
    int     i,j,dup,row,col,nummodes;
    char    *desc;
    char    temp[80];

    // draw tittle
    M_DrawMenuTitle();

#ifdef LINUX
    VID_PrepareModeList(); // FIXME: hack
#endif
    vidm_nummodes = 0;
    nummodes = VID_NumModes ();

#ifdef __WIN32__
    //faB: clean that later : skip windowed mode 0, video modes menu only shows
    //     FULL SCREEN modes
    if (nummodes<1) {
        // put the windowed mode so that there is at least one mode
        modedescs[0].modenum = 0;
        modedescs[0].desc = VID_GetModeName (0);
        modedescs[0].iscur = 1;
        vidm_nummodes = 1;
    }
    for (i=1 ; i<=nummodes && vidm_nummodes<MAXMODEDESCS ; i++)
#else
    // DOS does not skip mode 0, because mode 0 is ALWAYS present
    for (i=0 ; i<nummodes && vidm_nummodes<MAXMODEDESCS ; i++)
#endif
    {
        desc = VID_GetModeName (i);
        if (desc)
        {
            dup = 0;

            //when a resolution exists both under VGA and VESA, keep the
            // VESA mode, which is always a higher modenum
            for (j=0 ; j<vidm_nummodes ; j++)
            {
                if (!strcmp (modedescs[j].desc, desc))
                {
                    //mode(0): 320x200 is always standard VGA, not vesa
                    if (modedescs[j].modenum != 0)
                    {
                        modedescs[j].modenum = i;
                        dup = 1;

                        if (i == vid.modenum)
                            modedescs[j].iscur = 1;
                    }
                    else
                    {
                        dup = 1;
                    }

                    break;
                }
            }

            if (!dup)
            {
                modedescs[vidm_nummodes].modenum = i;
                modedescs[vidm_nummodes].desc = desc;
                modedescs[vidm_nummodes].iscur = 0;

                if (i == vid.modenum)
                    modedescs[vidm_nummodes].iscur = 1;

                vidm_nummodes++;
            }
        }
    }

    vidm_column_size = (vidm_nummodes+2) / 3;


    row = 16;
    col = VidModeDef.y;
    for(i=0; i<vidm_nummodes; i++)
    {
        V_DrawString (row, col, modedescs[i].iscur ? V_WHITEMAP : 0, modedescs[i].desc);

        col += 8;
        if((i % vidm_column_size) == (vidm_column_size-1))
        {
            row += 8*13;
            col = 36;
        }
    }

    if (vidm_testingmode>0)
    {
        sprintf(temp, "TESTING MODE %s", modedescs[vidm_current].desc );
        M_CentreText(VidModeDef.y+80+24, temp );
        M_CentreText(VidModeDef.y+90+24, "Please wait 5 seconds..." );
    }
    else
    {
        M_CentreText(VidModeDef.y+60+24,"Press ENTER to set mode");

        M_CentreText(VidModeDef.y+70+24,"T to test mode for 5 seconds");

        sprintf(temp, "D to make %s the default", VID_GetModeName(vid.modenum));
        M_CentreText(VidModeDef.y+80+24,temp);

        sprintf(temp, "Current default is %dx%d (%d bits)", cv_scr_width.value, cv_scr_height.value, cv_scr_depth.value);
        M_CentreText(VidModeDef.y+90+24,temp);

        M_CentreText(VidModeDef.y+100+24,"Press ESC to exit");
    }

// Draw the cursor for the VidMode menu
    if (skullAnimCounter<4)    //use the Skull anim counter to blink the cursor
    {
        i = 16 - 10 + ((vidm_current / vidm_column_size)*8*13);
        j = VidModeDef.y + ((vidm_current % vidm_column_size)*8);
        V_DrawCharacter( i, j, '*' | 0x80);
    }
}


//added:30-01-98: special menuitem key handler for video mode list
void M_HandleVideoMode (int ch)
{
    if (vidm_testingmode>0)
    {
       // change back to the previous mode quickly
       if (ch==KEY_ESCAPE)
       {
           setmodeneeded = vidm_previousmode+1;
           vidm_testingmode = 0;
       }
       return;
    }

    switch( ch )
    {
      case KEY_DOWNARROW:
        S_StartSound(NULL,sfx_pstop);
        vidm_current++;
        if (vidm_current>=vidm_nummodes)
            vidm_current = 0;
        break;

      case KEY_UPARROW:
        S_StartSound(NULL,sfx_pstop);
        vidm_current--;
        if (vidm_current<0)
            vidm_current = vidm_nummodes-1;
        break;

      case KEY_LEFTARROW:
        S_StartSound(NULL,sfx_pstop);
        vidm_current -= vidm_column_size;
        if (vidm_current<0)
            vidm_current = (vidm_column_size*3) + vidm_current;
        if (vidm_current>=vidm_nummodes)
            vidm_current = vidm_nummodes-1;
        break;

      case KEY_RIGHTARROW:
        S_StartSound(NULL,sfx_pstop);
        vidm_current += vidm_column_size;
        if (vidm_current>=(vidm_column_size*3))
            vidm_current %= vidm_column_size;
        if (vidm_current>=vidm_nummodes)
            vidm_current = vidm_nummodes-1;
        break;

      case KEY_ENTER:
        S_StartSound(NULL,sfx_pstop);
        if (!setmodeneeded) //in case the previous setmode was not finished
            setmodeneeded = modedescs[vidm_current].modenum+1;
        break;

      case KEY_ESCAPE:      //this one same as M_Responder
        S_StartSound(NULL,sfx_swtchx);

        if (currentMenu->prevMenu)
            M_SetupNextMenu (currentMenu->prevMenu);
        else
            M_ClearMenus (true);
        return;

      case 'T':
      case 't':
        S_StartSound(NULL,sfx_swtchx);
        vidm_testingmode = TICRATE*5;
        vidm_previousmode = vid.modenum;
        if (!setmodeneeded) //in case the previous setmode was not finished
            setmodeneeded = modedescs[vidm_current].modenum+1;
        return;

      case 'D':
      case 'd':
        // current active mode becomes the default mode.
        S_StartSound(NULL,sfx_swtchx);
        SCR_SetDefaultMode ();
        return;

      default:
        break;
    }

}


//===========================================================================
//LOAD GAME MENU
//===========================================================================
void M_DrawLoad(void);

void M_LoadSelect(int choice);

enum
{
    load1,
    load2,
    load3,
    load4,
    load5,
    load6,
    load_end
} load_e;

menuitem_t LoadGameMenu[]=
{
    {IT_CALL | IT_NOTHING,"",0, M_LoadSelect,'1'},
    {IT_CALL | IT_NOTHING,"",0, M_LoadSelect,'2'},
    {IT_CALL | IT_NOTHING,"",0, M_LoadSelect,'3'},
    {IT_CALL | IT_NOTHING,"",0, M_LoadSelect,'4'},
    {IT_CALL | IT_NOTHING,"",0, M_LoadSelect,'5'},
    {IT_CALL | IT_NOTHING,"",0, M_LoadSelect,'6'}
};

menu_t  LoadDef =
{
    "M_LOADG",
    "Load Game",
    load_end,
    &MainDef,
    LoadGameMenu,
    M_DrawLoad,
    80,54,
    0
};

//
// M_LoadGame & Cie.
//
void M_DrawLoad(void)
{
    int             i;

    M_DrawGenericMenu();

    for (i = 0;i < load_end; i++)
    {
        M_DrawSaveLoadBorder(LoadDef.x,LoadDef.y+LINEHEIGHT*i);
        V_DrawString(LoadDef.x,LoadDef.y+LINEHEIGHT*i,0,savegamestrings[i]);
    }
}

//
// User wants to load this game
//
void M_LoadSelect(int choice)
{
    G_LoadGame (choice);
    M_ClearMenus (true);
}

//
// M_ReadSaveStrings
//  read the strings from the savegame files
//  and put it in savegamestrings global variable
//
void M_ReadSaveStrings(void)
{
    int     handle;
    int     count;
    int     i;
    char    name[256];

    for (i = 0;i < load_end;i++)
    {
        sprintf(name,savegamename,i);

        handle = open (name, O_RDONLY | 0, 0666);
        if (handle == -1)
        {
            strcpy(&savegamestrings[i][0],EMPTYSTRING);
            LoadGameMenu[i].status = 0;
            continue;
        }
        count = read (handle, &savegamestrings[i], SAVESTRINGSIZE);
        close (handle);
        LoadGameMenu[i].status = 1;
    }
}

//
// Selected from DOOM menu
//
void M_LoadGame (int choice)
{
// change can't load message to can't load in server mode
    if (netgame && !server)
    {
        M_StartMessage(LOADNET,NULL,MM_NOTHING);
        return;
    }

    M_SetupNextMenu(&LoadDef);
    M_ReadSaveStrings();
}


//===========================================================================
//                                SAVE GAME MENU
//===========================================================================
void M_DrawSave(void);

void M_SaveSelect(int choice);

menuitem_t SaveMenu[]=
{
    {IT_CALL | IT_NOTHING,"",0, M_SaveSelect,'1'},
    {IT_CALL | IT_NOTHING,"",0, M_SaveSelect,'2'},
    {IT_CALL | IT_NOTHING,"",0, M_SaveSelect,'3'},
    {IT_CALL | IT_NOTHING,"",0, M_SaveSelect,'4'},
    {IT_CALL | IT_NOTHING,"",0, M_SaveSelect,'5'},
    {IT_CALL | IT_NOTHING,"",0, M_SaveSelect,'6'}
};

menu_t  SaveDef =
{
    "M_SAVEG",
    "Save Game",
    load_end,
    &MainDef,
    SaveMenu,
    M_DrawSave,
    80,54,
    0
};



//
// Draw border for the savegame description
//
void M_DrawSaveLoadBorder(int x,int y)
{
    int             i;

    if( gamemode == heretic )
        V_DrawScaledPatch(x-8, y-4, 0, W_CachePatchName("M_FSLOT", PU_CACHE));
    else
    {
        V_DrawScaledPatch (x-8,y+7,0,W_CachePatchName("M_LSLEFT",PU_CACHE));
        
        for (i = 0;i < 24;i++)
        {
            V_DrawScaledPatch (x,y+7,0,W_CachePatchName("M_LSCNTR",PU_CACHE));
            x += 8;
        }
        
        V_DrawScaledPatch (x,y+7,0,W_CachePatchName("M_LSRGHT",PU_CACHE));
    }
}


//
//  M_SaveGame & Cie.
//
void M_DrawSave(void)
{
    int             i;

    M_DrawGenericMenu();

    for (i = 0;i < load_end; i++)
    {
        M_DrawSaveLoadBorder(LoadDef.x,LoadDef.y+LINEHEIGHT*i);
        V_DrawString(LoadDef.x,LoadDef.y+LINEHEIGHT*i,0,savegamestrings[i]);
    }

    if (saveStringEnter)
    {
        i = V_StringWidth(savegamestrings[saveSlot]);
        V_DrawString(LoadDef.x + i,LoadDef.y+LINEHEIGHT*saveSlot,0,"_");
    }
}

//
// M_Responder calls this when user is finished
//
void M_DoSave(int slot)
{
    G_SaveGame (slot,savegamestrings[slot]);
    M_ClearMenus (true);

    // PICK QUICKSAVE SLOT YET?
    if (quickSaveSlot == -2)
        quickSaveSlot = slot;
}

//
// User wants to save. Start string input for M_Responder
//
void M_SaveSelect(int choice)
{
    // we are going to be intercepting all chars
    saveStringEnter = 1;

    saveSlot = choice;
    strcpy(saveOldString,savegamestrings[choice]);
    if (!strcmp(savegamestrings[choice],EMPTYSTRING))
        savegamestrings[choice][0] = 0;
    saveCharIndex = strlen(savegamestrings[choice]);
}

//
// Selected from DOOM menu
//
void M_SaveGame (int choice)
{
    if(demorecording)
    {
        M_StartMessage("You can't save while recording demos\n\nPress a key\n",NULL,MM_NOTHING);
        return;
    }

    if (demoplayback || demorecording)
    {
        M_StartMessage(SAVEDEAD,NULL,MM_NOTHING);
        return;
    }

    if (gamestate != GS_LEVEL)
        return;

    if (netgame && !server)
    {
        M_StartMessage("You are not the server",NULL,MM_NOTHING);
        return;
    }

    M_SetupNextMenu(&SaveDef);
    M_ReadSaveStrings();
}

//===========================================================================
//                            QuickSAVE & QuickLOAD
//===========================================================================

//
//      M_QuickSave
//
char    tempstring[80];

void M_QuickSaveResponse(int ch)
{
    if (ch == 'y')
    {
        M_DoSave(quickSaveSlot);
        S_StartSound(NULL,sfx_swtchx);
    }
}

void M_QuickSave(void)
{
    if (demoplayback || demorecording)
    {
        S_StartSound(NULL,sfx_oof);
        return;
    }

    if (gamestate != GS_LEVEL)
        return;

    if (quickSaveSlot < 0)
    {
        M_StartControlPanel();
        M_ReadSaveStrings();
        M_SetupNextMenu(&SaveDef);
        quickSaveSlot = -2;     // means to pick a slot now
        return;
    }
    sprintf(tempstring,QSPROMPT,savegamestrings[quickSaveSlot]);
    M_StartMessage(tempstring,M_QuickSaveResponse,MM_YESNO);
}



//
// M_QuickLoad
//
void M_QuickLoadResponse(int ch)
{
    if (ch == 'y')
    {
        M_LoadSelect(quickSaveSlot);
        S_StartSound(NULL,sfx_swtchx);
    }
}


void M_QuickLoad(void)
{
    if (netgame)
    {
        M_StartMessage(QLOADNET,NULL,MM_NOTHING);
        return;
    }

    if (quickSaveSlot < 0)
    {
        M_StartMessage(QSAVESPOT,NULL,MM_NOTHING);
        return;
    }
    sprintf(tempstring,QLPROMPT,savegamestrings[quickSaveSlot]);
    M_StartMessage(tempstring,M_QuickLoadResponse,MM_YESNO);
}


//===========================================================================
//                                 END GAME
//===========================================================================

//
// M_EndGame
//
void M_EndGameResponse(int ch)
{
    if (ch != 'y')
        return;

    currentMenu->lastOn = itemOn;
    M_ClearMenus (true);
    COM_BufAddText("exitgame\n");
}

void M_EndGame(int choice)
{
    choice = 0;
    if (demoplayback || demorecording)
    {
        S_StartSound(NULL,sfx_oof);
        return;
    }
/*
    if (netgame)
    {
        M_StartMessage(NETEND,NULL,MM_NOTHING);
        return;
    }
*/
    M_StartMessage(ENDGAME,M_EndGameResponse,MM_YESNO);
}

//===========================================================================
//                                 Quit Game
//===========================================================================

//
// M_QuitDOOM
//
int     quitsounds[8] =
{
    sfx_pldeth,
    sfx_dmpain,
    sfx_popain,
    sfx_slop,
    sfx_telept,
    sfx_posit1,
    sfx_posit3,
    sfx_sgtatk
};

int     quitsounds2[8] =
{
    sfx_vilact,
    sfx_getpow,
    sfx_boscub,
    sfx_slop,
    sfx_skeswg,
    sfx_kntdth,
    sfx_bspact,
    sfx_sgtatk
};



void M_QuitResponse(int ch)
{
    tic_t   time;
    if (ch != 'y')
        return;
    if (!netgame)
    {
        //added:12-02-98: quitsounds are much more fun than quisounds2
        //if (gamemode == commercial)
        //    S_StartSound(NULL,quitsounds2[(gametic>>2)&7]);
        //else
            S_StartSound(NULL,quitsounds[(gametic>>2)&7]);

        //added:12-02-98: do that instead of I_WaitVbl which does not work
        if(!nosound)
        {
            time = I_GetTime() + TICRATE*2;
            while (time > I_GetTime()) ;
        }
        //I_WaitVBL(105);
    }
    I_Quit ();
}




void M_QuitDOOM(int choice)
{
  // We pick index 0 which is language sensitive,
  //  or one at random, between 1 and maximum number.
  static char s[200];
  sprintf(s,text[DOSY_NUM],text[ QUITMSG_NUM+(gametic%NUM_QUITMESSAGES)]);
  M_StartMessage( s,M_QuitResponse,MM_YESNO);
}


//===========================================================================
//                              Some Draw routine
//===========================================================================

//
//      Menu Functions
//
void M_DrawThermo ( int   x,
                    int   y,
                    consvar_t *cv)
{
    int xx,i;
    int leftlump,rightlump,centerlump[2],cursorlump;
    patch_t *p;

    xx = x;
    if( raven )
    {
        xx -= 32-8;
        leftlump      = W_GetNumForName("M_SLDLT");
        rightlump     = W_GetNumForName("M_SLDRT"); 
        centerlump[0] = W_GetNumForName("M_SLDMD1"); 
        centerlump[1] = W_GetNumForName("M_SLDMD2"); 
        cursorlump    = W_GetNumForName("M_SLDKB");  
    }
    else
    {
        leftlump      = W_GetNumForName("M_THERML");
        rightlump     = W_GetNumForName("M_THERMR"); 
        centerlump[0] = W_GetNumForName("M_THERMM"); 
        centerlump[1] = W_GetNumForName("M_THERMM"); 
        cursorlump    = W_GetNumForName("M_THERMO");  
    }
    V_DrawScaledPatch (xx,y,0,p=W_CachePatchNum(leftlump,PU_CACHE));
    xx += SHORT(p->width)-SHORT(p->leftoffset);
    for (i=0;i<16;i++)
    {
        V_DrawScaledPatch (xx,y,0,W_CachePatchNum(centerlump[i & 1],PU_CACHE));
        xx += 8;
    }
    V_DrawScaledPatch (xx,y,0,W_CachePatchNum(rightlump,PU_CACHE));

    xx = (cv->value - cv->PossibleValue[0].value) * (15*8) /
         (cv->PossibleValue[1].value - cv->PossibleValue[0].value);

    V_DrawScaledPatch ((x+8) + xx, raven ? y+7 : y,
                       0,W_CachePatchNum(cursorlump,PU_CACHE));
}


void M_DrawEmptyCell( menu_t*       menu,
                      int           item )
{
    V_DrawScaledPatch (menu->x - 10,        menu->y+item*LINEHEIGHT - 1, 0,
                       W_CachePatchName("M_CELL1",PU_CACHE));
}

void M_DrawSelCell ( menu_t*       menu,
                     int           item )
{
    V_DrawScaledPatch (menu->x - 10,        menu->y+item*LINEHEIGHT - 1, 0,
                       W_CachePatchName("M_CELL2",PU_CACHE));
}


//
//  Draw a textbox, like Quake does, because sometimes it's difficult
//  to read the text with all the stuff in the background...
//
//added:06-02-98:
extern int st_borderpatchnum;   //st_stuff.c (for Glide)
void M_DrawTextBox (int x, int y, int width, int lines)
{
    patch_t  *p;
    int      cx, cy;
    int      n;
    int      step,boff; 

    if( gamemode == heretic )
    {
        // humf.. border will stand if we do not adjust size ...
        x+=4;
        y+=4;
        lines = (lines+1)/2;
        width = (width+1)/2;
        step = 16;
        boff = 4; // borderoffset
    }
    else
    {
        step = 8;
        boff = 8;
    }

    // draw left side
    cx = x;
    cy = y;
    V_DrawScaledPatch (cx, cy, 0, 
        W_CachePatchNum(viewborderlump[BRDR_TL],PU_CACHE));
    cy += boff;
    p = W_CachePatchNum (viewborderlump[BRDR_L],PU_CACHE);
    for (n = 0; n < lines; n++)
    {
        V_DrawScaledPatch (cx, cy, 0, p);
        cy += step;
    }
    V_DrawScaledPatch (cx, cy, 0, 
        W_CachePatchNum (viewborderlump[BRDR_BL],PU_CACHE));

    // draw middle
    V_DrawFlatFill (x+boff, y+boff ,width*step,lines*step,st_borderpatchnum);

    cx += boff;
    cy = y;
    while (width > 0)
    {
        V_DrawScaledPatch (cx, cy, 0, 
            W_CachePatchNum (viewborderlump[BRDR_T],PU_CACHE));

        V_DrawScaledPatch (cx, y+boff+lines*step, 0, 
            W_CachePatchNum (viewborderlump[BRDR_B],PU_CACHE));
        width --;
        cx += step;
    }

    // draw right side
    cy = y;
    V_DrawScaledPatch (cx, cy, 0, 
        W_CachePatchNum (viewborderlump[BRDR_TR],PU_CACHE));
    cy += boff;
    p = W_CachePatchNum (viewborderlump[BRDR_R],PU_CACHE);
    for (n = 0; n < lines; n++)
    {
        V_DrawScaledPatch (cx, cy, 0, p);
        cy += step;
    }
    V_DrawScaledPatch (cx, cy, 0, 
        W_CachePatchNum (viewborderlump[BRDR_BR],PU_CACHE));
}

//==========================================================================
//                        Message is now a (hackeble) Menu
//==========================================================================
void M_DrawMessageMenu(void);

menuitem_t MessageMenu[]=
{
    // TO HACK
    {0 ,NULL , 0, NULL ,0}
};

menu_t MessageDef =
{
    NULL,               // title
    NULL,
    1,                  // # of menu items
    NULL,               // previous menu       (TO HACK)
    MessageMenu,        // menuitem_t ->
    M_DrawMessageMenu,  // drawing routine ->
    0,0,                // x,y                 (TO HACK)
    0                   // lastOn, flags       (TO HACK)
};


void M_StartMessage ( const char*       string,
                      void*             routine,
                      menumessagetype_t itemtype )
{
    int   max,start,i,lines;
#define message MessageDef.menuitems[0].text
    if( message )
        Z_Free( message );
    message = Z_StrDup(string);
    DEBFILE(message);

    M_StartControlPanel(); // can't put menuactiv to true
    MessageDef.prevMenu = currentMenu;
    MessageDef.menuitems[0].text     = message;
    MessageDef.menuitems[0].alphaKey = itemtype;
    switch(itemtype) {
        case MM_NOTHING:
             MessageDef.menuitems[0].status     = IT_MSGHANDLER;
             MessageDef.menuitems[0].itemaction = M_StopMessage;
             break;
        case MM_YESNO:
             MessageDef.menuitems[0].status     = IT_MSGHANDLER;
             MessageDef.menuitems[0].itemaction = routine;
             break;
        case MM_EVENTHANDLER:
             MessageDef.menuitems[0].status     = IT_MSGHANDLER;
             MessageDef.menuitems[0].itemaction = routine;
             break;
    }
    //added:06-02-98: now draw a textbox around the message
    // compute lenght max and the numbers of lines
    max = 0;
    start = 0;
    for (lines=0; *(message+start); lines++)
    {
        for (i = 0;i < (int)strlen(message+start);i++)
        {
            if (*(message+start+i) == '\n')
            {
                if (i > max)
                    max = i;
                start += i+1;
                i = -1; //added:07-02-98:damned!
                break;
            }
        }

        if (i == (int)strlen(message+start))
            start += i;
    }

    MessageDef.x=(BASEVIDWIDTH-8*max-16)/2;
    MessageDef.y=(BASEVIDHEIGHT - M_StringHeight(message))/2;

    MessageDef.lastOn = (lines<<8)+max;

//    M_SetupNextMenu();
    currentMenu = &MessageDef;
    itemOn=0;
}

#define MAXMSGLINELEN 256

void M_DrawMessageMenu(void)
{
    int    y;
    short  i,max;
    char   string[MAXMSGLINELEN];
    int    start,lines;
    char   *msg=currentMenu->menuitems[0].text;

    y=currentMenu->y;
    start = 0;
    lines = currentMenu->lastOn>>8;
    max = (currentMenu->lastOn & 0xFF)*8;
    M_DrawTextBox (currentMenu->x,y-8,(max+7)>>3,lines);

    while(*(msg+start))
    {
        for (i = 0;i < (int)strlen(msg+start);i++)
        {
            if (*(msg+start+i) == '\n')
            {
                memset(string,0,MAXMSGLINELEN);
                if(i >= MAXMSGLINELEN)
                {
                    CONS_Printf("M_DrawMessageMenu: too long segment in %s\n", msg);
                    return;
                }
                else
                {
                    strncpy(string,msg+start,i);
                    start += i+1;
                    i = -1; //added:07-02-98:damned!
                }
                
                break;
            }
        }

        if (i == (int)strlen(msg+start))
        {
            if(i >= MAXMSGLINELEN)
            {
                CONS_Printf("M_DrawMessageMenu: too long segment in %s\n", msg);
                return;
            }
            else
            {
                strcpy(string,msg+start);
                start += i;
            }
        }

        V_DrawString((BASEVIDWIDTH - V_StringWidth(string))/2,y,0,string);
        y += 8; //SHORT(hu_font[0]->height);
    }
}

// default message handler
void M_StopMessage(int choice)
{
    M_SetupNextMenu(MessageDef.prevMenu);
    S_StartSound(NULL,sfx_swtchx);
}

//==========================================================================
//                        Menu stuffs
//==========================================================================

//added:30-01-98:
//
//  Write a string centered using the hu_font
//
void M_CentreText (int y, char* string)
{
    int x;
    //added:02-02-98:centre on 320, because V_DrawString centers on vid.width...
    x = (BASEVIDWIDTH - V_StringWidth(string))>>1;
    V_DrawString(x,y,0,string);
}


//
// CONTROL PANEL
//

void M_ChangeCvar(int choise)
{
    consvar_t *cv=(consvar_t *)currentMenu->menuitems[itemOn].itemaction;

    if(((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_SLIDER )
     ||((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_NOMOD  ))
    {
        CV_SetValue(cv,cv->value+choise*2-1);
    }
    else
        if(cv->flags & CV_FLOAT)
        {
            char s[20];
            sprintf(s,"%f",(float)cv->value/FRACUNIT+(choise*2-1)*(1.0/16.0));
            CV_Set(cv,s);
        }
        else
            CV_AddValue(cv,choise*2-1);
}

boolean M_ChangeStringCvar(int choise)
{
    consvar_t *cv=(consvar_t *)currentMenu->menuitems[itemOn].itemaction;
    char buf[255];
    int  len;

    switch( choise ) {
        case KEY_BACKSPACE :
                len=strlen(cv->string);
                if( len>0 )
                {
                    memcpy(buf,cv->string,len);
                    buf[len-1]=0;
                    CV_Set(cv, buf);
                }
                return true;
        default:
            if( choise >= 32 && choise <= 127 )
            {
                len=strlen(cv->string);
                if( len<MAXSTRINGLENGTH-1 )
                {
                    memcpy(buf,cv->string,len);
                    buf[len++] = choise;
                    buf[len] = 0;
                    CV_Set(cv, buf);
                }
                return true;
            }
            break;
    }
    return false;
}

//
// M_Responder
//
boolean M_Responder (event_t* ev)
{
    int             ch;
    int             i;
    static  tic_t   joywait = 0;
    static  tic_t   mousewait = 0;
    static  int     mousey = 0;
    static  int     lasty = 0;
    static  int     mousex = 0;
    static  int     lastx = 0;
    void  (*routine)(int choice);  // for some casting problem

    ch = -1;

    if (ev->type == ev_keydown)
    {
        ch = ev->data1;
        
        // added 5-2-98 remap virtual keys (mouse & joystick buttons)
        switch(ch) {
        case KEY_MOUSE1   : ch=KEY_ENTER;break;
        case KEY_MOUSE1+1 : ch=KEY_BACKSPACE;break;
        case KEY_JOY1     :
        case KEY_JOY1+2   :
        case KEY_JOY1+3   : ch=KEY_ENTER;break;
        case KEY_JOY1+1   : ch=KEY_BACKSPACE;break;
        }
    }
    else if( menuactive )
    {
        if (ev->type == ev_joystick && joywait < I_GetTime())
        {
            if (ev->data3 == -1)
            {
                ch = KEY_UPARROW;
                joywait = I_GetTime() + TICRATE/7;
            }
            else if (ev->data3 == 1)
            {
                ch = KEY_DOWNARROW;
                joywait = I_GetTime() + TICRATE/7;
            }
            
            if (ev->data2 == -1)
            {
                ch = KEY_LEFTARROW;
                joywait = I_GetTime() + TICRATE/17;
            }
            else if (ev->data2 == 1)
            {
                ch = KEY_RIGHTARROW;
                joywait = I_GetTime() + TICRATE/17;
            }
        }
        else
        {
            if (ev->type == ev_mouse && mousewait < I_GetTime())
            {
                mousey += ev->data3;
                if (mousey < lasty-30)
                {
                    ch = KEY_DOWNARROW;
                    mousewait = I_GetTime() + TICRATE/7;
                    mousey = lasty -= 30;
                }
                else if (mousey > lasty+30)
                {
                    ch = KEY_UPARROW;
                    mousewait = I_GetTime() + TICRATE/7;
                    mousey = lasty += 30;
                }
                
                mousex += ev->data2;
                if (mousex < lastx-30)
                {
                    ch = KEY_LEFTARROW;
                    mousewait = I_GetTime() + TICRATE/7;
                    mousex = lastx -= 30;
                }
                else if (mousex > lastx+30)
                {
                    ch = KEY_RIGHTARROW;
                    mousewait = I_GetTime() + TICRATE/7;
                    mousex = lastx += 30;
                }
            }
        }
    }

    if (ch == -1)
        return false;


    // Save Game string input
    if (saveStringEnter)
    {
        switch(ch)
        {
          case KEY_BACKSPACE:
            if (saveCharIndex > 0)
            {
                saveCharIndex--;
                savegamestrings[saveSlot][saveCharIndex] = 0;
            }
            break;

          case KEY_ESCAPE:
            saveStringEnter = 0;
            strcpy(&savegamestrings[saveSlot][0],saveOldString);
            break;

          case KEY_ENTER:
            saveStringEnter = 0;
            if (savegamestrings[saveSlot][0])
                M_DoSave(saveSlot);
            break;

          default:
            ch = toupper(ch);
            if (ch != 32)
                if (ch-HU_FONTSTART < 0 || ch-HU_FONTSTART >= HU_FONTSIZE)
                    break;
            if (ch >= 32 && ch <= 127 &&
                saveCharIndex < SAVESTRINGSIZE-1 &&
                V_StringWidth(savegamestrings[saveSlot]) <
                (SAVESTRINGSIZE-2)*8)
            {
                savegamestrings[saveSlot][saveCharIndex++] = ch;
                savegamestrings[saveSlot][saveCharIndex] = 0;
            }
            break;
        }
        return true;
    }

    if (devparm && ch == KEY_F1)
    {
        COM_BufAddText("screenshot\n");
        return true;
    }


    // F-Keys
    if (!menuactive)
    {
        switch(ch)
        {
          case KEY_MINUS:         // Screen size down
            if (automapactive || chat_on || con_destlines)     // DIRTY !!!
                return false;
            CV_SetValue (&cv_viewsize, cv_viewsize.value-1);
            S_StartSound(NULL,sfx_stnmov);
            return true;

          case KEY_EQUALS:        // Screen size up
            if (automapactive || chat_on || con_destlines)     // DIRTY !!!
                return false;
            CV_SetValue (&cv_viewsize, cv_viewsize.value+1);
            S_StartSound(NULL,sfx_stnmov);
            return true;

          case KEY_F1:            // Help key
            M_StartControlPanel ();

            if ( gamemode == retail )
              currentMenu = &ReadDef2;
            else
              currentMenu = &ReadDef1;

            itemOn = 0;
            S_StartSound(NULL,sfx_swtchn);
            return true;

          case KEY_F2:            // Save
            M_StartControlPanel();
            S_StartSound(NULL,sfx_swtchn);
            M_SaveGame(0);
            return true;

          case KEY_F3:            // Load
            M_StartControlPanel();
            S_StartSound(NULL,sfx_swtchn);
            M_LoadGame(0);
            return true;

          case KEY_F4:            // Sound Volume
            M_StartControlPanel ();
            currentMenu = &SoundDef;
            itemOn = sfx_vol;
            S_StartSound(NULL,sfx_swtchn);
            return true;

          //added:26-02-98: now F5 calls the Video Menu
          case KEY_F5:
            S_StartSound(NULL,sfx_swtchn);
            M_StartControlPanel();
            M_SetupNextMenu (&VidModeDef);
            //M_ChangeDetail(0);
            return true;

          case KEY_F6:            // Quicksave
            S_StartSound(NULL,sfx_swtchn);
            M_QuickSave();
            return true;

          //added:26-02-98: F7 changed to Options menu
          case KEY_F7:            // End game
            S_StartSound(NULL,sfx_swtchn);
            M_StartControlPanel();
            M_SetupNextMenu (&OptionsDef);
            //M_EndGame(0);
            return true;

          case KEY_F8:            // Toggle messages
            CV_AddValue(&cv_showmessages,+1);
            S_StartSound(NULL,sfx_swtchn);
            return true;

          case KEY_F9:            // Quickload
            S_StartSound(NULL,sfx_swtchn);
            M_QuickLoad();
            return true;

          case KEY_F10:           // Quit DOOM
            S_StartSound(NULL,sfx_swtchn);
            M_QuitDOOM(0);
            return true;

          //added:10-02-98: the gamma toggle is now also in the Options menu
          case KEY_F11:
            S_StartSound(NULL,sfx_swtchn);
            CV_AddValue (&cv_usegamma,+1);
            return true;

          // Pop-up menu
          case KEY_ESCAPE:
            M_StartControlPanel ();
            S_StartSound(NULL,sfx_swtchn);
            return true;
        }
        return false;
    }

    routine = currentMenu->menuitems[itemOn].itemaction;

    //added:30-01-98:
    // Handle menuitems which need a specific key handling
    if(routine && (currentMenu->menuitems[itemOn].status & IT_TYPE) == IT_KEYHANDLER )
    {
        routine(ch);
        return true;
    }

    if(currentMenu->menuitems[itemOn].status==IT_MSGHANDLER)
    {
        if(currentMenu->menuitems[itemOn].alphaKey==true)
        {
            if(ch == ' ' || ch == 'n' || ch == 'y' || ch == KEY_ESCAPE)
            {
                if(routine) routine(ch);
                M_StopMessage(0);
                return true;
            }
            return true;
        }
        else
        {
            //added:07-02-98:dirty hak:for the customise controls, I want only
            //      buttons/keys, not moves
            if (ev->type==ev_mouse || ev->type==ev_joystick )
                return true;
            if(routine) routine((int)ev);
            return true;
        }
    }

    // BP: one of the more big hack i have never made
    if( routine && (currentMenu->menuitems[itemOn].status & IT_TYPE) == IT_CVAR )
    {
        if( (currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_STRING )
        {
            if( M_ChangeStringCvar(ch) )
                return true;
            else
                routine = NULL;
        }
        else
            routine=M_ChangeCvar;
    }
    // Keys usable within menu
    switch (ch)
    {
      case KEY_DOWNARROW:
        do
        {
            if (itemOn+1 > currentMenu->numitems-1)
                itemOn = 0;
            else itemOn++;
        } while((currentMenu->menuitems[itemOn].status & IT_TYPE)==IT_SPACE);
        S_StartSound(NULL,sfx_pstop);
        return true;

      case KEY_UPARROW:
        do
        {
            if (!itemOn)
                itemOn = currentMenu->numitems-1;
            else itemOn--;
        } while((currentMenu->menuitems[itemOn].status & IT_TYPE)==IT_SPACE);
        S_StartSound(NULL,sfx_pstop);
        return true;

      case KEY_LEFTARROW:
        if (  routine &&
            ( (currentMenu->menuitems[itemOn].status & IT_TYPE) == IT_ARROWS
            ||(currentMenu->menuitems[itemOn].status & IT_TYPE) == IT_CVAR   ))
        {
            S_StartSound(NULL,sfx_stnmov);
            routine(0);
        }
        return true;

      case KEY_RIGHTARROW:
        if ( routine &&
            ( (currentMenu->menuitems[itemOn].status & IT_TYPE) == IT_ARROWS
            ||(currentMenu->menuitems[itemOn].status & IT_TYPE) == IT_CVAR   ))
        {
            S_StartSound(NULL,sfx_stnmov);
            routine(1);
        }
        return true;

      case KEY_ENTER:
        currentMenu->lastOn = itemOn;
        if ( routine )
        {
            switch (currentMenu->menuitems[itemOn].status & IT_TYPE)  {
                case IT_CVAR:
                case IT_ARROWS:
                    routine(1);            // right arrow
                    S_StartSound(NULL,sfx_stnmov);
                    break;
                case IT_CALL:
                    routine(itemOn);
                    S_StartSound(NULL,sfx_pistol);
                    break;
                case IT_SUBMENU:
                    currentMenu->lastOn = itemOn;
                    M_SetupNextMenu((menu_t *)currentMenu->menuitems[itemOn].itemaction);
                    S_StartSound(NULL,sfx_pistol);
                    break;
            }
        }
        return true;

      case KEY_ESCAPE:
        currentMenu->lastOn = itemOn;
        if (currentMenu->prevMenu)
        {
            currentMenu = currentMenu->prevMenu;
            itemOn = currentMenu->lastOn;
	    S_StartSound(NULL,sfx_swtchx); // it´s a matter of taste which sound to choose
            //S_StartSound(NULL,sfx_swtchn);
        }
	else
	{
	    M_ClearMenus (true);
	    S_StartSound(NULL,sfx_swtchx);
	}
	
        return true;
      case KEY_BACKSPACE:
        if((currentMenu->menuitems[itemOn].status)==IT_CONTROL)
        {
            S_StartSound(NULL,sfx_stnmov);
            // detach any keys associated to the game control
            G_ClearControlKeys (setupcontrols, currentMenu->menuitems[itemOn].alphaKey);
            return true;
        }
        currentMenu->lastOn = itemOn;
        if (currentMenu->prevMenu)
        {
            currentMenu = currentMenu->prevMenu;
            itemOn = currentMenu->lastOn;
            S_StartSound(NULL,sfx_swtchn);
        }
        return true;

      default:
        for (i = itemOn+1;i < currentMenu->numitems;i++)
            if (currentMenu->menuitems[i].alphaKey == ch)
            {
                itemOn = i;
                S_StartSound(NULL,sfx_pstop);
                return true;
            }
        for (i = 0;i <= itemOn;i++)
            if (currentMenu->menuitems[i].alphaKey == ch)
            {
                itemOn = i;
                S_StartSound(NULL,sfx_pstop);
                return true;
            }
        break;

    }

    return true;
}



//
//      Find string height from hu_font chars
//
int M_StringHeight(char* string)
{
    int      i;
    int      h;
    int      height = 8; //(hu_font[0]->height);

    h = height;
    for (i = 0;i < (int)strlen(string);i++)
        if (string[i] == '\n')
            h += height;

    return h;
}


//
// M_Drawer
// Called after the view has been rendered,
// but before it has been blitted.
//
void M_Drawer (void)
{
    if (!menuactive)
        return;

    //added:18-02-98:
    // center the scaled graphics for the menu,
    //  set it 0 again before return!!!
    scaledofs = vid.centerofs;

    // now that's more readable with a faded background (yeah like Quake...)
    V_DrawFadeScreen ();

    if (currentMenu->drawroutine)
        currentMenu->drawroutine();      // call current menu Draw routine

    //added:18-02-98: it should always be 0 for non-menu scaled graphics.
    scaledofs = 0;

}

//
// M_StartControlPanel
//
void M_StartControlPanel (void)
{
    // intro might call this repeatedly
    if (menuactive)
        return;

    menuactive = 1;
    currentMenu = &MainDef;         // JDC
    itemOn = currentMenu->lastOn;   // JDC

    CON_ToggleOff ();   // dirty hack : move away console
}

//
// M_ClearMenus
//
void M_ClearMenus (boolean callexitmenufunc)
{
    if(!menuactive)
        return;

    if( currentMenu->quitroutine && callexitmenufunc)
    {
        if( !currentMenu->quitroutine())
            return; // we can't quit this menu (also used to set parameter from the menu)
    }

    menuactive = 0;
}


//
// M_SetupNextMenu
//
void M_SetupNextMenu(menu_t *menudef)
{
    if( currentMenu->quitroutine )
    {
        if( !currentMenu->quitroutine())
            return; // we can't quit this menu (also used to set parameter from the menu)
    }
    currentMenu = menudef;
    itemOn = currentMenu->lastOn;

    // in case of...
    if (itemOn >= currentMenu->numitems)
        itemOn = currentMenu->numitems - 1;

    // the curent item can be desabled,
    // this code go up until a enabled item found
    while(currentMenu->menuitems[itemOn].status==IT_DISABLED && itemOn)
        itemOn--;
}


//
// M_Ticker
//
void M_Ticker (void)
{
    if(dedicated)
	return;
    
    if (--skullAnimCounter <= 0)
    {
        whichSkull ^= 1;
        skullAnimCounter = 8 * NEWTICRATERATIO;
    }

    //added:30-01-98:test mode for five seconds
    if( vidm_testingmode>0 )
    {
        // restore the previous video mode
        if (--vidm_testingmode==0)
            setmodeneeded = vidm_previousmode+1;
    }
}


//
// M_Init
//
void M_Init (void)
{
    if(dedicated)
	return;
    
    currentMenu = &MainDef;
    menuactive = 0;
    itemOn = currentMenu->lastOn;

    whichSkull = 0;
    skullAnimCounter = 10;

    quickSaveSlot = -1;

    // Here we could catch other version dependencies,
    //  like HELP1/2, and four episodes.

    if( !inventory )
    {
        // remove the inventory key from the menu !
        int i;

        for( i=0;i<ControlDef2.numitems;i++)
            if( ControlMenu2[i].alphaKey == gc_invprev ||
                ControlMenu2[i].alphaKey == gc_invnext ||
                ControlMenu2[i].alphaKey == gc_invuse )
                ControlMenu2[i].status = IT_LITLSPACE;
    }

    if( gamemode !=heretic )
    {
        // remove the fly down key from the menu !
        int i;

        for( i=0;i<ControlDef2.numitems;i++)
            if( ControlMenu2[i].alphaKey == gc_flydown )
                ControlMenu2[i].status = IT_LITLSPACE;
    }

    if( W_CheckNumForName("E2M1")<0 )
    {
        exmy_cons_t[9].value = 0;
        exmy_cons_t[9].strvalue = NULL;
    }
    else
    if( W_CheckNumForName("E3M1")<0 )
    {
        exmy_cons_t[18].value = 0;
        exmy_cons_t[18].strvalue = NULL;
    }
    else
    if( W_CheckNumForName("E4M1")<0 )
    {
        exmy_cons_t[27].value = 0;
        exmy_cons_t[27].strvalue = NULL;
    }
    else
    if( W_CheckNumForName("E5M1")<0 )
    {
        exmy_cons_t[36].value = 0;
        exmy_cons_t[36].strvalue = NULL;
    }

    switch ( gamemode )
    {
      case commercial:
        // This is used because DOOM 2 had only one HELP
        //  page. I use CREDIT as second page now, but
        //  kept this hack for educational purposes.
        MainMenu[readthis] = MainMenu[quitdoom];
        MainDef.numitems--;
        MainDef.y += 8;
        NewDef.prevMenu = &MainDef;
        ReadDef1.drawroutine = M_DrawReadThis1;
        ReadDef1.x = 330;
        ReadDef1.y = 165;
        ReadMenu1[0].itemaction = &MainDef;
        break;
      case shareware:
        // Episode 2 and 3 are handled,
        //  branching to an ad screen.
      case registered:
          // We need to remove the fourth episode.
          EpiDef.numitems--;
      case retail:
          // We are fine.
          cv_nextmap.PossibleValue = exmy_cons_t;
          cv_nextmap.defaultvalue = "11";
          // We need to remove the fifth episode.
          EpiDef.numitems--;
          break;
      case heretic:
          cv_nextmap.PossibleValue = exmy_cons_t;
          cv_nextmap.defaultvalue = "11";

          MainDef.menutitlepic = "M_HTIC";
          MainDef.drawroutine = HereticMainMenuDrawer;
          SkullBaseLump = W_GetNumForName("M_SKL00");
          strcpy(skullName[0], "M_SLCTR1");
          strcpy(skullName[1], "M_SLCTR2");

          EpisodeMenu[0].text = "CITY OF THE DAMNED";
          EpisodeMenu[1].text = "HELL'S MAW";
          EpisodeMenu[2].text = "THE DOME OF D'SPARIL";
          EpisodeMenu[3].text = "THE OSSUARY";
          EpisodeMenu[4].text = "THE STAGNANT DEMESNE";

          NewGameMenu[0].text = "THOU NEEDETH A WET-NURSE";
          NewGameMenu[1].text = "YELLOWBELLIES-R-US";
          NewGameMenu[2].text = "BRINGEST THEM ONETH";
          NewGameMenu[3].text = "THOU ART A SMITE-MEISTER";
          NewGameMenu[4].text = "BLACK PLAGUE POSSESSES THEE";

      default:
        break;
    }
    CV_RegisterVar(&cv_skill);
    CV_RegisterVar(&cv_monsters);
    CV_RegisterVar(&cv_nextmap );
    CV_RegisterVar(&cv_newdeathmatch);
    CV_RegisterVar(&cv_serversearch);
}


//======================================================================
// OpenGL specifics options
//======================================================================

#ifdef HWRENDER

void M_DrawOpenGLMenu(void);
void M_OGL_DrawFogMenu(void);
void M_OGL_DrawColorMenu(void);
void M_HandleFogColor (int choice);
void M_HandleScreenDepth(int choice);
menu_t OGL_LightingDef, OGL_FogDef, OGL_ColorDef, OGL_DevDef;

menuitem_t OpenGLOptionsMenu[]=
{
    {IT_STRING | IT_CVAR,0, "Mouse look"          , &cv_grcrappymlook     ,  0},
    {IT_STRING | IT_CVAR,0, "Field of view"       , &cv_grfov             , 10},
    {IT_STRING | IT_CVAR,0, "Quality"             , &cv_scr_depth         , 20},
    {IT_STRING | IT_CVAR,0, "Texture Filter"      , &cv_grfiltermode      , 30},
    {IT_STRING | IT_CVAR | IT_CV_SLIDER,0, "Translucent HUD", &cv_grtranslucenthud  , 40},

    {IT_SUBMENU|IT_WHITESTRING,0, "Lighting..."       , &OGL_LightingDef   , 65},
    {IT_SUBMENU|IT_WHITESTRING,0, "Fog..."            , &OGL_FogDef        , 75},
    {IT_SUBMENU|IT_WHITESTRING,0, "Gamma..."          , &OGL_ColorDef      , 85},
    {IT_SUBMENU|IT_WHITESTRING,0, "Development..."    , &OGL_DevDef        , 95},
};

menuitem_t OGL_LightingMenu[]=
{
    {IT_STRING | IT_CVAR,0, "Coronas"                 , &cv_grcoronas         ,  0},
    {IT_STRING | IT_CVAR,0, "Coronas size"            , &cv_grcoronasize      , 10},
    {IT_STRING | IT_CVAR,0, "Dynamic lighting"        , &cv_grdynamiclighting , 20},
    {IT_STRING | IT_CVAR,0, "Static lighting"         , &cv_grstaticlighting  , 30},
    {IT_STRING | IT_CVAR,0, "Monsters' balls lighting", &cv_grmblighting      , 40},
};

menuitem_t OGL_FogMenu[]=
{
    {IT_STRING | IT_CVAR, 0,"Fog"             , &cv_grfog              ,  0},
    {IT_STRING | IT_KEYHANDLER,0, "Fog color" , M_HandleFogColor       , 10},
    {IT_STRING | IT_CVAR, 0,"Fog density"     , &cv_grfogdensity       , 20},
};                                         

menuitem_t OGL_ColorMenu[]=
{
    //{IT_STRING | NOTHING, "Gamma correction", NULL                   ,  0},
    {IT_STRING | IT_CVAR | IT_CV_SLIDER, 0,"red"  , &cv_grgammared     , 10},
    {IT_STRING | IT_CVAR | IT_CV_SLIDER, 0,"green", &cv_grgammagreen   , 20},
    {IT_STRING | IT_CVAR | IT_CV_SLIDER, 0,"blue" , &cv_grgammablue    , 30},
    //{IT_STRING | IT_CVAR | IT_CV_SLIDER, "Constrast", &cv_grcontrast , 50},
};

menuitem_t OGL_DevMenu[]=
{
//    {IT_STRING | IT_CVAR, "Polygon smooth"  , &cv_grpolygonsmooth    ,  0},
    {IT_STRING | IT_CVAR, 0, "MD2 models"      , &cv_grmd2              , 10},
    {IT_STRING | IT_CVAR, 0, "Translucent walls", &cv_grtranswall       , 20},
};

menu_t  OpenGLOptionDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(OpenGLOptionsMenu)/sizeof(menuitem_t),
    &VideoOptionsDef,
    OpenGLOptionsMenu,
    M_DrawOpenGLMenu,
    60,40,
    0
};

menu_t  OGL_LightingDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(OGL_LightingMenu)/sizeof(menuitem_t),
    &OpenGLOptionDef,
    OGL_LightingMenu,
    M_DrawGenericMenu,
    60,40,
    0,
};

menu_t  OGL_FogDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(OGL_FogMenu)/sizeof(menuitem_t),
    &OpenGLOptionDef,
    OGL_FogMenu,
    M_OGL_DrawFogMenu,
    60,40,
    0,
};

menu_t  OGL_ColorDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(OGL_ColorMenu)/sizeof(menuitem_t),
    &OpenGLOptionDef,
    OGL_ColorMenu,
    M_OGL_DrawColorMenu,
    60,40,
    0,
};

menu_t  OGL_DevDef =
{
    "M_OPTTTL",
    "OPTIONS",
    sizeof(OGL_DevMenu)/sizeof(menuitem_t),
    &OpenGLOptionDef,
    OGL_DevMenu,
    M_DrawGenericMenu,
    60,40,
    0,
};


//======================================================================
// M_DrawOpenGLMenu()
//======================================================================
void M_DrawOpenGLMenu(void)
{
    int             mx,my;

    mx = OpenGLOptionDef.x;
    my = OpenGLOptionDef.y;
    M_DrawGenericMenu(); // use generic drawer for cursor, items and title
    V_DrawString(BASEVIDWIDTH-mx-V_StringWidth(cv_scr_depth.string),
                 my+currentMenu->menuitems[2].alphaKey,
                 V_WHITEMAP,
                 cv_scr_depth.string);
}


#define FOG_COLOR_ITEM  1
//======================================================================
// M_OGL_DrawFogMenu()
//======================================================================
void M_OGL_DrawFogMenu(void)
{
    int             mx,my;

    mx = OGL_FogDef.x;
    my = OGL_FogDef.y;
    M_DrawGenericMenu(); // use generic drawer for cursor, items and title
    V_DrawString(BASEVIDWIDTH-mx-V_StringWidth (cv_grfogcolor.string),
                 my+currentMenu->menuitems[FOG_COLOR_ITEM].alphaKey,
                 V_WHITEMAP,
                 cv_grfogcolor.string);
    if (itemOn==FOG_COLOR_ITEM && skullAnimCounter<4) //blink cursor on FOG_COLOR_ITEM if selected
        V_DrawCharacter( BASEVIDWIDTH-mx, my+currentMenu->menuitems[FOG_COLOR_ITEM].alphaKey, '_' | 0x80);
}


//======================================================================
// M_OGL_DrawColorMenu()
//======================================================================
void M_OGL_DrawColorMenu(void)
{
    int             mx,my;

    mx = OGL_ColorDef.x;
    my = OGL_ColorDef.y;
    M_DrawGenericMenu(); // use generic drawer for cursor, items and title
    V_DrawString(mx, my+currentMenu->menuitems[0].alphaKey-10,
                 V_WHITEMAP,"Gamma correction");
}


//======================================================================
// M_OpenGLOption()
//======================================================================
void M_OpenGLOption(int choice)
{
    if (rendermode != render_soft )
        M_SetupNextMenu(&OpenGLOptionDef);
    else
        M_StartMessage("You are in software mode\nYou can't change the options\n",NULL,MM_NOTHING);
}


//======================================================================
// M_HandleFogColor()
//======================================================================
void M_HandleFogColor (int choice)
{
    int      i, l;
    char     temp[8];
    boolean  exitmenu = false;  // exit to previous menu and send name change

    switch( choice )
    {
      case KEY_DOWNARROW:
        S_StartSound(NULL,sfx_pstop);
        itemOn++;
        break;

      case KEY_UPARROW:
        S_StartSound(NULL,sfx_pstop);
        itemOn--;
        break;

      case KEY_ESCAPE:
        S_StartSound(NULL,sfx_swtchx);
        exitmenu = true;
        break;

      case KEY_BACKSPACE:
        S_StartSound(NULL,sfx_stnmov);
        strcpy(temp, cv_grfogcolor.string);
        strcpy(cv_grfogcolor.string, "000000");
        l = strlen(temp)-1;
        for (i=0; i<l; i++)
            cv_grfogcolor.string[i+6-l] = temp[i];
        break;

      default:
        if ((choice >= '0' && choice <= '9') ||
            (choice >= 'a' && choice <= 'f') ||
            (choice >= 'A' && choice <= 'F')) {
            S_StartSound(NULL,sfx_stnmov);
            strcpy(temp, cv_grfogcolor.string);
            strcpy(cv_grfogcolor.string, "000000");
            l = strlen(temp);
            for (i=0; i<l; i++)
                cv_grfogcolor.string[5-i] = temp[l-i];
            cv_grfogcolor.string[5] = choice;
        }
        break;
    }
    if (exitmenu)
    {
        if (currentMenu->prevMenu)
            M_SetupNextMenu (currentMenu->prevMenu);
        else
            M_ClearMenus (true);
    }
}

#endif
