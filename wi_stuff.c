// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: wi_stuff.c,v 1.15 2003/05/04 04:21:39 sburke Exp $
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
// $Log: wi_stuff.c,v $
// Revision 1.15  2003/05/04 04:21:39  sburke
// Use SHORT macro to convert little-endian shorts on big-endian machines.
//
// Revision 1.14  2001/06/30 15:06:01  bpereira
// fixed wronf next level name in intermission
//
// Revision 1.13  2001/05/16 21:21:15  bpereira
// no message
//
// Revision 1.12  2001/05/14 19:02:58  metzgermeister
//   * Fixed floor not moving up with player on E3M1
//   * Fixed crash due to oversized string in screen message ... bad bug!
//   * Corrected some typos
//   * fixed sound bug in SDL
//
// Revision 1.11  2001/03/03 06:17:34  bpereira
// no message
//
// Revision 1.10  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.9  2001/02/10 12:27:14  bpereira
// no message
//
// Revision 1.8  2001/01/27 11:02:36  bpereira
// no message
//
// Revision 1.7  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.6  2000/11/02 17:50:10  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.5  2000/09/21 16:45:09  bpereira
// no message
//
// Revision 1.4  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.3  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Intermission screens.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "wi_stuff.h"
#include "g_game.h"
#include "hu_stuff.h"
#include "m_random.h"
#include "r_local.h"
#include "s_sound.h"
#include "st_stuff.h"
#include "i_video.h"
#include "v_video.h"
#include "z_zone.h"
#include "console.h"
#include "p_info.h"

//
// Data needed to add patches to full screen intermission pics.
// Patches are statistics messages, and animations.
// Loads of by-pixel layout and placement, offsets etc.
//


//
// Different vetween registered DOOM (1994) and
//  Ultimate DOOM - Final edition (retail, 1995?).
// This is supposedly ignored for commercial
//  release (aka DOOM II), which had 34 maps
//  in one episode. So there.
#define NUMEPISODES     4
#define NUMMAPS         9


// GLOBAL LOCATIONS
#define WI_TITLEY               2
#define WI_SPACINGY             16 //TODO: was 33

// SINGPLE-PLAYER STUFF
#define SP_STATSX               50
#define SP_STATSY               50

#define SP_TIMEX                16
#define SP_TIMEY                (BASEVIDHEIGHT-32)


// NET GAME STUFF
#define NG_STATSY               50
#define NG_STATSX               (32 + SHORT(star->width)/2 + 32*!dofrags)

#define NG_SPACINGX             64


// DEATHMATCH STUFF
#define DM_MATRIXX              16
#define DM_MATRIXY              24

#define DM_SPACINGX             32

#define DM_TOTALSX              269

#define DM_KILLERSX             0
#define DM_KILLERSY             100
#define DM_VICTIMSX             5
#define DM_VICTIMSY             50
// in sec
#define DM_WAIT                 20



typedef enum
{
    ANIM_ALWAYS,
    ANIM_RANDOM,
    ANIM_LEVEL

} animenum_t;

typedef struct
{
    int         x;
    int         y;

} point_t;


//
// Animation.
// There is another anim_t used in p_spec.
//
typedef struct
{
    animenum_t  type;

    // period in tics between animations
    int         period;

    // number of animation frames
    int         nanims;

    // location of animation
    point_t     loc;

    // ALWAYS: n/a,
    // RANDOM: period deviation (<256),
    // LEVEL: level
    int         data1;

    // ALWAYS: n/a,
    // RANDOM: random base period,
    // LEVEL: n/a
    int         data2;

    // actual graphics for frames of animations
    patch_t*    p[3];

    // following must be initialized to zero before use!

    // next value of bcnt (used in conjunction with period)
    int         nexttic;

    // last drawn animation frame
    int         lastdrawn;

    // next frame number to animate
    int         ctr;

    // used by RANDOM and LEVEL when animating
    int         state;

} anim_t;

static point_t doomlnodes[NUMEPISODES][NUMMAPS] =
{
    // Episode 0 World Map
    {
        { 185, 164 },   // location of level 0 (CJ)
        { 148, 143 },   // location of level 1 (CJ)
        { 69, 122 },    // location of level 2 (CJ)
        { 209, 102 },   // location of level 3 (CJ)
        { 116, 89 },    // location of level 4 (CJ)
        { 166, 55 },    // location of level 5 (CJ)
        { 71, 56 },     // location of level 6 (CJ)
        { 135, 29 },    // location of level 7 (CJ)
        { 71, 24 }      // location of level 8 (CJ)
    },

    // Episode 1 World Map should go here
    {
        { 254, 25 },    // location of level 0 (CJ)
        { 97, 50 },     // location of level 1 (CJ)
        { 188, 64 },    // location of level 2 (CJ)
        { 128, 78 },    // location of level 3 (CJ)
        { 214, 92 },    // location of level 4 (CJ)
        { 133, 130 },   // location of level 5 (CJ)
        { 208, 136 },   // location of level 6 (CJ)
        { 148, 140 },   // location of level 7 (CJ)
        { 235, 158 }    // location of level 8 (CJ)
    },

    // Episode 2 World Map should go here
    {
        { 156, 168 },   // location of level 0 (CJ)
        { 48, 154 },    // location of level 1 (CJ)
        { 174, 95 },    // location of level 2 (CJ)
        { 265, 75 },    // location of level 3 (CJ)
        { 130, 48 },    // location of level 4 (CJ)
        { 279, 23 },    // location of level 5 (CJ)
        { 198, 48 },    // location of level 6 (CJ)
        { 140, 25 },    // location of level 7 (CJ)
        { 281, 136 }    // location of level 8 (CJ)
    }
};

static point_t YAHspot[3][9] =
{
    {
        { 172, 78 },
        { 86, 90 },
        { 73, 66 },
        { 159, 95 },
        { 148, 126 },
        { 132, 54 },
        { 131, 74 },
        { 208, 138 },
        { 52, 101 }
    },
    {
        { 218, 57 },
        { 137, 81 },
        { 155, 124 },
        { 171, 68 },
        { 250, 86 },
        { 136, 98 },
        { 203, 90 },
        { 220, 140 },
        { 279, 106 }
    },
    {
        { 86, 99 },
        { 124, 103 },
        { 154, 79 },
        { 202, 83 },
        { 178, 59 },
        { 142, 58 },
        { 219, 66 },
        { 247, 57 },
        { 107, 80 }
    }
};


//
// Animation locations for episode 0 (1).
// Using patches saves a lot of space,
//  as they replace 320x200 full screen frames.
//
static anim_t epsd0animinfo[] =
{
    { ANIM_ALWAYS, TICRATE/3, 3, { 224, 104 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 184, 160 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 112, 136 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 72, 112 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 88, 96 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 64, 48 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 192, 40 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 136, 16 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 80, 16 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 64, 24 } }
};

static anim_t epsd1animinfo[] =
{
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 1 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 2 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 3 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 4 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 5 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 6 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 7 },
    { ANIM_LEVEL, TICRATE/3, 3, { 192, 144 }, 8 },
    { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 8 }
};

static anim_t epsd2animinfo[] =
{
    { ANIM_ALWAYS, TICRATE/3, 3, { 104, 168 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 40, 136 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 160, 96 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 104, 80 } },
    { ANIM_ALWAYS, TICRATE/3, 3, { 120, 32 } },
    { ANIM_ALWAYS, TICRATE/4, 3, { 40, 0 } }
};

static int NUMANIMS[NUMEPISODES] =
{
    sizeof(epsd0animinfo)/sizeof(anim_t),
    sizeof(epsd1animinfo)/sizeof(anim_t),
    sizeof(epsd2animinfo)/sizeof(anim_t)
};

static anim_t *anims[NUMEPISODES] =
{
    epsd0animinfo,
    epsd1animinfo,
    epsd2animinfo
};


//
// GENERAL DATA
//

//
// Locally used stuff.
//
#define FB 0

// States for the intermission

typedef enum
{
    NoState = -1,
    StatCount,
    ShowNextLoc

} stateenum_t;

// States for single-player
#define SP_KILLS                0
#define SP_ITEMS                2
#define SP_SECRET               4
#define SP_FRAGS                6
#define SP_TIME                 8
#define SP_PAR                  ST_TIME

#define SP_PAUSE                1

// in seconds
#define SHOWNEXTLOCDELAY        4
//#define SHOWLASTLOCDELAY      SHOWNEXTLOCDELAY


// used to accelerate or skip a stage
static int              acceleratestage;

// wbs->pnum
static int              me;

 // specifies current state
static stateenum_t      state;

// contains information passed into intermission
static wbstartstruct_t* wbs;

static wbplayerstruct_t* plrs;  // wbs->plyr[]

// used for general timing
static int              cnt;

// used for timing of background animation
static int              bcnt;

// signals to refresh everything for one frame
static int              firstrefresh;

static int              cnt_kills[MAXPLAYERS];
static int              cnt_items[MAXPLAYERS];
static int              cnt_secret[MAXPLAYERS];
static int              cnt_time;
static int              cnt_par;
static int              cnt_pause;

// # of commercial levels
static int              NUMCMAPS;


//
//      GRAPHICS
//

// background (map of levels).
//static patch_t*       bg;
static char             bgname[9];

// You Are Here graphic
static patch_t*         yah[2];

// splat
static patch_t*         splat;

// %, : graphics
static patch_t*         percent;
static patch_t*         colon;

// 0-9 graphic
static patch_t*         num[10];

// minus sign
static patch_t*         wiminus;

// "Finished!" graphics
static patch_t*         finished;

// "Entering" graphic
static patch_t*         entering;

// "secret"
static patch_t*         sp_secret;

 // "Kills", "Scrt", "Items", "Frags"
static patch_t*         kills;
static patch_t*         secret;
static patch_t*         items;
static patch_t*         frags;

// Time sucks.
static patch_t*         timePatch;
static patch_t*         par;
static patch_t*         sucks;

// "killers", "victims"
static patch_t*         killers;
static patch_t*         victims;

// "Total", your face, your dead face
static patch_t*         total;
static patch_t*         star;
static patch_t*         bstar;

//added:08-02-98: use STPB0 for all players, but translate the colors
static patch_t*         stpb;

// Name graphics of each level (centered)
static patch_t**        lnames;

//
// CODE
//

// slam background
// UNUSED static unsigned char *background=0;

static void WI_slamBackground(void)
{
    if( gamemode == heretic && state == StatCount)
        V_DrawFlatFill( 0, 0, vid.width/vid.dupx, vid.height/vid.dupy, W_CheckNumForName("FLOOR16"));
    else
    if (rendermode==render_soft) 
    {
        memcpy(screens[0], screens[1], vid.width * vid.height);
        V_MarkRect (0, 0, vid.width, vid.height);
    }
    else 
        V_DrawScaledPatch(0, 0, 1+V_NOSCALESTART, W_CachePatchName(bgname, PU_CACHE));
}

// The ticker is used to detect keys
//  because of timing issues in netgames.
boolean WI_Responder(event_t* ev)
{
    return false;
}


// Draws "<Levelname> Finished!"
static void WI_drawLF(void)
{
    int y = WI_TITLEY;

    // draw <LevelName>
    if( FontBBaseLump )
    {
        V_DrawTextB(P_LevelName(), (BASEVIDWIDTH - V_TextBWidth(P_LevelName()))/2, y);
        y += (5*V_TextBHeight(P_LevelName()))/4;
        V_DrawTextB("Finished", (BASEVIDWIDTH - V_TextBWidth("Finished"))/2, y);
    }
    else
    {
        V_DrawScaledPatch ((BASEVIDWIDTH - SHORT(lnames[wbs->last]->width))/2,
                            y, FB, lnames[wbs->last]);
        y += (5 * SHORT(lnames[wbs->last]->height))/4;
        // draw "Finished!"
        V_DrawScaledPatch ((BASEVIDWIDTH - SHORT(finished->width))/2,
                            y, FB, finished);
    }
}



// Draws "Entering <LevelName>"
static void WI_drawEL(void)
{
    int y = WI_TITLEY;

    // draw "Entering"
    if( FontBBaseLump )
    {
        V_DrawTextB("Entering", (BASEVIDWIDTH - V_TextBWidth("Entering"))/2, y);
        y += (5*V_TextBHeight("Entering"))/4;
        V_DrawTextB(P_LevelNameByNum(wbs->epsd+1, wbs->next+1), 
                    (BASEVIDWIDTH - V_TextBWidth(P_LevelNameByNum(wbs->epsd+1, wbs->next+1)))/2, y);
    }
    else
    {
        V_DrawScaledPatch((BASEVIDWIDTH - SHORT(entering->width))/2,
                          y, FB, entering);

        // draw level
        y += (5 * SHORT(lnames[wbs->next]->height))/4;

        V_DrawScaledPatch((BASEVIDWIDTH - SHORT(lnames[wbs->next]->width))/2,
                           y, FB, lnames[wbs->next]);
    }

}

static void WI_drawOnLnode ( int           n,
                             patch_t*      c[] )
{

    int         i;
    int         left;
    int         top;
    int         right;
    int         bottom;
    boolean     fits = false;

    point_t     *lnodes;

    lnodes = &doomlnodes[wbs->epsd][n];

    i = 0;
    do
    {
        left   = lnodes->x - SHORT(c[i]->leftoffset);
        top    = lnodes->y - SHORT(c[i]->topoffset);
        right  = left + SHORT(c[i]->width);
        bottom =  top + SHORT(c[i]->height);

        if (left >= 0
            && right < BASEVIDWIDTH
            && top >= 0
            && bottom < BASEVIDHEIGHT)
        {
            fits = true;
        }
        else
        {
            i++;
        }
    } while (!fits && i!=2);

    if (fits && i<2)
        V_DrawScaledPatch(lnodes->x, lnodes->y, FB, c[i]);
    else
        // DEBUG
        CONS_Printf("Could not place patch on level %d\n", n+1);
}


//========================================================================
//
// IN_DrawYAH
//
//========================================================================
static void IN_DrawYAH(void)
{
    int i;
    int x;
    int prevmap;    

    x = (BASEVIDWIDTH-V_StringWidth("NOW ENTERING:"))/2;
    V_DrawString(x, 10, 0, "NOW ENTERING:");

    x = (BASEVIDWIDTH-V_TextBWidth(P_LevelNameByNum(wbs->epsd+1, wbs->next+1)))/2;
    V_DrawTextB(P_LevelNameByNum(wbs->epsd+1, wbs->next+1), x, 20);

    prevmap = (wbs->last == 8) ? wbs->next - 1 : wbs->last;

    for(i=0; i<=prevmap; i++)
    {
        V_DrawScaledPatch(YAHspot[gameepisode-1][i].x, YAHspot[gameepisode-1][i].y, 0,
            splat);
    }
    if(players[consoleplayer].didsecret)
    {
        V_DrawScaledPatch(YAHspot[gameepisode-1][8].x, YAHspot[gameepisode-1][8].y, 0,
            splat);
    }
    if(!(bcnt&16) || state == ShowNextLoc)
    { // draw the destination 'X'
        V_DrawScaledPatch(YAHspot[gameepisode-1][wbs->next].x,
                          YAHspot[gameepisode-1][wbs->next].y, 0, yah[0]);
    }
}



static void WI_initAnimatedBack(void)
{
    int         i;
    anim_t*     a;

    if (gamemode == commercial || gamemode == heretic)
        return;

    if (wbs->epsd > 2)
        return;

    for (i=0;i<NUMANIMS[wbs->epsd];i++)
    {
        a = &anims[wbs->epsd][i];

        // init variables
        a->ctr = -1;

        // specify the next time to draw it
        if (a->type == ANIM_ALWAYS)
            a->nexttic = bcnt + 1 + (M_Random()%a->period);
        else if (a->type == ANIM_RANDOM)
            a->nexttic = bcnt + 1 + a->data2+(M_Random()%a->data1);
        else if (a->type == ANIM_LEVEL)
            a->nexttic = bcnt + 1;
    }

}

static void WI_updateAnimatedBack(void)
{
    int         i;
    anim_t*     a;

    if (gamemode == commercial || gamemode == heretic)
        return;

    if (wbs->epsd > 2)
        return;

    for (i=0;i<NUMANIMS[wbs->epsd];i++)
    {
        a = &anims[wbs->epsd][i];

        if (bcnt >= a->nexttic)
        {
            switch (a->type)
            {
              case ANIM_ALWAYS:
                if (++a->ctr >= a->nanims) a->ctr = 0;
                a->nexttic = bcnt + a->period;
                break;

              case ANIM_RANDOM:
                a->ctr++;
                if (a->ctr == a->nanims)
                {
                    a->ctr = -1;
                    a->nexttic = bcnt+a->data2+(M_Random()%a->data1);
                }
                else a->nexttic = bcnt + a->period;
                break;

              case ANIM_LEVEL:
                // gawd-awful hack for level anims
                if (!(state == StatCount && i == 7)
                    && wbs->next == a->data1)
                {
                    a->ctr++;
                    if (a->ctr == a->nanims) a->ctr--;
                    a->nexttic = bcnt + a->period;
                }
                break;
            }
        }

    }

}

static void WI_drawAnimatedBack(void)
{
    int                 i;
    anim_t*             a;

    //BP: fixed it was "if (commercial)" 
    if (gamemode == commercial || gamemode == heretic)
        return;

    if (wbs->epsd > 2)
        return;

    for (i=0 ; i<NUMANIMS[wbs->epsd] ; i++)
    {
        a = &anims[wbs->epsd][i];

        if (a->ctr >= 0)
            V_DrawScaledPatch(a->loc.x, a->loc.y, FB, a->p[a->ctr]);
    }

}

//
// Draws a number.
// If digits > 0, then use that many digits minimum,
//  otherwise only use as many as necessary.
// Returns new x position.
//

static int WI_drawNum ( int           x,
                        int           y,
                        int           n,
                        int           digits )
{

    int         fontwidth = SHORT(num[0]->width);
    int         neg;
    int         temp;

    if (digits < 0)
    {
        if (!n)
        {
            // make variable-length zeros 1 digit long
            digits = 1;
        }
        else
        {
            // figure out # of digits in #
            digits = 0;
            temp = n;

            while (temp)
            {
                temp /= 10;
                digits++;
            }
        }
    }

    neg = n < 0;
    if (neg)
        n = -n;

    // if non-number, do not draw it
    if (n == 1994)
        return 0;

    // draw the new number
    while (digits--)
    {
        x -= fontwidth;
        V_DrawScaledPatch(x, y, FB, num[ n % 10 ]);
        n /= 10;
    }

    // draw a minus sign if necessary
    if (neg)
        V_DrawScaledPatch(x-=8, y, FB, wiminus);

    return x;

}

static void WI_drawPercent( int           x,
                            int           y,
                            int           p )
{
    if (p < 0)
        return;

    V_DrawScaledPatch(x, y, FB, percent);
    WI_drawNum(x, y, p, -1);
}



//
// Display level completion time and par,
//  or "sucks" message if overflow.
//
static void WI_drawTime ( int           x,
                          int           y,
                          int           t )
{

    int         div;
    int         n;

    if (t<0)
        return;

    if (t <= 61*59)
    {
        div = 1;

        do
        {
            n = (t / div) % 60;
            x = WI_drawNum(x, y, n, 2) - SHORT(colon->width);
            div *= 60;

            // draw
            if (div==60 || t / div)
                V_DrawScaledPatch(x, y, FB, colon);

        } while (t / div);
    }
    else
    {
        // "sucks"
        V_DrawScaledPatch(x - SHORT(sucks->width), y, FB, sucks);
    }
}

static void WI_unloadData(void);

static void WI_End(void)
{
    WI_unloadData();
}

// used for write introduce next level
static void WI_initNoState(void)
{
    state = NoState;
    acceleratestage = 0;
    cnt = 10;
}

static void WI_updateNoState(void) {

    WI_updateAnimatedBack();

    if (--cnt==0)
    {
        WI_End();
        G_NextLevel();
    }

}

static boolean          snl_pointeron = false;


static void WI_initShowNextLoc(void)
{
    state = ShowNextLoc;
    acceleratestage = 0;
    cnt = SHOWNEXTLOCDELAY * TICRATE;

    WI_initAnimatedBack();
}

static void WI_updateShowNextLoc(void)
{
    WI_updateAnimatedBack();

    if (!--cnt || acceleratestage)
        WI_initNoState();
    else
        snl_pointeron = (cnt & 31) < 20;
}

static void WI_drawShowNextLoc(void)
{

    int         i;
    int         last;

    if (cnt<=0)  // all removed no draw !!!
        return;

    WI_slamBackground();

    // draw animated background
    WI_drawAnimatedBack();

    if( gamemode == heretic )
    {
        if( gameepisode < 4 )
            IN_DrawYAH();
    }
    else
    if ( gamemode != commercial && wbs->epsd<=2)
    {
        last = (wbs->last == 8) ? wbs->next - 1 : wbs->last;

        // draw a splat on taken cities.
        for (i=0 ; i<=last ; i++)
            WI_drawOnLnode(i, &splat);

        // splat the secret level?
        if (wbs->didsecret)
            WI_drawOnLnode(8, &splat);

        // draw flashing ptr
        if (snl_pointeron)
            WI_drawOnLnode(wbs->next, yah);
    }

    // draws which level you are entering..
    if ( (gamemode != commercial || wbs->next != 30) && 
          gamemode != heretic)
        WI_drawEL();

}

static void WI_drawNoState(void)
{
    snl_pointeron = true;
    WI_drawShowNextLoc();
}


static int              dm_frags[MAXPLAYERS][MAXPLAYERS];
static int              dm_totals[MAXPLAYERS];

static void WI_initDeathmatchStats(void)
{

    int         i;
    int         j;

    state = StatCount;
    acceleratestage = 0;

    cnt_pause = TICRATE*DM_WAIT;

    for (i=0 ; i<MAXPLAYERS ; i++)
         if (playeringame[i])
         {
             for(j=0; j<MAXPLAYERS; j++)
                 if( playeringame[j] )
                     dm_frags[i][j] = plrs[i].frags[j];
             
             dm_totals[i] = ST_PlayerFrags(i);
    }

    WI_initAnimatedBack();
}

static void WI_updateDeathmatchStats(void)
{
    WI_updateAnimatedBack();

    if( paused )
        return;
    if (cnt_pause>0) cnt_pause--;
    if (cnt_pause==0)
    {
        S_StartSound(0, sfx_slop);

        WI_initNoState();
    }
}


//  Quick-patch for the Cave party 19-04-1998 !!
//
void WI_drawRancking(char *title,int x,int y,fragsort_t *fragtable
                   , int scorelines, boolean large, int white)
{
    int   i,j;
    int   color;
    char  num[12];
    int   plnum;
    int   frags;
    int   colornum;
    fragsort_t temp;

    if( gamemode == heretic )
        colornum = 230;
    else
        colornum = 0x78;

    // sort the frags count
    for (i=0; i<scorelines; i++)
        for(j=0; j<scorelines-1-i; j++)
            if( fragtable[j].count < fragtable[j+1].count )
            {
                temp = fragtable[j];
                fragtable[j] = fragtable[j+1];
                fragtable[j+1] = temp;
            }

    if(title)
        V_DrawString (x, y-14, 0, title);
    // draw rankings
    for (i=0; i<scorelines; i++)
    {
        frags = fragtable[i].count;
        plnum = fragtable[i].num;

        // draw color background
        color = fragtable[i].color;
        if (!color)
            color = *( (byte *)colormaps + colornum );
        else
            color = *( (byte *)translationtables - 256 + (color<<8) + colornum );
        V_DrawFill (x-1,y-1,large ? 40 : 26,9,color);

        // draw frags count
        sprintf(num,"%3i", frags );
        V_DrawString (x+(large ? 32 : 24)-V_StringWidth(num), y, 0, num);

        // draw name
        V_DrawString (x+(large ? 64 : 29), y, plnum == white ? V_WHITEMAP : 0, fragtable[i].name);

        y += 12;
        if (y>=BASEVIDHEIGHT)
            break;            // dont draw past bottom of screen
    }
}

#define RANKINGY 60

static void WI_drawDeathmatchStats(void)
{
    int          i,j;
    int          scorelines;
    int          whiteplayer;
    fragsort_t   fragtab[MAXPLAYERS];
    char         *timeleft;

    WI_slamBackground();

    // draw animated background
    WI_drawAnimatedBack();
    WI_drawLF();

    //Fab:25-04-98: when you play, you quickly see your frags because your
    //  name is displayed white, when playback demo, you quicly see who's the
    //  view.
    whiteplayer = demoplayback ? displayplayer : consoleplayer;

    // count frags for each present player
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (playeringame[i])
        {
            fragtab[scorelines].count = dm_totals[i];
            fragtab[scorelines].num   = i;
            fragtab[scorelines].color = players[i].skincolor;
            fragtab[scorelines].name  = player_names[i];
            scorelines++;
        }
    WI_drawRancking("Frags",5,RANKINGY,fragtab,scorelines,false,whiteplayer);

    // count buchholz
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (playeringame[i])
        {
            fragtab[scorelines].count = 0;
            for (j=0; j<MAXPLAYERS; j++)
                if (playeringame[j] && i!=j)
                     fragtab[scorelines].count+= dm_frags[i][j]*(dm_totals[j]+dm_frags[j][j]);

            fragtab[scorelines].num = i;
            fragtab[scorelines].color = players[i].skincolor;
            fragtab[scorelines].name  = player_names[i];
            scorelines++;
        }
    WI_drawRancking("Buchholz",85,RANKINGY,fragtab,scorelines,false,whiteplayer);

    // count individuel
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (playeringame[i])
        {
            fragtab[scorelines].count = 0;
            for (j=0; j<MAXPLAYERS; j++)
                if (playeringame[j] && i!=j)
                {
                     if(dm_frags[i][j]>dm_frags[j][i])
                         fragtab[scorelines].count+=3;
                     else
                         if(dm_frags[i][j]==dm_frags[j][i])
                              fragtab[scorelines].count+=1;
                }

            fragtab[scorelines].num = i;
            fragtab[scorelines].color = players[i].skincolor;
            fragtab[scorelines].name  = player_names[i];
            scorelines++;
        }
    WI_drawRancking("indiv.",165,RANKINGY,fragtab,scorelines,false,whiteplayer);

    // count deads
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (playeringame[i])
        {
            fragtab[scorelines].count = 0;
            for (j=0; j<MAXPLAYERS; j++)
                if (playeringame[j])
                     fragtab[scorelines].count+=dm_frags[j][i];
            fragtab[scorelines].num   = i;
            fragtab[scorelines].color = players[i].skincolor;
            fragtab[scorelines].name  = player_names[i];

            scorelines++;
        }
    WI_drawRancking("deads",245,RANKINGY,fragtab,scorelines,false,whiteplayer);

    timeleft=va("start in %d",cnt_pause/TICRATE);
    //i=V_StringWidth(num);
    V_DrawString (200, 30, V_WHITEMAP, timeleft);
}

boolean teamingame(int teamnum)
{
   int i;

   if (cv_teamplay.value == 1)
   {
       for(i=0;i<MAXPLAYERS;i++)
          if(playeringame[i] && players[i].skincolor==teamnum)
              return true;
   }
   else
   if (cv_teamplay.value == 2)
   {
       for(i=0;i<MAXPLAYERS;i++)
          if(playeringame[i] && players[i].skin==teamnum)
              return true;
   }
   return false;
}

static void WI_drawTeamsStats(void)
{
    int          i,j;
    int          scorelines;
    int          whiteplayer;
    fragsort_t   fragtab[MAXPLAYERS];

    WI_slamBackground();

    // draw animated background
    WI_drawAnimatedBack();
    WI_drawLF();

    //Fab:25-04-98: when you play, you quickly see your frags because your
    //  name is displayed white, when playback demo, you quicly see who's the
    //  view.
    if(cv_teamplay.value==1)
        whiteplayer = demoplayback ? players[displayplayer].skincolor
                                   : players[consoleplayer].skincolor;
    else
        whiteplayer = demoplayback ? players[displayplayer].skin
                                   : players[consoleplayer].skin;

    // count frags for each present player
    scorelines = HU_CreateTeamFragTbl(fragtab,dm_totals,dm_frags);

    WI_drawRancking("Frags",5,80,fragtab,scorelines,false,whiteplayer);

    // count buchholz
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (teamingame(i))
        {
            fragtab[scorelines].count = 0;
            for (j=0; j<MAXPLAYERS; j++)
                if (teamingame(j) && i!=j)
                    fragtab[scorelines].count+= dm_frags[i][j]*dm_totals[j];

            fragtab[scorelines].num   = i;
            fragtab[scorelines].color = i;
            fragtab[scorelines].name  = team_names[i];
            scorelines++;
        }
    WI_drawRancking("Buchholz",85,80,fragtab,scorelines,false,whiteplayer);

    // count individuel
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (teamingame(i))
        {
            fragtab[scorelines].count = 0;
            for (j=0; j<MAXPLAYERS; j++)
                if (teamingame(j) && i!=j)
                {
                     if(dm_frags[i][j]>dm_frags[j][i])
                         fragtab[scorelines].count+=3;
                     else
                         if(dm_frags[i][j]==dm_frags[j][i])
                              fragtab[scorelines].count+=1;
                }

            fragtab[scorelines].num = i;
            fragtab[scorelines].color = i;
            fragtab[scorelines].name  = team_names[i];
            scorelines++;
        }
    WI_drawRancking("indiv.",165,80,fragtab,scorelines,false,whiteplayer);

    // count deads
    scorelines = 0;
    for (i=0; i<MAXPLAYERS; i++)
        if (teamingame(i))
        {
            fragtab[scorelines].count = 0;
            for (j=0; j<MAXPLAYERS; j++)
                if (teamingame(j))
                     fragtab[scorelines].count+=dm_frags[j][i];
            fragtab[scorelines].num   = i;
            fragtab[scorelines].color = i;
            fragtab[scorelines].name  = team_names[i];

            scorelines++;
        }
    WI_drawRancking("deads",245,80,fragtab,scorelines,false,whiteplayer);
}


/* old code
static void WI_ddrawDeathmatchStats(void)
{

    int         i;
    int         j;
    int         x;
    int         y;
    int         w;

    int         lh;     // line height

    byte*       colormap;       //added:08-02-98:see below

    lh = WI_SPACINGY;

    WI_slamBackground();

    // draw animated background
    WI_drawAnimatedBack();
    WI_drawLF();

    // draw stat titles (top line)
    V_DrawScaledPatch(DM_TOTALSX-SHORT(total->width)/2,
                DM_MATRIXY-WI_SPACINGY+10,
                FB,
                total);

    V_DrawScaledPatch(DM_KILLERSX, DM_KILLERSY, FB, killers);
    V_DrawScaledPatch(DM_VICTIMSX, DM_VICTIMSY, FB, victims);

    // draw P?
    x = DM_MATRIXX + DM_SPACINGX;
    y = DM_MATRIXY;

    for (i=0 ; i<MAXPLAYERS ; i++)
    {
        if (playeringame[i])
        {
            //added:08-02-98: use V_DrawMappedPatch instead of
            //                    V_DrawScaledPatch, so that the
            // graphics are 'colormapped' to the player's colors!
            if (players[i].skincolor==0)
                colormap = colormaps;
            else
                colormap = (byte *) translationtables - 256 + (players[i].skincolor<<8);

            V_DrawMappedPatch(x-SHORT(stpb->width)/2,
                        DM_MATRIXY - WI_SPACINGY,
                        FB,
                        stpb,      //p[i], now uses a common STPB0 translated
                        colormap); //      to the right colors

            V_DrawMappedPatch(DM_MATRIXX-SHORT(stpb->width)/2,
                        y,
                        FB,
                        stpb,      //p[i]
                        colormap);

            if (i == me)
            {
                V_DrawScaledPatch(x-SHORT(stpb->width)/2,
                            DM_MATRIXY - WI_SPACINGY,
                            FB,
                            bstar);

                V_DrawScaledPatch(DM_MATRIXX-SHORT(stpb->width)/2,
                            y,
                            FB,
                            star);
            }
        }
        else
        {
            // V_DrawPatch(x-SHORT(bp[i]->width)/2,
            //   DM_MATRIXY - WI_SPACINGY, FB, bp[i]);
            // V_DrawPatch(DM_MATRIXX-SHORT(bp[i]->width)/2,
            //   y, FB, bp[i]);
        }
        x += DM_SPACINGX;
        y += WI_SPACINGY;
    }

    // draw stats
    y = DM_MATRIXY+10;
    w = SHORT(num[0]->width);

    for (i=0 ; i<MAXPLAYERS ; i++)
    {
        x = DM_MATRIXX + DM_SPACINGX;

        if (playeringame[i])
        {
            for (j=0 ; j<MAXPLAYERS ; j++)
            {
                if (playeringame[j])
                    WI_drawNum(x+w, y, dm_frags[i][j], 2);

                x += DM_SPACINGX;
            }
            WI_drawNum(DM_TOTALSX+w, y, dm_totals[i], 2);
        }
        y += WI_SPACINGY;
    }
}

*/

static int      cnt_frags[MAXPLAYERS];
static int      dofrags;
static int      ng_state;

static void WI_initNetgameStats(void)
{

    int i;

    state = StatCount;
    acceleratestage = 0;
    ng_state = 1;

    cnt_pause = TICRATE;

    for (i=0 ; i<MAXPLAYERS ; i++)
    {
        if (!playeringame[i])
            continue;

        cnt_kills[i] = cnt_items[i] = cnt_secret[i] = cnt_frags[i] = 0;

        dofrags += ST_PlayerFrags(i);
    }

    dofrags = !!dofrags;

    WI_initAnimatedBack();
}



static void WI_updateNetgameStats(void)
{

    int         i;
    int         fsum;

    boolean     stillticking;

    WI_updateAnimatedBack();

    if (acceleratestage && ng_state != 10)
    {
        acceleratestage = 0;

        for (i=0 ; i<MAXPLAYERS ; i++)
        {
            if (!playeringame[i])
                continue;

            cnt_kills[i] = (plrs[i].skills * 100) / wbs->maxkills;
            cnt_items[i] = (plrs[i].sitems * 100) / wbs->maxitems;
            cnt_secret[i] = (plrs[i].ssecret * 100) / wbs->maxsecret;

            if (dofrags)
                cnt_frags[i] = ST_PlayerFrags(i);
        }
        S_StartSound(0, sfx_barexp);
        ng_state = 10;
    }

    if (ng_state == 2)
    {
        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        stillticking = false;

        for (i=0 ; i<MAXPLAYERS ; i++)
        {
            if (!playeringame[i])
                continue;

            cnt_kills[i] += 2;

            if (cnt_kills[i] >= (plrs[i].skills * 100) / wbs->maxkills)
                cnt_kills[i] = (plrs[i].skills * 100) / wbs->maxkills;
            else
                stillticking = true;
        }

        if (!stillticking)
        {
            S_StartSound(0, sfx_barexp);
            ng_state++;
        }
    }
    else if (ng_state == 4)
    {
        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        stillticking = false;

        for (i=0 ; i<MAXPLAYERS ; i++)
        {
            if (!playeringame[i])
                continue;

            cnt_items[i] += 2;
            if (cnt_items[i] >= (plrs[i].sitems * 100) / wbs->maxitems)
                cnt_items[i] = (plrs[i].sitems * 100) / wbs->maxitems;
            else
                stillticking = true;
        }
        if (!stillticking)
        {
            S_StartSound(0, sfx_barexp);
            ng_state++;
        }
    }
    else if (ng_state == 6)
    {
        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        stillticking = false;

        for (i=0 ; i<MAXPLAYERS ; i++)
        {
            if (!playeringame[i])
                continue;

            cnt_secret[i] += 2;

            if (cnt_secret[i] >= (plrs[i].ssecret * 100) / wbs->maxsecret)
                cnt_secret[i] = (plrs[i].ssecret * 100) / wbs->maxsecret;
            else
                stillticking = true;
        }

        if (!stillticking)
        {
            S_StartSound(0, sfx_barexp);
            ng_state += 1 + 2*!dofrags;
        }
    }
    else if (ng_state == 8)
    {
        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        stillticking = false;

        for (i=0 ; i<MAXPLAYERS ; i++)
        {
            if (!playeringame[i])
                continue;

            cnt_frags[i] += 1;

            if (cnt_frags[i] >= (fsum = ST_PlayerFrags(i)))
                cnt_frags[i] = fsum;
            else
                stillticking = true;
        }

        if (!stillticking)
        {
            S_StartSound(0, sfx_pldeth);
            ng_state++;
        }
    }
    else if (ng_state == 10)
    {
        if (acceleratestage)
        {
            S_StartSound(0, sfx_sgcock);
            if ( gamemode == commercial )
                WI_initNoState();
            else
                WI_initShowNextLoc();
        }
    }
    else if (ng_state & 1)
    {
        if (!--cnt_pause)
        {
            ng_state++;
            cnt_pause = TICRATE;
        }
    }
}



static void WI_drawNetgameStats(void)
{
    int         i;
    int         x;
    int         y;
    int         pwidth = SHORT(percent->width);

    byte*       colormap;   //added:08-02-98: remap STBP0 to player color

    WI_slamBackground();

    // draw animated background
    WI_drawAnimatedBack();

    WI_drawLF();

    // draw stat titles (top line)
    if( FontBBaseLump )
    {
        // use FontB if any
        V_DrawTextB("Kills", NG_STATSX+  NG_SPACINGX-V_TextBWidth("Kills"), NG_STATSY);
        V_DrawTextB("Items", NG_STATSX+2*NG_SPACINGX-V_TextBWidth("Items"), NG_STATSY);
        V_DrawTextB("Scrt", NG_STATSX+3*NG_SPACINGX-V_TextBWidth("Scrt"), NG_STATSY);
        if (dofrags)
            V_DrawTextB("Frgs", NG_STATSX+4*NG_SPACINGX-V_TextBWidth("Frgs"), NG_STATSY);

        y = NG_STATSY + V_TextBHeight("Kills");
    }
    else
    {
        V_DrawScaledPatch(NG_STATSX+NG_SPACINGX-SHORT(kills->width),
            NG_STATSY, FB, kills);
        
        V_DrawScaledPatch(NG_STATSX+2*NG_SPACINGX-SHORT(items->width),
            NG_STATSY, FB, items);
        
        V_DrawScaledPatch(NG_STATSX+3*NG_SPACINGX-SHORT(secret->width),
            NG_STATSY, FB, secret);
        if (dofrags)
            V_DrawScaledPatch(NG_STATSX+4*NG_SPACINGX-SHORT(frags->width),
                              NG_STATSY, FB, frags);
        // draw stats
        y = NG_STATSY + SHORT(kills->height);
    }


    //added:08-02-98: p[i] replaced by stpb (see WI_loadData for more)
    for (i=0 ; i<MAXPLAYERS ; i++)
    {
        if (!playeringame[i])
            continue;

        x = NG_STATSX;
        if (players[i].skincolor==0)
            colormap = colormaps;       //no translation table for green guy
        else
            colormap = (byte *) translationtables - 256 + (players[i].skincolor<<8);

        V_DrawMappedPatch(x-SHORT(stpb->width), y, FB, stpb, colormap);

        if (i == me)
            V_DrawScaledPatch(x-SHORT(stpb->width), y, FB, star);

        x += NG_SPACINGX;
        WI_drawPercent(x-pwidth, y+10, cnt_kills[i]);   x += NG_SPACINGX;
        WI_drawPercent(x-pwidth, y+10, cnt_items[i]);   x += NG_SPACINGX;
        WI_drawPercent(x-pwidth, y+10, cnt_secret[i]);  x += NG_SPACINGX;

        if (dofrags)
            WI_drawNum(x, y+10, cnt_frags[i], -1);

        y += WI_SPACINGY;
    }

}

static int sp_state;

static void WI_initStats(void)
{
    state = StatCount;
    acceleratestage = 0;
    sp_state = 1;
    cnt_kills[0] = cnt_items[0] = cnt_secret[0] = -1;
    cnt_time = cnt_par = -1;
    cnt_pause = TICRATE;

    WI_initAnimatedBack();
}

static void WI_updateStats(void)
{

    WI_updateAnimatedBack();

    if (acceleratestage && sp_state != 10)
    {
        acceleratestage = 0;
        cnt_kills[0] = (plrs[me].skills * 100) / wbs->maxkills;
        cnt_items[0] = (plrs[me].sitems * 100) / wbs->maxitems;
        cnt_secret[0] = (plrs[me].ssecret * 100) / wbs->maxsecret;
        cnt_time = plrs[me].stime / TICRATE;
        cnt_par = wbs->partime / TICRATE;
        S_StartSound(0, sfx_barexp);
        sp_state = 10;
    }

    if (sp_state == 2)
    {
        cnt_kills[0] += 2;

        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        if (cnt_kills[0] >= (plrs[me].skills * 100) / wbs->maxkills)
        {
            cnt_kills[0] = (plrs[me].skills * 100) / wbs->maxkills;
            S_StartSound(0, sfx_barexp);
            sp_state++;
        }
    }
    else if (sp_state == 4)
    {
        cnt_items[0] += 2;

        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        if (cnt_items[0] >= (plrs[me].sitems * 100) / wbs->maxitems)
        {
            cnt_items[0] = (plrs[me].sitems * 100) / wbs->maxitems;
            S_StartSound(0, sfx_barexp);
            sp_state++;
        }
    }
    else if (sp_state == 6)
    {
        cnt_secret[0] += 2;

        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        if (cnt_secret[0] >= (plrs[me].ssecret * 100) / wbs->maxsecret)
        {
            cnt_secret[0] = (plrs[me].ssecret * 100) / wbs->maxsecret;
            S_StartSound(0, sfx_barexp);
            sp_state++;
        }
    }

    else if (sp_state == 8)
    {
        if (!(bcnt&3))
            S_StartSound(0, sfx_pistol);

        cnt_time += 3;

        if (cnt_time >= plrs[me].stime / TICRATE)
            cnt_time = plrs[me].stime / TICRATE;

        cnt_par += 3;

        if (cnt_par >= wbs->partime / TICRATE)
        {
            cnt_par = wbs->partime / TICRATE;

            if (cnt_time >= plrs[me].stime / TICRATE)
            {
                S_StartSound(0, sfx_barexp);
                sp_state++;
            }
        }
    }
    else if (sp_state == 10)
    {
        if (acceleratestage)
        {
            S_StartSound(0, sfx_sgcock);

            if (gamemode == commercial)
                WI_initNoState();
            else
                WI_initShowNextLoc();
        }
    }
    else if (sp_state & 1)
    {
        if (!--cnt_pause)
        {
            sp_state++;
            cnt_pause = TICRATE;
        }
    }

}

static void WI_drawStats(void)
{
    // line height
    int lh;

    lh = (3 * SHORT(num[0]->height))/2;

    WI_slamBackground();

    // draw animated background
    WI_drawAnimatedBack();

    WI_drawLF();

    if( FontBBaseLump )
    {
        // use FontB if any
        V_DrawTextB("Kills", SP_STATSX, SP_STATSY);
        V_DrawTextB("Items", SP_STATSX, SP_STATSY+lh);
        V_DrawTextB("Secrets", SP_STATSX, SP_STATSY+2*lh);
        V_DrawTextB("Time", SP_TIMEX, SP_TIMEY);
        if (wbs->epsd < 3 && gamemode!=heretic)
            V_DrawTextB("Par", BASEVIDWIDTH/2 + SP_TIMEX, SP_TIMEY);
    }
    else
    {
        V_DrawScaledPatch(SP_STATSX, SP_STATSY, FB, kills);
        V_DrawScaledPatch(SP_STATSX, SP_STATSY+lh, FB, items);
        V_DrawScaledPatch(SP_STATSX, SP_STATSY+2*lh, FB, sp_secret);
        V_DrawScaledPatch(SP_TIMEX, SP_TIMEY, FB, timePatch);
        if (wbs->epsd < 3 && gamemode!=heretic)
            V_DrawScaledPatch(BASEVIDWIDTH/2 + SP_TIMEX, SP_TIMEY, FB, par);
    }
    WI_drawPercent(BASEVIDWIDTH - SP_STATSX, SP_STATSY, cnt_kills[0]);
    WI_drawPercent(BASEVIDWIDTH - SP_STATSX, SP_STATSY+lh, cnt_items[0]);
    WI_drawPercent(BASEVIDWIDTH - SP_STATSX, SP_STATSY+2*lh, cnt_secret[0]);
    WI_drawTime(BASEVIDWIDTH/2 - SP_TIMEX, SP_TIMEY, cnt_time);

    if (wbs->epsd < 3 && gamemode!=heretic)
        WI_drawTime(BASEVIDWIDTH - SP_TIMEX, SP_TIMEY, cnt_par);

}

static void WI_checkForAccelerate(void)
{
    int   i;
    player_t  *player;

    // check for button presses to skip delays
    for (i=0, player = players ; i<MAXPLAYERS ; i++, player++)
    {
        if (playeringame[i])
        {
            if (player->cmd.buttons & BT_ATTACK)
            {
                if (!player->attackdown)
                    acceleratestage = 1;
                player->attackdown = true;
            }
            else
                player->attackdown = false;
            if (player->cmd.buttons & BT_USE)
            {
                if (!player->usedown)
                    acceleratestage = 1;
                player->usedown = true;
            }
            else
                player->usedown = false;
        }
    }
}



// Updates stuff each tick
void WI_Ticker(void)
{
    // counter for general background animation
    bcnt++;

    if (bcnt == 1)
    {
        // intermission music
        if ( gamemode == commercial )
          S_ChangeMusic(mus_dm2int, true);
        else
          S_ChangeMusic(mus_inter, true);
    }

    WI_checkForAccelerate();

    switch (state)
    {
      case StatCount:
        if (cv_deathmatch.value) WI_updateDeathmatchStats();
        else if (multiplayer) WI_updateNetgameStats();
        else WI_updateStats();
        break;

      case ShowNextLoc:
        WI_updateShowNextLoc();
        break;

      case NoState:
        WI_updateNoState();
        break;
    }

}

static void WI_loadData(void)
{
    int         i;
    int         j;
    anim_t*     a;
    char        name[9];

    // choose the background of the intermission
    if (*info_interpic)
         strcpy(bgname, info_interpic);
    else if (gamemode == commercial)
        strcpy(bgname, "INTERPIC");
    else if( gamemode == heretic )
        sprintf(bgname, "MAPE%d", wbs->epsd+1);
    else
        sprintf(bgname, "WIMAP%d", wbs->epsd);

    if ( gamemode == retail )
    {
        if (wbs->epsd == 3)
            strcpy(bgname,"INTERPIC");
    }
    
    
    if (rendermode == render_soft)
    {
        memset(screens[0],0,vid.width*vid.height*vid.bpp);

        // clear backbuffer from status bar stuff and borders
        memset(screens[1],0,vid.width*vid.height*vid.bpp);
  
        // background stored in backbuffer        
        V_DrawScaledPatch(0, 0, 1, W_CachePatchName(bgname, PU_CACHE));
    }

    // UNUSED unsigned char *pic = screens[1];
    // if (gamemode == commercial)
    // {
    // darken the background image
    // while (pic != screens[1] + SCREENHEIGHT*SCREENWIDTH)
    // {
    //   *pic = colormaps[256*25 + *pic];
    //   pic++;
    // }
    //}

    if (gamemode == commercial)
    {
        NUMCMAPS = 32;
        lnames = (patch_t **) Z_Malloc(sizeof(patch_t*) * NUMCMAPS,
                                       PU_STATIC, 0);
        for (i=0 ; i<NUMCMAPS ; i++)
        {
            sprintf(name, "CWILV%2.2d", i);
            lnames[i] = W_CachePatchName(name, PU_STATIC);
        }
    }
    else
    {
        lnames = (patch_t **) Z_Malloc(sizeof(patch_t*) * NUMMAPS,
                                       PU_STATIC, 0);
        for (i=0 ; i<NUMMAPS ; i++)
        {
            sprintf(name, "WILV%d%d", wbs->epsd, i);
            lnames[i] = W_CachePatchName(name, PU_STATIC);
        }

        // you are here
        yah[0] = W_CachePatchName(gamemode == heretic ? "IN_YAH" : "WIURH0", PU_STATIC);

        // you are here (alt.)
        yah[1] = W_CachePatchName("WIURH1", PU_STATIC);

        // splat
        splat = W_CachePatchName(gamemode == heretic ? "IN_X" : "WISPLAT", PU_STATIC);

        if (wbs->epsd < 3)
        {
            for (j=0;j<NUMANIMS[wbs->epsd];j++)
            {
                a = &anims[wbs->epsd][j];
                for (i=0;i<a->nanims;i++)
                {
                    // MONDO HACK!
                    if (wbs->epsd != 1 || j != 8)
                    {
                        // animations
                        sprintf(name, "WIA%d%.2d%.2d", wbs->epsd, j, i);
                        a->p[i] = W_CachePatchName(name, PU_STATIC);
                    }
                    else
                    {
                        // HACK ALERT!
                        a->p[i] = anims[1][4].p[i];
                    }
                }
            }
        }
    }

    // More hacks on minus sign.
    wiminus = W_CachePatchName(gamemode == heretic ? "FONTB13" : "WIMINUS", PU_STATIC);

    for (i=0;i<10;i++)
    {
         // numbers 0-9
        if( gamemode == heretic )
            sprintf(name, "FONTB%d", 16+i);
        else
            sprintf(name, "WINUM%d", i);
        num[i] = W_CachePatchName(name, PU_STATIC);
    }

    // percent sign
    percent = W_CachePatchName(gamemode == heretic ? "FONTB05" : "WIPCNT", PU_STATIC);

    if( gamemode != heretic )
    {
        // "finished"
        finished = W_CachePatchName("WIF", PU_STATIC);
        
        // "entering"
        entering = W_CachePatchName("WIENTER", PU_STATIC);
        
        // "kills"
        kills = W_CachePatchName("WIOSTK", PU_STATIC);
        
        // "scrt"
        secret = W_CachePatchName("WIOSTS", PU_STATIC);
        
        // "secret"
        sp_secret = W_CachePatchName("WISCRT2", PU_STATIC);
        
        // "items"
        items = W_CachePatchName("WIOSTI", PU_STATIC);
        
        // "frgs"
        frags = W_CachePatchName("WIFRGS", PU_STATIC);
        
        // "time"
        timePatch = W_CachePatchName("WITIME", PU_STATIC);
        
        // "sucks"
        sucks = W_CachePatchName("WISUCKS", PU_STATIC);
        
        // "par"
        par = W_CachePatchName("WIPAR", PU_STATIC);

        
        // "killers" (vertical)
        killers = W_CachePatchName("WIKILRS", PU_STATIC);
        
        // "victims" (horiz)
        victims = W_CachePatchName("WIVCTMS", PU_STATIC);
        
        // "total"
        total = W_CachePatchName("WIMSTT", PU_STATIC);
    }
    
    // ":"
    colon = W_CachePatchName(gamemode == heretic ? "FONTB26" : "WICOLON", PU_STATIC);

    // your face
    star = W_CachePatchName("STFST01", PU_STATIC);

    // dead face
    bstar = W_CachePatchName("STFDEAD0", PU_STATIC);


    //added:08-02-98: now uses a single STPB0 which is remapped to the
    //                player translation table. Whatever new colors we add
    //                since we'll have to define a translation table for
    //                it, we'll have the right colors here automatically.
    stpb = W_CachePatchName("STPB0", PU_STATIC);
}

static void WI_unloadData(void)
{
    int         i;
    int         j;

    //faB: never Z_ChangeTag() a pointer returned by W_CachePatchxxx()
    //     it doesn't work and is unecessary
    if (rendermode==render_soft)
    {
    Z_ChangeTag(wiminus, PU_CACHE);

    for (i=0 ; i<10 ; i++)
        Z_ChangeTag(num[i], PU_CACHE);

    if (gamemode == commercial)
    {
        for (i=0 ; i<NUMCMAPS ; i++)
            Z_ChangeTag(lnames[i], PU_CACHE);
    }
    else
    {
        Z_ChangeTag(yah[0], PU_CACHE);
        Z_ChangeTag(yah[1], PU_CACHE);

        Z_ChangeTag(splat, PU_CACHE);

        for (i=0 ; i<NUMMAPS ; i++)
            Z_ChangeTag(lnames[i], PU_CACHE);

        if (wbs->epsd < 3)
        {
            for (j=0;j<NUMANIMS[wbs->epsd];j++)
            {
                if (wbs->epsd != 1 || j != 8)
                    for (i=0;i<anims[wbs->epsd][j].nanims;i++)
                        Z_ChangeTag(anims[wbs->epsd][j].p[i], PU_CACHE);
            }
        }
    }
    }

    Z_Free(lnames);

    if (rendermode==render_soft)
    {
        Z_ChangeTag(percent, PU_CACHE);
        Z_ChangeTag(colon, PU_CACHE);

        if( gamemode != heretic )
        {

            Z_ChangeTag(finished, PU_CACHE);
            Z_ChangeTag(entering, PU_CACHE);
            Z_ChangeTag(kills, PU_CACHE);
            Z_ChangeTag(secret, PU_CACHE);
            Z_ChangeTag(sp_secret, PU_CACHE);
            Z_ChangeTag(items, PU_CACHE);
            Z_ChangeTag(frags, PU_CACHE);
            Z_ChangeTag(timePatch, PU_CACHE);
            Z_ChangeTag(sucks, PU_CACHE);
            Z_ChangeTag(par, PU_CACHE);
            
            Z_ChangeTag(victims, PU_CACHE);
            Z_ChangeTag(killers, PU_CACHE);
            Z_ChangeTag(total, PU_CACHE);
        }
    }
}

void WI_Drawer (void)
{
    switch (state)
    {
      case StatCount:
        if (cv_deathmatch.value)
        {
            if(cv_teamplay.value)
                WI_drawTeamsStats();
            else
                WI_drawDeathmatchStats();
        }
        else if (multiplayer)
            WI_drawNetgameStats();
        else
            WI_drawStats();
        break;

      case ShowNextLoc:
        WI_drawShowNextLoc();
        break;

      case NoState:
        WI_drawNoState();
        break;
    }
}


static void WI_initVariables(wbstartstruct_t* wbstartstruct)
{

    wbs = wbstartstruct;

#ifdef RANGECHECKING
    if (gamemode != commercial)
    {
      if ( gamemode == retail )
        RNGCHECK(wbs->epsd, 0, 3);
      else
        RNGCHECK(wbs->epsd, 0, 2);
    }
    else
    {
        RNGCHECK(wbs->last, 0, 8);
        RNGCHECK(wbs->next, 0, 8);
    }
    RNGCHECK(wbs->pnum, 0, MAXPLAYERS);
    RNGCHECK(wbs->pnum, 0, MAXPLAYERS);
#endif

    acceleratestage = 0;
    cnt = bcnt = 0;
    firstrefresh = 1;
    me = wbs->pnum;
    plrs = wbs->plyr;

    if (!wbs->maxkills)
        wbs->maxkills = 1;

    if (!wbs->maxitems)
        wbs->maxitems = 1;

    if (!wbs->maxsecret)
        wbs->maxsecret = 1;

    if ( gamemode != retail )
      if (wbs->epsd > 2)
        wbs->epsd -= 3;
}

void WI_Start(wbstartstruct_t* wbstartstruct)
{

    WI_initVariables(wbstartstruct);
    WI_loadData();

    if (cv_deathmatch.value)
        WI_initDeathmatchStats();
    else if (multiplayer)
        WI_initNetgameStats();
    else
        WI_initStats();
}
