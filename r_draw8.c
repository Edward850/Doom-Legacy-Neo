// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: r_draw8.c,v 1.25 2003/07/13 18:04:57 hurdler Exp $
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
// $Log: r_draw8.c,v $
// Revision 1.25  2003/07/13 18:04:57  hurdler
// go RC1
//
// Revision 1.24  2003/07/13 13:16:15  hurdler
// go RC1
//
// Revision 1.23  2003/06/10 23:36:09  ssntails
// Variable flat support (32x32 to 2048x2048)
//
// Revision 1.22  2002/11/12 00:06:05  ssntails
// Support for translated translucent columns in software mode.
//
// Revision 1.21  2002/01/12 02:21:36  stroggonmeth
// Big commit
//
// Revision 1.20  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.19  2001/04/02 18:54:32  bpereira
// no message
//
// Revision 1.18  2001/04/01 17:35:07  bpereira
// no message
//
// Revision 1.17  2001/03/21 18:24:39  stroggonmeth
// Misc changes and fixes. Code cleanup
//
// Revision 1.16  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.15  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.14  2000/11/21 21:13:18  stroggonmeth
// Optimised 3D floors and fixed crashing bug in high resolutions.
//
// Revision 1.13  2000/11/06 20:52:16  bpereira
// no message
//
// Revision 1.12  2000/11/02 17:50:09  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.11  2000/09/28 20:57:17  bpereira
// no message
//
// Revision 1.10  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.9  2000/04/24 20:24:38  bpereira
// no message
//
// Revision 1.8  2000/04/18 17:39:39  stroggonmeth
// Bug fixes and performance tuning.
//
// Revision 1.7  2000/04/08 17:29:25  stroggonmeth
// no message
//
// Revision 1.6  2000/04/06 21:06:19  stroggonmeth
// Optimized extra_colormap code...
// Added #ifdefs for older water code.
//
// Revision 1.5  2000/04/05 15:47:46  stroggonmeth
// Added hack for Dehacked lumps. Transparent sprites are now affected by colormaps.
//
// Revision 1.4  2000/04/04 19:28:43  stroggonmeth
// Global colormaps working. Added a new linedef type 272.
//
// Revision 1.3  2000/04/04 00:32:47  stroggonmeth
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
//      8bpp span/column drawer functions
//
//  NOTE: no includes because this is included as part of r_draw.c
//
//-----------------------------------------------------------------------------

// ==========================================================================
// COLUMNS
// ==========================================================================

//  A column is a vertical slice/span of a wall texture that uses
//  a has a constant z depth from top to bottom.
//

#include "doomdef.h"
#include "doomstat.h"
#include "r_local.h"
#include "st_stuff.h"   //added:24-01-98:need ST_HEIGHT
#include "i_video.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"
#include "console.h" //Som: Until I get buffering finished

#define FUZZTABLE     50
extern int      fuzzoffset[FUZZTABLE];
extern int      fuzzpos;

#define USEBOOMFUNC

#ifndef USEASM
#ifndef USEBOOMFUNC
void R_DrawColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    count = dc_yh - dc_yl + 1;

    // Zero length, column does not exceed a pixel.
    if (count <= 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // Framebuffer destination address.
    // Use ylookup LUT to avoid multiply with ScreenWidth.
    // Use columnofs LUT for subwindows?
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Determine scaling,
    //  which is the only mapping to be done.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.
    do
    {
        // Re-map color indices from wall texture column
        //  using a lighting/special effects LUT.
        *dest = dc_colormap[dc_source[(frac >> FRACBITS) & 127]];

        dest += vid.width;
        frac += fracstep;

    }
    while (--count);
}
#else //USEBOOMFUNC
// SoM: Experiment to make software go faster. Taken from the Boom source
void R_DrawColumn_8(void)
{
    int count, ccount;
    register byte *dest;
    register fixed_t frac;
    fixed_t fracstep;

    count = dc_yh - dc_yl + 1;

    if (count <= 0)     // Zero length, column does not exceed a pixel.
        return;

    ccount = count;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // Framebuffer destination address.
    // Use ylookup LUT to avoid multiply with ScreenWidth.
    // Use columnofs LUT for subwindows? 

    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Determine scaling, which is the only mapping to be done.

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.

    {
        register const byte *source = dc_source;
        register const lighttable_t *colormap = dc_colormap;
        register int heightmask = dc_texheight - 1;
        if (dc_texheight & heightmask)
        {
            heightmask++;
            heightmask <<= FRACBITS;

            if (frac < 0)
                while ((frac += heightmask) < 0);
            else
                while (frac >= heightmask)
                    frac -= heightmask;

            do
            {
                // Re-map color indices from wall texture column
                //  using a lighting/special effects LUT.
                // heightmask is the Tutti-Frutti fix -- killough

                *dest = colormap[source[frac >> FRACBITS]];
                dest += vid.width;
                if ((frac += fracstep) >= heightmask)
                    frac -= heightmask;
            }
            while (--count);
        }
        else
        {
            while ((count -= 2) >= 0)   // texture height is a power of 2 -- killough
            {
                *dest = colormap[source[(frac >> FRACBITS) & heightmask]];
                dest += vid.width;
                frac += fracstep;
                *dest = colormap[source[(frac >> FRACBITS) & heightmask]];
                dest += vid.width;
                frac += fracstep;
            }
            if (count & 1)
                *dest = colormap[source[(frac >> FRACBITS) & heightmask]];
        }
    }
}
#endif //USEBOOMFUNC
#endif

#ifndef USEASM
#ifndef USEBOOMFUNC
void R_DrawSkyColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    count = dc_yh - dc_yl;

    // Zero length, column does not exceed a pixel.
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // Framebuffer destination address.
    // Use ylookup LUT to avoid multiply with ScreenWidth.
    // Use columnofs LUT for subwindows?
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Determine scaling,
    //  which is the only mapping to be done.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.
    do
    {
        // Re-map color indices from wall texture column
        //  using a lighting/special effects LUT.
        *dest = dc_colormap[dc_source[(frac >> FRACBITS) & 255]];

        dest += vid.width;
        frac += fracstep;

    }
    while (count--);
}
#else
void R_DrawSkyColumn_8(void)
{
    int count;
    register byte *dest;
    register fixed_t frac;
    fixed_t fracstep;

    count = dc_yh - dc_yl + 1;

    if (count <= 0)     // Zero length, column does not exceed a pixel.
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // Framebuffer destination address.
    // Use ylookup LUT to avoid multiply with ScreenWidth.
    // Use columnofs LUT for subwindows? 

    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Determine scaling, which is the only mapping to be done.

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.

    {
        register const byte *source = dc_source;
        register const lighttable_t *colormap = dc_colormap;
        register int heightmask = 255;
        if (dc_texheight & heightmask)
        {
            heightmask++;
            heightmask <<= FRACBITS;

            if (frac < 0)
                while ((frac += heightmask) < 0);
            else
                while (frac >= heightmask)
                    frac -= heightmask;

            do
            {
                // Re-map color indices from wall texture column
                //  using a lighting/special effects LUT.
                // heightmask is the Tutti-Frutti fix -- killough

                *dest = colormap[source[frac >> FRACBITS]];
                dest += vid.width;
                if ((frac += fracstep) >= heightmask)
                    frac -= heightmask;
            }
            while (--count);
        }
        else
        {
            while ((count -= 2) >= 0)   // texture height is a power of 2 -- killough
            {
                *dest = colormap[source[(frac >> FRACBITS) & heightmask]];
                dest += vid.width;
                frac += fracstep;
                *dest = colormap[source[(frac >> FRACBITS) & heightmask]];
                dest += vid.width;
                frac += fracstep;
            }
            if (count & 1)
                *dest = colormap[source[(frac >> FRACBITS) & heightmask]];
        }
    }
}
#endif // USEBOOMFUNC
#endif

//  The standard Doom 'fuzzy' (blur, shadow) effect
//  originally used for spectres and when picking up the blur sphere
//
//#ifndef USEASM // NOT IN ASSEMBLER, TO DO.. IF WORTH IT
void R_DrawFuzzColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    // Adjust borders. Low...
    if (!dc_yl)
        dc_yl = 1;

    // .. and high.
    if (dc_yh == viewheight - 1)
        dc_yh = viewheight - 2;

    count = dc_yh - dc_yl;

    // Zero length.
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
    {
        I_Error("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
    }
#endif

    // Does not work with blocky mode.
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    do
    {
        // Lookup framebuffer, and retrieve
        //  a pixel that is either one column
        //  left or right of the current one.
        // Add index from colormap to index.
        *dest = colormaps[6 * 256 + dest[fuzzoffset[fuzzpos]]];

        // Clamp table lookup index.
        if (++fuzzpos == FUZZTABLE)
            fuzzpos = 0;

        dest += vid.width;

        frac += fracstep;
    }
    while (count--);
}

//#endif

#ifndef USEASM
// used in tiltview, but never called for now, but who know...
void R_DrawSpanNoWrap(void)
{
}
#endif

#ifndef USEASM
void R_DrawShadeColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    // check out coords for src*
    if ((dc_yl < 0) || (dc_x >= vid.width))
        return;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
    {
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
    }

#endif

    // FIXME. As above.
    //src  = ylookup[dc_yl] + columnofs[dc_x+2];
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Here we do an additional index re-mapping.
    do
    {
        *dest = *(colormaps + (dc_source[frac >> FRACBITS] << 8) + (*dest));
        dest += vid.width;
        frac += fracstep;
    }
    while (count--);
}
#endif

//
// I've made an asm routine for the transparency, because it slows down
// a lot in 640x480 with big sprites (bfg on all screen, or transparent
// walls on fullscreen)
//
#ifndef USEASM
#ifndef USEBOOMFUNC
void R_DrawTranslucentColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    // check out coords for src*
    if ((dc_yl < 0) || (dc_x >= vid.width))
        return;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
    {
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
    }

#endif

    // FIXME. As above.
    //src  = ylookup[dc_yl] + columnofs[dc_x+2];
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Here we do an additional index re-mapping.
    do
    {
        *dest = dc_colormap[*(dc_transmap + (dc_source[frac >> FRACBITS] << 8) + (*dest))];
        dest += vid.width;
        frac += fracstep;
    }
    while (count--);
}
#else
void R_DrawTranslucentColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    count = dc_yh - dc_yl + 1;

    if (count <= 0)     // Zero length, column does not exceed a pixel.
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // Framebuffer destination address.
    // Use ylookup LUT to avoid multiply with ScreenWidth.
    // Use columnofs LUT for subwindows? 

    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Determine scaling, which is the only mapping to be done.

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.

    {
        register const byte *source = dc_source;
        //register const lighttable_t *colormap = dc_colormap;
        register int heightmask = dc_texheight - 1;
        if (dc_texheight & heightmask)
        {
            heightmask++;
            heightmask <<= FRACBITS;

            if (frac < 0)
                while ((frac += heightmask) < 0);
            else
                while (frac >= heightmask)
                    frac -= heightmask;

            do
            {
                // Re-map color indices from wall texture column
                //  using a lighting/special effects LUT.
                // heightmask is the Tutti-Frutti fix -- killough

                *dest = dc_colormap[*(dc_transmap + (source[frac >> FRACBITS] << 8) + (*dest))];
                dest += vid.width;
                if ((frac += fracstep) >= heightmask)
                    frac -= heightmask;
            }
            while (--count);
        }
        else
        {
            while ((count -= 2) >= 0)   // texture height is a power of 2 -- killough
            {
                *dest = dc_colormap[*(dc_transmap + (source[frac >> FRACBITS] << 8) + (*dest))];
                dest += vid.width;
                frac += fracstep;
                *dest = dc_colormap[*(dc_transmap + (source[frac >> FRACBITS] << 8) + (*dest))];
                dest += vid.width;
                frac += fracstep;
            }
            if (count & 1)
                *dest = dc_colormap[*(dc_transmap + (source[frac >> FRACBITS] << 8) + (*dest))];
        }
    }
}
#endif // USEBOOMFUNC
#endif

// New spiffy function.
// Not only does it colormap a sprite,
// but does translucency as well.
// SSNTails 11-11-2002
// Uber-kudos to Cyan Helkaraxe for
// helping me get the brain juices flowing!
void R_DrawTranslatedTranslucentColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    count = dc_yh - dc_yl + 1;

    if (count <= 0)     // Zero length, column does not exceed a pixel.
        return;

    // FIXME. As above.
    //src  = ylookup[dc_yl] + columnofs[dc_x+2];
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.

    {
        register const byte *source = dc_source;
        //register const lighttable_t *colormap = dc_colormap;
        register int heightmask = dc_texheight - 1;
        if (dc_texheight & heightmask)
        {
            heightmask++;
            heightmask <<= FRACBITS;

            if (frac < 0)
                while ((frac += heightmask) < 0);
            else
                while (frac >= heightmask)
                    frac -= heightmask;

            do
            {
                // Re-map color indices from wall texture column
                //  using a lighting/special effects LUT.
                // heightmask is the Tutti-Frutti fix -- killough

                *dest = dc_colormap[*(dc_transmap + (dc_colormap[dc_translation[dc_source[frac >> FRACBITS]]] << 8) + (*dest))];

                dest += vid.width;
                if ((frac += fracstep) >= heightmask)
                    frac -= heightmask;
            }
            while (--count);
        }
        else
        {
            while ((count -= 2) >= 0)   // texture height is a power of 2 -- killough
            {
                *dest = dc_colormap[*(dc_transmap + (dc_colormap[dc_translation[dc_source[frac >> FRACBITS]]] << 8) + (*dest))];
                dest += vid.width;
                frac += fracstep;
                *dest = dc_colormap[*(dc_transmap + (dc_colormap[dc_translation[dc_source[frac >> FRACBITS]]] << 8) + (*dest))];
                dest += vid.width;
                frac += fracstep;
            }
            if (count & 1)
            {
                *dest = dc_colormap[*(dc_transmap + (dc_colormap[dc_translation[dc_source[frac >> FRACBITS]]] << 8) + (*dest))];
            }
        }
    }
}

//
//  Draw columns upto 128high but remap the green ramp to other colors
//
//#ifndef USEASM        // STILL NOT IN ASM, TO DO..
void R_DrawTranslatedColumn_8(void)
{
    register int count;
    register byte *dest;
    register fixed_t frac;
    register fixed_t fracstep;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
    {
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
    }

#endif
    // FIXME. As above.
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    // Here we do an additional index re-mapping.
    do
    {
        // Translation tables are used
        //  to map certain colorramps to other ones,
        //  used with PLAY sprites.
        // Thus the "green" ramp of the player 0 sprite
        //  is mapped to gray, red, black/indigo.
        *dest = dc_colormap[dc_translation[dc_source[frac >> FRACBITS]]];

        dest += vid.width;

        frac += fracstep;
    }
    while (count--);
}

//#endif

// ==========================================================================
// SPANS
// ==========================================================================

//  Draws the actual span.
//
//#ifndef USEASM //Hurdler: in tmap.nas the old func is now called R_DrawSpan_8a
#ifndef USEBOOMFUNC
void R_DrawSpan_8(void)
{
    register ULONG xfrac;
    register ULONG yfrac;
    register byte *dest;
    register int count;

#ifdef RANGECHECK
    if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= vid.width || (unsigned) ds_y > vid.height)
    {
        I_Error("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
    }
#endif

    xfrac = ds_xfrac & 0x3fFFff;
    yfrac = ds_yfrac;

    dest = ylookup[ds_y] + columnofs[ds_x1];

    // We do not check for zero spans here?
    count = ds_x2 - ds_x1;

    do
    {
        // Lookup pixel from flat texture tile,
        //  re-index using light/colormap.
        *dest++ = ds_colormap[ds_source[((yfrac >> (16 - 6)) & (0x3f << 6)) | (xfrac >> 16)]];

        // Next step in u,v.
        xfrac += ds_xstep;
        yfrac += ds_ystep;
        xfrac &= 0x3fFFff;
    }
    while (count--);
}
#else
void R_DrawSpan_8(void)
{
    register ULONG xfrac;
    register ULONG yfrac;
    register byte *dest;
    register int count;

#ifdef RANGECHECK
    if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= vid.width || (unsigned) ds_y > vid.height)
    {
        I_Error("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
    }
#endif

    xfrac = ds_xfrac & ((flatsize << FRACBITS) - 1);
    yfrac = ds_yfrac;

    dest = ylookup[ds_y] + columnofs[ds_x1];

    // We do not check for zero spans here?
    count = ds_x2 - ds_x1;

    do
    {
        count = count;
        // Lookup pixel from flat texture tile,
        //  re-index using light/colormap.
        *dest++ = ds_colormap[ds_source[((yfrac >> (16 - flatsubtract)) & (flatmask)) | (xfrac >> 16)]];

        // Next step in u,v.
        xfrac += ds_xstep;
        yfrac += ds_ystep;
        xfrac &= (flatsize << FRACBITS) - 1;
    }
    while (count--);
}
#endif // USEBOOMFUNC
//#endif USESASM

void R_DrawTranslucentSpan_8(void)
{
    fixed_t xfrac;
    fixed_t yfrac;
    fixed_t xstep;
    fixed_t ystep;
    byte *dest;
    int count;

#ifdef RANGECHECK
    if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= screen->width || ds_y > screen->height)
    {
        I_Error("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
    }
//              dscount++;
#endif

    xfrac = ds_xfrac & ((flatsize << FRACBITS) - 1);
    yfrac = ds_yfrac;

    dest = ylookup[ds_y] + columnofs[ds_x1];

    // We do not check for zero spans here?
    count = ds_x2 - ds_x1 + 1;

    xstep = ds_xstep;
    ystep = ds_ystep;

    do
    {
        // Current texture index in u,v.

        // Awesome! 256x256 flats!
//              spot = ((yfrac>>(16-8))&(0xff00)) + (xfrac>>(16));

        // Lookup pixel from flat texture tile,
        //  re-index using light/colormap.
        //      *dest++ = ds_colormap[ds_source[spot]];
//              *dest++ = ds_colormap[*(ds_transmap + (ds_source[spot] << 8) + (*dest))];
        *dest++ = ds_colormap[*(ds_transmap + (ds_source[((yfrac >> (16 - flatsubtract)) & (flatmask)) | (xfrac >> 16)] << 8) + (*dest))];

        // Next step in u,v.
        xfrac += xstep;
        yfrac += ystep;
        xfrac &= ((flatsize << FRACBITS) - 1);
    }
    while (--count);
    /*
       register unsigned position;
       unsigned step;

       byte *source;
       byte *colormap;
       byte *transmap;
       byte *dest;

       unsigned count;
       unsigned spot; 
       unsigned xtemp;
       unsigned ytemp;

       position = ((ds_xfrac<<10)&0xffff0000) | ((ds_yfrac>>6)&0xffff);
       step = ((ds_xstep<<10)&0xffff0000) | ((ds_ystep>>6)&0xffff);

       source = ds_source;
       colormap = ds_colormap;
       transmap = ds_transmap;
       dest = ylookup[ds_y] + columnofs[ds_x1];
       count = ds_x2 - ds_x1 + 1; 

       while (count >= 4)
       {
       ytemp = position>>4;
       ytemp = ytemp & 0xff00;
       xtemp = position>>26;
       spot = xtemp | ytemp;
       position += step;
       dest[0] = colormap[*(transmap + (source[spot] << 8) + (dest[0]))];

       ytemp = position>>4;
       ytemp = ytemp & 0xff00;
       xtemp = position>>26;
       spot = xtemp | ytemp;
       position += step;
       dest[1] = colormap[*(transmap + (source[spot] << 8) + (dest[1]))];

       ytemp = position>>4;
       ytemp = ytemp & 0xff00;
       xtemp = position>>26;
       spot = xtemp | ytemp;
       position += step;
       dest[2] = colormap[*(transmap + (source[spot] << 8) + (dest[2]))];

       ytemp = position>>4;
       ytemp = ytemp & 0xff00;
       xtemp = position>>26;
       spot = xtemp | ytemp;
       position += step;
       dest[3] = colormap[*(transmap + (source[spot] << 8) + (dest[3]))];

       dest += 4;
       count -= 4;
       }

       while (count--)
       { 
       ytemp = position>>4;
       ytemp = ytemp & 0xff00;
       xtemp = position>>26;
       spot = xtemp | ytemp;
       position += step;
       *dest++ = colormap[*(transmap + (source[spot] << 8) + (*dest))];
       //count--;
       } 
     */
}

void R_DrawFogSpan_8(void)
{
    byte *colormap;
    byte *transmap;
    byte *dest;

    unsigned count;

    colormap = ds_colormap;
    transmap = ds_transmap;
    dest = ylookup[ds_y] + columnofs[ds_x1];
    count = ds_x2 - ds_x1 + 1;

    while (count >= 4)
    {
        dest[0] = colormap[dest[0]];

        dest[1] = colormap[dest[1]];

        dest[2] = colormap[dest[2]];

        dest[3] = colormap[dest[3]];

        dest += 4;
        count -= 4;
    }

    while (count--)
        *dest++ = colormap[*dest];
}

//SoM: Fog wall.
void R_DrawFogColumn_8(void)
{
    int count;
    byte *dest;

    count = dc_yh - dc_yl;

    // Zero length, column does not exceed a pixel.
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // Framebuffer destination address.
    // Use ylookup LUT to avoid multiply with ScreenWidth.
    // Use columnofs LUT for subwindows?
    dest = ylookup[dc_yl] + columnofs[dc_x];

    // Determine scaling,
    //  which is the only mapping to be done.

    do
    {
        //Simple. Apply the colormap to what's allready on the screen.
        *dest = dc_colormap[*dest];
        dest += vid.width;
    }
    while (count--);
}

// SoM: This is for 3D floors that cast shadows on walls.
// This function just cuts the column up into sections and calls
// R_DrawColumn_8
void R_DrawColumnShadowed_8(void)
{
    int count;
    int realyh, realyl;
    int i;
    int height, bheight = 0;
    int solid = 0;

    realyh = dc_yh;
    realyl = dc_yl;

    count = dc_yh - dc_yl;

    // Zero length, column does not exceed a pixel.
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= vid.width || dc_yl < 0 || dc_yh >= vid.height)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    // SoM: This runs through the lightlist from top to bottom and cuts up
    // the column accordingly.
    for (i = 0; i < dc_numlights; i++)
    {
        // If the height of the light is above the column, get the colormap
        // anyway because the lighting of the top should be effected.
        solid = dc_lightlist[i].flags & FF_CUTSOLIDS;

        height = dc_lightlist[i].height >> 12;
        if (solid)
            bheight = dc_lightlist[i].botheight >> 12;
        if (height <= dc_yl)
        {
            dc_colormap = dc_lightlist[i].rcolormap;
            if (solid && dc_yl < bheight)
                dc_yl = bheight;
            continue;
        }
        // Found a break in the column!
        dc_yh = height;

        if (dc_yh > realyh)
            dc_yh = realyh;
        R_DrawColumn_8();
        if (solid)
            dc_yl = bheight;
        else
            dc_yl = dc_yh + 1;

        dc_colormap = dc_lightlist[i].rcolormap;
    }
    dc_yh = realyh;
    if (dc_yl <= realyh)
        R_DrawColumn_8();
}
