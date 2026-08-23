// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: d_think.h,v 1.2 2000/02/27 00:42:10 hurdler Exp $
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
// $Log: d_think.h,v $
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//  MapObj data. Map Objects or mobjs are actors, entities,
//  thinker, take-your-pick... anything that moves, acts, or
//  suffers state changes of more or less violent nature.
//
//-----------------------------------------------------------------------------

#pragma once

#include <cstddef>

typedef struct mobj_s mobj_t;
typedef struct player_s player_t;
typedef struct thinker_s thinker_t;
typedef struct pspdef_s pspdef_t;

// Edward 8/23/2026: C++ conversions for actionf_t necesseary to make info.cpp compile.
// The original code used a union of function pointers with different signatures, which is not type-safe in C++.
typedef void (*actionf_v)();
typedef void (*actionf_p1)(mobj_t*);
typedef void (*actionf_t1)(thinker_t*);
typedef void (*actionf_p2)(player_t*, pspdef_t*);

struct actionf_t
{
    union
    {
        actionf_v  acv;
        actionf_p1 acp1;
        actionf_t1 act1;
        actionf_p2 acp2;
    };

    constexpr actionf_t() : acv(nullptr) {}
    constexpr actionf_t(std::nullptr_t) : acv(nullptr) {}
    constexpr actionf_t(actionf_v f) : acv(f) {}
    constexpr actionf_t(actionf_p1 f) : acp1(f) {}
    constexpr actionf_t(actionf_t1 f) : act1(f) {}
    constexpr actionf_t(actionf_p2 f) : acp2(f) {}
};

// Historically, "think_t" is yet another
//  function pointer to a routine to handle
//  an actor.
typedef actionf_t  think_t;


// Doubly linked list of actors.
typedef struct thinker_s
{
    struct thinker_s* prev;
    struct thinker_s* next;
    think_t           function;

} thinker_t;

