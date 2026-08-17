// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: sb_bar.c,v 1.8 2001/08/27 19:59:35 hurdler Exp $
//
// Copyright (C) 1993-1996 by Raven Software, Corp.
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
// $Log: sb_bar.c,v $
// Revision 1.8  2001/08/27 19:59:35  hurdler
// Fix colormap in heretic + opengl, fixedcolormap and NEWCORONA
//
// Revision 1.7  2001/08/02 19:15:59  bpereira
// fix player reset in secret level of doom2
//
// Revision 1.6  2001/06/30 15:06:01  bpereira
// fixed wrong next level name in intermission
//
// Revision 1.5  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.4  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.3  2001/02/10 13:20:55  hurdler
// update license
//
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "p_local.h"
#include "s_sound.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"
#include "dstrings.h"

#include "am_map.h"

#include "g_game.h"
#include "m_cheat.h"

#include "screen.h"
#include "r_local.h"
#include "p_local.h"
#include "p_inter.h"
#include "m_random.h"

#include "i_video.h"

#include "keys.h"

#include "hu_stuff.h"
#include "st_stuff.h"
#include "st_lib.h"

#ifdef HWRENDER
#include "hardware/hw_drv.h"
#include "hardware/hw_main.h"
#endif


// Macros

#define STARTREDPALS    1
#define NUMREDPALS      8
#define STARTBONUSPALS  9
#define NUMBONUSPALS    4

#define I_NOUPDATE      0
#define I_FULLVIEW      1
#define I_STATBAR       2
#define I_MESSAGES      4
#define I_FULLSCRN      8


// Types

// Private Functions

static void ShadeLine(int x, int y, int height, int shade);
static void ShadeChain(void);
static void DrINumber(signed int val, int x, int y);
static void DrBNumber(signed int val, int x, int y);
static void DrawCommonBar(void);
static void DrawMainBar(void);
static void DrawInventoryBar(void);
static void DrawFullScreenStuff(void);

void SB_PaletteFlash(void);
// Public Data

int ArtifactFlash;

// Private Data

static int HealthMarker;
static int ChainWiggle;
static player_t *CPlayer;
int playpalette;
int UpdateState;

patch_t *PatchLTFACE;
patch_t *PatchRTFACE;
patch_t *PatchBARBACK;
patch_t *PatchCHAIN;
patch_t *PatchSTATBAR;
patch_t *PatchLIFEGEM;
//patch_t *PatchEMPWEAP;
//patch_t *PatchLIL4BOX;
patch_t *PatchLTFCTOP;
patch_t *PatchRTFCTOP;
//patch_t *PatchARMORBOX;
//patch_t *PatchARTIBOX;
patch_t *PatchSELECTBOX;
//patch_t *PatchKILLSPIC;
//patch_t *PatchMANAPIC;
//patch_t *PatchPOWERICN;
patch_t *PatchINVLFGEM1;
patch_t *PatchINVLFGEM2;
patch_t *PatchINVRTGEM1;
patch_t *PatchINVRTGEM2;
patch_t *PatchINumbers[10];
patch_t *PatchNEGATIVE;
patch_t *PatchSmNumbers[10];
patch_t *PatchBLACKSQ;
patch_t *PatchINVBAR;
patch_t *PatchARMCLEAR;
patch_t *PatchCHAINBACK;
//byte *ShadeTables;
//extern byte *screen;
int FontBNumBase;
int spinbooklump;
int spinflylump;

//---------------------------------------------------------------------------
//
// PROC SB_Init
//
//---------------------------------------------------------------------------

void SB_Init(void)
{
        int i;
        int startLump;

        PatchLTFACE = W_CachePatchName("LTFACE", PU_STATIC);
        PatchRTFACE = W_CachePatchName("RTFACE", PU_STATIC);
        PatchBARBACK = W_CachePatchName("BARBACK", PU_STATIC);
        PatchINVBAR = W_CachePatchName("INVBAR", PU_STATIC);
        PatchCHAIN = W_CachePatchName("CHAIN", PU_STATIC);
        if(cv_deathmatch.value)
        {
                PatchSTATBAR = W_CachePatchName("STATBAR", PU_STATIC);
        }
        else
        {
                PatchSTATBAR = W_CachePatchName("LIFEBAR", PU_STATIC);
        }
        if(!multiplayer)
        { // single player game uses red life gem
                PatchLIFEGEM = W_CachePatchName("LIFEGEM2", PU_STATIC);
        }
        else
        {
                PatchLIFEGEM = W_CachePatchNum(W_GetNumForName("LIFEGEM0")
                        + consoleplayer, PU_STATIC);
        }
        PatchLTFCTOP = W_CachePatchName("LTFCTOP", PU_STATIC);
        PatchRTFCTOP = W_CachePatchName("RTFCTOP", PU_STATIC);
        PatchSELECTBOX = W_CachePatchName("SELECTBOX", PU_STATIC);
        PatchINVLFGEM1 = W_CachePatchName("INVGEML1", PU_STATIC);
        PatchINVLFGEM2 = W_CachePatchName("INVGEML2", PU_STATIC);
        PatchINVRTGEM1 = W_CachePatchName("INVGEMR1", PU_STATIC);
        PatchINVRTGEM2 = W_CachePatchName("INVGEMR2", PU_STATIC);
        PatchBLACKSQ    =   W_CachePatchName("BLACKSQ", PU_STATIC);
        PatchARMCLEAR = W_CachePatchName("ARMCLEAR", PU_STATIC);
        PatchCHAINBACK = W_CachePatchName("CHAINBACK", PU_STATIC);
        startLump = W_GetNumForName("IN0");
        for(i = 0; i < 10; i++)
        {
                PatchINumbers[i] = W_CachePatchNum(startLump+i, PU_STATIC);
        }
        PatchNEGATIVE = W_CachePatchName("NEGNUM", PU_STATIC);
        FontBNumBase = W_GetNumForName("FONTB16");
        startLump = W_GetNumForName("SMALLIN0");
        for(i = 0; i < 10; i++)
        {
                PatchSmNumbers[i] = W_CachePatchNum(startLump+i, PU_STATIC);
        }
        playpalette = W_GetNumForName("PLAYPAL");
        spinbooklump = W_GetNumForName("SPINBK0");
        spinflylump = W_GetNumForName("SPFLY0");
}

//---------------------------------------------------------------------------
//
// PROC SB_Ticker
//
//---------------------------------------------------------------------------

void SB_Ticker(void)
{
        int delta;
        int curHealth;

        if(leveltime&1)
        {
                ChainWiggle = M_Random()&1;
        }
        curHealth = players[consoleplayer].mo->health;
        if(curHealth < 0)
        {
                curHealth = 0;
        }
        if(curHealth < HealthMarker)
        {
                delta = (HealthMarker-curHealth)>>2;
                if(delta < 1)
                {
                        delta = 1;
                }
                else if(delta > 8)
                {
                        delta = 8;
                }
                HealthMarker -= delta;
        }
        else if(curHealth > HealthMarker)
        {
                delta = (curHealth-HealthMarker)>>2;
                if(delta < 1)
                {
                        delta = 1;
                }
                else if(delta > 8)
                {
                        delta = 8;
                }
                HealthMarker += delta;
        }
}

//---------------------------------------------------------------------------
//
// PROC DrINumber
//
// Draws a three digit number.
//
//---------------------------------------------------------------------------

static void DrINumber(signed int val, int x, int y)
{
        patch_t *patch;
        int oldval;

        oldval = val;
        if(val < 0)
        {
                if(val < -9)
                {
                        V_DrawScaledPatch(x+1, y+1, fgbuffer, W_CachePatchName("LAME", PU_CACHE));
                }
                else
                {
                        val = -val;
                        V_DrawScaledPatch(x+18, y, fgbuffer, PatchINumbers[val]);
                        V_DrawScaledPatch(x+9, y, fgbuffer, PatchNEGATIVE);
                }
                return;
        }
        if(val > 99)
        {
                patch = PatchINumbers[val/100];
                V_DrawScaledPatch(x, y, fgbuffer, patch);
        }
        val = val%100;
        if(val > 9 || oldval > 99)
        {
                patch = PatchINumbers[val/10];
                V_DrawScaledPatch(x+9, y, fgbuffer, patch);
        }
        val = val%10;
        patch = PatchINumbers[val];
        V_DrawScaledPatch(x+18, y, fgbuffer, patch);
}

//---------------------------------------------------------------------------
//
// PROC DrBNumber
//
// Draws a three digit number using FontB
//
//---------------------------------------------------------------------------

//#define V_DrawShadowedPatch(x,y,p) V_DrawTranslucentPatch(x,y,V_SCALESTART|0,p)
void V_DrawShadowedPatch(int x,int y,patch_t *p)
{
//    V_DrawTranslucentPatch(x+2,y+2,V_SCALESTART|0,p/*,transtables*/);
    V_DrawScaledPatch(x,y,V_SCALESTART|0,p);
}

#define V_DrawFuzzPatch(x,y,p)     V_DrawTranslucentPatch(x,y,V_SCALESTART|0,p)

static void DrBNumber(signed int val, int x, int y)
{
        patch_t *patch;
        int xpos;
        int oldval;

        oldval = val;
        xpos = x;
        if(val < 0)
        {
                val = 0;
        }
        if(val > 99)
        {
                patch = W_CachePatchNum(FontBNumBase+val/100, PU_CACHE);
                V_DrawShadowedPatch(xpos+6-patch->width/2, y, patch);
        }
        val = val%100;
        xpos += 12;
        if(val > 9 || oldval > 99)
        {
                patch = W_CachePatchNum(FontBNumBase+val/10, PU_CACHE);
                V_DrawShadowedPatch(xpos+6-patch->width/2, y, patch);
        }
        val = val%10;
        xpos += 12;
        patch = W_CachePatchNum(FontBNumBase+val, PU_CACHE);
        V_DrawShadowedPatch(xpos+6-patch->width/2, y, patch);
}

//---------------------------------------------------------------------------
//
// PROC DrSmallNumber
//
// Draws a small two digit number.
//
//---------------------------------------------------------------------------

static void DrSmallNumber(int val, int x, int y)
{
        patch_t *patch;

        if(val == 1)
        {
                return;
        }
        if(val > 9)
        {
                patch = PatchSmNumbers[val/10];
                V_DrawScaledPatch(x, y, fgbuffer, patch);
        }
        val = val%10;
        patch = PatchSmNumbers[val];
        V_DrawScaledPatch(x+4, y, fgbuffer, patch);
}

//---------------------------------------------------------------------------
//
// PROC ShadeLine
//
//---------------------------------------------------------------------------

static void ShadeLine(int x, int y, int height, int shade)
{
    byte *dest;
    byte *shades;
    
    shades = colormaps+9*256+shade*2*256;
    dest = screens[0]+y*vid.width+x;
    while(height--)
    {
        *(dest) = *(shades+*dest);
        dest += vid.width;
    }
}

//---------------------------------------------------------------------------
//
// PROC ShadeChain
//
//---------------------------------------------------------------------------

static void ShadeChain(void)
{
    int i;

    if( rendermode != render_soft )
        return;
    
    for(i = 0; i < 16*st_scalex; i++)
    {
        ShadeLine((st_x+277)*st_scalex+i, (ST_Y+32)*st_scaley, 10*st_scaley, i/4);
        ShadeLine((st_x+19)*st_scalex+i, (ST_Y+32)*st_scaley, 10*st_scaley, 7-(i/4));
    }
}

//---------------------------------------------------------------------------
//
// PROC SB_Drawer
//
//---------------------------------------------------------------------------

char patcharti[][10] =
{
        {"ARTIBOX"},    // none
        {"ARTIINVU"},   // invulnerability
        {"ARTIINVS"},   // invisibility
        {"ARTIPTN2"},   // health
        {"ARTISPHL"},   // superhealth
        {"ARTIPWBK"},   // tomeofpower
        {"ARTITRCH"},   // torch
        {"ARTIFBMB"},   // firebomb
        {"ARTIEGGC"},   // egg
        {"ARTISOAR"},   // fly
        {"ARTIATLP"}    // teleport
};

char ammopic[][10] =
{
        {"INAMGLD"},
        {"INAMBOW"},
        {"INAMBST"},
        {"INAMRAM"},
        {"INAMPNX"},
        {"INAMLOB"}
};

int SB_state = -1;
static int oldarti = -1;
static int oldartiCount = 0;
static int oldfrags = -9999;
static int oldammo = -1;
static int oldarmor = -1;
static int oldweapon = -1;
static int oldhealth = -1;
static int oldlife = -1;
static int oldkeys = -1;

int playerkeys = 0;

extern boolean automapactive;

void SB_Drawer( boolean refresh )
{
    int frame;
    static boolean hitCenterFrame;

    if( st_recalc )
    {
        ST_CalcPos();
        refresh = 1;
    }

    if( refresh )
        SB_state = -1;


    CPlayer = &players[displayplayer];
    if( !st_statusbaron )
    {
        if( cv_viewsize.value == 11 )
        {
            DrawFullScreenStuff();
            SB_state = -1;
        }
    }
    else
    {
        if(SB_state == -1)
        {
            if ( rendermode==render_soft )
                V_CopyRect(0, vid.height-stbarheight, BG, vid.width, stbarheight, 0, vid.height-stbarheight, FG);
            
            V_DrawScaledPatch(st_x, ST_Y, fgbuffer, PatchBARBACK);
            if(players[consoleplayer].cheats&CF_GODMODE)
            {
                V_DrawScaledPatch(st_x+16, ST_Y+9, fgbuffer, W_CachePatchName("GOD1", PU_CACHE));
                V_DrawScaledPatch(st_x+287, ST_Y+9, fgbuffer, W_CachePatchName("GOD2", PU_CACHE));
            }
            oldhealth = -1;
        }
        DrawCommonBar();
        if(!CPlayer->st_inventoryTics)
        {
            if(SB_state != 0)
            {
                // Main interface
                V_DrawScaledPatch(st_x+34, ST_Y+2, fgbuffer, PatchSTATBAR);
                oldarti = -1;
                oldammo = -1;
                oldarmor = -1;
                oldweapon = -1;
                oldfrags = -9999; //can't use -1, 'cuz of negative frags
                oldlife = -1;
                oldkeys = -1;
            }
            DrawMainBar();
            SB_state = 0;
        }
        else
        {
            if(SB_state != 1)
            {
                V_DrawScaledPatch(st_x+34, ST_Y+2, fgbuffer, PatchINVBAR);
            }
            DrawInventoryBar();
            SB_state = 1;
        }
    }
    SB_PaletteFlash();
    
    // Flight icons
    if(CPlayer->powers[pw_flight])
    {
        if(CPlayer->powers[pw_flight] > BLINKTHRESHOLD
            || !(CPlayer->powers[pw_flight]&16))
        {
            frame = (leveltime/3)&15;
            if(CPlayer->mo->flags2&MF2_FLY)
            {
                if(hitCenterFrame && (frame != 15 && frame != 0))
                {
                    V_DrawScaledPatch(20, 17, FG, W_CachePatchNum(spinflylump+15,
                        PU_CACHE));
                }
                else
                {
                    V_DrawScaledPatch(20, 17, FG, W_CachePatchNum(spinflylump+frame,
                        PU_CACHE));
                    hitCenterFrame = false;
                }
            }
            else
            {
                if(!hitCenterFrame && (frame != 15 && frame != 0))
                {
                    V_DrawScaledPatch(20, 17, FG, W_CachePatchNum(spinflylump+frame,
                        PU_CACHE));
                    hitCenterFrame = false;
                }
                else
                {
                    V_DrawScaledPatch(20, 17, FG, W_CachePatchNum(spinflylump+15,
                        PU_CACHE));
                    hitCenterFrame = true;
                }
            }
            //                  BorderTopRefresh = true;
            //                  UpdateState |= I_MESSAGES;
        }
        else
        {
            //                  BorderTopRefresh = true;
            //                  UpdateState |= I_MESSAGES;
        }
    }
    
    if(CPlayer->powers[pw_weaponlevel2] && !CPlayer->chickenTics)
    {
        if(CPlayer->powers[pw_weaponlevel2] > BLINKTHRESHOLD
            || !(CPlayer->powers[pw_weaponlevel2]&16))
        {
            frame = (leveltime/3)&15;
            V_DrawScaledPatch(300, 17, FG, W_CachePatchNum(spinbooklump+frame, PU_CACHE));
            //                  BorderTopRefresh = true;
            //                  UpdateState |= I_MESSAGES;
        }
        else
        {
            //                  BorderTopRefresh = true;
            //                  UpdateState |= I_MESSAGES;
        }
    }
    /*
    if(CPlayer->powers[pw_weaponlevel2] > BLINKTHRESHOLD
    || (CPlayer->powers[pw_weaponlevel2]&8))
    {
    V_DrawScaledPatch(st_x+291, 0, 0, W_CachePatchName("ARTIPWBK", PU_CACHE));
    }
    else
    {
    BorderTopRefresh = true;
    }
    }
    */
}

// sets the new palette based upon current values of player->damagecount
// and player->bonuscount
void SB_PaletteFlash(void)
{
        static int sb_palette = 0;
        int palette;

        CPlayer = &players[consoleplayer];

        if(CPlayer->damagecount)
        {
                palette = (CPlayer->damagecount+7)>>3;
                if(palette >= NUMREDPALS)
                {
                        palette = NUMREDPALS-1;
                }
                palette += STARTREDPALS;
        }
        else if(CPlayer->bonuscount)
        {
                palette = (CPlayer->bonuscount+7)>>3;
                if(palette >= NUMBONUSPALS)
                {
                        palette = NUMBONUSPALS-1;
                }
                palette += STARTBONUSPALS;
        }
        else
        {
                palette = 0;
        }


        if(palette != sb_palette)
        {
            sb_palette = palette;

#ifdef HWRENDER
            if ( (rendermode == render_opengl) || (rendermode == render_d3d) )
            {
                //Hurdler: TODO: see if all heretic palettes are properly managed
                switch (palette) {
                    case 0x00: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0x0); break;  // no changes
                    case 0x01: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff373797); break; // red
                    case 0x02: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff373797); break; // red
                    case 0x03: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff3030a7); break; // red
                    case 0x04: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff2727b7); break; // red
                    case 0x05: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff2020c7); break; // red
                    case 0x06: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff1717d7); break; // red
                    case 0x07: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff1010e7); break; // red
                    case 0x08: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff0707f7); break; // red
                    case 0x09: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xffff6060); break; // blue
                    case 0x0a: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff70a090); break; // light green
                    case 0x0b: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff67b097); break; // light green
                    case 0x0c: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff60c0a0); break; // light green
                    case 0x0d: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xff60ff60); break; // green
                    case 0x0e: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xffff6060); break; // blue
                    case 0x0f: HWD.pfnSetSpecialState(HWD_SET_PALETTECOLOR, 0xffff6060); break; // blue
                }
            }
            else
#endif
            {
                if( !cv_splitscreen.value )
                    V_SetPalette (palette);
            }
        }
}

//---------------------------------------------------------------------------
//
// PROC DrawCommonBar
//
//---------------------------------------------------------------------------

static void DrawCommonBar(void)
{
        int chainY;
        int healthPos;

        V_DrawScaledPatch(st_x, ST_Y-10, fgbuffer, PatchLTFCTOP);
        V_DrawScaledPatch(st_x+290, ST_Y-10, fgbuffer, PatchRTFCTOP);

        if(oldhealth != HealthMarker)
        {
                oldhealth = HealthMarker;
                healthPos = HealthMarker;
                if(healthPos < 0)
                {
                        healthPos = 0;
                }
                if(healthPos > 100)
                {
                        healthPos = 100;
                }
                healthPos = (healthPos*256)/100;
                chainY = (HealthMarker == CPlayer->mo->health) ? 0 : ChainWiggle;
                V_DrawScaledPatch(st_x, ST_Y+32, fgbuffer, PatchCHAINBACK);
                V_DrawScaledPatch(st_x+2+(healthPos%17), ST_Y+33+chainY, fgbuffer, PatchCHAIN);
                V_DrawScaledPatch(st_x+17+healthPos, ST_Y+33+chainY, fgbuffer, PatchLIFEGEM);
                V_DrawScaledPatch(st_x, ST_Y+32, fgbuffer, PatchLTFACE);
                V_DrawScaledPatch(st_x+276, ST_Y+32, fgbuffer, PatchRTFACE);
                ShadeChain();
                UpdateState |= I_STATBAR;
        }
}

//---------------------------------------------------------------------------
//
// PROC DrawMainBar
//
//---------------------------------------------------------------------------

static void DrawMainBar(void)
{
        int temp;

        // Ready artifact
        if(ArtifactFlash)
        {
                V_DrawScaledPatch(st_x+180, ST_Y+3, fgbuffer, PatchBLACKSQ);
                V_DrawScaledPatch(st_x+182, ST_Y+3, fgbuffer, W_CachePatchNum(W_GetNumForName("useartia")
                        + ArtifactFlash - 1, PU_CACHE));
                ArtifactFlash--;
                oldarti = -1; // so that the correct artifact fills in after the flash
                UpdateState |= I_STATBAR;
        }
        else if(oldarti != CPlayer->inv_ptr
                || oldartiCount != CPlayer->inventory[CPlayer->inv_ptr].count)
        {
                V_DrawScaledPatch(st_x+180, ST_Y+3, fgbuffer, PatchBLACKSQ);
                if( CPlayer->inventory[CPlayer->inv_ptr].type > 0 )
                {
                        V_DrawScaledPatch(st_x+179,ST_Y+2, fgbuffer, 
                            W_CachePatchName(patcharti[CPlayer->inventory[CPlayer->inv_ptr].type], PU_CACHE));
                        DrSmallNumber(CPlayer->inventory[CPlayer->inv_ptr].count, st_x+201, ST_Y+24);
                }
                oldarti = CPlayer->inv_ptr;
                oldartiCount = CPlayer->inventory[CPlayer->inv_ptr].count;
                UpdateState |= I_STATBAR;
        }

        // Frags
        if(cv_deathmatch.value)
        {
                temp = ST_PlayerFrags(CPlayer-players);
                if(temp != oldfrags)
                {
                        V_DrawScaledPatch(st_x+57, ST_Y+13, fgbuffer, PatchARMCLEAR);
                        DrINumber(temp, st_x+61, ST_Y+12);
                        oldfrags = temp;
                        UpdateState |= I_STATBAR;
                }
        }
        else
        {
                temp = min(max(0,HealthMarker),100);
                if(oldlife != temp)
                {
                        oldlife = temp;
                        V_DrawScaledPatch(st_x+57, ST_Y+13, fgbuffer, PatchARMCLEAR);
                        DrINumber(temp, st_x+61, ST_Y+12);
                        UpdateState |= I_STATBAR;
                }
        }

        // Keys
        if(oldkeys != playerkeys)
        {
                if(CPlayer->cards & it_yellowcard)
                {
                        V_DrawScaledPatch(st_x+153, ST_Y+6, fgbuffer, W_CachePatchName("ykeyicon", PU_CACHE));
                }
                if(CPlayer->cards & it_redcard)
                {
                        V_DrawScaledPatch(st_x+153, ST_Y+14, fgbuffer, W_CachePatchName("gkeyicon", PU_CACHE));
                }
                if(CPlayer->cards & it_bluecard)
                {
                        V_DrawScaledPatch(st_x+153, ST_Y+22, fgbuffer, W_CachePatchName("bkeyicon", PU_CACHE));
                }
                oldkeys = playerkeys;
                UpdateState |= I_STATBAR;
        }
        // Ammo
        temp = CPlayer->ammo[wpnlev1info[CPlayer->readyweapon].ammo];
        if(oldammo != temp || oldweapon != CPlayer->readyweapon)
        {
                V_DrawScaledPatch(st_x+108, ST_Y+3, fgbuffer, PatchBLACKSQ);
                if(temp && CPlayer->readyweapon > 0 && CPlayer->readyweapon < 7)
                {
                        DrINumber(temp, st_x+109, ST_Y+4);
                        V_DrawScaledPatch(st_x+111, ST_Y+14, fgbuffer, W_CachePatchName(
                                ammopic[CPlayer->readyweapon-1], PU_CACHE));
                }
                oldammo = temp;
                oldweapon = CPlayer->readyweapon;
                UpdateState |= I_STATBAR;
        }

        // Armor
        if(oldarmor != CPlayer->armorpoints)
        {
                V_DrawScaledPatch(st_x+224, ST_Y+13, fgbuffer, PatchARMCLEAR);
                DrINumber(CPlayer->armorpoints, st_x+228, ST_Y+12);
                oldarmor = CPlayer->armorpoints;
                UpdateState |= I_STATBAR;
        }
}

//---------------------------------------------------------------------------
//
// PROC DrawInventoryBar
//
//---------------------------------------------------------------------------

static void DrawInventoryBar(void)
{
        int i;
        int x;

        x = CPlayer->inv_ptr-CPlayer->st_curpos;
        UpdateState |= I_STATBAR;
        V_DrawScaledPatch(st_x+34, ST_Y+2, fgbuffer, PatchINVBAR);
        for(i = 0; i < 7; i++)
        {
                //V_DrawScaledPatch(st_x+50+i*31, ST_Y+2, 0, W_CachePatchName("ARTIBOX", PU_CACHE));
                if(CPlayer->inventorySlotNum > x+i
                        && CPlayer->inventory[x+i].type != arti_none)
                {
                        V_DrawScaledPatch(st_x+50+i*31, ST_Y+2, fgbuffer, W_CachePatchName(
                                patcharti[CPlayer->inventory[x+i].type], PU_CACHE));
                        DrSmallNumber(CPlayer->inventory[x+i].count, st_x+69+i*31, ST_Y+24);
                }
        }
        V_DrawScaledPatch(st_x+50+CPlayer->st_curpos*31, ST_Y+31, fgbuffer, PatchSELECTBOX);
        if(x != 0)
        {
                V_DrawScaledPatch(st_x+38, ST_Y+1, fgbuffer, !(leveltime&4) ? PatchINVLFGEM1 :
                        PatchINVLFGEM2);
        }
        if(CPlayer->inventorySlotNum-x > 7)
        {
                V_DrawScaledPatch(st_x+269, ST_Y+1, fgbuffer, !(leveltime&4) ?
                        PatchINVRTGEM1 : PatchINVRTGEM2);
        }
}

static void DrawFullScreenStuff(void)
{
        int i;
        int x;
        int temp;

        UpdateState |= I_FULLSCRN;
        if(CPlayer->mo->health > 0)
                DrBNumber(CPlayer->mo->health, 5, ST_Y+22);
        else
                DrBNumber(0, 5, ST_Y+22);
        if(cv_deathmatch.value)
        {
                temp = ST_PlayerFrags(CPlayer-players);
                DrINumber(temp, 45, ST_Y+27);
        }
        if(!CPlayer->st_inventoryTics)
        {
                if( CPlayer->inventory[CPlayer->inv_ptr].type > 0)
                {
                        V_DrawFuzzPatch(st_x+286, ST_Y+12, W_CachePatchName("ARTIBOX",
                                PU_CACHE));
                        V_DrawScaledPatch(st_x+286, ST_Y+12, fgbuffer, 
                                W_CachePatchName(patcharti[CPlayer->inventory[CPlayer->inv_ptr].type], PU_CACHE));
                        DrSmallNumber(CPlayer->inventory[CPlayer->inv_ptr].count, st_x+307, ST_Y+34);
                }
        }
        else
        {
                x = CPlayer->inv_ptr-CPlayer->st_curpos;
                for(i = 0; i < 7; i++)
                {
                        V_DrawFuzzPatch(st_x+50+i*31, ST_Y+10, W_CachePatchName("ARTIBOX",
                                PU_CACHE));
                        if(CPlayer->inventorySlotNum > x+i
                                && CPlayer->inventory[x+i].type != arti_none)
                        {
                                V_DrawScaledPatch(st_x+50+i*31, ST_Y+10, fgbuffer, W_CachePatchName(
                                        patcharti[CPlayer->inventory[x+i].type], PU_CACHE));
                                DrSmallNumber(CPlayer->inventory[x+i].count, 69+i*31, ST_Y+32);
                        }
                }
                V_DrawScaledPatch(st_x+50+CPlayer->st_curpos*31, ST_Y+39, fgbuffer, PatchSELECTBOX);
                if(x != 0)
                {
                        V_DrawScaledPatch(st_x+38, ST_Y+9, fgbuffer, !(leveltime&4) ? PatchINVLFGEM1 :
                                PatchINVLFGEM2);
                }
                if(CPlayer->inventorySlotNum-x > 7)
                {
                        V_DrawScaledPatch(st_x+269, ST_Y+9, fgbuffer, !(leveltime&4) ?
                                PatchINVRTGEM1 : PatchINVRTGEM2);
                }
        }
}

//--------------------------------------------------------------------------
//
// FUNC SB_Responder
//
//--------------------------------------------------------------------------

boolean SB_Responder(event_t *event)
{
    return(false);
}
