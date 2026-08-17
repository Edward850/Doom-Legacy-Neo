// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw3dsdrv.h,v 1.2 2002/01/21 23:27:06 judgecutor Exp $
//
// Copyright (C) 2001 by DooM Legacy Team.
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
// $Log: hw3dsdrv.h,v $
// Revision 1.2  2002/01/21 23:27:06  judgecutor
// Added HW3S_I_StartSound low-level fuction for arbitrary managing of 3D sources
//
// Revision 1.1  2001/04/04 19:42:42  judgecutor
// Initial release of the 3D Sound Support
//
// 
//
// DESCRIPTION:
//      3D sound import/export prototypes for low-level
//      hardware interface
//      
//
//-----------------------------------------------------------------------------

#ifndef __HW_3DS_DRV_H__
#define __HW_3DS_DRV_H__

// Use standart hardware API
#include "hw_drv.h"


typedef struct source_pos_s
{
    double   x;
    double   y;
    double   z;
} source_pos_t;

typedef struct snddev_s
{
    int     sample_rate;
    int     bps;
    
// Windows specific data
#ifdef __WIN32__
    int     cooplevel;
    HWND    hWnd;
#endif
} snddev_t;

typedef struct source3D_pos_s
{
    float   x;
    float   y;
    float   z;
//    float   angle;
    float   momx;
    float   momy;
    float   momz;
    
} source3D_pos_t;


enum {NORMAL_PITCH = 128};

/*typedef struct source2D_data_s
{
    int     volume;
    int     sep;

} source2D_data_t;*/


// General 3D sound source description
typedef struct source3D_data_s
{
    float           min_distance;       // 
    float           max_distance;       //
    int             head_relative;      //
    int             permanent;          //
    source3D_pos_t  pos;                // source position in 3D

} source3D_data_t;


// Sound data
typedef struct sfx_data_s
{
    int     length;
    void    *data;
    int     priority;
    int     id;                     // Doom sfx id
    int     pitch;
    int     volume;
    int     sep;                    // Only when source is 2D sound
} sfx_data_t;


// Sound cone (for 3D sources)
typedef struct cone_def_s
{
    float   inner;
    float   outer;
    int     outer_gain;
    /*float   f_angle;
    float   h_angle;*/
} cone_def_t;


typedef struct listener_data_s
{
    // Listener position
    double  x;
    double  y;
    double  z;

    // Listener front and head orientation (degrees)
    double  f_angle;
    double  h_angle;

    // Listener momentums
    double  momx;
    double  momy;
    double  momz;
} listener_data_t;


// Use standart Init and Shutdown functions

EXPORT BOOL HWRAPI (Startup) (I_Error_t FatalErrorFunction, snddev_t *snd_dev);
EXPORT int  HWRAPI (Add3DSource )(source3D_data_t *src, sfx_data_t *sfx);
EXPORT int  HWRAPI (Add2DSource) (sfx_data_t *sfx);
EXPORT int  HWRAPI (StartSource) (int handle);
EXPORT void HWRAPI (StopSource) (int handle);
EXPORT int  HWRAPI (GetHW3DSVersion) ( void );
EXPORT void HWRAPI (BeginFrameUpdate) (void);
EXPORT void HWRAPI (EndFrameUpdate) (void);
EXPORT int  HWRAPI (IsPlaying) (int handle);
EXPORT void HWRAPI (UpdateListener) (listener_data_t *data);
EXPORT void HWRAPI (UpdateSourceVolume) (int handle, int volume);
EXPORT void HWRAPI (Update2DSource) (int handle, int vol, int sep);
EXPORT void HWRAPI (SetGlobalSfxVolume) (int volume);
EXPORT int  HWRAPI (SetCone) (int handle, cone_def_t *cone_def);
EXPORT void HWRAPI (Update3DSource) (int handle, source3D_pos_t *data);
//EXPORT int  HWRAPI (StartSound) (int handle);
EXPORT int  HWRAPI (Reload3DSource) (int handle, sfx_data_t *data);
EXPORT void HWRAPI (KillSource) (int handle);
//EXPORT void HWRAPI (GetHW3DSTitle) (char *buf, int size);


#if !defined(_CREATE_DLL_)

struct hardware3ds_s
{
    Startup             pfnStartup;
    Shutdown            pfnShutdown;
    Add3DSource         pfnAdd3DSource;
    Add2DSource         pfnAdd2DSource;
    StopSource          pfnStopSource;
    StartSource         pfnStartSource;
    GetHW3DSVersion     pfnGetHW3DSVersion;
    BeginFrameUpdate    pfnBeginFrameUpdate;
    EndFrameUpdate      pfnEndFrameUpdate;
    IsPlaying           pfnIsPlaying;
    UpdateListener      pfnUpdateListener;
    SetGlobalSfxVolume  pfnSetGlobalSfxVolume;
    SetCone             pfnSetCone;
    Update2DSource      pfnUpdate2DSoundParms;
    Update3DSource      pfnUpdate3DSource;
    UpdateSourceVolume  pfnUpdateSourceVolume;
//    StartSound          pfnStartSound;
    Reload3DSource      pfnReload3DSource;
    KillSource          pfnKillSource;
};

extern struct hardware3ds_s hw3ds_driver;

#define HW3DS hw3ds_driver


#endif  // _CREATE_DLL_

#endif // __HW_3DS_DRV_H__