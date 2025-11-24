// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw_md2.h,v 1.5 2001/08/07 00:44:05 hurdler Exp $
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
// $Log: hw_md2.h,v $
// Revision 1.5  2001/08/07 00:44:05  hurdler
// MD2 implementation is getting better but still need lots of work
//
// Revision 1.4  2001/08/06 14:13:46  hurdler
// Crappy MD2 implementation (still need lots of work)
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
//      Inspired from md2.h by Mete Ciragan (mete@swissquake.ch)
//
//-----------------------------------------------------------------------------


#ifndef _HW_MD2_H_
#define _HW_MD2_H_

#include "hw_glob.h"

#define MD2_MAX_TRIANGLES               4096
#define MD2_MAX_VERTICES                2048
#define MD2_MAX_TEXCOORDS               2048
#define MD2_MAX_FRAMES                  512
#define MD2_MAX_SKINS                   32
#define MD2_MAX_FRAMESIZE               (MD2_MAX_VERTICES * 4 + 128)

typedef struct 
{ 
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
} md2_header_t;

typedef struct
{
   unsigned char  vertex[3];
   unsigned char  lightNormalIndex;
} md2_alias_triangleVertex_t;

typedef struct
{
   float vertex[3];
   float normal[3];
} md2_triangleVertex_t;

typedef struct
{
   short vertexIndices[3];
   short textureIndices[3];
} md2_triangle_t;

typedef struct
{
   short s, t;
} md2_textureCoordinate_t;

typedef struct
{
   float scale[3];
   float translate[3];
   char  name[16];
   md2_alias_triangleVertex_t alias_vertices[1];
} md2_alias_frame_t;

typedef struct
{
   char name[16];
   md2_triangleVertex_t *vertices;
} md2_frame_t;

typedef char md2_skin_t[64];

typedef struct
{
   float s, t;
   int   vertexIndex;
} md2_glCommandVertex_t;

typedef struct
{
    md2_header_t            header;
    md2_skin_t              *skins;
    md2_textureCoordinate_t *texCoords;
    md2_triangle_t          *triangles;
    md2_frame_t             *frames;
    int                     *glCommandBuffer;
} md2_model_t;

typedef struct
{
    char        filename[32];
    float       scale;
    float       offset;
    md2_model_t *model;
    int         texture;
} md2_t;

extern md2_t md2_models[NUMSPRITES];

md2_model_t *md2_readModel (const char *filename);
void md2_freeModel (md2_model_t *model);
void md2_getBoundingBox (md2_model_t *model, float *minmax);
int  md2_getAnimationCount (md2_model_t *model);
const char *md2_getAnimationName (md2_model_t *model, int animation);
void md2_getAnimationFrames (md2_model_t *model, int animation, int *startFrame, int *endFrame);
void md2_printModelInfo (md2_model_t *model);
int  md2_loadTexture (const char *filename);
void HWR_InitMD2();
void HWR_DrawMD2(gr_vissprite_t* spr);


#endif // _HW_MD2_H_
