// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: st_lib.c,v 1.7 2003/05/04 04:17:17 sburke Exp $
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
// $Log: st_lib.c,v $
// Revision 1.7  2003/05/04 04:17:17  sburke
// Add SHORT() conversion for big-endian machines.
//
// Revision 1.6  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.5  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.4  2000/10/04 16:19:24  hurdler
// Change all those "3dfx names" to more appropriate names
//
// Revision 1.3  2000/09/28 20:57:18  bpereira
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
//      The status bar widget code.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "st_lib.h"
#include "st_stuff.h"
#include "v_video.h"
#include "z_zone.h"

#include "i_video.h"    //rendermode
//#define DEBUG

//
// Hack display negative frags.
//  Loads and store the stminus lump.
//
patch_t*                sttminus;

void STlib_init(void)
{
    sttminus = (patch_t *) W_CachePatchName("STTMINUS", PU_STATIC);
}


// Initialize number widget
void STlib_initNum ( st_number_t*          n,
                     int                   x,
                     int                   y,
                     patch_t**             pl,
                     int*                  num,
                     boolean*              on,
                     int                   width )
{
    n->x        = x;
    n->y        = y;
    n->oldnum   = 0;
    n->width    = width;        // number of digits
    n->num      = num;
    n->on       = on;
    n->p        = pl;
}


//
// A fairly efficient way to draw a number
//  based on differences from the old number.
// Note: worth the trouble?
//
void STlib_drawNum ( st_number_t*  n,
                     boolean       refresh )
{

    int    numdigits = n->width;
    int    num = *n->num;

    int    w = SHORT(n->p[0]->width);
    int    h = SHORT(n->p[0]->height);
    int    x = n->x;

    int    neg;

    n->oldnum = *n->num;

    neg = num < 0;

    if (neg)
    {
        if (numdigits == 2 && num < -9)
            num = -9;
        else if (numdigits == 3 && num < -99)
            num = -99;

        num = -num;
    }

    // clear the area
    x = n->x - numdigits*w;

#ifdef DEBUG
       CONS_Printf("V_CopyRect1: %d %d %d %d %d %d %d %d val: %d\n",
              x, n->y, BG, w*numdigits, h, x, n->y, fgbuffer,num);
#endif
    // dont clear background in overlay
    if (!st_overlay &&
         rendermode==render_soft)   //faB:current hardware mode always refresh the statusbar
        V_CopyRect(x, n->y, BG, w*numdigits, h, x, n->y, fgbuffer);

    // if non-number, do not draw it
    if (num == 1994)
        return;

    x = n->x;

    // in the special case of 0, you draw 0
    if (!num)
        V_DrawScaledPatch(x - w, n->y, fgbuffer, n->p[ 0 ]);

    // draw the new number
    while (num && numdigits--)
    {
        x -= w;
        V_DrawScaledPatch(x, n->y, fgbuffer, n->p[ num % 10 ]);
        num /= 10;
    }

    // draw a minus sign if necessary
    if (neg)
        V_DrawScaledPatch(x - 8, n->y, fgbuffer, sttminus);
}


//
void STlib_updateNum ( st_number_t*          n,
                       boolean               refresh )
{
    if (*n->on) STlib_drawNum(n, refresh);
}


//
void STlib_initPercent ( st_percent_t*         p,
                         int                   x,
                         int                   y,
                         patch_t**             pl,
                         int*                  num,
                         boolean*              on,
                         patch_t*              percent )
{
    STlib_initNum(&p->n, x, y, pl, num, on, 3);
    p->p = percent;
}




void STlib_updatePercent ( st_percent_t*         per,
                           int                   refresh )
{
    if (refresh && *per->n.on)
        V_DrawScaledPatch(per->n.x, per->n.y, fgbuffer, per->p);

    STlib_updateNum(&per->n, refresh);
}



void STlib_initMultIcon ( st_multicon_t*        i,
                          int                   x,
                          int                   y,
                          patch_t**             il,
                          int*                  inum,
                          boolean*              on )
{
    i->x        = x;
    i->y        = y;
    i->oldinum  = -1;
    i->inum     = inum;
    i->on       = on;
    i->p        = il;
}



void STlib_updateMultIcon ( st_multicon_t*        mi,
                            boolean               refresh )
{
    int                 w;
    int                 h;
    int                 x;
    int                 y;

    if (*mi->on
        && (mi->oldinum != *mi->inum || refresh)
        && (*mi->inum!=-1))
    {
        if (mi->oldinum != -1)
        {
            x = mi->x - SHORT(mi->p[mi->oldinum]->leftoffset);
            y = mi->y - SHORT(mi->p[mi->oldinum]->topoffset);
            w = SHORT(mi->p[mi->oldinum]->width);
            h = SHORT(mi->p[mi->oldinum]->height);

#ifdef DEBUG
       CONS_Printf("V_CopyRect2: %d %d %d %d %d %d %d %d\n",
                            x, y, BG, w, h, x, y, fgbuffer);
#endif
            //faB:current hardware mode always refresh the statusbar
            if (!st_overlay && rendermode==render_soft)   
                V_CopyRect(x, y, BG, w, h, x, y, fgbuffer);
        }
        V_DrawScaledPatch(mi->x, mi->y, fgbuffer, mi->p[*mi->inum]);
        mi->oldinum = *mi->inum;
    }
}



void STlib_initBinIcon ( st_binicon_t*         b,
                         int                   x,
                         int                   y,
                         patch_t*              i,
                         boolean*              val,
                         boolean*              on )
{
    b->x        = x;
    b->y        = y;
    b->oldval   = 0;
    b->val      = val;
    b->on       = on;
    b->p        = i;
}



void STlib_updateBinIcon ( st_binicon_t*         bi,
                           boolean               refresh )
{
    int                 x;
    int                 y;
    int                 w;
    int                 h;

    if (*bi->on
        && (bi->oldval != *bi->val || refresh))
    {
        x = bi->x - SHORT(bi->p->leftoffset);
        y = bi->y - SHORT(bi->p->topoffset);
        w = SHORT(bi->p->width);
        h = SHORT(bi->p->height);

        if (*bi->val)
            V_DrawScaledPatch(bi->x, bi->y, fgbuffer, bi->p);
        else
        {
#ifdef DEBUG
       CONS_Printf("V_CopyRect3: %d %d %d %d %d %d %d %d\n",
                            x, y, BG, w, h, x, y, fgbuffer);
#endif
            if (!st_overlay &&
                rendermode==render_soft ) //faB:current hardware mode always refresh the statusbar
                V_CopyRect(x, y, BG, w, h, x, y, fgbuffer);
        }

        bi->oldval = *bi->val;
    }

}
