// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_spec.h,v 1.3 2003/06/11 04:22:36 ssntails Exp $
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
// $Log: p_spec.h,v $
// Revision 1.3  2003/06/11 04:22:36  ssntails
// Vertical wind
//
// Revision 1.2  2002/06/30 13:58:38  ssntails
// Added vertical water currents.
//
// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_spec.h,v 1.3 2003/06/11 04:22:36 ssntails Exp $
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
// Revision 1.1  2001/04/17 22:23:38  calumr
// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_spec.h,v 1.3 2003/06/11 04:22:36 ssntails Exp $
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
// Initial add
// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_spec.h,v 1.3 2003/06/11 04:22:36 ssntails Exp $
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
//
// Revision 1.11  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.10  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.9  2000/11/11 13:59:46  bpereira
// no message
//
// Revision 1.8  2000/11/02 17:50:08  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.7  2000/10/21 08:43:30  bpereira
// no message
//
// Revision 1.6  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.5  2000/04/07 18:48:56  hurdler
// AnyKey doesn't seem to compile under Linux, now renamed to AnyKey_
//
// Revision 1.4  2000/04/06 20:54:28  hurdler
// Mostly remove warnings under windows
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
//      Implements special effects:
//      Texture animation, height or lighting changes
//       according to adjacent sectors, respective
//       utility functions, etc.
//
//-----------------------------------------------------------------------------


#ifndef __P_SPEC__
#define __P_SPEC__

//      Define values for map objects
#define MO_TELEPORTMAN          14


// at game start
void    P_InitPicAnims (void);

// at map load (sectors)
void    P_SetupLevelFlatAnims (void);

// at map load
void    P_SpawnSpecials (void);

// every tic
void    P_UpdateSpecials (void);

// when needed
boolean
P_UseSpecialLine
( mobj_t*       thing,
  line_t*       line,
  int           side );

void
P_ShootSpecialLine
( mobj_t*       thing,
  line_t*       line );

void
P_CrossSpecialLine
( int           linenum,
  int           side,
  mobj_t*       thing );

void
P_ActivateCrossedLine
( line_t*       line,
  int           side,
  mobj_t*       thing);

void    P_PlayerInSpecialSector (player_t* player);

int
twoSided
( int           sector,
  int           line );

sector_t*
getSector
( int           currentSector,
  int           line,
  int           side );

side_t*
getSide
( int           currentSector,
  int           line,
  int           side );

fixed_t P_FindLowestFloorSurrounding(sector_t* sec);
fixed_t P_FindHighestFloorSurrounding(sector_t* sec);

fixed_t
P_FindNextHighestFloor
( sector_t*     sec,
  int           currentheight );

//SoM: 3/6/2000
fixed_t P_FindNextLowestFloor
( sector_t* sec,
  int currentheight );

fixed_t P_FindLowestCeilingSurrounding(sector_t* sec);
fixed_t P_FindHighestCeilingSurrounding(sector_t* sec);

int
P_FindSectorFromLineTag
( line_t*       line,
  int           start );

int
P_FindSectorFromTag
( int           tag,
  int           start );

int P_FindLineFromLineTag(const line_t *line, int start); //SoM: 3/16/2000

int
P_FindMinSurroundingLight
( sector_t*     sector,
  int           max );

sector_t*
getNextSector
( line_t*       line,
  sector_t*     sec );

//SoM: 3/6/2000
sector_t* P_FindModelFloorSector
( fixed_t floordestheight,
  int secnum );

//SoM: 3/15/2000
fixed_t P_FindNextHighestCeiling(sector_t *sec, int currentheight);
fixed_t P_FindNextLowestCeiling(sector_t *sec, int currentheight);
fixed_t P_FindShortestUpperAround(int secnum);
fixed_t P_FindShortestTextureAround(int secnum);
sector_t *P_FindModelCeilingSector(fixed_t ceildestheight,int secnum);
boolean P_CanUnlockGenDoor( line_t* line, player_t* player);
int P_CheckTag(line_t *line);

//
// SPECIAL
//
int EV_DoDonut(line_t* line);



//
// P_LIGHTS
//
typedef struct
{
    thinker_t   thinker;
    sector_t*   sector;
    int         count;
    int         maxlight;
    int         minlight;

} fireflicker_t;



typedef struct
{
    thinker_t   thinker;
    sector_t*   sector;
    int         count;
    int         maxlight;
    int         minlight;
    int         maxtime;
    int         mintime;

} lightflash_t;



typedef struct
{
    thinker_t   thinker;
    sector_t*   sector;
    int         count;
    int         minlight;
    int         maxlight;
    int         darktime;
    int         brighttime;

} strobe_t;




typedef struct
{
    thinker_t   thinker;
    sector_t*   sector;
    int         minlight;
    int         maxlight;
    int         direction;

} glow_t;

//SoM: thinker struct for fading lights. ToDo: Add effects for light
// transition
typedef struct
{
  thinker_t thinker;
  sector_t *sector;
  int destlevel;
  int speed;
} lightlevel_t;

#define GLOWSPEED               8
#define STROBEBRIGHT            5
#define FASTDARK                15
#define SLOWDARK                35

void    T_FireFlicker (fireflicker_t* flick);
void    P_SpawnFireFlicker (sector_t* sector);
void    T_LightFlash (lightflash_t* flash);
void    P_SpawnLightFlash (sector_t* sector);
void    T_StrobeFlash (strobe_t* flash);

void
P_SpawnStrobeFlash
( sector_t*     sector,
  int           fastOrSlow,
  int           inSync );

int    EV_StartLightStrobing(line_t* line);
int    EV_TurnTagLightsOff(line_t* line);

int
EV_LightTurnOn
( line_t*       line,
  int           bright );

void    T_Glow(glow_t* g);
void    P_SpawnGlowingLight(sector_t* sector);


void P_FadeLight(int tag, int destvalue, int speed);
void T_LightFade(lightlevel_t *ll);



//
// P_SWITCH
//
#pragma pack(1) //Hurdler: 04/04/2000: I think pragma is more portable
typedef struct
{
    char        name1[9];
    char        name2[9];
    short       episode;
} switchlist_t; 
//} __attribute__ ((packed)) switchlist_t; //SoM: 3/22/2000: Packed to read from memory.
#pragma pack()


typedef enum
{
    top,
    middle,
    bottom

} bwhere_e;


typedef struct
{
    line_t*     line;
    bwhere_e    where;
    int         btexture;
    int         btimer;
    mobj_t*     soundorg;

} button_t;




 // max # of wall switches in a level
#define MAXSWITCHES             50

 // 4 players, 4 buttons each at once, max.
 // added 19-1-98 16->MAXPLAYERS*4
#define MAXBUTTONS           (MAXPLAYERS*4) //16

 // 1 second, in ticks.
#define BUTTONTIME      35

extern button_t buttonlist[MAXBUTTONS];

void
P_ChangeSwitchTexture
( line_t*       line,
  int           useAgain );

void P_InitSwitchList(void);

// SoM: 3/4/2000: Misc Boom stuff for thinkers that can share sectors, and some other stuff

typedef enum
{
  floor_special,
  ceiling_special,
  lighting_special,
} special_e;


//SoM: 3/6/2000
int P_SectorActive
( special_e t,
  sector_t* s );


typedef enum
{
  trigChangeOnly,
  numChangeOnly,
} change_e;


//
// P_PLATS
//
typedef enum
{
    up,
    down,
    waiting,
    in_stasis

} plat_e;



typedef enum
{
    perpetualRaise,
    downWaitUpStay,
    raiseAndChange,
    raiseToNearestAndChange,
    blazeDWUS,
    //SoM:3/4/2000: Added boom stuffs
    genLift,      //General stuff
    genPerpetual, 
    toggleUpDn,   //Instant toggle of stuff.

} plattype_e;




typedef struct
{
    thinker_t   thinker;
    sector_t*   sector;
    fixed_t     speed;
    fixed_t     low;
    fixed_t     high;
    int         wait;
    int         count;
    plat_e      status;
    plat_e      oldstatus;
    boolean     crush;
    int         tag;
    plattype_e  type;

    struct platlist *list; //SoM: 3/6/2000: Boom's improved code without limits.
} plat_t;

//SoM: 3/6/2000: Boom's improved code without limits.
typedef struct platlist {
  plat_t *plat; 
  struct platlist *next,**prev;
} platlist_t;

void P_RemoveAllActivePlats(void); //SoM: 3/9/2000

#define PLATWAIT                3
#define PLATSPEED               (FRACUNIT/NEWTICRATERATIO)
#define MAXPLATS                30


extern platlist_t  *activeplats;

void    T_PlatRaise(plat_t*     plat);

int
EV_DoPlat
( line_t*       line,
  plattype_e    type,
  int           amount );

void    P_AddActivePlat(plat_t* plat);
void    P_RemoveActivePlat(plat_t* plat);
int     EV_StopPlat(line_t* line);
void    P_ActivateInStasis(int tag);


//
// P_DOORS
//
typedef enum
{
    normalDoor,
    close30ThenOpen,
    doorclose,
    dooropen,
    raiseIn5Mins,
    blazeRaise,
    blazeOpen,
    blazeClose,

    //SoM: 3/4/2000: General door types...
    genRaise,
    genBlazeRaise,
    genOpen,
    genBlazeOpen,
    genClose,
    genBlazeClose,
    genCdO,
    genBlazeCdO,
} vldoor_e;



typedef struct
{
    thinker_t   thinker;
    vldoor_e    type;
    sector_t*   sector;
    fixed_t     topheight;
    fixed_t     speed;

    // 1 = up, 0 = waiting at top, -1 = down
    int             direction;

    // tics to wait at the top
    int             topwait;
    // (keep in case a door going down is reset)
    // when it reaches 0, start going down
    int             topcountdown;

    //SoM: 3/6/2000: the line that triggered the door.
    line_t *line;
} vldoor_t;



#define VDOORSPEED              (FRACUNIT*2/NEWTICRATERATIO)
#define VDOORWAIT               150

int //SoM: 3/6/2000: boom support
EV_VerticalDoor
( line_t*       line,
  mobj_t*       thing );

int
EV_DoDoor
( line_t*       line,
  vldoor_e      type,
  fixed_t       speed);

void EV_OpenDoor(int sectag, int speed, int wait_time);
void EV_CloseDoor(int sectag, int speed);

int
EV_DoLockedDoor
( line_t*       line,
  vldoor_e      type,
  mobj_t*       thing, fixed_t speed );

void    T_VerticalDoor (vldoor_t* door);
void    P_SpawnDoorCloseIn30 (sector_t* sec);

void
P_SpawnDoorRaiseIn5Mins
( sector_t*     sec,
  int           secnum );



#if 0 // UNUSED
//
//      Sliding doors...
//
typedef enum
{
    sd_opening,
    sd_waiting,
    sd_closing

} sd_e;



typedef enum
{
    sdt_openOnly,
    sdt_closeOnly,
    sdt_openAndClose

} sdt_e;




typedef struct
{
    thinker_t   thinker;
    sdt_e       type;
    line_t*     line;
    int         frame;
    int         whichDoorIndex;
    int         timer;
    sector_t*   frontsector;
    sector_t*   backsector;
    sd_e         status;

} slidedoor_t;



typedef struct
{
    char        frontFrame1[9];
    char        frontFrame2[9];
    char        frontFrame3[9];
    char        frontFrame4[9];
    char        backFrame1[9];
    char        backFrame2[9];
    char        backFrame3[9];
    char        backFrame4[9];

} slidename_t;



typedef struct
{
    int             frontFrames[4];
    int             backFrames[4];

} slideframe_t;



// how many frames of animation
#define SNUMFRAMES              4

#define SDOORWAIT               (35*3)
#define SWAITTICS               4

// how many diff. types of anims
#define MAXSLIDEDOORS   5

void P_InitSlidingDoorFrames(void);

void
EV_SlidingDoor
( line_t*       line,
  mobj_t*       thing );
#endif



//
// P_CEILNG
//
typedef enum
{
    lowerToFloor,
    raiseToHighest,
    //SoM:3/4/2000: Extra boom stuffs that tricked me...
    lowerToLowest,
    lowerToMaxFloor,

    lowerAndCrush,
    crushAndRaise,
    fastCrushAndRaise,
    silentCrushAndRaise,
    instantRaise, // Insantly raises SSNTails 06-13-2002

    //SoM:3/4/2000
    //jff 02/04/98 add types for generalized ceiling mover
    genCeiling,
    genCeilingChg,
    genCeilingChg0,
    genCeilingChgT,

    //jff 02/05/98 add types for generalized ceiling mover
    genCrusher,
    genSilentCrusher,


} ceiling_e;



typedef struct
{
    thinker_t   thinker;
    ceiling_e   type;
    sector_t*   sector;
    fixed_t     bottomheight;
    fixed_t     topheight;
    fixed_t     speed;
    fixed_t     oldspeed; //SoM: 3/6/2000
    boolean     crush;

    //SoM: 3/6/2000: Support ceiling changers
    int newspecial;
    int oldspecial;
    short texture;

    // 1 = up, 0 = waiting, -1 = down
    int         direction;

    // ID
    int         tag;
    int         olddirection;

    struct ceilinglist *list;   // SoM: 3/6/2000: by jff: copied from killough's plats
} ceiling_t;

//SoM: 3/6/2000: Boom's improved ceiling list.
typedef struct ceilinglist {
  ceiling_t *ceiling; 
  struct ceilinglist *next,**prev;
} ceilinglist_t;


void P_RemoveAllActiveCeilings(void); //SoM: 3/9/2000


#define CEILSPEED               (FRACUNIT/NEWTICRATERATIO)
#define CEILWAIT                150
#define MAXCEILINGS             30

extern ceilinglist_t *activeceilings;  //SoM: 3/6/2000: New improved boom code.

int
EV_DoCeiling
( line_t*       line,
  ceiling_e     type );

void    T_MoveCeiling (ceiling_t* ceiling);
void    P_AddActiveCeiling(ceiling_t* ceiling);
void    P_RemoveActiveCeiling(ceiling_t* ceiling);
int     EV_CeilingCrushStop(line_t* line);
int     P_ActivateInStasisCeiling(line_t* line);


//
// P_FLOOR
//
typedef enum
{
    // lower floor to highest surrounding floor
    lowerFloor,

    // lower floor to lowest surrounding floor
    lowerFloorToLowest,

    // lower floor to highest surrounding floor VERY FAST
    turboLower,

    // raise floor to lowest surrounding CEILING
    raiseFloor,

    // raise floor to next highest surrounding floor
    raiseFloorToNearest,

    // lower floor to lowest surrounding floor
    lowerFloorToNearest,

    // lower floor 24
    lowerFloor24,

    // lower floor 32
    lowerFloor32Turbo,

    // raise floor to shortest height texture around it
    raiseToTexture,

    // lower floor to lowest surrounding floor
    //  and change floorpic
    lowerAndChange,

    raiseFloor24,

    //raise floor 32
    raiseFloor32Turbo,

    raiseFloor24AndChange,
    raiseFloorCrush,

     // raise to next highest floor, turbo-speed
    raiseFloorTurbo,
    donutRaise,
    raiseFloor512,
	instantLower, // Lowers instantly SSNTails 06-13-2002

    //SoM: 3/4/2000 Boom copy YEAH YEAH
    genFloor,
    genFloorChg,
    genFloorChg0,
    genFloorChgT,

    //new types for stair builders
    buildStair,
    genBuildStair,

} floor_e;

//SoM:3/4/2000: Anothe boom code copy.
typedef enum
{
  elevateUp,
  elevateDown,
  elevateCurrent,
} elevator_e;


typedef enum
{
    build8,     // slowly build by 8
    turbo16     // quickly build by 16

} stair_e;



typedef struct
{
    thinker_t   thinker;
    floor_e     type;
    boolean     crush;
    sector_t*   sector;
    int         direction;
    int         newspecial;
    int         oldspecial; //SoM: 3/6/2000
    short       texture;
    fixed_t     floordestheight;
    fixed_t     speed;

} floormove_t;


typedef struct //SoM: 3/6/2000: Elevator struct.
{
  thinker_t thinker;
  elevator_e type;
  sector_t* sector;
  int direction;
  fixed_t floordestheight;
  fixed_t ceilingdestheight;
  fixed_t speed;
} elevator_t;


#define ELEVATORSPEED (FRACUNIT*4/NEWTICRATERATIO) //SoM: 3/6/2000
#define FLOORSPEED    (FRACUNIT/NEWTICRATERATIO)

typedef enum
{
    res_ok,
    crushed,
    pastdest

} result_e;

result_e
T_MovePlane
( sector_t*     sector,
  fixed_t       speed,
  fixed_t       dest,
  boolean       crush,
  int           floorOrCeiling,
  int           direction );

int
EV_BuildStairs
( line_t*       line,
  stair_e       type );

int
EV_DoFloor
( line_t*       line,
  floor_e       floortype );

int EV_DoChange
( line_t*       line,
  change_e      changetype ); //SoM: 3/16/2000

int EV_DoElevator
( line_t*       line,
  elevator_e    elevtype ); //SoM: 3/16/2000

void T_MoveFloor( floormove_t* floor);

//SoM: New thinker functions.
void T_MoveElevator(elevator_t* elevator);

//
// P_TELEPT
//
int
EV_Teleport
( line_t*       line,
  int           side,
  mobj_t*       thing );

//SoM: 3/15/2000: Boom silent teleport functions
int EV_SilentTeleport(line_t *line, int side, mobj_t *thing);
int EV_SilentLineTeleport(line_t *line, int side, mobj_t *thing, boolean reverse);




/* SoM: 3/4/2000: This is a large section of copied code. Sorry if this offends people, but
   I really don't want to read, understand and rewrite all the changes to the source and entire
   team made! Anyway, this is for the generalized linedef types. */

//jff 3/14/98 add bits and shifts for generalized sector types

#define DAMAGE_MASK     0x60
#define DAMAGE_SHIFT    5
#define SECRET_MASK     0x80
#define SECRET_SHIFT    7
#define FRICTION_MASK   0x100
#define FRICTION_SHIFT  8
#define PUSH_MASK       0x200
#define PUSH_SHIFT      9

//jff 02/04/98 Define masks, shifts, for fields in 
// generalized linedef types

#define GenFloorBase          0x6000
#define GenCeilingBase        0x4000
#define GenDoorBase           0x3c00
#define GenLockedBase         0x3800
#define GenLiftBase           0x3400
#define GenStairsBase         0x3000
#define GenCrusherBase        0x2F80

#define TriggerType           0x0007
#define TriggerTypeShift      0

// define masks and shifts for the floor type fields

#define FloorCrush            0x1000
#define FloorChange           0x0c00
#define FloorTarget           0x0380
#define FloorDirection        0x0040
#define FloorModel            0x0020
#define FloorSpeed            0x0018

#define FloorCrushShift           12
#define FloorChangeShift          10
#define FloorTargetShift           7
#define FloorDirectionShift        6
#define FloorModelShift            5
#define FloorSpeedShift            3
                               
// define masks and shifts for the ceiling type fields

#define CeilingCrush          0x1000
#define CeilingChange         0x0c00
#define CeilingTarget         0x0380
#define CeilingDirection      0x0040
#define CeilingModel          0x0020
#define CeilingSpeed          0x0018

#define CeilingCrushShift         12
#define CeilingChangeShift        10
#define CeilingTargetShift         7
#define CeilingDirectionShift      6
#define CeilingModelShift          5
#define CeilingSpeedShift          3

// define masks and shifts for the lift type fields

#define LiftTarget            0x0300
#define LiftDelay             0x00c0
#define LiftMonster           0x0020
#define LiftSpeed             0x0018

#define LiftTargetShift            8
#define LiftDelayShift             6
#define LiftMonsterShift           5
#define LiftSpeedShift             3

// define masks and shifts for the stairs type fields

#define StairIgnore           0x0200
#define StairDirection        0x0100
#define StairStep             0x00c0
#define StairMonster          0x0020
#define StairSpeed            0x0018

#define StairIgnoreShift           9
#define StairDirectionShift        8
#define StairStepShift             6
#define StairMonsterShift          5
#define StairSpeedShift            3

// define masks and shifts for the crusher type fields

#define CrusherSilent         0x0040
#define CrusherMonster        0x0020
#define CrusherSpeed          0x0018

#define CrusherSilentShift         6
#define CrusherMonsterShift        5
#define CrusherSpeedShift          3

// define masks and shifts for the door type fields

#define DoorDelay             0x0300
#define DoorMonster           0x0080
#define DoorKind              0x0060
#define DoorSpeed             0x0018

#define DoorDelayShift             8
#define DoorMonsterShift           7
#define DoorKindShift              5
#define DoorSpeedShift             3

// define masks and shifts for the locked door type fields

#define LockedNKeys           0x0200
#define LockedKey             0x01c0
#define LockedKind            0x0020
#define LockedSpeed           0x0018

#define LockedNKeysShift           9
#define LockedKeyShift             6
#define LockedKindShift            5
#define LockedSpeedShift           3


//SoM: 3/9/2000: p_genlin

int EV_DoGenFloor
( line_t* line );

int EV_DoGenCeiling
( line_t* line );

int EV_DoGenLift
( line_t* line );

int EV_DoGenStairs
( line_t* line );

int EV_DoGenCrusher
( line_t* line );

int EV_DoGenDoor
( line_t* line );

int EV_DoGenLockedDoor
( line_t* line );

// define names for the TriggerType field of the general linedefs

typedef enum
{
  WalkOnce,
  WalkMany,
  SwitchOnce,
  SwitchMany,
  GunOnce,
  GunMany,
  PushOnce,
  PushMany,
} triggertype_e;

// define names for the Speed field of the general linedefs

typedef enum
{
  SpeedSlow,
  SpeedNormal,
  SpeedFast,
  SpeedTurbo,
} motionspeed_e;

// define names for the Target field of the general floor

typedef enum
{
  FtoHnF,
  FtoLnF,
  FtoNnF,
  FtoLnC,
  FtoC,
  FbyST,
  Fby24,
  Fby32,
} floortarget_e;

// define names for the Changer Type field of the general floor

typedef enum
{
  FNoChg,
  FChgZero,
  FChgTxt,
  FChgTyp,
} floorchange_e;

// define names for the Change Model field of the general floor

typedef enum
{
  FTriggerModel,
  FNumericModel,
} floormodel_t;

// define names for the Target field of the general ceiling

typedef enum
{
  CtoHnC,
  CtoLnC,
  CtoNnC,
  CtoHnF,
  CtoF,
  CbyST,
  Cby24,
  Cby32,
} ceilingtarget_e;

// define names for the Changer Type field of the general ceiling

typedef enum
{
  CNoChg,
  CChgZero,
  CChgTxt,
  CChgTyp,
} ceilingchange_e;

// define names for the Change Model field of the general ceiling

typedef enum
{
  CTriggerModel,
  CNumericModel,
} ceilingmodel_t;

// define names for the Target field of the general lift

typedef enum
{
  F2LnF,
  F2NnF,
  F2LnC,
  LnF2HnF,
} lifttarget_e;

// define names for the door Kind field of the general ceiling

typedef enum
{
  OdCDoor,
  ODoor,
  CdODoor,
  CDoor,
} doorkind_e;

// define names for the locked door Kind field of the general ceiling

typedef enum
{
  AnyKey_,
  RCard,
  BCard,
  YCard,
  RSkull,
  BSkull,
  YSkull,
  AllKeys,
} keykind_e;

/* SoM: End generalized linedef code */


//SoM: 3/8/2000: Add generalized scroller code
typedef struct {
  thinker_t thinker;   // Thinker structure for scrolling
  fixed_t dx, dy;      // (dx,dy) scroll speeds
  int affectee;        // Number of affected sidedef, sector, tag, or whatever
  int control;         // Control sector (-1 if none) used to control scrolling
  fixed_t last_height; // Last known height of control sector
  fixed_t vdx, vdy;    // Accumulated velocity if accelerative
  int accel;           // Whether it's accelerative
  enum
  {
    sc_side,
    sc_floor,
    sc_ceiling,
    sc_carry,
    sc_carry_ceiling,
  } type;
} scroll_t;

void T_Scroll(scroll_t *s);


//SoM: 3/8/2000: added new model of friction for ice/sludge effects

typedef struct {
  thinker_t thinker;   // Thinker structure for friction
  int friction;        // friction value (E800 = normal)
  int movefactor;      // inertia factor when adding to momentum
  int affectee;        // Number of affected sector
} friction_t;

//SoM: Friction defines.
#define MORE_FRICTION_MOMENTUM 15000       // mud factor based on momentum
#define ORIG_FRICTION          0xE800      // original value
#define ORIG_FRICTION_FACTOR   2048        // original value

//SoM: 3/9/2000: Otherwise, the compiler whines!
void T_Friction(friction_t *f);

//SoM: 3/8/2000: Model for Pushers for push/pull effects

typedef struct {
  thinker_t thinker;   // Thinker structure for Pusher
  enum
  {
    p_push,
    p_pull,
    p_wind,
    p_current,
    p_upcurrent, // SSNTails 06-10-2002
    p_downcurrent, // SSNTails 06-10-2002
	p_upwind, // SSNTails 06-10-2003 WOAH! EXACTLY ONE YEAR LATER! FREAKY!
	p_downwind, // SSNTails 06-10-2003
  } type;
  mobj_t* source;      // Point source if point pusher
  int x_mag;           // X Strength
  int y_mag;           // Y Strength
  int magnitude;       // Vector strength for point pusher
  int radius;          // Effective radius for point pusher
  int x;               // X of point source if point pusher
  int y;               // Y of point source if point pusher
  int affectee;        // Number of affected sector
} pusher_t;

//SoM: 3/9/2000: Prototype functions for pushers
boolean  PIT_PushThing(mobj_t* thing);
void     T_Pusher(pusher_t *p);
mobj_t*  P_GetPushThing(int s);


//SoM: 3/16/2000
void P_CalcHeight (player_t* player);


// heretic stuff
void P_InitLava(void);
void P_AmbientSound(void);
void P_AddAmbientSfx(int sequence);
void P_InitAmbientSound(void);

#endif
