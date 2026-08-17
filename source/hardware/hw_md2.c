// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw_md2.c,v 1.10 2002/07/26 15:22:45 hurdler Exp $
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
// $Log: hw_md2.c,v $
// Revision 1.10  2002/07/26 15:22:45  hurdler
// near RC release
//
// Revision 1.9  2002/06/30 21:37:48  hurdler
// Ready for 1.32 beta 5 release
//
// Revision 1.8  2001/12/31 13:47:46  hurdler
// Add setcorona FS command and prepare the code for beta 4
//
// Revision 1.7  2001/08/08 20:34:43  hurdler
// Big TANDL update
//
// Revision 1.6  2001/08/07 00:44:05  hurdler
// MD2 implementation is getting better but still need lots of work
//
// Revision 1.5  2001/08/06 14:13:46  hurdler
// Crappy MD2 implementation (still need lots of work)
//
// Revision 1.4  2000/10/04 16:21:57  hurdler
// small clean-up
//
// Revision 1.3  2000/03/29 20:17:31  hurdler
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      MD2 Handling
//      Inspired from md2.c by Mete Ciragan (mete@swissquake.ch)
//
//-----------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hw_drv.h"
#include "hw_light.h"
#include "../doomdef.h"
#include "../r_main.h"
#include "../w_wad.h"
#include "../z_zone.h"


md2_t md2_models[NUMSPRITES];

//
// load model
//
// Hurdler: the current path is the Legacy.exe path
md2_model_t* md2_readModel (const char *filename)
{
    FILE        *file;
    md2_model_t *model;
    byte        buffer[MD2_MAX_FRAMESIZE];
    int         i;

    model = (md2_model_t *) malloc (sizeof (md2_model_t));
    if (!model)
        return 0;

    file = fopen (filename, "rb");
    if (!file)
    {
        free (model);
        return 0;
    }

    // initialize model and read header 
    memset (model, 0, sizeof (md2_model_t));
    fread (&model->header, sizeof (md2_header_t), 1, file);

    if (model->header.magic != (int) (('2' << 24) + ('P' << 16) + ('D' << 8) + 'I'))
    {
        fclose (file);
        free (model);
        return 0;
    }

    // read skins
    fseek (file, model->header.offsetSkins, SEEK_SET);
    if (model->header.numSkins > 0)
    {
        model->skins = (md2_skin_t *) malloc (sizeof (md2_skin_t) * model->header.numSkins);
        if (!model->skins)
        {
            md2_freeModel (model);
            return 0;
        }

        for (i = 0; i < model->header.numSkins; i++)
            fread (&model->skins[i], sizeof (md2_skin_t), 1, file);
    }

    // read texture coordinates 
    fseek (file, model->header.offsetTexCoords, SEEK_SET);
    if (model->header.numTexCoords > 0)
    {
        model->texCoords = (md2_textureCoordinate_t *) malloc (sizeof (md2_textureCoordinate_t) * model->header.numTexCoords);
        if (!model->texCoords)
        {
            md2_freeModel (model);
            return 0;
        }

        for (i = 0; i < model->header.numTexCoords; i++)
            fread (&model->texCoords[i], sizeof (md2_textureCoordinate_t), 1, file);
    }

    // read triangles 
    fseek (file, model->header.offsetTriangles, SEEK_SET);
    if (model->header.numTriangles > 0)
    {
        model->triangles = (md2_triangle_t *) malloc (sizeof (md2_triangle_t) * model->header.numTriangles);
        if (!model->triangles)
        {
            md2_freeModel (model);
            return 0;
        }

        for (i = 0; i < model->header.numTriangles; i++)
            fread (&model->triangles[i], sizeof (md2_triangle_t), 1, file);
    }

    // read alias frames 
    fseek (file, model->header.offsetFrames, SEEK_SET);
    if (model->header.numFrames > 0)
    {
        model->frames = (md2_frame_t *) malloc (sizeof (md2_frame_t) * model->header.numFrames);
        if (!model->frames)
        {
            md2_freeModel (model);
            return 0;
        }

        for (i = 0; i < model->header.numFrames; i++)
        {
            md2_alias_frame_t *frame = (md2_alias_frame_t *) buffer;
            int j;

            model->frames[i].vertices = (md2_triangleVertex_t *) malloc (sizeof (md2_triangleVertex_t) * model->header.numVertices);
            if (!model->frames[i].vertices)
            {
                md2_freeModel (model);
                return 0;
            }

            fread (frame, 1, model->header.frameSize, file);
            strcpy (model->frames[i].name, frame->name);
            for (j = 0; j < model->header.numVertices; j++)
            {
                model->frames[i].vertices[j].vertex[0] = (float) ((int) frame->alias_vertices[j].vertex[0]) * frame->scale[0] + frame->translate[0];
                model->frames[i].vertices[j].vertex[2] = -1* ((float) ((int) frame->alias_vertices[j].vertex[1]) * frame->scale[1] + frame->translate[1]);
                model->frames[i].vertices[j].vertex[1] = (float) ((int) frame->alias_vertices[j].vertex[2]) * frame->scale[2] + frame->translate[2];
            }
        }
    }

    // read gl commands 
    fseek (file, model->header.offsetGlCommands, SEEK_SET);
    if (model->header.numGlCommands)
    {
        model->glCommandBuffer = (int *) malloc (sizeof (int) * model->header.numGlCommands);
        if (!model->glCommandBuffer)
        {
            md2_freeModel (model);
            return 0;
        }

        fread (model->glCommandBuffer, sizeof (int), model->header.numGlCommands, file);
    }

    fclose (file);

    return model;
}



/*
 * free model
 */
void md2_freeModel (md2_model_t *model)
{
    if (model)
    {
        if (model->skins)
            free (model->skins);

        if (model->texCoords)
            free (model->texCoords);

        if (model->triangles)
            free (model->triangles);

        if (model->frames)
        {
            int i;

            for (i = 0; i < model->header.numFrames; i++)
            {
                if (model->frames[i].vertices)
                    free (model->frames[i].vertices);
            }
            free (model->frames);
        }

        if (model->glCommandBuffer)
            free (model->glCommandBuffer);

        free (model);
    }
}


/*
 * center model
 */
void md2_getBoundingBox (md2_model_t *model, float *minmax)
{
    int i;
    float minx, maxx;
    float miny, maxy;
    float minz, maxz;

    minx = miny = minz = 999999.0f;
    maxx = maxy = maxz = -999999.0f;

    /* get bounding box */
    for (i = 0; i < model->header.numVertices; i++)
    {
        md2_triangleVertex_t *v = &model->frames[0].vertices[i];

        if (v->vertex[0] < minx)
            minx = v->vertex[0];
        else if (v->vertex[0] > maxx)
            maxx = v->vertex[0];

        if (v->vertex[1] < miny)
            miny = v->vertex[1];
        else if (v->vertex[1] > maxy)
            maxy = v->vertex[1];

        if (v->vertex[2] < minz)
            minz = v->vertex[2];
        else if (v->vertex[2] > maxz)
            maxz = v->vertex[2];
    }

    minmax[0] = minx;
    minmax[1] = maxx;
    minmax[2] = miny;
    minmax[3] = maxy;
    minmax[4] = minz;
    minmax[5] = maxz;
}


int md2_getAnimationCount (md2_model_t *model)
{
    int i, j, pos;
    int count;
    int lastId;
    char name[16], last[16];

    strcpy (last, model->frames[0].name);
    pos = strlen (last) - 1;
    j = 0;
    while (last[pos] >= '0' && last[pos] <= '9' && j < 2)
    {
        pos--;
        j++;
    }
    last[pos + 1] = '\0';

    lastId = 0;
    count = 0;

    for (i = 0; i <= model->header.numFrames; i++)
    {
        if (i == model->header.numFrames)
            strcpy (name, ""); // some kind of a sentinel
        else
            strcpy (name, model->frames[i].name);
        pos = strlen (name) - 1;
        j = 0;
        while (name[pos] >= '0' && name[pos] <= '9' && j < 2)
        {
            pos--;
            j++;
        }
        name[pos + 1] = '\0';

        if (strcmp (last, name))
        {
            strcpy (last, name);
            count++;
        }
    }

    return count;
}



const char * md2_getAnimationName (md2_model_t *model, int animation)
{
    int i, j, pos;
    int count;
    int lastId;
    static char last[32];
    char name[32];

    strcpy (last, model->frames[0].name);
    pos = strlen (last) - 1;
    j = 0;
    while (last[pos] >= '0' && last[pos] <= '9' && j < 2)
    {
        pos--;
        j++;
    }
    last[pos + 1] = '\0';

    lastId = 0;
    count = 0;

    for (i = 0; i <= model->header.numFrames; i++)
    {
        if (i == model->header.numFrames)
            strcpy (name, ""); // some kind of a sentinel
        else
            strcpy (name, model->frames[i].name);
        pos = strlen (name) - 1;
        j = 0;
        while (name[pos] >= '0' && name[pos] <= '9' && j < 2)
        {
            pos--;
            j++;
        }
        name[pos + 1] = '\0';

        if (strcmp (last, name))
        {
            if (count == animation)
                return last;

            strcpy (last, name);
            count++;
        }
    }

    return 0;
}


void md2_getAnimationFrames (md2_model_t *model, int animation, int *startFrame, int *endFrame)
{
    int i, j, pos;
    int count, numFrames, frameCount;
    int lastId;
    char name[16], last[16];

    strcpy (last, model->frames[0].name);
    pos = strlen (last) - 1;
    j = 0;
    while (last[pos] >= '0' && last[pos] <= '9' && j < 2)
    {
        pos--;
        j++;
    }
    last[pos + 1] = '\0';

    lastId = 0;
    count = 0;
    numFrames = 0;
    frameCount = 0;

    for (i = 0; i <= model->header.numFrames; i++)
    {
        if (i == model->header.numFrames)
            strcpy (name, ""); // some kind of a sentinel
        else
            strcpy (name, model->frames[i].name);
        pos = strlen (name) - 1;
        j = 0;
        while (name[pos] >= '0' && name[pos] <= '9' && j < 2)
        {
            pos--;
            j++;
        }
        name[pos + 1] = '\0';

        if (strcmp (last, name))
        {
            strcpy (last, name);

            if (count == animation)
            {
                *startFrame = frameCount - numFrames;
                *endFrame = frameCount - 1;
                return;
            }

            count++;
            numFrames = 0;
        }
        frameCount++;
        numFrames++;
    }
    *startFrame = *endFrame = 0;
}


void md2_printModelInfo (md2_model_t *model)
{
    int i;

    CONS_Printf ("magic:\t\t\t%c%c%c%c\n", model->header.magic>>24, 
                                          (model->header.magic>>16)&0xff, 
                                          (model->header.magic>>8)&0xff, 
                                           model->header.magic&0xff);
    CONS_Printf ("version:\t\t%d\n", model->header.version);
    CONS_Printf ("skinWidth:\t\t%d\n", model->header.skinWidth);
    CONS_Printf ("skinHeight:\t\t%d\n", model->header.skinHeight);
    CONS_Printf ("frameSize:\t\t%d\n", model->header.frameSize);
    CONS_Printf ("numSkins:\t\t%d\n", model->header.numSkins);
    CONS_Printf ("numVertices:\t\t%d\n", model->header.numVertices);
    CONS_Printf ("numTexCoords:\t\t%d\n", model->header.numTexCoords);
    CONS_Printf ("numTriangles:\t\t%d\n", model->header.numTriangles);
    CONS_Printf ("numGlCommands:\t\t%d\n", model->header.numGlCommands);
    CONS_Printf ("numFrames:\t\t%d\n", model->header.numFrames);
    CONS_Printf ("offsetSkins:\t\t%d\n", model->header.offsetSkins);
    CONS_Printf ("offsetTexCoords:\t%d\n", model->header.offsetTexCoords);
    CONS_Printf ("offsetTriangles:\t%d\n", model->header.offsetTriangles);
    CONS_Printf ("offsetFrames:\t\t%d\n", model->header.offsetFrames);
    CONS_Printf ("offsetGlCommands:\t%d\n", model->header.offsetGlCommands);
    CONS_Printf ("offsetEnd:\t\t%d\n", model->header.offsetEnd);

    for (i = 0; i < model->header.numFrames; i++)
        CONS_Printf ("%s ", model->frames[i].name);
    CONS_Printf ("\n");
}


#define word short
typedef struct
{
    byte manufacturer;   
    byte version;        
    byte encoding;    
    byte bitsPerPixel;
    word xmin;
    word ymin;
    word xmax;
    word ymax;
    word hDpi;
    word vDpi;
    byte colorMap[48];
    byte reserved;    
    byte numPlanes;   
    word bytesPerLine;
    word paletteInfo; 
    word hScreenSize;  
    word vScreenSize;
    byte filler[54];  
} PcxHeader;

GlidePatch_t md2_tex_patch;

// -----------------+
// md2_loadTexture  : Download a pcx texture for MD2 models
// -----------------+
int md2_loadTexture (const char *filename)
{    
    GlidePatch_t    *grpatch;

    grpatch = &md2_tex_patch;
    //if (!grpatch->mipmap.downloaded && !grpatch->mipmap.grInfo.data)
    {
        PcxHeader       header;
        unsigned char   palette[768];
        unsigned char   *image;
        int             w, h;
        int             ptr = 0;
        int             ch, rep;
        int             size;
        FILE            *file; 

        if (!(file = fopen (filename, "rb")))
            return 0;

        if (fread (&header, sizeof (PcxHeader), 1, file) == -1)
        {
            fclose (file);
            return 0;
        }
        fseek (file, -768, SEEK_END);

        w = header.xmax - header.xmin + 1;
        h = header.ymax - header.ymin + 1;
        image= Z_Malloc( w*h, PU_HWRCACHE, &grpatch->mipmap.grInfo.data );

        if (fread ((byte *) palette, sizeof (byte), 768, file) == -1)
        {
            fclose (file);
            return 0;
        }
        fseek(file, sizeof (PcxHeader), SEEK_SET);

        size = w * h;
        while (ptr < size)
        {
            ch = fgetc(file);  //Hurdler: beurk
            if (ch >= 192)
            {
                rep = ch - 192;
                ch = fgetc(file);
            }
            else {
                rep = 1;
            }
            while (rep--)
                image[ptr++] = ch;
        }
        fclose(file);

        grpatch->mipmap.downloaded = 0;
        grpatch->mipmap.grInfo.format = GR_TEXFMT_P_8; 
        grpatch->mipmap.flags = 0;

        grpatch->width = w;
        grpatch->height = h;
        grpatch->mipmap.width = w;
        grpatch->mipmap.height = h;

        // not correct!
        grpatch->mipmap.grInfo.smallLodLog2 = GR_LOD_LOG2_256;
        grpatch->mipmap.grInfo.largeLodLog2 = GR_LOD_LOG2_256;
        grpatch->mipmap.grInfo.aspectRatioLog2 = GR_ASPECT_LOG2_1x1;
    }
    HWD.pfnSetTexture( &grpatch->mipmap );
    return grpatch->mipmap.downloaded;
}

void HWR_InitMD2()
{
    int     i;
    FILE    *f;
    char    name[5], filename[32];
    float   scale, offset;

    CONS_Printf("InitMD2()...\n");
    for (i=0; i<NUMSPRITES; i++)
    {
        md2_models[i].scale = -1.0f;
        md2_models[i].model = NULL;
        md2_models[i].texture = 0;
    }
    // read the md2.dat file

    if (!(f = fopen("md2.dat", "rt")))
    {
        CONS_Printf("Error while loading md2.dat\n");
        return;
    }
    while ( (i=fscanf(f, "%s %s %f %f", name, filename, &scale, &offset)) == 4)
    {
        for (i=0; i<NUMSPRITES; i++)
        {
            if (!strcmp(name, sprnames[i]))
            {
                CONS_Printf("  Found: %s %s %f %f\n", name, filename, scale, offset);
                md2_models[i].scale = scale;
                md2_models[i].offset = offset;
                strcpy(md2_models[i].filename, filename);
                break;
            }
        }
        if (i==NUMSPRITES)
        {
            CONS_Printf("    Not found: %s\n", name);
        }
    }
    fclose(f);
}



// -----------------+
// HWR_DrawMD2      : Draw MD2
//                  : (monsters, bonuses, weapons, lights, ...)
// Returns          :
// -----------------+
    /*
    wait/stand
    death
    pain
    walk
    shoot/fire

    die?
    atka?
    atkb?
    attacka/b/c/d?
    res?
    run?
    */
void HWR_DrawMD2( gr_vissprite_t* spr )
{
    FOutVector      wallVerts[4];
    GlidePatch_t    *gpatch;      //sprite patch converted to hardware
    FSurfaceInfo    Surf;

    char            *ptr;
    char            filename[64];
    int             frame;
    FTransform      p;
    md2_t           *md2;

    // cache sprite graphics
    //12/12/99: Hurdler:
    //          OK, I don't change anything for MD2 support because I want to be
    //          sure to do it the right way. So actually, we keep normal sprite
    //          in memory and we add the md2 model if it exists for that sprite

    // convert srpite differently when fxtranslucent is detected
    if( (spr->mobj->frame & FF_TRANSMASK) == tr_transfx1<<FF_TRANSSHIFT)
    {
        firetranslucent = true;
        gpatch = W_CachePatchNum (spr->patchlumpnum, PU_CACHE );
        firetranslucent = false;
    }
    else
        gpatch = W_CachePatchNum (spr->patchlumpnum, PU_CACHE );    

    //TODO: manage spr->flip

    //12/12/99: Hurdler: same comment as above (for md2)
    HWR_GetMappedPatch(gpatch, spr->colormap);

    // sprite (TODO: coloured-) lighting by modulating the RGB components
    Surf.FlatColor.s.red = Surf.FlatColor.s.green = Surf.FlatColor.s.blue = spr->sectorlight;

    // Look at HWR_ProjetctSprite for more
    if (cv_grmd2.value && (md2_models[spr->mobj->sprite].scale > 0))
    {
        int blend=0;
        if( spr->mobj->frame & FF_TRANSMASK )
            blend = HWR_TranstableToAlpha((spr->mobj->frame & FF_TRANSMASK)>>FF_TRANSSHIFT, &Surf);
        else
        if( spr->mobj->frame & FF_SMOKESHADE )
        {
            Surf.FlatColor.s.alpha = 0x80;blend = PF_Translucent;
        }
        else if (spr->mobj->flags & MF_SHADOW)
        {
            Surf.FlatColor.s.alpha = 0x40;blend = PF_Translucent;
        }
        else
        {
            Surf.FlatColor.s.alpha = 0xFF;blend = PF_Translucent|PF_Occlude;
        }
        // hack for updating the light level before drawing the md2
        HWD.pfnDrawPolygon( &Surf, wallVerts, 4, blend|PF_Modulated|PF_Clip|PF_MD2 );

        // dont forget to enabled the depth test because we can't do this like
        // before: polygons models are not sorted

        // 1. load model+texture if not already loaded
        // 2. draw model with correct position, rotation,...
        md2 = &md2_models[spr->mobj->sprite];
        if (!md2->model)
        {
            CONS_Printf ("Loading MD2... (%s)", sprnames[spr->mobj->sprite]);
            sprintf(filename, "md2/%s", md2->filename);
            if ((md2->model = md2_readModel(filename)))
            {
                int i;

                CONS_Printf (" OK\n");
                //Hurdler: Actually load only the first skin (index 0)
                //         Also, it suppose the texture is in a PCX file
                // This should be put in md2_loadTexture()
                strcpy(filename, "md2/");
                for (i=4, ptr=md2->model->skins[0]; (*ptr) != '\0'; i++, ptr++)
                {
                    if ((*ptr == '\\') || (*ptr == '/'))
                        i = 3;
                    else
                        filename[i] = *ptr;
                }
                filename[i] = '\0';
                md2->texture = md2_loadTexture( filename );
                //md2_printModelInfo (md2->model);
            }
            else
            {
                CONS_Printf (" FAILED\n");
                return;
            }
        }
        //Hurdler: arf, I don't like that implementation at all... too much crappy
        md2_tex_patch.mipmap.downloaded = md2->texture;
        HWD.pfnSetTexture( &md2_tex_patch.mipmap );

        //FIXME: this is not yet correct
        frame = spr->mobj->frame % md2->model->header.numFrames;

        //Hurdler: it seems there is still a small problem with mobj angle
        p.x = spr->mobj->x*crapmul;
        p.y = spr->mobj->y*crapmul+md2->offset;
        p.z = spr->mobj->z*crapmul;
        p.angley = 45*((spr->mobj->angle>>29));
        p.anglex = 0.0f;

        HWD.pfnDrawMD2(md2->model->glCommandBuffer, &md2->model->frames[frame], &p, md2->scale);
    }
}

