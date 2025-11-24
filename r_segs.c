// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: r_segs.c,v 1.32 2003/05/04 04:15:09 sburke Exp $
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
// $Log: r_segs.c,v $
// Revision 1.32  2003/05/04 04:15:09  sburke
// Wrap patch->width, patch->height references in SHORT for big-endian machines.
//
// Revision 1.31  2002/09/25 16:38:35  ssntails
// Alpha support for trans 3d floors in software
//
// Revision 1.30  2002/01/12 02:21:36  stroggonmeth
// Big commit
//
// Revision 1.29  2001/08/29 18:58:57  hurdler
// little "fix" (need to be fixed properly)
//
// Revision 1.28  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.27  2001/05/30 04:00:52  stroggonmeth
// Fixed crashing bugs in software with 3D floors.
//
// Revision 1.26  2001/05/27 13:42:48  bpereira
// no message
//
// Revision 1.25  2001/04/30 17:19:24  stroggonmeth
// HW fix and misc. changes
//
// Revision 1.24  2001/03/21 18:24:39  stroggonmeth
// Misc changes and fixes. Code cleanup
//
// Revision 1.23  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.22  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.21  2000/11/26 01:02:27  hurdler
// small bug fixes
//
// Revision 1.20  2000/11/25 18:41:21  stroggonmeth
// Crash fix
//
// Revision 1.19  2000/11/21 21:13:18  stroggonmeth
// Optimised 3D floors and fixed crashing bug in high resolutions.
//
// Revision 1.18  2000/11/14 16:23:16  hurdler
// Please fix this bug
//
// Revision 1.17  2000/11/09 17:56:20  stroggonmeth
// Hopefully fixed a few bugs and did a few optimizations.
//
// Revision 1.16  2000/11/03 03:27:17  stroggonmeth
// Again with the bug fixing...
//
// Revision 1.15  2000/11/02 19:49:36  bpereira
// no message
//
// Revision 1.14  2000/11/02 17:50:09  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.13  2000/09/28 20:57:17  bpereira
// no message
//
// Revision 1.12  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.11  2000/04/18 17:39:40  stroggonmeth
// Bug fixes and performance tuning.
//
// Revision 1.10  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.9  2000/04/15 22:12:58  stroggonmeth
// Minor bug fixes
//
// Revision 1.8  2000/04/13 23:47:47  stroggonmeth
// See logs
//
// Revision 1.7  2000/04/08 17:29:25  stroggonmeth
// no message
//
// Revision 1.6  2000/04/06 21:06:19  stroggonmeth
// Optimized extra_colormap code...
// Added #ifdefs for older water code.
//
// Revision 1.5  2000/04/05 15:47:47  stroggonmeth
// Added hack for Dehacked lumps. Transparent sprites are now affected by colormaps.
//
// Revision 1.4  2000/04/04 19:28:43  stroggonmeth
// Global colormaps working. Added a new linedef type 272.
//
// Revision 1.3  2000/04/04 00:32:48  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      All the clipping: columns, horizontal spans, sky columns.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "r_local.h"
#include "r_sky.h"

#include "r_splats.h"           //faB: testing

#include "w_wad.h"
#include "z_zone.h"
#include "d_netcmd.h"
#include "p_local.h" //Camera...
#include "console.h" //Con_clipviewtop

#ifdef OLDWATER
extern fixed_t  waterheight;
static boolean  markwater;
static fixed_t  waterz;
static fixed_t  waterfrac;
static fixed_t  waterstep;
#endif

// OPTIMIZE: closed two sided lines as single sided

// True if any of the segs textures might be visible.
static boolean         segtextured;
static boolean         markfloor; // False if the back side is the same plane.
static boolean         markceiling;

static boolean         maskedtexture;
static int             toptexture;
static int             bottomtexture;
static int             midtexture;
static int             numthicksides;
//static short*          thicksidecol;


angle_t         rw_normalangle;
// angle to line origin
int             rw_angle1;
fixed_t         rw_distance;

//
// regular wall
//
static int             rw_x;
static int             rw_stopx;
static angle_t         rw_centerangle;
static fixed_t         rw_offset;
static fixed_t         rw_offset2; // for splats

static fixed_t         rw_scale;
static fixed_t         rw_scalestep;
static fixed_t         rw_midtexturemid;
static fixed_t         rw_toptexturemid;
static fixed_t         rw_bottomtexturemid;

static int             worldtop;
static int             worldbottom;
static int             worldhigh;
static int             worldlow;

static fixed_t         pixhigh;
static fixed_t         pixlow;
static fixed_t         pixhighstep;
static fixed_t         pixlowstep;

static fixed_t         topfrac;
static fixed_t         topstep;

static fixed_t         bottomfrac;
static fixed_t         bottomstep;

lighttable_t** walllights;

short* maskedtexturecol;

extern double adjustedlightscale;


// ==========================================================================
// R_Splats Wall Splats Drawer
// ==========================================================================

#ifdef WALLSPLATS
#define BORIS_FIX
#ifdef BORIS_FIX
static short last_ceilingclip[MAXVIDWIDTH];
static short last_floorclip[MAXVIDWIDTH];
#endif

static void R_DrawSplatColumn(column_t* column)
{
    int         topscreen;
    int         bottomscreen;
    fixed_t     basetexturemid;

    basetexturemid = dc_texturemid;

    for (; column->topdelta != 0xff; )
    {
        // calculate unclipped screen coordinates
        //  for post
        topscreen = sprtopscreen + spryscale * column->topdelta;
        bottomscreen = topscreen + spryscale * column->length;

        dc_yl = (topscreen + FRACUNIT - 1) >> FRACBITS;
        dc_yh = (bottomscreen - 1) >> FRACBITS;


#ifndef BORIS_FIX
        if (dc_yh >= mfloorclip[dc_x])
            dc_yh = mfloorclip[dc_x] - 1;
        if (dc_yl < mceilingclip[dc_x])
            dc_yl = mceilingclip[dc_x] + 1;
#else
        if (dc_yh >= last_floorclip[dc_x])
            dc_yh = last_floorclip[dc_x] - 1;
        if (dc_yl <= last_ceilingclip[dc_x])
            dc_yl = last_ceilingclip[dc_x] + 1;
#endif
        if (dc_yl <= dc_yh)
        {
            dc_source = (byte*)column + 3;
            dc_texturemid = basetexturemid - (column->topdelta << FRACBITS);

            //CONS_Printf("l %d h %d %d\n",dc_yl,dc_yh, column->length);
            // Drawn by either R_DrawColumn
            //  or (SHADOW) R_DrawFuzzColumn.
            colfunc();
        }
        column = (column_t*)((byte*)column + column->length + 4);
    }

    dc_texturemid = basetexturemid;
}


static void R_DrawWallSplats()
{
    wallsplat_t* splat;
    seg_t* seg;
    angle_t     angle, angle1, angle2;
    int         x1, x2;
    unsigned    index;
    column_t* col;
    patch_t* patch;
    fixed_t     texturecolumn;

    splat = (wallsplat_t*)linedef->splats;

#ifdef PARANOIA
    if (!splat)
        I_Error("R_DrawWallSplats: splat is NULL");
#endif

    seg = ds_p->curline;

    // draw all splats from the line that touches the range of the seg
    for (; splat; splat = splat->next)
    {
        angle1 = R_PointToAngle(splat->v1.x, splat->v1.y);
        angle2 = R_PointToAngle(splat->v2.x, splat->v2.y);
        angle1 = (angle1 - viewangle + ANG90) >> ANGLETOFINESHIFT;
        angle2 = (angle2 - viewangle + ANG90) >> ANGLETOFINESHIFT;
#if 0
        if (angle1 > clipangle)
            angle1 = clipangle;
        if (angle2 > clipangle)
            angle2 = clipangle;
        if ((int)angle1 < -(int)clipangle)
            angle1 = -clipangle;
        if ((int)angle2 < -(int)clipangle)
            angle2 = -clipangle;
#else
        // BP: out of the viewangletox lut, TODO clip it to the screen
        if (angle1 > FINEANGLES / 2 || angle2 > FINEANGLES / 2)
            continue;
#endif
        x1 = viewangletox[angle1];
        x2 = viewangletox[angle2];

        if (x1 >= x2)
            continue;                         // does not cross a pixel

        // splat is not in this seg range
        if (x2 < ds_p->x1 || x1 > ds_p->x2)
            continue;

        if (x1 < ds_p->x1)
            x1 = ds_p->x1;
        if (x2 > ds_p->x2)
            x2 = ds_p->x2;
        if (x2 <= x1)
            continue;

        // calculate incremental stepping values for texture edges
        rw_scalestep = ds_p->scalestep;
        spryscale = ds_p->scale1 + (x1 - ds_p->x1) * rw_scalestep;
        mfloorclip = floorclip;
        mceilingclip = ceilingclip;

        patch = W_CachePatchNum(splat->patch, PU_CACHE);

        // clip splat range to seg range left
        /*if (x1 < ds_p->x1)
        {
            spryscale += (rw_scalestep * (ds_p->x1 - x1));
            x1 = ds_p->x1;
        }*/
        // clip splat range to seg range right


        // SoM: This is set allready. THIS IS WHAT WAS CAUSING PROBLEMS WITH
        // BOOM WATER!
        // frontsector = ds_p->curline->frontsector;
        dc_texturemid = splat->top + (SHORT(patch->height) << (FRACBITS - 1)) - viewz;
        if (splat->yoffset)
            dc_texturemid += *splat->yoffset;

        sprtopscreen = centeryfrac - FixedMul(dc_texturemid, spryscale);

        // set drawing mode
        switch (splat->flags & SPLATDRAWMODE_MASK)
        {
        case SPLATDRAWMODE_OPAQUE:
            colfunc = basecolfunc;
            break;
        case SPLATDRAWMODE_TRANS:
            if (cv_translucency.value == 0)
                colfunc = basecolfunc;
            else
            {
                dc_transmap = ((tr_transmed - 1) << FF_TRANSSHIFT) + transtables;
                colfunc = fuzzcolfunc;
            }

            break;
        case SPLATDRAWMODE_SHADE:
            colfunc = shadecolfunc;
            break;
        }
        if (fixedcolormap)
            dc_colormap = fixedcolormap;

        dc_texheight = 0;

        // draw the columns
        for (dc_x = x1; dc_x <= x2; dc_x++, spryscale += rw_scalestep)
        {
            if (!fixedcolormap)
            {
                fixed_t lightScale = adjustedlightscale * FRACUNIT;
                lightScale = FixedMul(spryscale, lightScale);
                index = lightScale >> LIGHTSCALESHIFT;
                if (index >= MAXLIGHTSCALE)
                    index = MAXLIGHTSCALE - 1;
                dc_colormap = walllights[index];
            }

            if (frontsector->extra_colormap && !fixedcolormap)
                dc_colormap = frontsector->extra_colormap->colormap + (dc_colormap - colormaps);

            sprtopscreen = centeryfrac - FixedMul(dc_texturemid, spryscale);
            dc_iscale = 0xffffffffu / (unsigned)spryscale;

            // find column of patch, from perspective
            angle = (rw_centerangle + xtoviewangle[dc_x]) >> ANGLETOFINESHIFT;
            texturecolumn = rw_offset2 - splat->offset - FixedMul(finetangent[angle], rw_distance);

            //texturecolumn &= 7;
            //DEBUG

            // FIXME !
//            CONS_Printf ("%.2f width %d, %d[x], %.1f[off]-%.1f[soff]-tg(%d)=%.1f*%.1f[d] = %.1f\n", 
//                         FIXED_TO_FLOAT(texturecolumn), SHORT(patch->width),
//                         dc_x,FIXED_TO_FLOAT(rw_offset2),FIXED_TO_FLOAT(splat->offset),angle,FIXED_TO_FLOAT(finetangent[angle]),FIXED_TO_FLOAT(rw_distance),FIXED_TO_FLOAT(FixedMul(finetangent[angle],rw_distance)));
            texturecolumn >>= FRACBITS;
            if (texturecolumn < 0 || texturecolumn >= SHORT(patch->width))
                continue;

            // draw the texture
            col = (column_t*)((byte*)patch + LONG(patch->columnofs[texturecolumn]));
            R_DrawSplatColumn(col);

        }

    }// next splat

    colfunc = basecolfunc;
}

#endif //WALLSPLATS



// ==========================================================================
// R_RenderMaskedSegRange
// ==========================================================================

// If we have a multi-patch texture on a 2sided wall (rare) then we draw
//  it using R_DrawColumn, else we draw it using R_DrawMaskedColumn, this
//  way we don't have to store extra post_t info with each column for
//  multi-patch textures. They are not normally needed as multi-patch
//  textures don't have holes in it. At least not for now.
static int  column2s_length;     // column->length : for multi-patch on 2sided wall = texture->height

void R_Render2sidedMultiPatchColumn(column_t* column)
{
    int         topscreen;
    int         bottomscreen;

    topscreen = sprtopscreen; // + spryscale*column->topdelta;  topdelta is 0 for the wall
    bottomscreen = topscreen + spryscale * column2s_length;

    dc_yl = (sprtopscreen + FRACUNIT - 1) >> FRACBITS;
    dc_yh = (bottomscreen - 1) >> FRACBITS;

    if (windowtop != MAXINT && windowbottom != MAXINT)
    {
        dc_yl = ((windowtop + FRACUNIT) >> FRACBITS);
        dc_yh = (windowbottom - 1) >> FRACBITS;
    }

    {
        if (dc_yh >= mfloorclip[dc_x])
            dc_yh = mfloorclip[dc_x] - 1;
        if (dc_yl <= mceilingclip[dc_x])
            dc_yl = mceilingclip[dc_x] + 1;
    }

    if (dc_yl >= vid.height || dc_yh < 0)
        return;

    if (dc_yl <= dc_yh)
    {
        dc_source = (byte*)column + 3;
        colfunc();
    }
}


void R_RenderMaskedSegRange(drawseg_t* ds,
    int        x1,
    int        x2)
{
    unsigned        index;
    column_t* col;
    int             lightnum;
    int             texnum;
    int             i;
    fixed_t         height;
    fixed_t         realbot;
    lightlist_t* light;
    r_lightlist_t* rlight;

    void (*colfunc_2s) (column_t*);

    line_t* ldef;   //faB

    // Calculate light table.
    // Use different light tables
    //   for horizontal / vertical / diagonal. Diagonal?
    // OPTIMIZE: get rid of LIGHTSEGSHIFT globally
    curline = ds->curline;
    frontsector = curline->frontsector;
    backsector = curline->backsector;
    texnum = texturetranslation[curline->sidedef->midtexture];
    windowbottom = windowtop = sprbotscreen = MAXINT;

    //faB: hack translucent linedef types (201-205 for transtables 1-5)
    //SoM: 201-205 are taken... So I'm switching to 284 - 288
    ldef = curline->linedef;
    if (ldef->special >= 284 && ldef->special <= 288)
    {
        dc_transmap = ((ldef->special - 284) << FF_TRANSSHIFT) + transtables;
        colfunc = fuzzcolfunc;
    }
    else
        if (ldef->special == 260)
        {
            dc_transmap = transtables; // get first transtable 50/50
            colfunc = fuzzcolfunc;
        }
        else
            if (ldef->special == 283)
            {
                colfunc = R_DrawFogColumn_8;
                windowtop = frontsector->ceilingheight;
                windowbottom = frontsector->floorheight;
            }
            else
                colfunc = basecolfunc;

    rw_scalestep = ds->scalestep;
    spryscale = ds->scale1 + (x1 - ds->x1) * rw_scalestep;

    //faB: handle case where multipatch texture is drawn on a 2sided wall, multi-patch textures
    //     are not stored per-column with post info anymore in Doom Legacy
    if (textures[texnum]->patchcount == 1)
        colfunc_2s = R_DrawMaskedColumn;                    //render the usual 2sided single-patch packed texture
    else {
        colfunc_2s = R_Render2sidedMultiPatchColumn;        //render multipatch with no holes (no post_t info)
        column2s_length = textures[texnum]->height;
    }


    // Setup lighting based on the presence/lack-of 3D floors.
    dc_numlights = 0;
    if (frontsector->numlights)
    {
        int lightnum;

        dc_numlights = frontsector->numlights;
        if (dc_numlights >= dc_maxlights)
        {
            dc_maxlights = dc_numlights;
            dc_lightlist = realloc(dc_lightlist, sizeof(r_lightlist_t) * dc_maxlights);
        }

        for (i = 0; i < dc_numlights; i++)
        {
            light = &frontsector->lightlist[i];
            rlight = &dc_lightlist[i];
            rlight->height = (centeryfrac)-FixedMul((light->height - viewz), spryscale);
            rlight->heightstep = -FixedMul(rw_scalestep, (light->height - viewz));
            rlight->lightlevel = *light->lightlevel;
            rlight->extra_colormap = light->extra_colormap;
            rlight->flags = light->flags;

            if (rlight->flags & FF_FOG || (rlight->extra_colormap && rlight->extra_colormap->fog))
                lightnum = (rlight->lightlevel >> LIGHTSEGSHIFT);
            else if (colfunc == fuzzcolfunc)
                lightnum = LIGHTLEVELS - 1;
            else
                lightnum = (rlight->lightlevel >> LIGHTSEGSHIFT) + extralight;

            if (rlight->extra_colormap && rlight->extra_colormap->fog);
            else if (curline->v1->y == curline->v2->y)
                lightnum--;
            else if (curline->v1->x == curline->v2->x)
                lightnum++;

            rlight->lightnum = lightnum;
        }
    }
    else
    {
        if (colfunc == fuzzcolfunc)
        {
            if (frontsector->extra_colormap && frontsector->extra_colormap->fog)
                lightnum = (frontsector->lightlevel >> LIGHTSEGSHIFT);
            else
                lightnum = LIGHTLEVELS - 1;
        }
        else if (colfunc == R_DrawFogColumn_8)
            lightnum = (frontsector->lightlevel >> LIGHTSEGSHIFT);
        else
            lightnum = (frontsector->lightlevel >> LIGHTSEGSHIFT) + extralight;

        if (colfunc == R_DrawFogColumn_8 || (frontsector->extra_colormap && frontsector->extra_colormap->fog));
        else if (curline->v1->y == curline->v2->y)
            lightnum--;
        else if (curline->v1->x == curline->v2->x)
            lightnum++;

        if (lightnum < 0)
            walllights = scalelight[0];
        else if (lightnum >= LIGHTLEVELS)
            walllights = scalelight[LIGHTLEVELS - 1];
        else
            walllights = scalelight[lightnum];
    }

    maskedtexturecol = ds->maskedtexturecol;

    mfloorclip = ds->sprbottomclip;
    mceilingclip = ds->sprtopclip;

    if (curline->linedef->flags & ML_DONTPEGBOTTOM)
    {
        dc_texturemid = frontsector->floorheight > backsector->floorheight
            ? frontsector->floorheight : backsector->floorheight;
        dc_texturemid = dc_texturemid + textureheight[texnum] - viewz;
    }
    else
    {
        dc_texturemid = frontsector->ceilingheight < backsector->ceilingheight
            ? frontsector->ceilingheight : backsector->ceilingheight;
        dc_texturemid = dc_texturemid - viewz;
    }
    dc_texturemid += curline->sidedef->rowoffset;

    dc_texheight = textureheight[texnum] >> FRACBITS;

    if (fixedcolormap)
        dc_colormap = fixedcolormap;

    // draw the columns
    for (dc_x = x1; dc_x <= x2; dc_x++)
    {
        // calculate lighting
        if (maskedtexturecol[dc_x] != MAXSHORT)
        {
            if (dc_numlights)
            {
                lighttable_t** xwalllights;

                sprbotscreen = MAXINT;
                sprtopscreen = windowtop = (centeryfrac - FixedMul(dc_texturemid, spryscale));
                realbot = windowbottom = FixedMul(textureheight[texnum], spryscale) + sprtopscreen;
                dc_iscale = 0xffffffffu / (unsigned)spryscale;

                // draw the texture
                col = (column_t*)((byte*)R_GetColumn(texnum, maskedtexturecol[dc_x]) - 3);

                for (i = 0; i < dc_numlights; i++)
                {
                    rlight = &dc_lightlist[i];

                    if ((rlight->flags & FF_NOSHADE))
                        continue;

                    if (rlight->lightnum < 0)
                        xwalllights = scalelight[0];
                    else if (rlight->lightnum >= LIGHTLEVELS)
                        xwalllights = scalelight[LIGHTLEVELS - 1];
                    else
                        xwalllights = scalelight[rlight->lightnum];

                    fixed_t lightScale = adjustedlightscale * FRACUNIT;
                    lightScale = FixedMul(spryscale, lightScale);
                    index = lightScale >> LIGHTSCALESHIFT;

                    if (index >= MAXLIGHTSCALE)
                        index = MAXLIGHTSCALE - 1;

                    if (rlight->extra_colormap && !fixedcolormap)
                        rlight->rcolormap = rlight->extra_colormap->colormap + (xwalllights[index] - colormaps);
                    else if (!fixedcolormap)
                        rlight->rcolormap = xwalllights[index];
                    else
                        rlight->rcolormap = fixedcolormap;

                    rlight->height += rlight->heightstep;

                    height = rlight->height;
                    if (height <= windowtop)
                    {
                        dc_colormap = rlight->rcolormap;
                        continue;
                    }

                    windowbottom = height;
                    if (windowbottom >= realbot)
                    {
                        windowbottom = realbot;
                        colfunc_2s(col);
                        for (i++; i < dc_numlights; i++)
                        {
                            rlight = &dc_lightlist[i];
                            rlight->height += rlight->heightstep;
                        }

                        continue;
                    }
                    colfunc_2s(col);
                    windowtop = windowbottom + 1;
                    dc_colormap = rlight->rcolormap;
                }
                windowbottom = realbot;
                if (windowtop < windowbottom)
                    colfunc_2s(col);

                spryscale += rw_scalestep;
                continue;
            }

            // calculate lighting
            if (!fixedcolormap)
            {
                fixed_t lightScale = adjustedlightscale * FRACUNIT;
                lightScale = FixedMul(spryscale, lightScale);
                index = lightScale >> LIGHTSCALESHIFT;

                if (index >= MAXLIGHTSCALE)
                    index = MAXLIGHTSCALE - 1;

                dc_colormap = walllights[index];
            }

            if (frontsector->extra_colormap && !fixedcolormap)
                dc_colormap = frontsector->extra_colormap->colormap + (dc_colormap - colormaps);
            sprtopscreen = centeryfrac - FixedMul(dc_texturemid, spryscale);
            dc_iscale = 0xffffffffu / (unsigned)spryscale;

            // draw the texture
            col = (column_t*)(
                (byte*)R_GetColumn(texnum, maskedtexturecol[dc_x]) - 3);

            colfunc_2s(col);
        }
        spryscale += rw_scalestep;
    }
    colfunc = basecolfunc;
}





//
// R_RenderThickSideRange
// Renders all the thick sides in the given range.
void R_RenderThickSideRange(drawseg_t* ds,
    int        x1,
    int        x2,
    ffloor_t* ffloor)
{
    unsigned        index;
    column_t* col;
    int             lightnum;
    int             texnum;
    sector_t        tempsec;
    int             templight;
    int             i, p;
    fixed_t         bottombounds = viewheight << FRACBITS;
    fixed_t         topbounds = (con_clipviewtop - 1) << FRACBITS;
    fixed_t         offsetvalue = 0;
    lightlist_t* light;
    r_lightlist_t* rlight;
    fixed_t         lheight;

    void (*colfunc_2s) (column_t*);

    // Calculate light table.
    // Use different light tables
    //   for horizontal / vertical / diagonal. Diagonal?
    // OPTIMIZE: get rid of LIGHTSEGSHIFT globally

    curline = ds->curline;
    backsector = ffloor->target;
    frontsector = curline->frontsector == ffloor->target ? curline->backsector : curline->frontsector;
    texnum = texturetranslation[sides[ffloor->master->sidenum[0]].midtexture];

    colfunc = basecolfunc;

    if (ffloor->flags & FF_TRANSLUCENT)
    {
        // Hacked up support for alpha value in software mode SSNTails 09-24-2002
        if (ffloor->alpha < 64)
            dc_transmap = ((3) << FF_TRANSSHIFT) - 0x10000 + transtables;
        else if (ffloor->alpha < 128 && ffloor->alpha > 63)
            dc_transmap = ((2) << FF_TRANSSHIFT) - 0x10000 + transtables;
        else
            dc_transmap = ((1) << FF_TRANSSHIFT) - 0x10000 + transtables;

        colfunc = fuzzcolfunc;
    }
    else if (ffloor->flags & FF_FOG)
        colfunc = R_DrawFogColumn_8;

    //SoM: Moved these up here so they are available for my lightlist calculations
    rw_scalestep = ds->scalestep;
    spryscale = ds->scale1 + (x1 - ds->x1) * rw_scalestep;

    dc_numlights = 0;
    if (frontsector->numlights)
    {
        dc_numlights = frontsector->numlights;
        if (dc_numlights > dc_maxlights)
        {
            dc_maxlights = dc_numlights;
            dc_lightlist = realloc(dc_lightlist, sizeof(r_lightlist_t) * dc_maxlights);
        }

        for (i = p = 0; i < dc_numlights; i++)
        {
            light = &frontsector->lightlist[i];
            rlight = &dc_lightlist[p];

            if (light->height < *ffloor->bottomheight)
                continue;

            if (light->height > *ffloor->topheight)
                if (i + 1 < dc_numlights && frontsector->lightlist[i + 1].height > *ffloor->topheight)
                    continue;

            lheight = light->height;// > *ffloor->topheight ? *ffloor->topheight + FRACUNIT : light->height;
            rlight->heightstep = -FixedMul(rw_scalestep, (lheight - viewz));
            rlight->height = (centeryfrac)-FixedMul((lheight - viewz), spryscale) - rlight->heightstep;
            rlight->flags = light->flags;
            if (light->flags & FF_CUTLEVEL)
            {
                lheight = *light->caster->bottomheight;// > *ffloor->topheight ? *ffloor->topheight + FRACUNIT : *light->caster->bottomheight;
                rlight->botheightstep = -FixedMul(rw_scalestep, (lheight - viewz));
                rlight->botheight = (centeryfrac)-FixedMul((lheight - viewz), spryscale) - rlight->botheightstep;
            }

            rlight->lightlevel = *light->lightlevel;
            rlight->extra_colormap = light->extra_colormap;

            // Check if the current light effects the colormap/lightlevel
            if ((dc_lightlist[i].flags & FF_NOSHADE))
                continue;

            if (ffloor->flags & FF_FOG)
                rlight->lightnum = (ffloor->master->frontsector->lightlevel >> LIGHTSEGSHIFT);
            else if (rlight->flags & FF_FOG || (rlight->extra_colormap && rlight->extra_colormap->fog))
                rlight->lightnum = (rlight->lightlevel >> LIGHTSEGSHIFT);
            else
                rlight->lightnum = (rlight->lightlevel >> LIGHTSEGSHIFT) + extralight;

            if (ffloor->flags & FF_FOG || rlight->flags & FF_FOG || (rlight->extra_colormap && rlight->extra_colormap->fog));
            else if (curline->v1->y == curline->v2->y)
                rlight->lightnum--;
            else if (curline->v1->x == curline->v2->x)
                rlight->lightnum++;

            p++;
        }
        dc_numlights = p;
    }
    else
    {
        //SoM: Get correct light level!
        if ((frontsector->extra_colormap && frontsector->extra_colormap->fog))
            lightnum = (frontsector->lightlevel >> LIGHTSEGSHIFT);
        else if (ffloor->flags & FF_FOG)
            lightnum = (ffloor->master->frontsector->lightlevel >> LIGHTSEGSHIFT);
        else if (colfunc == fuzzcolfunc)
            lightnum = LIGHTLEVELS - 1;
        else
            lightnum = (R_FakeFlat(frontsector, &tempsec, &templight, &templight, false)
                ->lightlevel >> LIGHTSEGSHIFT) + extralight;

        if (ffloor->flags & FF_FOG || (frontsector->extra_colormap && frontsector->extra_colormap->fog));
        else if (curline->v1->y == curline->v2->y)
            lightnum--;
        else if (curline->v1->x == curline->v2->x)
            lightnum++;

        if (lightnum < 0)
            walllights = scalelight[0];
        else if (lightnum >= LIGHTLEVELS)
            walllights = scalelight[LIGHTLEVELS - 1];
        else
            walllights = scalelight[lightnum];
    }

    maskedtexturecol = ds->thicksidecol;

    mfloorclip = ds->sprbottomclip;
    mceilingclip = ds->sprtopclip;
    dc_texheight = textureheight[texnum] >> FRACBITS;

    dc_texturemid = *ffloor->topheight - viewz;

    offsetvalue = sides[ffloor->master->sidenum[0]].rowoffset;
    if (curline->linedef->flags & ML_DONTPEGBOTTOM)
        offsetvalue -= *ffloor->topheight - *ffloor->bottomheight;

    dc_texturemid += offsetvalue;

    if (fixedcolormap)
        dc_colormap = fixedcolormap;

    //faB: handle case where multipatch texture is drawn on a 2sided wall, multi-patch textures
    //     are not stored per-column with post info anymore in Doom Legacy
    if (textures[texnum]->patchcount == 1)
        colfunc_2s = R_DrawMaskedColumn;                    //render the usual 2sided single-patch packed texture
    else {
        colfunc_2s = R_Render2sidedMultiPatchColumn;        //render multipatch with no holes (no post_t info)
        column2s_length = textures[texnum]->height;
    }

    // draw the columns
    for (dc_x = x1; dc_x <= x2; dc_x++)
    {
        if (maskedtexturecol[dc_x] != MAXSHORT)
        {
            // SoM: New code does not rely on r_drawColumnShadowed_8 which
            // will (hopefully) put less strain on the stack.
            if (dc_numlights)
            {
                lighttable_t** xwalllights;
                fixed_t        height;
                fixed_t        bheight = 0;
                int            solid = 0;
                int            lighteffect = 0;

                sprtopscreen = windowtop = (centeryfrac - FixedMul((dc_texturemid - offsetvalue), spryscale));
                sprbotscreen = windowbottom = FixedMul(*ffloor->topheight - *ffloor->bottomheight, spryscale) + sprtopscreen;

                // SoM: If column is out of range, why bother with it??
                if (windowbottom < topbounds || windowtop > bottombounds)
                {
                    for (i = 0; i < dc_numlights; i++)
                    {
                        rlight = &dc_lightlist[i];
                        rlight->height += rlight->heightstep;
                        if (rlight->flags & FF_CUTLEVEL)
                            rlight->botheight += rlight->botheightstep;
                    }
                    spryscale += rw_scalestep;
                    continue;
                }

                dc_iscale = 0xffffffffu / (unsigned)spryscale;

                // draw the texture
                col = (column_t*)((byte*)R_GetColumn(texnum, maskedtexturecol[dc_x]) - 3);

                for (i = 0; i < dc_numlights; i++)
                {
                    // Check if the current light effects the colormap/lightlevel
                    rlight = &dc_lightlist[i];
                    lighteffect = !(dc_lightlist[i].flags & FF_NOSHADE);
                    if (lighteffect)
                    {
                        lightnum = rlight->lightnum;
                        if (lightnum < 0)
                            xwalllights = scalelight[0];
                        else if (lightnum >= LIGHTLEVELS)
                            xwalllights = scalelight[LIGHTLEVELS - 1];
                        else
                            xwalllights = scalelight[lightnum];

                        fixed_t lightScale = adjustedlightscale * FRACUNIT;
                        lightScale = FixedMul(spryscale, lightScale);
                        index = lightScale >> LIGHTSCALESHIFT;

                        if (index >= MAXLIGHTSCALE)
                            index = MAXLIGHTSCALE - 1;

                        if (ffloor->flags & FF_FOG)
                        {
                            if (ffloor->master->frontsector->extra_colormap && !fixedcolormap)
                                rlight->rcolormap = ffloor->master->frontsector->extra_colormap->colormap + (xwalllights[index] - colormaps);
                            else if (!fixedcolormap)
                                rlight->rcolormap = xwalllights[index];
                        }
                        else
                        {
                            if (rlight->extra_colormap && !fixedcolormap)
                                rlight->rcolormap = rlight->extra_colormap->colormap + (xwalllights[index] - colormaps);
                            else if (!fixedcolormap)
                                rlight->rcolormap = xwalllights[index];
                        }

                        if (fixedcolormap)
                            rlight->rcolormap = fixedcolormap;
                    }

                    // Check if the current light can cut the current 3D floor.
                    if (rlight->flags & FF_CUTSOLIDS && !(ffloor->flags & FF_EXTRA))
                        solid = 1;
                    else if (rlight->flags & FF_CUTEXTRA && ffloor->flags & FF_EXTRA)
                    {
                        if (rlight->flags & FF_EXTRA)
                        {
                            // The light is from an extra 3D floor... Check the flags so
                            // there are no undesired cuts.
                            if ((rlight->flags & (FF_TRANSLUCENT | FF_FOG)) == (ffloor->flags & (FF_TRANSLUCENT | FF_FOG)))
                                solid = 1;
                        }
                        else
                            solid = 1;
                    }
                    else
                        solid = 0;

                    rlight->height += rlight->heightstep;
                    height = rlight->height;

                    if (solid)
                    {
                        rlight->botheight += rlight->botheightstep;
                        bheight = rlight->botheight - (FRACUNIT >> 1);
                    }

                    if (height <= windowtop)
                    {
                        if (lighteffect)
                            dc_colormap = rlight->rcolormap;
                        if (solid && windowtop < bheight)
                            windowtop = bheight;
                        continue;
                    }

                    windowbottom = height;
                    if (windowbottom >= sprbotscreen)
                    {
                        windowbottom = sprbotscreen;
                        colfunc_2s(col);
                        for (i++; i < dc_numlights; i++)
                        {
                            rlight = &dc_lightlist[i];
                            rlight->height += rlight->heightstep;
                            if (rlight->flags & FF_CUTLEVEL)
                                rlight->botheight += rlight->botheightstep;
                        }
                        continue;
                    }
                    colfunc_2s(col);
                    if (solid)
                        windowtop = bheight;
                    else
                        windowtop = windowbottom + 1;
                    if (lighteffect)
                        dc_colormap = rlight->rcolormap;
                }
                windowbottom = sprbotscreen;
                if (windowtop < windowbottom)
                    colfunc_2s(col);

                spryscale += rw_scalestep;
                continue;
            }

            // calculate lighting
            if (!fixedcolormap)
            {
                fixed_t lightScale = adjustedlightscale * FRACUNIT;
                lightScale = FixedMul(spryscale, lightScale);
                index = lightScale >> LIGHTSCALESHIFT;

                if (index >= MAXLIGHTSCALE)
                    index = MAXLIGHTSCALE - 1;

                dc_colormap = walllights[index];
                if (frontsector->extra_colormap)
                    dc_colormap = frontsector->extra_colormap->colormap + (dc_colormap - colormaps);
                if (ffloor->flags & FF_FOG && ffloor->master->frontsector->extra_colormap)
                    dc_colormap = ffloor->master->frontsector->extra_colormap->colormap + (dc_colormap - colormaps);
            }

            sprtopscreen = windowtop = (centeryfrac - FixedMul((dc_texturemid - offsetvalue), spryscale));
            sprbotscreen = windowbottom = FixedMul(*ffloor->topheight - *ffloor->bottomheight, spryscale) + sprtopscreen;
            dc_iscale = 0xffffffffu / (unsigned)spryscale;

            // draw the texture
            col = (column_t*)((byte*)R_GetColumn(texnum, maskedtexturecol[dc_x]) - 3);

            colfunc_2s(col);
            spryscale += rw_scalestep;
        }
    }
    colfunc = basecolfunc;
}



//
// R_RenderSegLoop
// Draws zero, one, or two textures (and possibly a masked
//  texture) for walls.
// Can draw or mark the starting pixel of floor and ceiling
//  textures.
// CALLED: CORE LOOPING ROUTINE.
//
#define HEIGHTBITS              12
#define HEIGHTUNIT              (1<<HEIGHTBITS)


//profile stuff ---------------------------------------------------------
//#define TIMING
#ifdef TIMING
#include "p5prof.h"
long long mycount;
long long mytotal = 0;
unsigned long   nombre = 100000;
//static   char runtest[10][80];
#endif
//profile stuff ---------------------------------------------------------


void R_RenderSegLoop(void)
{
    angle_t             angle;
    unsigned            index;
    int                 yl;
    int                 yh;

#ifdef OLDWATER
    int                 yw;     //added:18-02-98:WATER!
#endif

    int                 mid;
    fixed_t             texturecolumn;
    int                 top;
    int                 bottom;
    int                 i;

    texturecolumn = 0;                                // shut up compiler warning

    for (; rw_x < rw_stopx; rw_x++)
    {
        // mark floor / ceiling areas
        yl = (topfrac + HEIGHTUNIT - 1) >> HEIGHTBITS;

        // no space above wall?
        if (yl < ceilingclip[rw_x] + 1)
            yl = ceilingclip[rw_x] + 1;

        if (markceiling)
        {
            top = ceilingclip[rw_x] + 1;
            bottom = yl - 1;

            if (bottom >= floorclip[rw_x])
                bottom = floorclip[rw_x] - 1;

            if (top <= bottom)
            {
                ceilingplane->top[rw_x] = top;
                ceilingplane->bottom[rw_x] = bottom;
            }
        }


        yh = bottomfrac >> HEIGHTBITS;

        if (yh >= floorclip[rw_x])
            yh = floorclip[rw_x] - 1;

        if (markfloor)
        {
            top = yh + 1;
            bottom = floorclip[rw_x] - 1;
            if (top <= ceilingclip[rw_x])
                top = ceilingclip[rw_x] + 1;
            if (top <= bottom && floorplane)
            {
                floorplane->top[rw_x] = top;
                floorplane->bottom[rw_x] = bottom;
            }
        }

#ifdef OLDWATER
        if (markwater)
        {
            //added:18-02-98:WATER!
            yw = waterfrac >> HEIGHTBITS;

            // the markwater stuff...
            if (waterplane->height < viewz)
            {
                top = yw;
                bottom = waterclip[rw_x] - 1;

                if (top <= ceilingclip[rw_x])
                    top = ceilingclip[rw_x] + 1;
            }
            else  //view from under
            {
                top = waterclip[rw_x] + 1;
                bottom = yw;

                if (bottom >= floorclip[rw_x])
                    bottom = floorclip[rw_x] - 1;
            }
            if (top <= bottom)
            {
                waterplane->top[rw_x] = top;
                waterplane->bottom[rw_x] = bottom;
            }

            // do it only if markwater else not needed!
            waterfrac += waterstep;   //added:18-02-98:WATER!
            //dc_wcolormap = colormaps+(32<<8);
        }
#endif

        if (numffloors)
        {
            firstseg->frontscale[rw_x] = frontscale[rw_x];
            for (i = 0; i < numffloors; i++)
            {
                if (ffloor[i].height < viewz)
                {
                    int top_w = (ffloor[i].f_frac >> HEIGHTBITS) + 1;
                    int bottom_w = ffloor[i].f_clip[rw_x];

                    if (top_w < ceilingclip[rw_x] + 1)
                        top_w = ceilingclip[rw_x] + 1;

                    if (bottom_w > floorclip[rw_x] - 1)
                        bottom_w = floorclip[rw_x] - 1;

                    if (top_w <= bottom_w)
                    {
                        ffloor[i].plane->top[rw_x] = top_w;
                        ffloor[i].plane->bottom[rw_x] = bottom_w;
                    }
                }
                else if (ffloor[i].height > viewz)
                {
                    int top_w = ffloor[i].c_clip[rw_x] + 1;
                    int bottom_w = (ffloor[i].f_frac >> HEIGHTBITS);

                    if (top_w < ceilingclip[rw_x] + 1)
                        top_w = ceilingclip[rw_x] + 1;

                    if (bottom_w > floorclip[rw_x] - 1)
                        bottom_w = floorclip[rw_x] - 1;

                    if (top_w <= bottom_w)
                    {
                        ffloor[i].plane->top[rw_x] = top_w;
                        ffloor[i].plane->bottom[rw_x] = bottom_w;
                    }
                }
            }
        }

        //SoM: Calculate offsets for Thick fake floors.
        // calculate texture offset
        angle = (rw_centerangle + xtoviewangle[rw_x]) >> ANGLETOFINESHIFT;
        angle &= 0xFFF; // Prevent finetangent overflow.
        texturecolumn = rw_offset - FixedMul(finetangent[angle], rw_distance);
        texturecolumn >>= FRACBITS;

        // texturecolumn and lighting are independent of wall tiers
        if (segtextured)
        {
            // calculate lighting
            fixed_t lightScale = adjustedlightscale * FRACUNIT;
            lightScale = FixedMul(rw_scale, lightScale);
            index = lightScale >> LIGHTSCALESHIFT;

            if (index >= MAXLIGHTSCALE)
                index = MAXLIGHTSCALE - 1;

            dc_colormap = walllights[index];
            dc_x = rw_x;
            dc_iscale = 0xffffffffu / (unsigned)rw_scale;

            if (frontsector->extra_colormap && !fixedcolormap)
                dc_colormap = frontsector->extra_colormap->colormap + (dc_colormap - colormaps);
        }

        if (dc_numlights)
        {
            lighttable_t** xwalllights;
            for (i = 0; i < dc_numlights; i++)
            {
                int lightnum;
                if ((frontsector->lightlist[i].caster && frontsector->lightlist[i].caster->flags & FF_FOG && frontsector->lightlist[i].height != *frontsector->lightlist[i].caster->bottomheight) || (dc_lightlist[i].extra_colormap && dc_lightlist[i].extra_colormap->fog))
                    lightnum = (dc_lightlist[i].lightlevel >> LIGHTSEGSHIFT);
                else
                    lightnum = (dc_lightlist[i].lightlevel >> LIGHTSEGSHIFT) + extralight;

                if (dc_lightlist[i].extra_colormap);
                else if (curline->v1->y == curline->v2->y)
                    lightnum--;
                else if (curline->v1->x == curline->v2->x)
                    lightnum++;

                if (lightnum < 0)
                    xwalllights = scalelight[0];
                else if (lightnum >= LIGHTLEVELS)
                    xwalllights = scalelight[LIGHTLEVELS - 1];
                else
                    xwalllights = scalelight[lightnum];

                fixed_t lightScale = adjustedlightscale * FRACUNIT;
                lightScale = FixedMul(rw_scale, lightScale);
                index = lightScale >> LIGHTSCALESHIFT;

                if (index >= MAXLIGHTSCALE)
                    index = MAXLIGHTSCALE - 1;

                if (dc_lightlist[i].extra_colormap && !fixedcolormap)
                    dc_lightlist[i].rcolormap = dc_lightlist[i].extra_colormap->colormap + (xwalllights[index] - colormaps);
                else if (!fixedcolormap)
                    dc_lightlist[i].rcolormap = xwalllights[index];
                else
                    dc_lightlist[i].rcolormap = fixedcolormap;

                colfunc = R_DrawColumnShadowed_8;
            }
        }

        frontscale[rw_x] = rw_scale;

        // draw the wall tiers
        if (midtexture)
        {
            // single sided line
            dc_yl = yl;
            dc_yh = yh;
            dc_texturemid = rw_midtexturemid;
            dc_source = R_GetColumn(midtexture, texturecolumn);
            dc_texheight = textureheight[midtexture] >> FRACBITS;
            //profile stuff ---------------------------------------------------------
#ifdef TIMING
            ProfZeroTimer();
#endif
#ifdef HORIZONTALDRAW
            hcolfunc();
#else
            colfunc();
#endif
#ifdef TIMING
            RDMSR(0x10, &mycount);
            mytotal += mycount;      //64bit add

            if (nombre-- == 0)
                I_Error("R_DrawColumn CPU Spy reports: 0x%d %d\n", *((int*)&mytotal + 1),
                    (int)mytotal);
#endif
            //profile stuff ---------------------------------------------------------

            // dont draw anything more for this column, since
            // a midtexture blocks the view
            ceilingclip[rw_x] = viewheight;
            floorclip[rw_x] = -1;
        }
        else
        {
            // two sided line
            if (toptexture)
            {
                // top wall
                mid = pixhigh >> HEIGHTBITS;
                pixhigh += pixhighstep;

                if (mid >= floorclip[rw_x])
                    mid = floorclip[rw_x] - 1;

                if (mid >= yl)
                {
                    dc_yl = yl;
                    dc_yh = mid;
                    dc_texturemid = rw_toptexturemid;
                    dc_source = R_GetColumn(toptexture, texturecolumn);
                    dc_texheight = textureheight[toptexture] >> FRACBITS;
#ifdef HORIZONTALDRAW
                    hcolfunc();
#else
                    colfunc();
#endif
                    ceilingclip[rw_x] = mid;
                }
                else
                    ceilingclip[rw_x] = yl - 1;
            }
            else
            {
                // no top wall
                if (markceiling)
                {
                    ceilingclip[rw_x] = yl - 1;
#ifdef OLDWATER
                    if (!waterplane || markwater)
                        waterclip[rw_x] = yl - 1;
#endif
                }
            }

            if (bottomtexture)
            {
                // bottom wall
                mid = (pixlow + HEIGHTUNIT - 1) >> HEIGHTBITS;
                pixlow += pixlowstep;

                // no space above wall?
                if (mid <= ceilingclip[rw_x])
                    mid = ceilingclip[rw_x] + 1;

                if (mid <= yh)
                {
                    dc_yl = mid;
                    dc_yh = yh;
                    dc_texturemid = rw_bottomtexturemid;
                    dc_source = R_GetColumn(bottomtexture,
                        texturecolumn);
                    dc_texheight = textureheight[bottomtexture] >> FRACBITS;
#ifdef HORIZONTALDRAW
                    hcolfunc();
#else
                    colfunc();
#endif
                    floorclip[rw_x] = mid;
#ifdef OLDWATER
                    if (waterplane && waterz < worldlow)
                        waterclip[rw_x] = mid;
#endif
                }
                else
                {
                    floorclip[rw_x] = yh + 1;
                }

            }
            else
            {
                // no bottom wall
                if (markfloor)
                {
                    floorclip[rw_x] = yh + 1;
#ifdef OLDWATER
                    if (!waterplane || markwater)
                        waterclip[rw_x] = yh + 1;
#endif
                }
            }
        }

        if (maskedtexture || numthicksides)
        {
            // save texturecol
            //  for backdrawing of masked mid texture
            maskedtexturecol[rw_x] = texturecolumn;
        }

        if (dc_numlights)
        {
            for (i = 0; i < dc_numlights; i++)
            {
                dc_lightlist[i].height += dc_lightlist[i].heightstep;
                if (dc_lightlist[i].flags & FF_SOLID)
                    dc_lightlist[i].botheight += dc_lightlist[i].botheightstep;
            }
        }


        /*if(dc_wallportals)
        {
          wallportal_t* wpr;
          for(wpr = dc_wallportals; wpr; wpr = wpr->next)
          {
            wpr->top += wpr->topstep;
            wpr->bottom += wpr->bottomstep;
          }
        }*/


        for (i = 0; i < MAXFFLOORS; i++)
        {
            if (ffloor[i].mark)
            {
                int y_w = ffloor[i].b_frac >> HEIGHTBITS;

                ffloor[i].f_clip[rw_x] = ffloor[i].c_clip[rw_x] = y_w;
                ffloor[i].b_frac += ffloor[i].b_step;
            }

            ffloor[i].f_frac += ffloor[i].f_step;
        }

        rw_scale += rw_scalestep;
        topfrac += topstep;
        bottomfrac += bottomstep;
    }
}



//
// R_StoreWallRange
// A wall segment will be drawn
//  between start and stop pixels (inclusive).
//
void R_StoreWallRange(int   start, int   stop)
{
    fixed_t             hyp;
    fixed_t             sineval;
    angle_t             distangle, offsetangle;
    fixed_t             vtop;
    int                 lightnum;
    int                 i, p;
    lightlist_t* light;
    r_lightlist_t* rlight;
    fixed_t             lheight;

    if (ds_p == drawsegs + maxdrawsegs)
    {
        unsigned pos = ds_p - drawsegs;
        unsigned pos2 = firstnewseg - drawsegs;
        unsigned newmax = maxdrawsegs ? maxdrawsegs * 2 : 128;
        if (firstseg)
            firstseg = (drawseg_t*)(firstseg - drawsegs);
        drawsegs = realloc(drawsegs, newmax * sizeof(*drawsegs));
        ds_p = drawsegs + pos;
        firstnewseg = drawsegs + pos2;
        maxdrawsegs = newmax;
        if (firstseg)
            firstseg = drawsegs + (int)firstseg;
    }


#ifdef RANGECHECK
    if (start >= viewwidth || start > stop)
        I_Error("Bad R_RenderWallRange: %i to %i", start, stop);
#endif

    sidedef = curline->sidedef;
    linedef = curline->linedef;

    // mark the segment as visible for auto map
    linedef->flags |= ML_MAPPED;

    // calculate rw_distance for scale calculation
    rw_normalangle = curline->angle + ANG90;
    offsetangle = abs(rw_normalangle - rw_angle1);

    if (offsetangle > ANG90)
        offsetangle = ANG90;

    distangle = ANG90 - offsetangle;
    hyp = R_PointToDist(curline->v1->x, curline->v1->y);
    sineval = finesine[distangle >> ANGLETOFINESHIFT];
    rw_distance = FixedMul(hyp, sineval);


    ds_p->x1 = rw_x = start;
    ds_p->x2 = stop;
    ds_p->curline = curline;
    rw_stopx = stop + 1;

    //SoM: Code to remove limits on openings.
    {
        extern short* openings;
        extern size_t maxopenings;
        size_t pos = lastopening - openings;
        size_t need = (rw_stopx - start) * 4 + pos;
        if (need > maxopenings)
        {
            drawseg_t* ds;  //needed for fix from *cough* zdoom *cough*
            short* oldopenings = openings;
            short* oldlast = lastopening;

            do
                maxopenings = maxopenings ? maxopenings * 2 : 16384;
            while (need > maxopenings);
            openings = realloc(openings, maxopenings * sizeof(*openings));
            lastopening = openings + pos;

            // borrowed fix from *cough* zdoom *cough*
            // [RH] We also need to adjust the openings pointers that
            //    were already stored in drawsegs.
            for (ds = drawsegs; ds < ds_p; ds++)
            {
#define ADJUST(p) if (ds->p + ds->x1 >= oldopenings && ds->p + ds->x1 <= oldlast)\
                        ds->p = ds->p - oldopenings + openings;
                ADJUST(maskedtexturecol);
                ADJUST(sprtopclip);
                ADJUST(sprbottomclip);
                ADJUST(thicksidecol);
            }
#undef ADJUST
        }
    }  // end of code to remove limits on openings



    // calculate scale at both ends and step
    ds_p->scale1 = rw_scale =
        R_ScaleFromGlobalAngle(viewangle + xtoviewangle[start]);

    if (stop > start)
    {
        ds_p->scale2 = R_ScaleFromGlobalAngle(viewangle + xtoviewangle[stop]);
        ds_p->scalestep = rw_scalestep = (ds_p->scale2 - rw_scale) / (stop - start);
    }
    else
    {
        // UNUSED: try to fix the stretched line bug
#if 0
        if (rw_distance < FRACUNIT / 2)
        {
            fixed_t         trx, try;
            fixed_t         gxt, gyt;

            trx = curline->v1->x - viewx;
            try = curline->v1->y - viewy;

            gxt = FixedMul(trx, viewcos);
            gyt = -FixedMul(try, viewsin);
            ds_p->scale1 = FixedDiv(projection, gxt - gyt) << detailshift;
        }
#endif
        ds_p->scale2 = ds_p->scale1;
    }

    // calculate texture boundaries
    //  and decide if floor / ceiling marks are needed
    worldtop = frontsector->ceilingheight - viewz;
    worldbottom = frontsector->floorheight - viewz;

#ifdef OLDWATER
    //added:18-02-98:WATER!
    if (waterplane)
    {
        waterz = waterplane->height - viewz;
        if (waterplane->height >= frontsector->ceilingheight)
            I_Error("eau plus haut que plafond");
    }
#endif

    midtexture = toptexture = bottomtexture = maskedtexture = 0;
    ds_p->maskedtexturecol = NULL;
    ds_p->numthicksides = numthicksides = 0;
    ds_p->thicksidecol = NULL;

    for (i = 0; i < MAXFFLOORS; i++)
    {
        ffloor[i].mark = false;
        ds_p->thicksides[i] = NULL;
    }

    if (numffloors)
    {
        for (i = 0; i < numffloors; i++)
            ffloor[i].f_pos = ffloor[i].height - viewz;
    }

    if (!backsector)
    {
        // single sided line
        midtexture = texturetranslation[sidedef->midtexture];
        // a single sided line is terminal, so it must mark ends
        markfloor = markceiling = true;


#ifdef OLDWATER
        //added:18-02-98:WATER! onesided marque toujours l'eau si ya dlo
        if (waterplane)
            markwater = true;
        else
            markwater = false;
#endif

        if (linedef->flags & ML_DONTPEGBOTTOM)
        {
            vtop = frontsector->floorheight +
                textureheight[sidedef->midtexture];
            // bottom of texture at bottom
            rw_midtexturemid = vtop - viewz;
        }
        else
        {
            // top of texture at top
            rw_midtexturemid = worldtop;
        }
        rw_midtexturemid += sidedef->rowoffset;

        ds_p->silhouette = SIL_BOTH;
        ds_p->sprtopclip = screenheightarray;
        ds_p->sprbottomclip = negonearray;
        ds_p->bsilheight = MAXINT;
        ds_p->tsilheight = MININT;
    }
    else
    {
        // two sided line
        ds_p->sprtopclip = ds_p->sprbottomclip = NULL;
        ds_p->silhouette = 0;

        if (frontsector->floorheight > backsector->floorheight)
        {
            ds_p->silhouette = SIL_BOTTOM;
            ds_p->bsilheight = frontsector->floorheight;
        }
        else if (backsector->floorheight > viewz)
        {
            ds_p->silhouette = SIL_BOTTOM;
            ds_p->bsilheight = MAXINT;
            // ds_p->sprbottomclip = negonearray;
        }

        if (frontsector->ceilingheight < backsector->ceilingheight)
        {
            ds_p->silhouette |= SIL_TOP;
            ds_p->tsilheight = frontsector->ceilingheight;
        }
        else if (backsector->ceilingheight < viewz)
        {
            ds_p->silhouette |= SIL_TOP;
            ds_p->tsilheight = MININT;
            // ds_p->sprtopclip = screenheightarray;
        }

        if (backsector->ceilingheight <= frontsector->floorheight)
        {
            ds_p->sprbottomclip = negonearray;
            ds_p->bsilheight = MAXINT;
            ds_p->silhouette |= SIL_BOTTOM;
        }

        if (backsector->floorheight >= frontsector->ceilingheight)
        {
            ds_p->sprtopclip = screenheightarray;
            ds_p->tsilheight = MININT;
            ds_p->silhouette |= SIL_TOP;
        }

        //SoM: 3/25/2000: This code fixes an automap bug that didn't check
        // frontsector->ceiling and backsector->floor to see if a door was closed.
        // Without the following code, sprites get displayed behind closed doors.
        {
            extern int doorclosed;    // killough 1/17/98, 2/8/98, 4/7/98
            if (doorclosed || backsector->ceilingheight <= frontsector->floorheight)
            {
                ds_p->sprbottomclip = negonearray;
                ds_p->bsilheight = MAXINT;
                ds_p->silhouette |= SIL_BOTTOM;
            }
            if (doorclosed || backsector->floorheight >= frontsector->ceilingheight)
            {                   // killough 1/17/98, 2/8/98
                ds_p->sprtopclip = screenheightarray;
                ds_p->tsilheight = MININT;
                ds_p->silhouette |= SIL_TOP;
            }
        }

        worldhigh = backsector->ceilingheight - viewz;
        worldlow = backsector->floorheight - viewz;

        // hack to allow height changes in outdoor areas
        if (frontsector->ceilingpic == skyflatnum
            && backsector->ceilingpic == skyflatnum)
        {
            worldtop = worldhigh;
        }


        if (worldlow != worldbottom
            || backsector->floorpic != frontsector->floorpic
            || backsector->lightlevel != frontsector->lightlevel
            //SoM: 3/22/2000: Check floor x and y offsets.
            || backsector->floor_xoffs != frontsector->floor_xoffs
            || backsector->floor_yoffs != frontsector->floor_yoffs
            //SoM: 3/22/2000: Prevents bleeding.
            || frontsector->heightsec != -1
            || backsector->floorlightsec != frontsector->floorlightsec
            //SoM: 4/3/2000: Check for colormaps
            || frontsector->extra_colormap != backsector->extra_colormap
            || (frontsector->ffloors != backsector->ffloors && frontsector->tag != backsector->tag))
        {
            markfloor = true;
        }
        else
        {
            // same plane on both sides
            markfloor = false;
        }


        if (worldhigh != worldtop
            || backsector->ceilingpic != frontsector->ceilingpic
            || backsector->lightlevel != frontsector->lightlevel
            //SoM: 3/22/2000: Check floor x and y offsets.
            || backsector->ceiling_xoffs != frontsector->ceiling_xoffs
            || backsector->ceiling_yoffs != frontsector->ceiling_yoffs
            //SoM: 3/22/2000: Prevents bleeding.
            || (frontsector->heightsec != -1 &&
                frontsector->ceilingpic != skyflatnum)
            || backsector->floorlightsec != frontsector->floorlightsec
            //SoM: 4/3/2000: Check for colormaps
            || frontsector->extra_colormap != backsector->extra_colormap
            || (frontsector->ffloors != backsector->ffloors && frontsector->tag != backsector->tag))
        {
            markceiling = true;
        }
        else
        {
            // same plane on both sides
            markceiling = false;
        }

        if (backsector->ceilingheight <= frontsector->floorheight
            || backsector->floorheight >= frontsector->ceilingheight)
        {
            // closed door
            markceiling = markfloor = true;
        }

#ifdef OLDWATER
        //added:18-02-98: WATER! jamais mark si l'eau ne touche pas
        //                d'upper et de bottom
        // (on s'en fout des differences de hauteur de plafond et
        //  de sol, tant que ca n'interrompt pas la surface de l'eau)
        markwater = false;
#endif


        // check TOP TEXTURE
        if (worldhigh < worldtop)
        {
#ifdef OLDWATER
            //added:18-02-98:WATER! toptexture, check si ca touche watersurf
            if (waterplane &&
                waterz > worldhigh &&
                waterz < worldtop)
                markwater = true;
#endif

            // top texture
            toptexture = texturetranslation[sidedef->toptexture];
            if (linedef->flags & ML_DONTPEGTOP)
            {
                // top of texture at top
                rw_toptexturemid = worldtop;
            }
            else
            {
                vtop = backsector->ceilingheight
                    + textureheight[sidedef->toptexture];

                // bottom of texture
                rw_toptexturemid = vtop - viewz;
            }
        }
        // check BOTTOM TEXTURE
        if (worldlow > worldbottom)     //seulement si VISIBLE!!!
        {
#ifdef OLDWATER
            //added:18-02-98:WATER! bottomtexture, check si ca touche watersurf
            if (waterplane &&
                waterz < worldlow &&
                waterz > worldbottom)
                markwater = true;
#endif

            // bottom texture
            bottomtexture = texturetranslation[sidedef->bottomtexture];

            if (linedef->flags & ML_DONTPEGBOTTOM)
            {
                // bottom of texture at bottom
                // top of texture at top
                rw_bottomtexturemid = worldtop;
            }
            else    // top of texture at top
                rw_bottomtexturemid = worldlow;
        }

        rw_toptexturemid += sidedef->rowoffset;
        rw_bottomtexturemid += sidedef->rowoffset;

        // allocate space for masked texture tables
        if (frontsector && backsector && frontsector->tag != backsector->tag && (backsector->ffloors || frontsector->ffloors))
        {
            ffloor_t* rover;
            ffloor_t* r2;
            fixed_t   lowcut, highcut;

            //markceiling = markfloor = true;
            maskedtexture = true;

            ds_p->thicksidecol = maskedtexturecol = lastopening - rw_x;
            lastopening += rw_stopx - rw_x;

            lowcut = frontsector->floorheight > backsector->floorheight ? frontsector->floorheight : backsector->floorheight;
            highcut = frontsector->ceilingheight < backsector->ceilingheight ? frontsector->ceilingheight : backsector->ceilingheight;

            if (frontsector->ffloors && backsector->ffloors)
            {
                i = 0;
                for (rover = backsector->ffloors; rover && i < MAXFFLOORS; rover = rover->next)
                {
                    if (!(rover->flags & FF_RENDERSIDES) || !(rover->flags & FF_EXISTS))
                        continue;
                    if (rover->flags & FF_INVERTSIDES)
                        continue;
                    if (*rover->topheight < lowcut || *rover->bottomheight > highcut)
                        continue;

                    for (r2 = frontsector->ffloors; r2; r2 = r2->next)
                    {
                        if (!(r2->flags & FF_EXISTS) || !(r2->flags & FF_RENDERSIDES)
                            || *r2->topheight < lowcut || *r2->bottomheight > highcut)
                            continue;

                        if (rover->flags & FF_EXTRA)
                        {
                            if (!(r2->flags & FF_CUTEXTRA))
                                continue;

                            if (r2->flags & FF_EXTRA && (r2->flags & (FF_TRANSLUCENT | FF_FOG)) != (rover->flags & (FF_TRANSLUCENT | FF_FOG)))
                                continue;
                        }
                        else
                        {
                            if (!(r2->flags & FF_CUTSOLIDS))
                                continue;
                        }

                        if (*rover->topheight > *r2->topheight || *rover->bottomheight < *r2->bottomheight)
                            continue;

                        break;
                    }
                    if (r2)
                        continue;

                    ds_p->thicksides[i] = rover;
                    i++;
                }

                for (rover = frontsector->ffloors; rover && i < MAXFFLOORS; rover = rover->next)
                {
                    if (!(rover->flags & FF_RENDERSIDES) || !(rover->flags & FF_EXISTS))
                        continue;
                    if (!(rover->flags & FF_ALLSIDES))
                        continue;
                    if (*rover->topheight < lowcut || *rover->bottomheight > highcut)
                        continue;

                    for (r2 = backsector->ffloors; r2; r2 = r2->next)
                    {
                        if (!(r2->flags & FF_EXISTS) || !(r2->flags & FF_RENDERSIDES)
                            || *r2->topheight < lowcut || *r2->bottomheight > highcut)
                            continue;

                        if (rover->flags & FF_EXTRA)
                        {
                            if (!(r2->flags & FF_CUTEXTRA))
                                continue;

                            if (r2->flags & FF_EXTRA && (r2->flags & (FF_TRANSLUCENT | FF_FOG)) != (rover->flags & (FF_TRANSLUCENT | FF_FOG)))
                                continue;
                        }
                        else
                        {
                            if (!(r2->flags & FF_CUTSOLIDS))
                                continue;
                        }

                        if (*rover->topheight > *r2->topheight || *rover->bottomheight < *r2->bottomheight)
                            continue;

                        break;
                    }
                    if (r2)
                        continue;

                    ds_p->thicksides[i] = rover;
                    i++;
                }
            }
            else if (backsector->ffloors)
            {
                for (rover = backsector->ffloors, i = 0; rover && i < MAXFFLOORS; rover = rover->next)
                {
                    if (!(rover->flags & FF_RENDERSIDES) || !(rover->flags & FF_EXISTS) || rover->flags & FF_INVERTSIDES)
                        continue;
                    if (*rover->topheight <= frontsector->floorheight || *rover->bottomheight >= frontsector->ceilingheight)
                        continue;

                    ds_p->thicksides[i] = rover;
                    i++;
                }
            }
            else if (frontsector->ffloors)
            {
                for (rover = frontsector->ffloors, i = 0; rover && i < MAXFFLOORS; rover = rover->next)
                {
                    if (!(rover->flags & FF_RENDERSIDES) || !(rover->flags & FF_EXISTS) || !(rover->flags & FF_ALLSIDES))
                        continue;
                    if (*rover->topheight <= frontsector->floorheight || *rover->bottomheight >= frontsector->ceilingheight)
                        continue;
                    if (*rover->topheight <= backsector->floorheight || *rover->bottomheight >= backsector->ceilingheight)
                        continue;

                    ds_p->thicksides[i] = rover;
                    i++;
                }
            }

            ds_p->numthicksides = numthicksides = i;
        }
        if (sidedef->midtexture)
        {
            // masked midtexture
            if (!ds_p->thicksidecol)
            {
                ds_p->maskedtexturecol = maskedtexturecol = lastopening - rw_x;
                lastopening += rw_stopx - rw_x;
            }
            else
                ds_p->maskedtexturecol = ds_p->thicksidecol;

            maskedtexture = true;
        }
    }

    // calculate rw_offset (only needed for textured lines)
    segtextured = midtexture || toptexture || bottomtexture || maskedtexture || (numthicksides > 0);

    if (segtextured)
    {
        offsetangle = rw_normalangle - rw_angle1;

        if (offsetangle > ANG180)
            offsetangle = -offsetangle;

        if (offsetangle > ANG90)
            offsetangle = ANG90;

        sineval = finesine[offsetangle >> ANGLETOFINESHIFT];
        rw_offset = FixedMul(hyp, sineval);

        if (rw_normalangle - rw_angle1 < ANG180)
            rw_offset = -rw_offset;

        /// don't use texture offset for splats
        rw_offset2 = rw_offset + curline->offset;
        rw_offset += sidedef->textureoffset + curline->offset;
        rw_centerangle = ANG90 + viewangle - rw_normalangle;

        // calculate light table
        //  use different light tables
        //  for horizontal / vertical / diagonal
        // OPTIMIZE: get rid of LIGHTSEGSHIFT globally
        if (!fixedcolormap)
        {
            lightnum = (frontsector->lightlevel >> LIGHTSEGSHIFT) + extralight;

            if (curline->v1->y == curline->v2->y)
                lightnum--;
            else if (curline->v1->x == curline->v2->x)
                lightnum++;

            if (lightnum < 0)
                walllights = scalelight[0];
            else if (lightnum >= LIGHTLEVELS)
                walllights = scalelight[LIGHTLEVELS - 1];
            else
                walllights = scalelight[lightnum];
        }
    }

    // if a floor / ceiling plane is on the wrong side
    //  of the view plane, it is definitely invisible
    //  and doesn't need to be marked.

    //added:18-02-98: WATER! cacher ici dans certaines conditions?
    //                la surface eau est visible de dessous et dessus...
    if (frontsector->heightsec == -1)
    {
        if (frontsector->floorheight >= viewz)
        {
            // above view plane
            markfloor = false;
        }

        if (frontsector->ceilingheight <= viewz
            && frontsector->ceilingpic != skyflatnum)
        {
            // below view plane
            markceiling = false;
        }
    }

    // calculate incremental stepping values for texture edges
    worldtop >>= 4;
    worldbottom >>= 4;

    topstep = -FixedMul(rw_scalestep, worldtop);
    topfrac = (centeryfrac >> 4) - FixedMul(worldtop, rw_scale);



#ifdef OLDWATER
    //added:18-02-98:WATER!
    waterz >>= 4;
    if (markwater)
    {
        if (waterplane == NULL)
            I_Error("fuck no waterplane!");
        waterstep = -FixedMul(rw_scalestep, waterz);
        waterfrac = (centeryfrac >> 4) - FixedMul(waterz, rw_scale);
    }
#endif

    bottomstep = -FixedMul(rw_scalestep, worldbottom);
    bottomfrac = (centeryfrac >> 4) - FixedMul(worldbottom, rw_scale);

    dc_numlights = 0;

    if (frontsector->numlights)
    {
        dc_numlights = frontsector->numlights;
        if (dc_numlights >= dc_maxlights)
        {
            dc_maxlights = dc_numlights;
            dc_lightlist = realloc(dc_lightlist, sizeof(r_lightlist_t) * dc_maxlights);
        }

        for (i = p = 0; i < dc_numlights; i++)
        {
            light = &frontsector->lightlist[i];
            rlight = &dc_lightlist[p];

            if (i != 0)
            {
                if (light->height < frontsector->floorheight)
                    continue;

                if (light->height > frontsector->ceilingheight)
                    if (i + 1 < dc_numlights && frontsector->lightlist[i + 1].height > frontsector->ceilingheight)
                        continue;
            }
            rlight->height = (centeryfrac >> 4) - FixedMul((light->height - viewz) >> 4, rw_scale);
            rlight->heightstep = -FixedMul(rw_scalestep, (light->height - viewz) >> 4);
            rlight->flags = light->flags;
            if (light->caster && light->caster->flags & FF_SOLID)
            {
                lheight = *light->caster->bottomheight > frontsector->ceilingheight ? frontsector->ceilingheight + FRACUNIT : *light->caster->bottomheight;
                rlight->botheight = (centeryfrac >> 4) - FixedMul((*light->caster->bottomheight - viewz) >> 4, rw_scale);
                rlight->botheightstep = -FixedMul(rw_scalestep, (*light->caster->bottomheight - viewz) >> 4);
            }

            rlight->lightlevel = *light->lightlevel;
            rlight->extra_colormap = light->extra_colormap;
            p++;
        }
        dc_numlights = p;
    }

    if (numffloors)
    {
        for (i = 0; i < numffloors; i++)
        {
            ffloor[i].f_pos >>= 4;
            ffloor[i].f_step = FixedMul(-rw_scalestep, ffloor[i].f_pos);
            ffloor[i].f_frac = (centeryfrac >> 4) - FixedMul(ffloor[i].f_pos, rw_scale);
        }
    }

    if (backsector)
    {
        worldhigh >>= 4;
        worldlow >>= 4;

        if (worldhigh < worldtop)
        {
            pixhigh = (centeryfrac >> 4) - FixedMul(worldhigh, rw_scale);
            pixhighstep = -FixedMul(rw_scalestep, worldhigh);
        }

        if (worldlow > worldbottom)
        {
            pixlow = (centeryfrac >> 4) - FixedMul(worldlow, rw_scale);
            pixlowstep = -FixedMul(rw_scalestep, worldlow);
        }

        {
            ffloor_t* rover;
            i = 0;

            if (backsector->ffloors)
            {
                for (rover = backsector->ffloors; rover && i < MAXFFLOORS; rover = rover->next)
                {
                    if (!(rover->flags & FF_EXISTS) || !(rover->flags & FF_RENDERPLANES))
                        continue;

                    if (*rover->bottomheight <= backsector->ceilingheight &&
                        *rover->bottomheight >= backsector->floorheight &&
                        ((viewz < *rover->bottomheight && !(rover->flags & FF_INVERTPLANES)) ||
                            (viewz > *rover->bottomheight && (rover->flags & FF_BOTHPLANES))))
                    {
                        ffloor[i].mark = true;
                        ffloor[i].b_pos = *rover->bottomheight;
                        ffloor[i].b_pos = (ffloor[i].b_pos - viewz) >> 4;
                        ffloor[i].b_step = FixedMul(-rw_scalestep, ffloor[i].b_pos);
                        ffloor[i].b_frac = (centeryfrac >> 4) - FixedMul(ffloor[i].b_pos, rw_scale);
                        i++;
                    }
                    if (i >= MAXFFLOORS)
                        break;
                    if (*rover->topheight >= backsector->floorheight &&
                        *rover->topheight <= backsector->ceilingheight &&
                        ((viewz > *rover->topheight && !(rover->flags & FF_INVERTPLANES)) ||
                            (viewz < *rover->topheight && (rover->flags & FF_BOTHPLANES))))
                    {
                        ffloor[i].mark = true;
                        ffloor[i].b_pos = *rover->topheight;
                        ffloor[i].b_pos = (ffloor[i].b_pos - viewz) >> 4;
                        ffloor[i].b_step = FixedMul(-rw_scalestep, ffloor[i].b_pos);
                        ffloor[i].b_frac = (centeryfrac >> 4) - FixedMul(ffloor[i].b_pos, rw_scale);
                        i++;
                    }
                }
            }
            else if (frontsector && frontsector->ffloors)
            {
                for (rover = frontsector->ffloors; rover && i < MAXFFLOORS; rover = rover->next)
                {
                    if (!(rover->flags & FF_EXISTS) || !(rover->flags & FF_RENDERPLANES))
                        continue;

                    if (*rover->bottomheight <= frontsector->ceilingheight &&
                        *rover->bottomheight >= frontsector->floorheight &&
                        ((viewz < *rover->bottomheight && !(rover->flags & FF_INVERTPLANES)) ||
                            (viewz > *rover->bottomheight && (rover->flags & FF_BOTHPLANES))))
                    {
                        ffloor[i].mark = true;
                        ffloor[i].b_pos = *rover->bottomheight;
                        ffloor[i].b_pos = (ffloor[i].b_pos - viewz) >> 4;
                        ffloor[i].b_step = FixedMul(-rw_scalestep, ffloor[i].b_pos);
                        ffloor[i].b_frac = (centeryfrac >> 4) - FixedMul(ffloor[i].b_pos, rw_scale);
                        i++;
                    }
                    if (i >= MAXFFLOORS)
                        break;
                    if (*rover->topheight >= frontsector->floorheight &&
                        *rover->topheight <= frontsector->ceilingheight &&
                        ((viewz > *rover->topheight && !(rover->flags & FF_INVERTPLANES)) ||
                            (viewz < *rover->topheight && (rover->flags & FF_BOTHPLANES))))
                    {
                        ffloor[i].mark = true;
                        ffloor[i].b_pos = *rover->topheight;
                        ffloor[i].b_pos = (ffloor[i].b_pos - viewz) >> 4;
                        ffloor[i].b_step = FixedMul(-rw_scalestep, ffloor[i].b_pos);
                        ffloor[i].b_frac = (centeryfrac >> 4) - FixedMul(ffloor[i].b_pos, rw_scale);
                        i++;
                    }
                }
            }
        }
    }

    // get a new or use the same visplane
    if (markceiling)
    {
        if (ceilingplane) //SoM: 3/29/2000: Check for null ceiling planes
            ceilingplane = R_CheckPlane(ceilingplane, rw_x, rw_stopx - 1);
        else
            markceiling = 0;
    }

    // get a new or use the same visplane
    if (markfloor)
    {
        if (floorplane) //SoM: 3/29/2000: Check for null planes
            floorplane = R_CheckPlane(floorplane, rw_x, rw_stopx - 1);
        else
            markfloor = 0;
    }

#ifdef OLDWATER
    //added:18-02-98: il me faut un visplane pour l'eau...WATER!
    if (markwater)
    {
        if (waterplane == NULL)
            I_Error("pas de waterplane avec markwater!?");
        waterplane = R_CheckPlane(waterplane, rw_x, rw_stopx - 1);
    }
    // render it
    //added:24-02-98:WATER! unused now, trying something neater
    if (markwater)
        colfunc = R_DrawWaterColumn;
#endif

    ds_p->numffloorplanes = 0;
    if (numffloors)
    {
        if (firstseg == NULL)
        {
            for (i = 0; i < numffloors; i++)
                ds_p->ffloorplanes[i] = ffloor[i].plane = R_CheckPlane(ffloor[i].plane, rw_x, rw_stopx - 1);

            ds_p->numffloorplanes = numffloors;
            firstseg = ds_p;
        }
        else
        {
            for (i = 0; i < numffloors; i++)
                R_ExpandPlane(ffloor[i].plane, rw_x, rw_stopx - 1);
        }
    }


#ifdef BORIS_FIX
    if (linedef->splats && cv_splats.value)
    {
        // SoM: Isn't a bit wasteful to copy the ENTIRE array for every drawseg?
        memcpy(last_ceilingclip + ds_p->x1, ceilingclip + ds_p->x1, sizeof(short) * (ds_p->x2 - ds_p->x1 + 1));
        memcpy(last_floorclip + ds_p->x1, floorclip + ds_p->x1, sizeof(short) * (ds_p->x2 - ds_p->x1 + 1));
        R_RenderSegLoop();
        R_DrawWallSplats();
    }
    else
        R_RenderSegLoop();
#else
    R_RenderSegLoop();
#ifdef WALLSPLATS
    if (linedef->splats)
        R_DrawWallSplats();
#endif
#endif
    colfunc = basecolfunc;


    // save sprite clipping info
    if (((ds_p->silhouette & SIL_TOP) || maskedtexture)
        && !ds_p->sprtopclip)
    {
        memcpy(lastopening, ceilingclip + start, 2 * (rw_stopx - start));
        ds_p->sprtopclip = lastopening - start;
        lastopening += rw_stopx - start;
    }

    if (((ds_p->silhouette & SIL_BOTTOM) || maskedtexture)
        && !ds_p->sprbottomclip)
    {
        memcpy(lastopening, floorclip + start, 2 * (rw_stopx - start));
        ds_p->sprbottomclip = lastopening - start;
        lastopening += rw_stopx - start;
    }

    if (maskedtexture && !(ds_p->silhouette & SIL_TOP))
    {
        ds_p->silhouette |= SIL_TOP;
        ds_p->tsilheight = sidedef->midtexture ? MININT : MAXINT;
    }
    if (maskedtexture && !(ds_p->silhouette & SIL_BOTTOM))
    {
        ds_p->silhouette |= SIL_BOTTOM;
        ds_p->bsilheight = sidedef->midtexture ? MAXINT : MININT;
    }
    ds_p++;
}
