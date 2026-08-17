// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: p_mobj.h,v 1.9 2002/01/21 23:14:28 judgecutor Exp $
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
// $Log: p_mobj.h,v $
// Revision 1.9  2002/01/21 23:14:28  judgecutor
// Frag's Weapon Falling fixes
//
// Revision 1.8  2001/11/17 22:12:53  hurdler
// Ready to work on beta 4 ;)
//
// Revision 1.7  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.6  2001/01/25 22:15:43  bpereira
// added heretic support
//
// Revision 1.5  2000/11/02 17:50:08  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.4  2000/04/30 10:30:10  bpereira
// no message
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
//      Map Objects, MObj, definition and handling.
//
//-----------------------------------------------------------------------------


#ifndef __P_MOBJ__
#define __P_MOBJ__


// Basics.
#include "tables.h"
#include "m_fixed.h"

// We need the thinker_t stuff.
#include "d_think.h"

// We need the WAD data structure for Map things,
// from the THINGS lump.
#include "doomdata.h"

// States are tied to finite states are
//  tied to animation frames.
// Needs precompiled tables/data structures.
#include "info.h"



//
// NOTES: mobj_t
//
// mobj_ts are used to tell the refresh where to draw an image,
// tell the world simulation when objects are contacted,
// and tell the sound driver how to position a sound.
//
// The refresh uses the next and prev links to follow
// lists of things in sectors as they are being drawn.
// The sprite, frame, and angle elements determine which patch_t
// is used to draw the sprite if it is visible.
// The sprite and frame values are allmost allways set
// from state_t structures.
// The statescr.exe utility generates the states.h and states.c
// files that contain the sprite/frame numbers from the
// statescr.txt source file.
// The xyz origin point represents a point at the bottom middle
// of the sprite (between the feet of a biped).
// This is the default origin position for patch_ts grabbed
// with lumpy.exe.
// A walking creature will have its z equal to the floor
// it is standing on.
//
// The sound code uses the x,y, and subsector fields
// to do stereo positioning of any sound effited by the mobj_t.
//
// The play simulation uses the blocklinks, x,y,z, radius, height
// to determine when mobj_ts are touching each other,
// touching lines in the map, or hit by trace lines (gunshots,
// lines of sight, etc).
// The mobj_t->flags element has various bit flags
// used by the simulation.
//
// Every mobj_t is linked into a single sector
// based on its origin coordinates.
// The subsector_t is found with R_PointInSubsector(x,y),
// and the sector_t can be found with subsector->sector.
// The sector links are only used by the rendering code,
// the play simulation does not care about them at all.
//
// Any mobj_t that needs to be acted upon by something else
// in the play world (block movement, be shot, etc) will also
// need to be linked into the blockmap.
// If the thing has the MF_NOBLOCK flag set, it will not use
// the block links. It can still interact with other things,
// but only as the instigator (missiles will run into other
// things, but nothing can run into a missile).
// Each block in the grid is 128*128 units, and knows about
// every line_t that it contains a piece of, and every
// interactable mobj_t that has its origin contained.
//
// A valid mobj_t is a mobj_t that has the proper subsector_t
// filled in for its xy coordinates and is linked into the
// sector from which the subsector was made, or has the
// MF_NOSECTOR flag set (the subsector_t needs to be valid
// even if MF_NOSECTOR is set), and is linked into a blockmap
// block or has the MF_NOBLOCKMAP flag set.
// Links should only be modified by the P_[Un]SetThingPosition()
// functions.
// Do not change the MF_NO? flags while a thing is valid.
//
// Any questions?
//

//
// Misc. mobj flags
//
typedef enum
{
    // Call P_SpecialThing when touched.
    MF_SPECIAL          = 0x0001,
    // Blocks.
    MF_SOLID            = 0x0002,
    // Can be hit.
    MF_SHOOTABLE        = 0x0004,
    // Don't use the sector links (invisible but touchable).
    MF_NOSECTOR         = 0x0008,
    // Don't use the blocklinks (inert but displayable)
    MF_NOBLOCKMAP       = 0x0010,

    // Not to be activated by sound, deaf monster.
    MF_AMBUSH           = 0x0020,
    // Will try to attack right back.
    MF_JUSTHIT          = 0x0040,
    // Will take at least one step before attacking.
    MF_JUSTATTACKED     = 0x0080,
    // On level spawning (initial position),
    //  hang from ceiling instead of stand on floor.
    MF_SPAWNCEILING     = 0x0100,
    // Don't apply gravity (every tic),
    //  that is, object will float, keeping current height
    //  or changing it actively.
    MF_NOGRAVITY        = 0x0200,

    // Movement flags.
    // This allows jumps from high places.
    MF_DROPOFF          = 0x0400,
    // For players, will pick up items.
    MF_PICKUP           = 0x0800,
    // Player cheat. ???
    MF_NOCLIP           = 0x1000,
    // Player: keep info about sliding along walls.
    MF_SLIDE            = 0x2000,
    // Allow moves to any height, no gravity.
    // For active floaters, e.g. cacodemons, pain elementals.
    MF_FLOAT            = 0x4000,
    // Don't cross lines
    //   ??? or look at heights on teleport.
    MF_TELEPORT         = 0x8000,
    // Don't hit same species, explode on block.
    // Player missiles as well as fireballs of various kinds.
    MF_MISSILE          = 0x10000,
    // Dropped by a demon, not level spawned.
    // E.g. ammo clips dropped by dying former humans.
    MF_DROPPED          = 0x20000,
    // DOOM2: Use fuzzy draw (shadow demons or spectres),
    //  temporary player invisibility powerup.
    // LEGACY: no more for translucency, but still makes targeting harder
    MF_SHADOW           = 0x40000,
    // Flag: don't bleed when shot (use puff),
    //  barrels and shootable furniture shall not bleed.
    MF_NOBLOOD          = 0x80000,
    // Don't stop moving halfway off a step,
    //  that is, have dead bodies slide down all the way.
    MF_CORPSE           = 0x100000,
    // Floating to a height for a move, ???
    //  don't auto float to target's height.
    MF_INFLOAT          = 0x200000,

    // On kill, count this enemy object
    //  towards intermission kill total.
    // Happy gathering.
    MF_COUNTKILL        = 0x400000,

    // On picking up, count this item object
    //  towards intermission item total.
    MF_COUNTITEM        = 0x800000,

    // Special handling: skull in flight.
    // Neither a cacodemon nor a missile.
    MF_SKULLFLY         = 0x1000000,

    // Don't spawn this object
    //  in death match mode (e.g. key cards).
    MF_NOTDMATCH        = 0x2000000,

    // Player sprites in multiplayer modes are modified
    //  using an internal color lookup table for re-indexing.
    // If 0x4 0x8 or 0xc,
    //  use a translation table for player colormaps
    MF_TRANSLATION      = 0x3C000000,    // 0xc000000, original 4color

    // Hmm ???.
    MF_TRANSSHIFT       = 26,

    // for chase camera, don't be blocked by things (parsial cliping)
    MF_NOCLIPTHING      = 0x40000000,

    MF_FLOORHUGGER      = 0x80000000

} mobjflag_t;


typedef enum {
    MF2_LOGRAV         =     0x00000001,      // alternate gravity setting
    MF2_WINDTHRUST     =     0x00000002,      // gets pushed around by the wind
                                              // specials
    MF2_FLOORBOUNCE    =     0x00000004,      // bounces off the floor
    MF2_THRUGHOST      =     0x00000008,      // missile will pass through ghosts
    MF2_FLY            =     0x00000010,      // fly mode is active
    MF2_FOOTCLIP       =     0x00000020,      // if feet are allowed to be clipped
    MF2_SPAWNFLOAT     =     0x00000040,      // spawn random float z
    MF2_NOTELEPORT     =     0x00000080,      // does not teleport
    MF2_RIP            =     0x00000100,      // missile rips through solid
                                              // targets
    MF2_PUSHABLE       =     0x00000200,      // can be pushed by other moving
                                              // mobjs
    MF2_SLIDE          =     0x00000400,      // slides against walls
    MF2_ONMOBJ         =     0x00000800,      // mobj is resting on top of another
                                              // mobj
    MF2_PASSMOBJ       =     0x00001000,      // Enable z block checking.  If on,
                                              // this flag will allow the mobj to
                                              // pass over/under other mobjs.
    MF2_CANNOTPUSH     =     0x00002000,      // cannot push other pushable mobjs
    MF2_FEETARECLIPPED =     0x00004000,      // a mobj's feet are now being cut
    MF2_BOSS           =     0x00008000,      // mobj is a major boss
    MF2_FIREDAMAGE     =     0x00010000,      // does fire damage
    MF2_NODMGTHRUST    =     0x00020000,      // does not thrust target when
                                              // damaging        
    MF2_TELESTOMP      =     0x00040000,      // mobj can stomp another
    MF2_FLOATBOB       =     0x00080000,      // use float bobbing z movement
    MF2_DONTDRAW       =     0X00100000,      // don't generate a vissprite
        
} mobjflag2_t;

//
//  New mobj extra flags
//
//added:28-02-98:
typedef enum
{
    // The mobj stands on solid floor (not on another mobj or in air)
    MF_ONGROUND          = 1,
    // The mobj just hit the floor while falling, this is cleared on next frame
    // (instant damage in lava/slime sectors to prevent jump cheat..)
    MF_JUSTHITFLOOR      = 2,
    // The mobj stands in a sector with water, and touches the surface
    // this bit is set once and for all at the start of mobjthinker
    MF_TOUCHWATER        = 4,
    // The mobj stands in a sector with water, and his waist is BELOW the water surface
    // (for player, allows swimming up/down)
    MF_UNDERWATER        = 8,
    // Set by P_MovePlayer() to disable gravity add in P_MobjThinker() ( for gameplay )
    MF_SWIMMING          = 16,
    // used for client prediction code, player can't be blocked in z by walls
    // it is set temporarely when player follow the spirit
    MF_NOZCHECKING       = 32,
} mobjeflag_t;


#if MAXSKINCOLOR > 16

MAXSKINCOLOR have changed
Change MF_TRANSLATION to take effect of the change

#endif

// Map Object definition.
typedef struct mobj_s
{
    // List: thinker links.
    thinker_t           thinker;

    // Info for drawing: position.
    fixed_t             x;
    fixed_t             y;
    fixed_t             z;

    // More list: links in sector (if needed)
    struct mobj_s*      snext;
    struct mobj_s*      sprev;

    //More drawing info: to determine current sprite.
    angle_t             angle;  // orientation
    spritenum_t         sprite; // used to find patch_t and flip value
    int                 frame;  // frame number, plus bits see p_pspr.h

    //Fab:02-08-98
    void*               skin;      // this one overrides 'sprite' when
                                   // non NULL (currently hack for player
                                   // bodies so they 'remember' the skin)
                                   //
                                   // secondary used when player die and
                                   // play the die sound problem is he is
                                   // already respawn and the corps play
                                   // the sound !!! (he yeah it happens :\)

    // Interaction info, by BLOCKMAP.
    // Links in blocks (if needed).
    struct mobj_s*      bnext;
    struct mobj_s*      bprev;

    struct subsector_s* subsector;

    // The closest interval over all contacted Sectors (or Things).
    fixed_t             floorz;
    fixed_t             ceilingz;

    // For movement checking.
    fixed_t             radius;
    fixed_t             height;

    // Momentums, used to update position.
    fixed_t             momx;
    fixed_t             momy;
    fixed_t             momz;

    // If == validcount, already checked.
    //int                 validcount;

    mobjtype_t          type;
    mobjinfo_t*         info;   // &mobjinfo[mobj->type]

    int                 tics;   // state tic counter
    state_t*            state;
    int                 flags;
    int                 eflags; //added:28-02-98: extra flags see above
    int                 flags2; // heretic stuff
    int                 special1;
    int                 special2;
    int                 health;

    // Movement direction, movement generation (zig-zagging).
    int                 movedir;        // 0-7
    int                 movecount;      // when 0, select a new dir

    // Thing being chased/attacked (or NULL),
    // also the originator for missiles.
    struct mobj_s*      target;

    // Reaction time: if non 0, don't attack yet.
    // Used by player to freeze a bit after teleporting.
    int                 reactiontime;

    // If >0, the target will be chased
    // no matter what (even if shot)
    int                 threshold;

    // Additional info record for player avatars only.
    // Only valid if type == MT_PLAYER
    struct player_s*    player;

    // Player number last looked for.
    int                 lastlook;

    // For nightmare and itemrespawn respawn.
    mapthing_t          *spawnpoint;

    // Thing being chased/attacked for tracers.
    struct mobj_s*      tracer;

    //SoM: Friction.
    int friction;
    int movefactor;

    // a linked list of sectors where this object appears
    struct msecnode_s* touching_sectorlist;

    // Support for Frag Weapon Falling
    // This field valid only for MF_DROPPED ammo and weapn objects
    int dropped_ammo_count;

    // WARNING : new field are not automaticely added to save game 
} mobj_t;

// check mobj against water content, before movement code
void P_MobjCheckWater (mobj_t* mobj);

void P_SpawnMapThing (mapthing_t*    mthing);
void P_SpawnPlayer (mapthing_t* mthing);
int P_HitFloor(mobj_t *thing);

#endif
