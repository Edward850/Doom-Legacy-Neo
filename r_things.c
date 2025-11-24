// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: r_things.c,v 1.44 2002/11/12 00:06:05 ssntails Exp $
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
// $Log: r_things.c,v $
// Revision 1.44  2002/11/12 00:06:05  ssntails
// Support for translated translucent columns in software mode.
//
// Revision 1.43  2002/06/30 21:37:48  hurdler
// Ready for 1.32 beta 5 release
//
// Revision 1.42  2001/12/31 16:56:39  metzgermeister
// see Dec 31 log
// .
//
// Revision 1.41  2001/08/12 15:21:04  bpereira
// see my log
//
// Revision 1.40  2001/08/06 23:57:09  stroggonmeth
// Removed portal code, improved 3D floors in hardware mode.
//
// Revision 1.39  2001/06/16 08:07:55  bpereira
// no message
//
// Revision 1.38  2001/06/10 21:16:01  bpereira
// no message
//
// Revision 1.37  2001/05/30 18:15:21  stroggonmeth
// Small crashing bug fix...
//
// Revision 1.36  2001/05/30 04:00:52  stroggonmeth
// Fixed crashing bugs in software with 3D floors.
//
// Revision 1.35  2001/05/22 14:22:23  hurdler
// show 3d-floors bug + hack for filesearch with vc++
//
// Revision 1.34  2001/05/07 20:27:16  stroggonmeth
// no message
//
// Revision 1.33  2001/04/27 13:32:14  bpereira
// no message
//
// Revision 1.32  2001/04/17 21:12:08  stroggonmeth
// Little commit. Re-enables colormaps for trans columns in C and fixes some sprite bugs.
//
// Revision 1.31  2001/03/30 17:12:51  bpereira
// no message
//
// Revision 1.30  2001/03/21 18:24:56  stroggonmeth
// Misc changes and fixes. Code cleanup
//
// Revision 1.29  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.28  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.27  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.26  2000/11/21 21:13:18  stroggonmeth
// Optimised 3D floors and fixed crashing bug in high resolutions.
//
// Revision 1.25  2000/11/12 14:15:46  hurdler
// Removed unecessary code
//
// Revision 1.24  2000/11/09 17:56:20  stroggonmeth
// Hopefully fixed a few bugs and did a few optimizations.
//
// Revision 1.23  2000/11/03 02:37:36  stroggonmeth
// Fix a few warnings when compiling.
//
// Revision 1.22  2000/11/02 17:50:10  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.21  2000/10/04 16:19:24  hurdler
// Change all those "3dfx names" to more appropriate names
//
// Revision 1.20  2000/10/02 18:25:45  bpereira
// no message
//
// Revision 1.19  2000/10/01 10:18:19  bpereira
// no message
//
// Revision 1.18  2000/09/30 16:33:08  metzgermeister
// fixed compilation
//
// Revision 1.17  2000/09/28 20:57:18  bpereira
// no message
//
// Revision 1.16  2000/09/21 16:45:08  bpereira
// no message
//
// Revision 1.15  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.14  2000/08/11 21:37:17  hurdler
// fix win32 compilation problem
//
// Revision 1.13  2000/08/11 19:10:13  metzgermeister
// *** empty log message ***
//
// Revision 1.12  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.11  2000/04/24 20:24:38  bpereira
// no message
//
// Revision 1.10  2000/04/20 21:47:24  stroggonmeth
// no message
//
// Revision 1.9  2000/04/18 17:39:40  stroggonmeth
// Bug fixes and performance tuning.
//
// Revision 1.8  2000/04/11 19:07:25  stroggonmeth
// Finished my logs, fixed a crashing bug.
//
// Revision 1.7  2000/04/09 02:30:57  stroggonmeth
// Fixed missing sprite def
//
// Revision 1.6  2000/04/08 17:29:25  stroggonmeth
// no message
//
// Revision 1.5  2000/04/06 21:06:20  stroggonmeth
// Optimized extra_colormap code...
// Added #ifdefs for older water code.
//
// Revision 1.4  2000/04/04 19:28:43  stroggonmeth
// Global colormaps working. Added a new linedef type 272.
//
// Revision 1.3  2000/04/04 00:32:48  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Refresh of things, i.e. objects represented by sprites.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "console.h"
#include "g_game.h"
#include "r_local.h"
#include "sounds.h"             //skin sounds
#include "st_stuff.h"
#include "w_wad.h"
#include "z_zone.h"

#include "i_video.h"            //rendermode

#ifdef LINUX
int strupr(char* n); // from dosstr.c
int strlwr(char* n); // from dosstr.c
#endif

static void R_InitSkins(void);

#define MINZ                  (FRACUNIT*4)
#define BASEYCENTER           (BASEVIDHEIGHT/2)

// put this in transmap of visprite to draw a shade
#define VIS_SMOKESHADE        ((void*)-1)       


typedef struct
{
    int         x1;
    int         x2;

    int         column;
    int         topclip;
    int         bottomclip;

} maskdraw_t;


// SoM: A drawnode is something that points to a 3D floor, 3D side or masked
// middle texture. This is used for sorting with sprites.
typedef struct drawnode_s
{
    visplane_t* plane;
    drawseg_t* seg;
    drawseg_t* thickseg;
    ffloor_t* ffloor;
    vissprite_t* sprite;

    struct drawnode_s* next;
    struct drawnode_s* prev;
} drawnode_t;


//
// Sprite rotation 0 is facing the viewer,
//  rotation 1 is one angle turn CLOCKWISE around the axis.
// This is not the same as the angle,
//  which increases counter clockwise (protractor).
// There was a lot of stuff grabbed wrong, so I changed it...
//
fixed_t         pspritescale;
fixed_t         pspriteyscale;  //added:02-02-98:aspect ratio for psprites
fixed_t         pspriteiscale;

lighttable_t** spritelights;

// constant arrays
//  used for psprite clipping and initializing clipping
short           negonearray[MAXVIDWIDTH];
short           screenheightarray[MAXVIDWIDTH];


//
// INITIALIZATION FUNCTIONS
//

// variables used to look up
//  and range check thing_t sprites patches
spritedef_t* sprites;
int             numsprites;

spriteframe_t   sprtemp[29];
int             maxframe;
char* spritename;

extern double adjustedlightscale;


// ==========================================================================
//
//  New sprite loading routines for Legacy : support sprites in pwad,
//  dehacked sprite renaming, replacing not all frames of an existing
//  sprite, add sprites at run-time, add wads at run-time.
//
// ==========================================================================

//
//
//
void R_InstallSpriteLump(int           lumppat,     // graphics patch
    int           lumpid,      // identifier
    unsigned      frame,
    unsigned      rotation,
    boolean       flipped)
{
    int         r;

    if (frame >= 29 || rotation > 8)
        I_Error("R_InstallSpriteLump: "
            "Bad frame characters in lump %i", lumpid);

    if ((int)frame > maxframe)
        maxframe = frame;

    if (rotation == 0)
    {
        // the lump should be used for all rotations
        if (sprtemp[frame].rotate == 0 && devparm)
            CONS_Printf("R_InitSprites: Sprite %s frame %c has "
                "multiple rot=0 lump\n", spritename, 'A' + frame);

        if (sprtemp[frame].rotate == 1 && devparm)
            CONS_Printf("R_InitSprites: Sprite %s frame %c has rotations "
                "and a rot=0 lump\n", spritename, 'A' + frame);

        sprtemp[frame].rotate = 0;
        for (r = 0; r < 8; r++)
        {
            sprtemp[frame].lumppat[r] = lumppat;
            sprtemp[frame].lumpid[r] = lumpid;
            sprtemp[frame].flip[r] = (byte)flipped;
        }
        return;
    }

    // the lump is only used for one rotation
    if (sprtemp[frame].rotate == 0 && devparm)
        CONS_Printf("R_InitSprites: Sprite %s frame %c has rotations "
            "and a rot=0 lump\n", spritename, 'A' + frame);

    sprtemp[frame].rotate = 1;

    // make 0 based
    rotation--;

    if (sprtemp[frame].lumpid[rotation] != -1 && devparm)
        CONS_Printf("R_InitSprites: Sprite %s : %c : %c "
            "has two lumps mapped to it\n",
            spritename, 'A' + frame, '1' + rotation);

    // lumppat & lumpid are the same for original Doom, but different
    // when using sprites in pwad : the lumppat points the new graphics
    sprtemp[frame].lumppat[rotation] = lumppat;
    sprtemp[frame].lumpid[rotation] = lumpid;
    sprtemp[frame].flip[rotation] = (byte)flipped;
}


// Install a single sprite, given its identifying name (4 chars)
//
// (originally part of R_AddSpriteDefs)
//
// Pass: name of sprite : 4 chars
//       spritedef_t
//       wadnum         : wad number, indexes wadfiles[], where patches
//                        for frames are found
//       startlump      : first lump to search for sprite frames
//       endlump        : AFTER the last lump to search
//
// Returns true if the sprite was succesfully added
//
boolean R_AddSingleSpriteDef(char* sprname, spritedef_t* spritedef, int wadnum, int startlump, int endlump)
{
    int         l;
    int         intname;
    int         frame;
    int         rotation;
    lumpinfo_t* lumpinfo;
    patch_t     patch;

    intname = *(int*)sprname;

    memset(sprtemp, -1, sizeof(sprtemp));
    maxframe = -1;

    // are we 'patching' a sprite already loaded ?
    // if so, it might patch only certain frames, not all
    if (spritedef->numframes) // (then spriteframes is not null)
    {
        // copy the already defined sprite frames
        memcpy(sprtemp, spritedef->spriteframes,
            spritedef->numframes * sizeof(spriteframe_t));
        maxframe = spritedef->numframes - 1;
    }

    // scan the lumps,
    //  filling in the frames for whatever is found
    lumpinfo = wadfiles[wadnum]->lumpinfo;
    if (endlump > wadfiles[wadnum]->numlumps)
        endlump = wadfiles[wadnum]->numlumps;

    for (l = startlump; l < endlump; l++)
    {
        if (*(int*)lumpinfo[l].name == intname)
        {
            frame = lumpinfo[l].name[4] - 'A';
            rotation = lumpinfo[l].name[5] - '0';

            // skip NULL sprites from very old dmadds pwads
            if (W_LumpLength((wadnum << 16) + l) <= 8)
                continue;

            // store sprite info in lookup tables
            //FIXME:numspritelumps do not duplicate sprite replacements
            W_ReadLumpHeader((wadnum << 16) + l, &patch, sizeof(patch_t));
            spritewidth[numspritelumps] = SHORT(patch.width) << FRACBITS;
            spriteoffset[numspritelumps] = SHORT(patch.leftoffset) << FRACBITS;
            spritetopoffset[numspritelumps] = SHORT(patch.topoffset) << FRACBITS;
            spriteheight[numspritelumps] = SHORT(patch.height) << FRACBITS;

#ifdef HWRENDER
            //BP: we cannot use special tric in hardware mode because feet in ground caused by z-buffer
            if (rendermode != render_soft && SHORT(patch.topoffset) > 0 // not for psprite
                && SHORT(patch.topoffset) < SHORT(patch.height))
                // perfect is patch.height but sometime it is too high
                spritetopoffset[numspritelumps] = min(SHORT(patch.topoffset + 4), SHORT(patch.height)) << FRACBITS;

#endif

            //----------------------------------------------------

            R_InstallSpriteLump((wadnum << 16) + l, numspritelumps, frame, rotation, false);

            if (lumpinfo[l].name[6])
            {
                frame = lumpinfo[l].name[6] - 'A';
                rotation = lumpinfo[l].name[7] - '0';
                R_InstallSpriteLump((wadnum << 16) + l, numspritelumps, frame, rotation, true);
            }

            if (++numspritelumps >= MAXSPRITELUMPS)
                I_Error("R_AddSingleSpriteDef: too much sprite replacements (numspritelumps)\n");
        }
    }

    //
    // if no frames found for this sprite
    //
    if (maxframe == -1)
    {
        // the first time (which is for the original wad),
        // all sprites should have their initial frames
        // and then, patch wads can replace it
        // we will skip non-replaced sprite frames, only if
        // they have already have been initially defined (original wad)

        //check only after all initial pwads added
        //if (spritedef->numframes == 0)
        //    I_Error ("R_AddSpriteDefs: no initial frames found for sprite %s\n",
        //             namelist[i]);

        // sprite already has frames, and is not replaced by this wad
        return false;
    }

    maxframe++;

    //
    //  some checks to help development
    //
    for (frame = 0; frame < maxframe; frame++)
    {
        switch ((int)sprtemp[frame].rotate)
        {
        case -1:
            // no rotations were found for that frame at all
            I_Error("R_InitSprites: No patches found "
                "for %s frame %c", sprname, frame + 'A');
            break;

        case 0:
            // only the first rotation is needed
            break;

        case 1:
            // must have all 8 frames
            for (rotation = 0; rotation < 8; rotation++)
                // we test the patch lump, or the id lump whatever
                // if it was not loaded the two are -1
                if (sprtemp[frame].lumppat[rotation] == -1)
                    I_Error("R_InitSprites: Sprite %s frame %c "
                        "is missing rotations",
                        sprname, frame + 'A');
            break;
        }
    }

    // allocate space for the frames present and copy sprtemp to it
    if (spritedef->numframes &&             // has been allocated
        spritedef->numframes < maxframe)   // more frames are defined ?
    {
        Z_Free(spritedef->spriteframes);
        spritedef->spriteframes = NULL;
    }

    // allocate this sprite's frames
    if (spritedef->spriteframes == NULL)
        spritedef->spriteframes =
        Z_Malloc(maxframe * sizeof(spriteframe_t), PU_STATIC, NULL);

    spritedef->numframes = maxframe;
    memcpy(spritedef->spriteframes, sprtemp, maxframe * sizeof(spriteframe_t));

    return true;
}



//
// Search for sprites replacements in a wad whose names are in namelist
//
void R_AddSpriteDefs(char** namelist, int wadnum)
{
    int         i;

    int         start;
    int         end;

    int         addsprites;

    // find the sprites section in this pwad
    // we need at least the S_END
    // (not really, but for speedup)

    start = W_CheckNumForNamePwad("S_START", wadnum, 0);
    if (start == -1)
        start = W_CheckNumForNamePwad("SS_START", wadnum, 0); //deutex compatib.
    if (start == -1)
        start = 0;      // search frames from start of wad
    // (lumpnum low word is 0)
    else
        start++;   // just after S_START

    start &= 0xFFFF;    // 0 based in lumpinfo

    end = W_CheckNumForNamePwad("S_END", wadnum, 0);
    if (end == -1)
        end = W_CheckNumForNamePwad("SS_END", wadnum, 0);     //deutex compatib.
    if (end == -1)
    {
        if (devparm)
            CONS_Printf("no sprites in pwad %d\n", wadnum);
        return;
        //I_Error ("R_AddSpriteDefs: S_END, or SS_END missing for sprites "
        //         "in pwad %d\n",wadnum);
    }
    end &= 0xFFFF;

    //
    // scan through lumps, for each sprite, find all the sprite frames
    //
    addsprites = 0;
    for (i = 0; i < numsprites; i++)
    {
        spritename = namelist[i];

        if (R_AddSingleSpriteDef(spritename, &sprites[i], wadnum, start, end))
        {
            // if a new sprite was added (not just replaced)
            addsprites++;
            if (devparm)
                CONS_Printf("sprite %s set in pwad %d\n", namelist[i], wadnum);//Fab
        }
    }

    CONS_Printf("%d sprites added from file %s\n", addsprites, wadfiles[wadnum]->filename);//Fab
    //CONS_Error ("press enter\n");
}



//
// GAME FUNCTIONS
//
static vissprite_t     vissprites[MAXVISSPRITES];
static vissprite_t* vissprite_p;


//
// R_InitSprites
// Called at program start.
//
void R_InitSprites(char** namelist)
{
    int         i;
    char** check;

    for (i = 0; i < MAXVIDWIDTH; i++)
    {
        negonearray[i] = -1;
    }

    //
    // count the number of sprite names, and allocate sprites table
    //
    check = namelist;
    while (*check != NULL)
        check++;
    numsprites = check - namelist;

    if (!numsprites)
        I_Error("R_AddSpriteDefs: no sprites in namelist\n");

    sprites = Z_Malloc(numsprites * sizeof(*sprites), PU_STATIC, NULL);
    memset(sprites, 0, numsprites * sizeof(*sprites));

    // find sprites in each -file added pwad
    for (i = 0; i < numwadfiles; i++)
        R_AddSpriteDefs(namelist, i);

    //
    // now check for skins
    //

    // it can be is do before loading config for skin cvar possible value
    R_InitSkins();
    for (i = 0; i < numwadfiles; i++)
        R_AddSkins(i);


    //
    // check if all sprites have frames
    //
    /*
    for (i=0; i<numsprites; i++)
         if (sprites[i].numframes<1)
             CONS_Printf ("R_InitSprites: sprite %s has no frames at all\n", sprnames[i]);
    */
}



//
// R_ClearSprites
// Called at frame start.
//
void R_ClearSprites(void)
{
    vissprite_p = vissprites;
}


//
// R_NewVisSprite
//
static vissprite_t     overflowsprite;

static vissprite_t* R_NewVisSprite(void)
{
    if (vissprite_p == &vissprites[MAXVISSPRITES])
        return &overflowsprite;

    vissprite_p++;
    return vissprite_p - 1;
}



//
// R_DrawMaskedColumn
// Used for sprites and masked mid textures.
// Masked means: partly transparent, i.e. stored
//  in posts/runs of opaque pixels.
//
short* mfloorclip;
short* mceilingclip;

fixed_t         spryscale;
fixed_t         sprtopscreen;
fixed_t         sprbotscreen;
fixed_t         windowtop;
fixed_t         windowbottom;

void R_DrawMaskedColumn(column_t* column)
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
        bottomscreen = sprbotscreen == MAXINT ? topscreen + spryscale * column->length :
            sprbotscreen + spryscale * column->length;

        dc_yl = (topscreen + FRACUNIT - 1) >> FRACBITS;
        dc_yh = (bottomscreen - 1) >> FRACBITS;

        if (windowtop != MAXINT && windowbottom != MAXINT)
        {
            if (windowtop > topscreen)
                dc_yl = (windowtop + FRACUNIT - 1) >> FRACBITS;
            if (windowbottom < bottomscreen)
                dc_yh = (windowbottom - 1) >> FRACBITS;
        }

        if (dc_yh >= mfloorclip[dc_x])
            dc_yh = mfloorclip[dc_x] - 1;
        if (dc_yl <= mceilingclip[dc_x])
            dc_yl = mceilingclip[dc_x] + 1;

        if (dc_yl <= dc_yh && dc_yl < vid.height && dc_yh > 0)
        {
            dc_source = (byte*)column + 3;
            dc_texturemid = basetexturemid - (column->topdelta << FRACBITS);
            // dc_source = (byte *)column + 3 - column->topdelta;

            // Drawn by either R_DrawColumn
            //  or (SHADOW) R_DrawFuzzColumn.
            //Hurdler: quick fix... something more proper should be done!!!
            if (!ylookup[dc_yl] && colfunc == R_DrawColumn_8)
            {
                static int first = 1;
                if (first)
                {
                    CONS_Printf("WARNING: avoiding a crash in %s %d\n", __FILE__, __LINE__);
                    first = 0;
                }
            }
            else
                colfunc();
        }
        column = (column_t*)((byte*)column + column->length + 4);
    }

    dc_texturemid = basetexturemid;
}



//
// R_DrawVisSprite
//  mfloorclip and mceilingclip should also be set.
//
static void R_DrawVisSprite(vissprite_t* vis,
    int                   x1,
    int                   x2)
{
    column_t* column;
    int                 texturecolumn;
    fixed_t             frac;
    patch_t* patch;


    //Fab:R_InitSprites now sets a wad lump number
    patch = W_CacheLumpNum(vis->patch, PU_CACHE);

    dc_colormap = vis->colormap;

    // Support for translated and translucent sprites. SSNTails 11-11-2002
    if (vis->mobjflags & MF_TRANSLATION && vis->transmap)
    {
        colfunc = transtransfunc;
        dc_transmap = vis->transmap;
        dc_translation = translationtables - 256 +
            ((vis->mobjflags & MF_TRANSLATION) >> (MF_TRANSSHIFT - 8));
    }
    if (vis->transmap == VIS_SMOKESHADE)
        // shadecolfunc uses 'colormaps'
        colfunc = shadecolfunc;
    else if (vis->transmap)
    {
        colfunc = fuzzcolfunc;
        dc_transmap = vis->transmap;    //Fab:29-04-98: translucency table
    }
    else if (vis->mobjflags & MF_TRANSLATION)
    {
        // translate green skin to another color
        colfunc = transcolfunc;
        dc_translation = translationtables - 256 +
            ((vis->mobjflags & MF_TRANSLATION) >> (MF_TRANSSHIFT - 8));
    }

    if (vis->extra_colormap && !fixedcolormap)
    {
        if (!dc_colormap)
            dc_colormap = vis->extra_colormap->colormap;
        else
            dc_colormap = &vis->extra_colormap->colormap[dc_colormap - colormaps];
    }
    if (!dc_colormap)
        dc_colormap = colormaps;

    //dc_iscale = abs(vis->xiscale)>>detailshift;  ???
    dc_iscale = FixedDiv(FRACUNIT, vis->scale);
    dc_texturemid = vis->texturemid;
    dc_texheight = 0;

    frac = vis->startfrac;
    spryscale = vis->scale;
    sprtopscreen = centeryfrac - FixedMul(dc_texturemid, spryscale);
    windowtop = windowbottom = sprbotscreen = MAXINT;

    for (dc_x = vis->x1; dc_x <= vis->x2; dc_x++, frac += vis->xiscale)
    {
        texturecolumn = frac >> FRACBITS;
#ifdef RANGECHECK
        if (texturecolumn < 0 || texturecolumn >= SHORT(patch->width))
            I_Error("R_DrawSpriteRange: bad texturecolumn");
#endif
        column = (column_t*)((byte*)patch +
            LONG(patch->columnofs[texturecolumn]));
        R_DrawMaskedColumn(column);
    }

    colfunc = basecolfunc;
}




//
// R_SplitSprite
// runs through a sector's lightlist and
static void R_SplitSprite(vissprite_t* sprite, mobj_t* thing)
{
    int           i, lightnum, index;
    fixed_t               cutfrac;
    sector_t* sector;
    vissprite_t* newsprite;

    sector = sprite->sector;

    for (i = 1; i < sector->numlights; i++)
    {
        if (sector->lightlist[i].height >= sprite->gzt || !(sector->lightlist[i].caster->flags & FF_CUTSPRITES))
            continue;
        if (sector->lightlist[i].height <= sprite->gz)
            return;

        cutfrac = (centeryfrac - FixedMul(sector->lightlist[i].height - viewz, sprite->scale)) >> FRACBITS;
        if (cutfrac < 0)
            continue;
        if (cutfrac > vid.height)
            return;

        // Found a split! Make a new sprite, copy the old sprite to it, and
    // adjust the heights.
        newsprite = R_NewVisSprite();
        memcpy(newsprite, sprite, sizeof(vissprite_t));

        sprite->cut |= SC_BOTTOM;
        sprite->gz = sector->lightlist[i].height;

        newsprite->gzt = sprite->gz;

        sprite->sz = cutfrac;
        newsprite->szt = sprite->sz - 1;

        if (sector->lightlist[i].height < sprite->pzt && sector->lightlist[i].height > sprite->pz)
            sprite->pz = newsprite->pzt = sector->lightlist[i].height;
        else
        {
            newsprite->pz = newsprite->gz;
            newsprite->pzt = newsprite->gzt;
        }

        newsprite->cut |= SC_TOP;
        if (!(sector->lightlist[i].caster->flags & FF_NOSHADE))
        {
            if (sector->lightlist[i].caster->flags & FF_FOG)
                lightnum = (*sector->lightlist[i].lightlevel >> LIGHTSEGSHIFT);
            else
                lightnum = (*sector->lightlist[i].lightlevel >> LIGHTSEGSHIFT) + extralight;

            if (lightnum < 0)
                spritelights = scalelight[0];
            else if (lightnum >= LIGHTLEVELS)
                spritelights = scalelight[LIGHTLEVELS - 1];
            else
                spritelights = scalelight[lightnum];

            newsprite->extra_colormap = sector->lightlist[i].extra_colormap;

            if (thing->frame & FF_SMOKESHADE)
                ;
            else
            {
                /*        if (thing->frame & FF_TRANSMASK)
                          ;
                        else if (thing->flags & MF_SHADOW)
                          ;*/

                if (fixedcolormap)
                    ;
                else if ((thing->frame & (FF_FULLBRIGHT | FF_TRANSMASK) || thing->flags & MF_SHADOW) && (!newsprite->extra_colormap || !newsprite->extra_colormap->fog))
                    ;
                else
                {
                    fixed_t lightScale = adjustedlightscale * FRACUNIT;
                    lightScale = FixedMul(sprite->xscale, lightScale);
                    index = lightScale >> (LIGHTSCALESHIFT - detailshift);

                    if (index >= MAXLIGHTSCALE)
                        index = MAXLIGHTSCALE - 1;
                    newsprite->colormap = spritelights[index];
                }
            }
        }
        sprite = newsprite;
    }
}


//
// R_ProjectSprite
// Generates a vissprite for a thing
//  if it might be visible.
//
static void R_ProjectSprite(mobj_t* thing)
{
    fixed_t             tr_x;
    fixed_t             tr_y;

    fixed_t             gxt;
    fixed_t             gyt;

    fixed_t             tx;
    fixed_t             tz;

    fixed_t             xscale;
    fixed_t             yscale; //added:02-02-98:aaargll..if I were a math-guy!!!

    int                 x1;
    int                 x2;

    spritedef_t* sprdef;
    spriteframe_t* sprframe;
    int                 lump;

    unsigned            rot;
    boolean             flip;

    int                 index;

    vissprite_t* vis;

    angle_t             ang;
    fixed_t             iscale;

    //SoM: 3/17/2000
    fixed_t             gzt;
    int                 heightsec;
    int                 light = 0;

    // transform the origin point
    tr_x = thing->x - viewx;
    tr_y = thing->y - viewy;

    gxt = FixedMul(tr_x, viewcos);
    gyt = -FixedMul(tr_y, viewsin);

    tz = gxt - gyt;

    // thing is behind view plane?
    if (tz < MINZ)
        return;

    // aspect ratio stuff :
    xscale = FixedDiv(projection, tz);
    yscale = FixedDiv(projectiony, tz);

    gxt = -FixedMul(tr_x, viewsin);
    gyt = FixedMul(tr_y, viewcos);
    tx = -(gyt + gxt);

    // too far off the side?
    if (abs(tx) > (tz << 2))
        return;

    // decide which patch to use for sprite relative to player
#ifdef RANGECHECK
    if ((unsigned)thing->sprite >= numsprites)
        I_Error("R_ProjectSprite: invalid sprite number %i ",
            thing->sprite);
#endif

    //Fab:02-08-98: 'skin' override spritedef currently used for skin
    if (thing->skin)
        sprdef = &((skin_t*)thing->skin)->spritedef;
    else
        sprdef = &sprites[thing->sprite];

#ifdef RANGECHECK
    if ((thing->frame & FF_FRAMEMASK) >= sprdef->numframes)
        I_Error("R_ProjectSprite: invalid sprite frame %i : %i for %s",
            thing->sprite, thing->frame, sprnames[thing->sprite]);
#endif
    sprframe = &sprdef->spriteframes[thing->frame & FF_FRAMEMASK];

#ifdef PARANOIA
    //heretic hack
    if (!sprframe)
        I_Error("sprframes NULL for sprite %d\n", thing->sprite);
#endif

    if (sprframe->rotate)
    {
        // choose a different rotation based on player view
        ang = R_PointToAngle(thing->x, thing->y);
        rot = (ang - thing->angle + (unsigned)(ANG45 / 2) * 9) >> 29;
        //Fab: lumpid is the index for spritewidth,spriteoffset... tables
        lump = sprframe->lumpid[rot];
        flip = (boolean)sprframe->flip[rot];
    }
    else
    {
        // use single rotation for all views
        rot = 0;                        //Fab: for vis->patch below
        lump = sprframe->lumpid[0];     //Fab: see note above
        flip = (boolean)sprframe->flip[0];
    }

    // calculate edges of the shape
    tx -= spriteoffset[lump];
    x1 = (centerxfrac + FixedMul(tx, xscale)) >> FRACBITS;

    // off the right side?
    if (x1 > viewwidth)
        return;

    tx += spritewidth[lump];
    x2 = ((centerxfrac + FixedMul(tx, xscale)) >> FRACBITS) - 1;

    // off the left side
    if (x2 < 0)
        return;

    //SoM: 3/17/2000: Disreguard sprites that are out of view..
    gzt = thing->z + spritetopoffset[lump];


    if (thing->subsector->sector->numlights)
    {
        int lightnum;
        light = R_GetPlaneLight(thing->subsector->sector, gzt, false);
        if (thing->subsector->sector->lightlist[light].caster && thing->subsector->sector->lightlist[light].caster->flags & FF_FOG)
            lightnum = (*thing->subsector->sector->lightlist[light].lightlevel >> LIGHTSEGSHIFT);
        else
            lightnum = (*thing->subsector->sector->lightlist[light].lightlevel >> LIGHTSEGSHIFT) + extralight;

        if (lightnum < 0)
            spritelights = scalelight[0];
        else if (lightnum >= LIGHTLEVELS)
            spritelights = scalelight[LIGHTLEVELS - 1];
        else
            spritelights = scalelight[lightnum];
    }

    heightsec = thing->subsector->sector->heightsec;

    if (heightsec != -1)   // only clip things which are in special sectors
    {
        int phs = viewplayer->mo->subsector->sector->heightsec;
        if (phs != -1 && viewz < sectors[phs].floorheight ?
            thing->z >= sectors[heightsec].floorheight :
            gzt < sectors[heightsec].floorheight)
            return;
        if (phs != -1 && viewz > sectors[phs].ceilingheight ?
            gzt < sectors[heightsec].ceilingheight &&
            viewz >= sectors[heightsec].ceilingheight :
            thing->z >= sectors[heightsec].ceilingheight)
            return;
    }

    // store information in a vissprite
    vis = R_NewVisSprite();
    vis->heightsec = heightsec; //SoM: 3/17/2000
    vis->mobjflags = thing->flags;
    vis->scale = yscale;           //<<detailshift;
    vis->gx = thing->x;
    vis->gy = thing->y;
    vis->gz = gzt - spriteheight[lump];
    vis->gzt = gzt;
    vis->thingheight = thing->height;
    vis->pz = thing->z;
    vis->pzt = vis->pz + vis->thingheight;
    vis->texturemid = vis->gzt - viewz;
    // foot clipping
    if (thing->flags2 & MF2_FEETARECLIPPED
        && thing->z <= thing->subsector->sector->floorheight)
        vis->texturemid -= 10 * FRACUNIT;

    vis->x1 = x1 < 0 ? 0 : x1;
    vis->x2 = x2 >= viewwidth ? viewwidth - 1 : x2;
    vis->xscale = xscale; //SoM: 4/17/2000
    vis->sector = thing->subsector->sector;
    vis->szt = (centeryfrac - FixedMul(vis->gzt - viewz, yscale)) >> FRACBITS;
    vis->sz = (centeryfrac - FixedMul(vis->gz - viewz, yscale)) >> FRACBITS;
    vis->cut = false;
    if (thing->subsector->sector->numlights)
        vis->extra_colormap = thing->subsector->sector->lightlist[light].extra_colormap;
    else
        vis->extra_colormap = thing->subsector->sector->extra_colormap;

    iscale = FixedDiv(FRACUNIT, xscale);

    if (flip)
    {
        vis->startfrac = spritewidth[lump] - 1;
        vis->xiscale = -iscale;
    }
    else
    {
        vis->startfrac = 0;
        vis->xiscale = iscale;
    }

    if (vis->x1 > x1)
        vis->startfrac += vis->xiscale * (vis->x1 - x1);

    //Fab: lumppat is the lump number of the patch to use, this is different
    //     than lumpid for sprites-in-pwad : the graphics are patched
    vis->patch = sprframe->lumppat[rot];


    //
    // determine the colormap (lightlevel & special effects)
    //
    vis->transmap = NULL;

    // specific translucency
    if (thing->frame & FF_SMOKESHADE)
        // not realy a colormap ... see R_DrawVisSprite
        vis->colormap = VIS_SMOKESHADE;
    else
    {
        if (thing->frame & FF_TRANSMASK)
            vis->transmap = (thing->frame & FF_TRANSMASK) - 0x10000 + transtables;
        else if (thing->flags & MF_SHADOW)
            // actually only the player should use this (temporary invisibility)
            // because now the translucency is set through FF_TRANSMASK
            vis->transmap = ((tr_transhi - 1) << FF_TRANSSHIFT) + transtables;


        if (fixedcolormap)
        {
            // fixed map : all the screen has the same colormap
            //  eg: negative effect of invulnerability
            vis->colormap = fixedcolormap;
        }
        else if (((thing->frame & (FF_FULLBRIGHT | FF_TRANSMASK)) || (thing->flags & MF_SHADOW)) && (!vis->extra_colormap || !vis->extra_colormap->fog))
        {
            // full bright : goggles
            vis->colormap = colormaps;
        }
        else
        {
            fixed_t lightScale = adjustedlightscale * FRACUNIT;
            lightScale = FixedMul(xscale, lightScale);
            index = lightScale >> (LIGHTSCALESHIFT - detailshift);

            if (index >= MAXLIGHTSCALE)
                index = MAXLIGHTSCALE - 1;

            vis->colormap = spritelights[index];
        }
    }

    if (thing->subsector->sector->numlights)
        R_SplitSprite(vis, thing);
}




//
// R_AddSprites
// During BSP traversal, this adds sprites by sector.
//
void R_AddSprites(sector_t* sec, int lightlevel)
{
    mobj_t* thing;
    int                 lightnum;

    if (rendermode != render_soft)
        return;

    // BSP is traversed by subsector.
    // A sector might have been split into several
    //  subsectors during BSP building.
    // Thus we check whether its already added.
    if (sec->validcount == validcount)
        return;

    // Well, now it will be done.
    sec->validcount = validcount;

    if (!sec->numlights)
    {
        if (sec->heightsec == -1) lightlevel = sec->lightlevel;

        lightnum = (lightlevel >> LIGHTSEGSHIFT) + extralight;

        if (lightnum < 0)
            spritelights = scalelight[0];
        else if (lightnum >= LIGHTLEVELS)
            spritelights = scalelight[LIGHTLEVELS - 1];
        else
            spritelights = scalelight[lightnum];
    }

    // Handle all things in sector.
    for (thing = sec->thinglist; thing; thing = thing->snext)
        if ((thing->flags2 & MF2_DONTDRAW) == 0)
            R_ProjectSprite(thing);
}


const int PSpriteSY[NUMWEAPONS] =
{
     0,             // staff
     5 * FRACUNIT,    // goldwand
    15 * FRACUNIT,    // crossbow
    15 * FRACUNIT,    // blaster
    15 * FRACUNIT,    // skullrod
    15 * FRACUNIT,    // phoenix rod
    15 * FRACUNIT,    // mace
    15 * FRACUNIT,    // gauntlets
    15 * FRACUNIT     // beak
};

//
// R_DrawPSprite
//
void R_DrawPSprite(pspdef_t* psp)
{
    fixed_t             tx;
    int                 x1;
    int                 x2;
    spritedef_t* sprdef;
    spriteframe_t* sprframe;
    int                 lump;
    boolean             flip;
    vissprite_t* vis;
    vissprite_t         avis;

    // decide which patch to use
#ifdef RANGECHECK
    if ((unsigned)psp->state->sprite >= numsprites)
        I_Error("R_ProjectSprite: invalid sprite number %i ",
            psp->state->sprite);
#endif
    sprdef = &sprites[psp->state->sprite];
#ifdef RANGECHECK
    if ((psp->state->frame & FF_FRAMEMASK) >= sprdef->numframes)
        I_Error("R_ProjectSprite: invalid sprite frame %i : %i for %s",
            psp->state->sprite, psp->state->frame, sprnames[psp->state->sprite]);
#endif
    sprframe = &sprdef->spriteframes[psp->state->frame & FF_FRAMEMASK];

#ifdef PARANOIA
    //Fab:debug
    if (sprframe == NULL)
        I_Error("sprframes NULL for state %d\n", psp->state - states);
#endif

    //Fab: see the notes in R_ProjectSprite about lumpid,lumppat
    lump = sprframe->lumpid[0];
    flip = (boolean)sprframe->flip[0];

    // calculate edges of the shape

    //added:08-01-98:replaced mul by shift
    tx = psp->sx - ((BASEVIDWIDTH / 2) << FRACBITS); //*FRACUNITS);

    //added:02-02-98:spriteoffset should be abs coords for psprites, based on
    //               320x200
    tx -= spriteoffset[lump];
    x1 = (centerxfrac + FixedMul(tx, pspritescale)) >> FRACBITS;

    // off the right side
    if (x1 > viewwidth)
        return;

    tx += spritewidth[lump];
    x2 = ((centerxfrac + FixedMul(tx, pspritescale)) >> FRACBITS) - 1;

    // off the left side
    if (x2 < 0)
        return;

    // store information in a vissprite
    vis = &avis;
    vis->mobjflags = 0;
    if (cv_splitscreen.value)
        vis->texturemid = (120 << (FRACBITS)) + FRACUNIT / 2 - (psp->sy - spritetopoffset[lump]);
    else
        vis->texturemid = (BASEYCENTER << FRACBITS) + FRACUNIT / 2 - (psp->sy - spritetopoffset[lump]);

    if (raven)
        if (viewheight == vid.height || (!cv_scalestatusbar.value && vid.dupy > 1))
            vis->texturemid -= PSpriteSY[viewplayer->readyweapon];

    //vis->texturemid += FRACUNIT/2;

    vis->x1 = x1 < 0 ? 0 : x1;
    vis->x2 = x2 >= viewwidth ? viewwidth - 1 : x2;
    vis->scale = pspriteyscale;  //<<detailshift;

    if (flip)
    {
        vis->xiscale = -pspriteiscale;
        vis->startfrac = spritewidth[lump] - 1;
    }
    else
    {
        vis->xiscale = pspriteiscale;
        vis->startfrac = 0;
    }

    if (vis->x1 > x1)
        vis->startfrac += vis->xiscale * (vis->x1 - x1);

    //Fab: see above for more about lumpid,lumppat
    vis->patch = sprframe->lumppat[0];
    vis->transmap = NULL;
    if (viewplayer->mo->flags & MF_SHADOW)      // invisibility effect
    {
        vis->colormap = NULL;   // use translucency

        // in Doom2, it used to switch between invis/opaque the last seconds
        // now it switch between invis/less invis the last seconds
        if (viewplayer->powers[pw_invisibility] > 4 * TICRATE
            || viewplayer->powers[pw_invisibility] & 8)
            vis->transmap = ((tr_transhi - 1) << FF_TRANSSHIFT) + transtables;
        else
            vis->transmap = ((tr_transmed - 1) << FF_TRANSSHIFT) + transtables;
    }
    else if (fixedcolormap)
    {
        // fixed color
        vis->colormap = fixedcolormap;
    }
    else if (psp->state->frame & FF_FULLBRIGHT)
    {
        // full bright
        vis->colormap = colormaps;
    }
    else
    {
        // local light
        vis->colormap = spritelights[MAXLIGHTSCALE - 1];
    }

    if (viewplayer->mo->subsector->sector->numlights)
    {
        int lightnum;
        int light = R_GetPlaneLight(viewplayer->mo->subsector->sector, viewplayer->mo->z + (41 << FRACBITS), false);
        vis->extra_colormap = viewplayer->mo->subsector->sector->lightlist[light].extra_colormap;
        lightnum = (*viewplayer->mo->subsector->sector->lightlist[light].lightlevel >> LIGHTSEGSHIFT) + extralight;

        if (lightnum < 0)
            spritelights = scalelight[0];
        else if (lightnum >= LIGHTLEVELS)
            spritelights = scalelight[LIGHTLEVELS - 1];
        else
            spritelights = scalelight[lightnum];

        vis->colormap = spritelights[MAXLIGHTSCALE - 1];
    }
    else
        vis->extra_colormap = viewplayer->mo->subsector->sector->extra_colormap;

    R_DrawVisSprite(vis, vis->x1, vis->x2);
}



//
// R_DrawPlayerSprites
//
void R_DrawPlayerSprites(void)
{
    int         i = 0;
    int         lightnum;
    int         light = 0;
    pspdef_t* psp;

    int kikhak;

    if (rendermode != render_soft)
        return;

    // get light level
    if (viewplayer->mo->subsector->sector->numlights)
    {
        light = R_GetPlaneLight(viewplayer->mo->subsector->sector, viewplayer->mo->z + viewplayer->mo->info->height, false);
        lightnum = (*viewplayer->mo->subsector->sector->lightlist[i].lightlevel >> LIGHTSEGSHIFT) + extralight;
    }
    else
        lightnum = (viewplayer->mo->subsector->sector->lightlevel >> LIGHTSEGSHIFT) + extralight;

    if (lightnum < 0)
        spritelights = scalelight[0];
    else if (lightnum >= LIGHTLEVELS)
        spritelights = scalelight[LIGHTLEVELS - 1];
    else
        spritelights = scalelight[lightnum];

    // clip to screen bounds
    mfloorclip = screenheightarray;
    mceilingclip = negonearray;

    //added:06-02-98: quickie fix for psprite pos because of freelook
    kikhak = centery;
    centery = centerypsp;             //for R_DrawColumn
    centeryfrac = centery << FRACBITS;  //for R_DrawVisSprite

    // add all active psprites
    for (i = 0, psp = viewplayer->psprites;
        i < NUMPSPRITES;
        i++, psp++)
    {
        if (psp->state)
            R_DrawPSprite(psp);
    }

    //added:06-02-98: oooo dirty boy
    centery = kikhak;
    centeryfrac = centery << FRACBITS;
}



//
// R_SortVisSprites
//
vissprite_t     vsprsortedhead;


void R_SortVisSprites(void)
{
    int                 i;
    int                 count;
    vissprite_t* ds;
    vissprite_t* best = NULL;      //shut up compiler
    vissprite_t         unsorted;
    fixed_t             bestscale;

    count = vissprite_p - vissprites;

    unsorted.next = unsorted.prev = &unsorted;

    if (!count)
        return;

    for (ds = vissprites; ds < vissprite_p; ds++)
    {
        ds->next = ds + 1;
        ds->prev = ds - 1;
    }

    vissprites[0].prev = &unsorted;
    unsorted.next = &vissprites[0];
    (vissprite_p - 1)->next = &unsorted;
    unsorted.prev = vissprite_p - 1;

    // pull the vissprites out by scale
    vsprsortedhead.next = vsprsortedhead.prev = &vsprsortedhead;
    for (i = 0; i < count; i++)
    {
        bestscale = MAXINT;
        for (ds = unsorted.next; ds != &unsorted; ds = ds->next)
        {
            if (ds->scale < bestscale)
            {
                bestscale = ds->scale;
                best = ds;
            }
        }
        best->next->prev = best->prev;
        best->prev->next = best->next;
        best->next = &vsprsortedhead;
        best->prev = vsprsortedhead.prev;
        vsprsortedhead.prev->next = best;
        vsprsortedhead.prev = best;
    }
}



//
// R_CreateDrawNodes
// Creates and sorts a list of drawnodes for the scene being rendered.
static void           R_CreateDrawNodes();
static drawnode_t* R_CreateDrawNode(drawnode_t* link);

static drawnode_t     nodebankhead;
static drawnode_t     nodehead;

static void R_CreateDrawNodes()
{
    drawnode_t* entry;
    drawseg_t* ds;
    int           i, p, best, x1, x2;
    fixed_t       bestdelta, delta;
    vissprite_t* rover;
    drawnode_t* r2;
    visplane_t* plane;
    int           sintersect;
    fixed_t       gzm;
    fixed_t       scale;

    // Add the 3D floors, thicksides, and masked textures...
    for (ds = ds_p; ds-- > drawsegs;)
    {
        if (ds->numthicksides)
        {
            for (i = 0; i < ds->numthicksides; i++)
            {
                entry = R_CreateDrawNode(&nodehead);
                entry->thickseg = ds;
                entry->ffloor = ds->thicksides[i];
            }
        }
        if (ds->maskedtexturecol)
        {
            entry = R_CreateDrawNode(&nodehead);
            entry->seg = ds;
        }
        if (ds->numffloorplanes)
        {
            for (i = 0; i < ds->numffloorplanes; i++)
            {
                best = -1;
                bestdelta = 0;
                for (p = 0; p < ds->numffloorplanes; p++)
                {
                    if (!ds->ffloorplanes[p])
                        continue;
                    plane = ds->ffloorplanes[p];
                    R_PlaneBounds(plane);
                    if (plane->low < con_clipviewtop || plane->high > vid.height || plane->high > plane->low)
                    {
                        ds->ffloorplanes[p] = NULL;
                        continue;
                    }

                    delta = abs(plane->height - viewz);
                    if (delta > bestdelta)
                    {
                        best = p;
                        bestdelta = delta;
                    }
                }
                if (best != -1)
                {
                    entry = R_CreateDrawNode(&nodehead);
                    entry->plane = ds->ffloorplanes[best];
                    entry->seg = ds;
                    ds->ffloorplanes[best] = NULL;
                }
                else
                    break;
            }
        }
    }

    if (vissprite_p == vissprites)
        return;

    R_SortVisSprites();
    for (rover = vsprsortedhead.prev; rover != &vsprsortedhead; rover = rover->prev)
    {
        if (rover->szt > vid.height || rover->sz < 0)
            continue;

        sintersect = (rover->x1 + rover->x2) / 2;
        gzm = (rover->gz + rover->gzt) / 2;

        for (r2 = nodehead.next; r2 != &nodehead; r2 = r2->next)
        {
            if (r2->plane)
            {
                if (r2->plane->minx > rover->x2 || r2->plane->maxx < rover->x1)
                    continue;
                if (rover->szt > r2->plane->low || rover->sz < r2->plane->high)
                    continue;

                if ((r2->plane->height < viewz && rover->pz < r2->plane->height) ||
                    (r2->plane->height > viewz && rover->pzt > r2->plane->height))
                {
                    // SoM: NOTE: Because a visplane's shape and scale is not directly
                    // bound to any single lindef, a simple poll of it's frontscale is
                    // not adiquate. We must check the entire frontscale array for any
                    // part that is in front of the sprite.

                    x1 = rover->x1;
                    x2 = rover->x2;
                    if (x1 < r2->plane->minx) x1 = r2->plane->minx;
                    if (x2 > r2->plane->maxx) x2 = r2->plane->maxx;

                    for (i = x1; i <= x2; i++)
                    {
                        if (r2->seg->frontscale[i] > rover->scale)
                            break;
                    }
                    if (i > x2)
                        continue;

                    entry = R_CreateDrawNode(NULL);
                    (entry->prev = r2->prev)->next = entry;
                    (entry->next = r2)->prev = entry;
                    entry->sprite = rover;
                    break;
                }
            }
            else if (r2->thickseg)
            {
                if (rover->x1 > r2->thickseg->x2 || rover->x2 < r2->thickseg->x1)
                    continue;

                scale = r2->thickseg->scale1 > r2->thickseg->scale2 ? r2->thickseg->scale1 : r2->thickseg->scale2;
                if (scale <= rover->scale)
                    continue;
                scale = r2->thickseg->scale1 + (r2->thickseg->scalestep * (sintersect - r2->thickseg->x1));
                if (scale <= rover->scale)
                    continue;

                if ((*r2->ffloor->topheight > viewz && *r2->ffloor->bottomheight < viewz) ||
                    (*r2->ffloor->topheight < viewz && rover->gzt < *r2->ffloor->topheight) ||
                    (*r2->ffloor->bottomheight > viewz && rover->gz > *r2->ffloor->bottomheight))
                {
                    entry = R_CreateDrawNode(NULL);
                    (entry->prev = r2->prev)->next = entry;
                    (entry->next = r2)->prev = entry;
                    entry->sprite = rover;
                    break;
                }
            }
            else if (r2->seg)
            {
                if (rover->x1 > r2->seg->x2 || rover->x2 < r2->seg->x1)
                    continue;

                scale = r2->seg->scale1 > r2->seg->scale2 ? r2->seg->scale1 : r2->seg->scale2;
                if (scale <= rover->scale)
                    continue;
                scale = r2->seg->scale1 + (r2->seg->scalestep * (sintersect - r2->seg->x1));

                if (rover->scale < scale)
                {
                    entry = R_CreateDrawNode(NULL);
                    (entry->prev = r2->prev)->next = entry;
                    (entry->next = r2)->prev = entry;
                    entry->sprite = rover;
                    break;
                }
            }
            else if (r2->sprite)
            {
                if (r2->sprite->x1 > rover->x2 || r2->sprite->x2 < rover->x1)
                    continue;
                if (r2->sprite->szt > rover->sz || r2->sprite->sz < rover->szt)
                    continue;

                if (r2->sprite->scale > rover->scale)
                {
                    entry = R_CreateDrawNode(NULL);
                    (entry->prev = r2->prev)->next = entry;
                    (entry->next = r2)->prev = entry;
                    entry->sprite = rover;
                    break;
                }
            }
        }
        if (r2 == &nodehead)
        {
            entry = R_CreateDrawNode(&nodehead);
            entry->sprite = rover;
        }
    }
}




static drawnode_t* R_CreateDrawNode(drawnode_t* link)
{
    drawnode_t* node;

    node = nodebankhead.next;
    if (node == &nodebankhead)
    {
        node = malloc(sizeof(drawnode_t));
    }
    else
        (nodebankhead.next = node->next)->prev = &nodebankhead;

    if (link)
    {
        node->next = link;
        node->prev = link->prev;
        link->prev->next = node;
        link->prev = node;
    }

    node->plane = NULL;
    node->seg = NULL;
    node->thickseg = NULL;
    node->ffloor = NULL;
    node->sprite = NULL;
    return node;
}



static void R_DoneWithNode(drawnode_t* node)
{
    (node->next->prev = node->prev)->next = node->next;
    (node->next = nodebankhead.next)->prev = node;
    (node->prev = &nodebankhead)->next = node;
}



static void R_ClearDrawNodes()
{
    drawnode_t* rover;
    drawnode_t* next;

    for (rover = nodehead.next; rover != &nodehead; )
    {
        next = rover->next;
        R_DoneWithNode(rover);
        rover = next;
    }

    nodehead.next = nodehead.prev = &nodehead;
}



void R_InitDrawNodes()
{
    nodebankhead.next = nodebankhead.prev = &nodebankhead;
    nodehead.next = nodehead.prev = &nodehead;
}



//
// R_DrawSprite
//
//Fab:26-04-98:
// NOTE : uses con_clipviewtop, so that when console is on,
//        don't draw the part of sprites hidden under the console
void R_DrawSprite(vissprite_t* spr)
{
    drawseg_t* ds;
    short               clipbot[MAXVIDWIDTH];
    short               cliptop[MAXVIDWIDTH];
    int                 x;
    int                 r1;
    int                 r2;
    fixed_t             scale;
    fixed_t             lowscale;
    int                 silhouette;

    for (x = spr->x1; x <= spr->x2; x++)
        clipbot[x] = cliptop[x] = -2;

    // Scan drawsegs from end to start for obscuring segs.
    // The first drawseg that has a greater scale
    //  is the clip seg.
    //SoM: 4/8/2000:
    // Pointer check was originally nonportable
    // and buggy, by going past LEFT end of array:

    //    for (ds=ds_p-1 ; ds >= drawsegs ; ds--)    old buggy code
    for (ds = ds_p; ds-- > drawsegs; )
    {

        // determine if the drawseg obscures the sprite
        if (ds->x1 > spr->x2
            || ds->x2 < spr->x1
            || (!ds->silhouette
                && !ds->maskedtexturecol))
        {
            // does not cover sprite
            continue;
        }

        r1 = ds->x1 < spr->x1 ? spr->x1 : ds->x1;
        r2 = ds->x2 > spr->x2 ? spr->x2 : ds->x2;

        if (ds->scale1 > ds->scale2)
        {
            lowscale = ds->scale2;
            scale = ds->scale1;
        }
        else
        {
            lowscale = ds->scale1;
            scale = ds->scale2;
        }

        if (scale < spr->scale
            || (lowscale < spr->scale
                && !R_PointOnSegSide(spr->gx, spr->gy, ds->curline)))
        {
            // masked mid texture?
            /*if (ds->maskedtexturecol)
                R_RenderMaskedSegRange (ds, r1, r2);*/
                // seg is behind sprite
            continue;
        }

        // clip this piece of the sprite
        silhouette = ds->silhouette;

        if (spr->gz >= ds->bsilheight)
            silhouette &= ~SIL_BOTTOM;

        if (spr->gzt <= ds->tsilheight)
            silhouette &= ~SIL_TOP;

        if (silhouette == 1)
        {
            // bottom sil
            for (x = r1; x <= r2; x++)
                if (clipbot[x] == -2)
                    clipbot[x] = ds->sprbottomclip[x];
        }
        else if (silhouette == 2)
        {
            // top sil
            for (x = r1; x <= r2; x++)
                if (cliptop[x] == -2)
                    cliptop[x] = ds->sprtopclip[x];
        }
        else if (silhouette == 3)
        {
            // both
            for (x = r1; x <= r2; x++)
            {
                if (clipbot[x] == -2)
                    clipbot[x] = ds->sprbottomclip[x];
                if (cliptop[x] == -2)
                    cliptop[x] = ds->sprtopclip[x];
            }
        }
    }
    //SoM: 3/17/2000: Clip sprites in water.
    if (spr->heightsec != -1)  // only things in specially marked sectors
    {
        fixed_t h, mh;
        int phs = viewplayer->mo->subsector->sector->heightsec;
        if ((mh = sectors[spr->heightsec].floorheight) > spr->gz &&
            (h = centeryfrac - FixedMul(mh -= viewz, spr->scale)) >= 0 &&
            (h >>= FRACBITS) < viewheight)
        {
            if (mh <= 0 || (phs != -1 && viewz > sectors[phs].floorheight))
            {                          // clip bottom
                for (x = spr->x1; x <= spr->x2; x++)
                    if (clipbot[x] == -2 || h < clipbot[x])
                        clipbot[x] = h;
            }
            else                        // clip top
            {
                for (x = spr->x1; x <= spr->x2; x++)
                    if (cliptop[x] == -2 || h > cliptop[x])
                        cliptop[x] = h;
            }
        }

        if ((mh = sectors[spr->heightsec].ceilingheight) < spr->gzt &&
            (h = centeryfrac - FixedMul(mh - viewz, spr->scale)) >= 0 &&
            (h >>= FRACBITS) < viewheight)
        {
            if (phs != -1 && viewz >= sectors[phs].ceilingheight)
            {                         // clip bottom
                for (x = spr->x1; x <= spr->x2; x++)
                    if (clipbot[x] == -2 || h < clipbot[x])
                        clipbot[x] = h;
            }
            else                       // clip top
            {
                for (x = spr->x1; x <= spr->x2; x++)
                    if (cliptop[x] == -2 || h > cliptop[x])
                        cliptop[x] = h;
            }
        }
    }
    if (spr->cut & SC_TOP && spr->cut & SC_BOTTOM)
    {
        fixed_t   h;
        for (x = spr->x1; x <= spr->x2; x++)
        {
            h = spr->szt;
            if (cliptop[x] == -2 || h > cliptop[x])
                cliptop[x] = h;

            h = spr->sz;
            if (clipbot[x] == -2 || h < clipbot[x])
                clipbot[x] = h;
        }
    }
    else if (spr->cut & SC_TOP)
    {
        fixed_t   h;
        for (x = spr->x1; x <= spr->x2; x++)
        {
            h = spr->szt;
            if (cliptop[x] == -2 || h > cliptop[x])
                cliptop[x] = h;
        }
    }
    else if (spr->cut & SC_BOTTOM)
    {
        fixed_t   h;
        for (x = spr->x1; x <= spr->x2; x++)
        {
            h = spr->sz;
            if (clipbot[x] == -2 || h < clipbot[x])
                clipbot[x] = h;
        }
    }

    // all clipping has been performed, so draw the sprite

    // check for unclipped columns
    for (x = spr->x1; x <= spr->x2; x++)
    {
        if (clipbot[x] == -2)
            clipbot[x] = viewheight;

        if (cliptop[x] == -2)
            //Fab:26-04-98: was -1, now clips against console bottom
            cliptop[x] = con_clipviewtop;
    }

    mfloorclip = clipbot;
    mceilingclip = cliptop;
    R_DrawVisSprite(spr, spr->x1, spr->x2);
}


//
// R_DrawMasked
//
void R_DrawMasked(void)
{
    drawnode_t* r2;
    drawnode_t* next;

    R_CreateDrawNodes();

    for (r2 = nodehead.next; r2 != &nodehead; r2 = r2->next)
    {
        if (r2->plane)
        {
            next = r2->prev;
            R_DrawSinglePlane(r2->plane, true);
            R_DoneWithNode(r2);
            r2 = next;
        }
        else if (r2->seg && r2->seg->maskedtexturecol != NULL)
        {
            next = r2->prev;
            R_RenderMaskedSegRange(r2->seg, r2->seg->x1, r2->seg->x2);
            r2->seg->maskedtexturecol = NULL;
            R_DoneWithNode(r2);
            r2 = next;
        }
        else if (r2->thickseg)
        {
            next = r2->prev;
            R_RenderThickSideRange(r2->thickseg, r2->thickseg->x1, r2->thickseg->x2, r2->ffloor);
            R_DoneWithNode(r2);
            r2 = next;
        }
        else if (r2->sprite)
        {
            next = r2->prev;
            R_DrawSprite(r2->sprite);
            R_DoneWithNode(r2);
            r2 = next;
        }
    }
    R_ClearDrawNodes();
}





// ==========================================================================
//
//                              SKINS CODE
//
// ==========================================================================

int         numskins = 0;
skin_t      skins[MAXSKINS + 1];
// don't work because it must be inistilised before the config load
//#define SKINVALUES
#ifdef SKINVALUES
CV_PossibleValue_t skin_cons_t[MAXSKINS + 1];
#endif

void Sk_SetDefaultValue(skin_t* skin)
{
    int   i;
    //
    // setup the 'marine' as default skin
    //
    memset(skin, 0, sizeof(skin_t));
    strcpy(skin->name, DEFAULTSKIN);
    strcpy(skin->faceprefix, "STF");
    for (i = 0; i < sfx_freeslot0; i++)
        if (S_sfx[i].skinsound != -1)
        {
            skin->soundsid[S_sfx[i].skinsound] = i;
        }
    memcpy(&skins[0].spritedef, &sprites[SPR_PLAY], sizeof(spritedef_t));
}

//
// Initialize the basic skins
//
void R_InitSkins(void)
{
#ifdef SKINVALUES
    int i;

    for (i = 0; i <= MAXSKINS; i++)
    {
        skin_cons_t[i].value = 0;
        skin_cons_t[i].strvalue = NULL;
    }
#endif

    // initialize free sfx slots for skin sounds
    S_InitRuntimeSounds();

    // skin[0] = marine skin
    Sk_SetDefaultValue(&skins[0]);
#ifdef SKINVALUES
    skin_cons_t[0].strvalue = skins[0].name;
#endif

    // make the standard Doom2 marine as the default skin
    numskins = 1;
}

// returns true if the skin name is found (loaded from pwad)
// warning return 0 (the default skin) if not found
int R_SkinAvailable(char* name)
{
    int  i;

    for (i = 0; i < numskins; i++)
    {
        if (stricmp(skins[i].name, name) == 0)
            return i;
    }
    return 0;
}

// network code calls this when a 'skin change' is received
void SetPlayerSkin(int playernum, char* skinname)
{
    int   i;

    for (i = 0; i < numskins; i++)
    {
        // search in the skin list
        if (stricmp(skins[i].name, skinname) == 0)
        {
            // change the face graphics
            if (playernum == statusbarplayer &&
                // for save time test it there is a real change
                strcmp(skins[players[playernum].skin].faceprefix, skins[i].faceprefix))
            {
                ST_unloadFaceGraphics();
                ST_loadFaceGraphics(skins[i].faceprefix);
            }

            players[playernum].skin = i;
            if (players[playernum].mo)
                players[playernum].mo->skin = &skins[i];

            return;
        }
    }

    CONS_Printf("Skin %s not found\n", skinname);
    players[playernum].skin = 0;  // not found put the old marine skin

    // a copy of the skin value
    // so that dead body detached from respawning player keeps the skin
    if (players[playernum].mo)
        players[playernum].mo->skin = &skins[0];
}

//
// Add skins from a pwad, each skin preceded by 'S_SKIN' marker
//

// Does the same is in w_wad, but check only for
// the first 6 characters (this is so we can have S_SKIN1, S_SKIN2..
// for wad editors that don't like multiple resources of the same name)
//
int W_CheckForSkinMarkerInPwad(int wadid, int startlump)
{
    int         i;
    int         v1;
    lumpinfo_t* lump_p;

    union {
        char    s[4];
        int             x;
    } name4;

    strncpy(name4.s, "S_SK", 4);
    v1 = name4.x;

    // scan forward, start at <startlump>
    if (startlump < wadfiles[wadid]->numlumps)
    {
        lump_p = wadfiles[wadid]->lumpinfo + startlump;
        for (i = startlump; i < wadfiles[wadid]->numlumps; i++, lump_p++)
        {
            if (*(int*)lump_p->name == v1 &&
                lump_p->name[4] == 'I' &&
                lump_p->name[5] == 'N')
            {
                return ((wadid << 16) + i);
            }
        }
    }
    return -1; // not found
}

//
// Find skin sprites, sounds & optional status bar face, & add them
//
void R_AddSkins(int wadnum)
{
    int         lumpnum;
    int         lastlump;

    lumpinfo_t* lumpinfo;
    char* sprname = NULL;
    int         intname;

    char* buf;
    char* buf2;

    char* token;
    char* value;

    int         i, size;

    //
    // search for all skin markers in pwad
    //

    lastlump = 0;
    while ((lumpnum = W_CheckForSkinMarkerInPwad(wadnum, lastlump)) != -1)
    {
        if (numskins > MAXSKINS)
        {
            CONS_Printf("ignored skin (%d skins maximum)\n", MAXSKINS);
            lastlump++;
            continue; //faB:so we know how many skins couldn't be added
        }

        buf = W_CacheLumpNum(lumpnum, PU_CACHE);
        size = W_LumpLength(lumpnum);

        // for strtok
        buf2 = (char*)malloc(size + 1);
        if (!buf2)
            I_Error("R_AddSkins: No more free memory\n");
        memcpy(buf2, buf, size);
        buf2[size] = '\0';

        // set defaults
        Sk_SetDefaultValue(&skins[numskins]);
        sprintf(skins[numskins].name, "skin %d", numskins);

        // parse
        token = strtok(buf2, "\r\n= ");
        while (token)
        {
            if (token[0] == '/' && token[1] == '/') // skip comments
            {
                token = strtok(NULL, "\r\n"); // skip end of line
                goto next_token;               // find the real next token
            }

            value = strtok(NULL, "\r\n= ");
            //            CONS_Printf("token = %s, value = %s",token,value);
            //            CONS_Error("ga");

            if (!value)
                I_Error("R_AddSkins: syntax error in S_SKIN lump# %d in WAD %s\n", lumpnum & 0xFFFF, wadfiles[wadnum]->filename);

            if (!stricmp(token, "name"))
            {
                // the skin name must uniquely identify a single skin
                // I'm lazy so if name is already used I leave the 'skin x'
                // default skin name set above
                if (!R_SkinAvailable(value))
                {
                    strncpy(skins[numskins].name, value, SKINNAMESIZE);
                    strlwr(skins[numskins].name);
                }
            }
            else
                if (!stricmp(token, "face"))
                {
                    strncpy(skins[numskins].faceprefix, value, 3);
                    skins[numskins].faceprefix[3] = 0;
                    strupr(skins[numskins].faceprefix);
                }
                else
                    if (!stricmp(token, "sprite"))
                    {
                        sprname = value;
                        strupr(sprname);
                    }
                    else
                    {
                        int found = false;
                        // copy name of sounds that are remapped for this skin
                        for (i = 0; i < sfx_freeslot0; i++)
                        {
                            if (!S_sfx[i].name)
                                continue;
                            if (S_sfx[i].skinsound != -1 &&
                                !stricmp(S_sfx[i].name, token + 2))
                            {
                                skins[numskins].soundsid[S_sfx[i].skinsound] =
                                    S_AddSoundFx(value + 2, S_sfx[i].singularity);
                                found = true;
                            }
                        }
                        if (!found)
                            I_Error("R_AddSkins: Unknown keyword '%s' in S_SKIN lump# %d (WAD %s)\n", token, lumpnum & 0xFFFF, wadfiles[wadnum]->filename);
                    }
        next_token:
            token = strtok(NULL, "\r\n= ");
        }

        // if no sprite defined use spirte juste after this one
        if (!sprname)
        {
            lumpnum &= 0xFFFF;      // get rid of wad number
            lumpnum++;
            lumpinfo = wadfiles[wadnum]->lumpinfo;

            // get the base name of this skin's sprite (4 chars)
            sprname = lumpinfo[lumpnum].name;
            intname = *(int*)sprname;

            // skip to end of this skin's frames
            lastlump = lumpnum;
            while (*(int*)lumpinfo[lastlump].name == intname)
                lastlump++;
            // allocate (or replace) sprite frames, and set spritedef
            R_AddSingleSpriteDef(sprname, &skins[numskins].spritedef, wadnum, lumpnum, lastlump);
        }
        else
        {
            // search in the normal sprite tables
            char** name;
            boolean found = false;
            for (name = sprnames; *name; name++)
                if (strcmp(*name, sprname) == 0)
                {
                    found = true;
                    skins[numskins].spritedef = sprites[sprnames - name];
                }

            // not found so make a new one
            if (!found)
                R_AddSingleSpriteDef(sprname, &skins[numskins].spritedef, wadnum, 0, MAXINT);
        }

        CONS_Printf("added skin '%s'\n", skins[numskins].name);
#ifdef SKINVALUES
        skin_cons_t[numskins].value = numskins;
        skin_cons_t[numskins].strvalue = skins[numskins].name;
#endif

        numskins++;
        free(buf2);
    }
    return;
}
