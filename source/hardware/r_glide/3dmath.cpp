// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: 3dmath.c,v 1.4 2001/08/19 15:40:07 bpereira Exp $
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
// $Log: 3dmath.c,v $
// Revision 1.4  2001/08/19 15:40:07  bpereira
// added Treansform (and lighting) to glide
//
// Revision 1.3  2000/03/29 19:39:49  bpereira
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
//      
//
//-----------------------------------------------------------------------------


#include "../hw_drv.h"
#include "3dmath.h"

// ==========================================================================
//                                                   CLIPPING TO NEAR Z PLANE
// ==========================================================================
#define ZCLIP_PLANE     4.0f
int ClipZ (FOutVector* inVerts, FOutVector* clipVerts, int numpoints)
{
    int     nrClipVerts = 0;
    int     a;
    byte    curin,nextin;
    int     nextvert;
    float   curdot, nextdot;
    FOutVector *pinvert, *poutvert;
    float   scale;

    nrClipVerts = 0;
    pinvert = inVerts;
    poutvert = clipVerts;

    curdot = pinvert->z;
    curin = (curdot >= ZCLIP_PLANE );

    for(a=0; a<numpoints; a++)
    {
        nextvert = a + 1;
        if (nextvert == numpoints)
            nextvert = 0;

        if (curin) {
            *poutvert++ = *pinvert;
            nrClipVerts++;
        }

        nextdot = inVerts[nextvert].z;
        nextin = (nextdot >= ZCLIP_PLANE );

        if ( curin != nextin )
        {
            if (curdot >= nextdot)
            {
                scale = (float)(ZCLIP_PLANE - curdot)/(nextdot - curdot );
                poutvert->x = pinvert->x + scale * (inVerts[nextvert].x - pinvert->x );
                poutvert->y = pinvert->y + scale * (inVerts[nextvert].y - pinvert->y );
                poutvert->z = pinvert->z + scale * (inVerts[nextvert].z - pinvert->z );

                poutvert->sow = pinvert->sow + scale * (inVerts[nextvert].sow - pinvert->sow );
                poutvert->tow = pinvert->tow + scale * (inVerts[nextvert].tow - pinvert->tow );
            }
            else
            {
                scale = (float)(ZCLIP_PLANE - nextdot)/(curdot - nextdot );
                poutvert->x = inVerts[nextvert].x + scale * (pinvert->x - inVerts[nextvert].x );
                poutvert->y = inVerts[nextvert].y + scale * (pinvert->y - inVerts[nextvert].y );
                poutvert->z = inVerts[nextvert].z + scale * (pinvert->z - inVerts[nextvert].z );

                poutvert->sow = inVerts[nextvert].sow + scale * (pinvert->sow - inVerts[nextvert].sow );
                poutvert->tow = inVerts[nextvert].tow + scale * (pinvert->tow - inVerts[nextvert].tow );
            }
            poutvert++;
            nrClipVerts++;
        }

        curdot = nextdot;
        curin = nextin;
        pinvert++;
    }

    return nrClipVerts;
}


// ==========================================================================
//                                                   3D VIEW FRUSTUM CLIPPING
// ==========================================================================



// faB: I used the 'Liang-Barsky' clipping algorithm, simply because that's
//      the one I found into a book called 'Procedural Elements for Computer
//      Graphics'.

// --------------------------------------------------------------------------
// ClipT performs trivial rejection tests and finds the maximum of the lower
// set of parameter values and the minimum of the upper set of parameter
// values
// --------------------------------------------------------------------------
static __inline boolean ClipT (float d, float q, float *tl, float *tu)
{
    boolean visible;
    float   t;

    visible = true;

    if ( (d == 0) && (q < 0) )       // line is outside and parallel to edge
        visible = false;
    else if ( d < 0 )                   // looking for upper limit
    {
        t = q / d;
        if ( t > *tu )                   // check for trivially invisible
            visible = false;
        else if ( t > *tl )              // find the minimum of the maximum
            *tl = t;
    }
    else if ( d > 0 )                   // looking for the lower limit
    {
        t = q / d;
        if ( t < *tl )                   // check for trivially invisible
            visible = false;
        else if ( t < *tu )              // find the maximum of the minimum
            *tu = t;
    }

    return visible;
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// WARNIIIIIIIIIIIIIIIIING!!!!
//             used by planes too, so make sure its large enough,
//              see 'planeVerts'
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!dirty code!!!!!!!!!!!!!!!!!!
static  FOutVector tempVerts[MAXCLIPVERTS*2];

// Liang-Barsky three-dimensional clipping algorithm,
//  assumes a 90degree left to right, and top to bottom fov

// --------------------------------------------------------------------------
// In:
//   nrInVerts: nr of coordinates passed into inVerts[]
//   inVerts :  3D coordinates of polygon, in order, to clip
// Out:
//   outVerts : 3D coordinates of CLIPPED polygon, in order
// Returns:
//   nr of coordinates returned into outVerts[]
// --------------------------------------------------------------------------

// FIXME: this is a shame, had to do left/right, top/bottom clipping in two
// passes, or else we have a problem clipping to corners of the frustum, in
// some cases a point is not generated.. until then..

int ClipToFrustum (FOutVector *inVerts, FOutVector *outVerts, int nrInVerts)
{
    float       tl,tu;
    float       deltax, deltay, deltaz;

    int         i;
    int         xClipVerts, nrClipVerts;
    FOutVector  *pinvert;
    FOutVector  *poutvert;
    FOutVector  *nextvert;


    // first pass left/right clipping
    xClipVerts = 0;
    pinvert = inVerts;
    poutvert = tempVerts;

    for (i=0; i<nrInVerts; i++)
    {
        nextvert = &inVerts[ i + 1 ];
        if (nextvert == &inVerts[nrInVerts])
            nextvert = inVerts;

        tl = 0; tu = 1;
        deltax = nextvert->x - pinvert->x;
        deltaz = nextvert->z - pinvert->z;
        if ( ClipT( -deltax - deltaz, pinvert->x + pinvert->z, &tl, &tu ) ) {
            if ( ClipT( deltax - deltaz, pinvert->z - pinvert->x, &tl, &tu ) ) {
                deltay = nextvert->y - pinvert->y;
                // clipped start point
                if ( tl > 0 ) {

                    poutvert->x = pinvert->x + tl * deltax;
                    poutvert->y = pinvert->y + tl * deltay;
                    poutvert->z = pinvert->z + tl * deltaz;

                    poutvert->sow = pinvert->sow + tl * (nextvert->sow - pinvert->sow );
                    poutvert->tow = pinvert->tow + tl * (nextvert->tow - pinvert->tow );
                }
                else {
                    // copy as it is
                    *poutvert = *pinvert;
                }
                poutvert++;
                xClipVerts++;

                // clipped end point
                if ( tu < 1 ) {
                    poutvert->x = pinvert->x + tu * deltax;
                    poutvert->y = pinvert->y + tu * deltay;
                    poutvert->z = pinvert->z + tu * deltaz;

                    poutvert->sow = pinvert->sow + tu * (nextvert->sow - pinvert->sow );
                    poutvert->tow = pinvert->tow + tu * (nextvert->tow - pinvert->tow );

                    poutvert++;
                    xClipVerts++;
                }
            }
        }
        pinvert++;
    }

    // SECOND pass top/bottom clipping
    pinvert = tempVerts;
    poutvert = outVerts;
    nrClipVerts = 0;

    for (i=0; i<xClipVerts; i++)
    {
        nextvert = &tempVerts[i+1];
        if (nextvert == &tempVerts[xClipVerts])
            nextvert = tempVerts;

        tl = 0; tu = 1;
        deltaz = nextvert->z - pinvert->z;
        deltay = nextvert->y - pinvert->y;
        if ( ClipT( -deltay - deltaz, pinvert->y + pinvert->z, &tl, &tu ) ) {
            if ( ClipT( deltay - deltaz, pinvert->z - pinvert->y, &tl, &tu ) ) {
                deltax = nextvert->x - pinvert->x;
                // clipped start point
                if ( tl > 0 ) {
                    poutvert->x = pinvert->x + tl * deltax;
                    poutvert->y = pinvert->y + tl * deltay;
                    poutvert->z = pinvert->z + tl * deltaz;

                    poutvert->sow = pinvert->sow + tl * (nextvert->sow - pinvert->sow );
                    poutvert->tow = pinvert->tow + tl * (nextvert->tow - pinvert->tow );
                }
                else {
                    // copy as it is
                    *poutvert = *pinvert;
                }
                poutvert++;
                nrClipVerts++;

                // clipped end point
                if ( tu < 1 ) {
                    poutvert->x = pinvert->x + tu * deltax;
                    poutvert->y = pinvert->y + tu * deltay;
                    poutvert->z = pinvert->z + tu * deltaz;

                    poutvert->sow = pinvert->sow + tu * (nextvert->sow - pinvert->sow );
                    poutvert->tow = pinvert->tow + tu * (nextvert->tow - pinvert->tow );

                    poutvert++;
                    nrClipVerts++;
                }
            }
        }
        pinvert++;
    }

    return nrClipVerts;
}
