// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: sounds.c,v 1.10 2001/03/30 17:12:51 bpereira Exp $
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
// $Log: sounds.c,v $
// Revision 1.10  2001/03/30 17:12:51  bpereira
// no message
//
// Revision 1.9  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.8  2001/02/24 13:35:21  bpereira
// no message
//
// Revision 1.7  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.6  2000/11/21 21:13:18  stroggonmeth
// Optimised 3D floors and fixed crashing bug in high resolutions.
//
// Revision 1.5  2000/11/03 11:48:40  hurdler
// Fix compiling problem under win32 with 3D-Floors and FragglScript (to verify!)
//
// Revision 1.4  2000/11/03 02:37:36  stroggonmeth
// Fix a few warnings when compiling.
//
// Revision 1.3  2000/11/02 17:50:10  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      music/sound tables, and related sound routines
//
// Note: the tables were originally created by a sound utility at Id,
//       kept as a sample, DOOM2 sounds.
//
//-----------------------------------------------------------------------------


#include "doomtype.h"
#include "i_sound.h"
#include "sounds.h"
#include "r_defs.h"
#include "r_things.h"
#include "z_zone.h"
#include "w_wad.h"

// NOTE: add \0 for stringlen=6, to allow dehacked patching

//
// Information about all the music
//

musicinfo_t S_music[NUMMUSIC] =
{
    { 0 },
    { "e1m1\0\0", 0 },
    { "e1m2\0\0", 0 },
    { "e1m3\0\0", 0 },
    { "e1m4\0\0", 0 },
    { "e1m5\0\0", 0 },
    { "e1m6\0\0", 0 },
    { "e1m7\0\0", 0 },
    { "e1m8\0\0", 0 },
    { "e1m9\0\0", 0 },
    { "e2m1\0\0", 0 },
    { "e2m2\0\0", 0 },
    { "e2m3\0\0", 0 },
    { "e2m4\0\0", 0 },
    { "e2m5\0\0", 0 },
    { "e2m6\0\0", 0 },
    { "e2m7\0\0", 0 },
    { "e2m8\0\0", 0 },
    { "e2m9\0\0", 0 },
    { "e3m1\0\0", 0 },
    { "e3m2\0\0", 0 },
    { "e3m3\0\0", 0 },
    { "e3m4\0\0", 0 },
    { "e3m5\0\0", 0 },
    { "e3m6\0\0", 0 },
    { "e3m7\0\0", 0 },
    { "e3m8\0\0", 0 },
    { "e3m9\0\0", 0 },
    { "inter\0" , 0 },
    { "intro\0" , 0 },
    { "bunny\0" , 0 },
    { "victor"  , 0 },
    { "introa"  , 0 },
    { "runnin"  , 0 },
    { "stalks"  , 0 },
    { "countd"  , 0 },
    { "betwee"  , 0 },
    { "doom\0\0", 0 },
    { "the_da"  , 0 },
    { "shawn\0" , 0 },
    { "ddtblu"  , 0 },
    { "in_cit"  , 0 },
    { "dead\0\0", 0 },
    { "stlks2"  , 0 },
    { "theda2"  , 0 },
    { "doom2\0" , 0 },
    { "ddtbl2"  , 0 },
    { "runni2"  , 0 },
    { "dead2\0" , 0 },
    { "stlks3"  , 0 },
    { "romero"  , 0 },
    { "shawn2"  , 0 },
    { "messag"  , 0 },
    { "count2"  , 0 },
    { "ddtbl3"  , 0 },
    { "ampie\0" , 0 },
    { "theda3"  , 0 },
    { "adrian"  , 0 },
    { "messg2"  , 0 },
    { "romer2"  , 0 },
    { "tense\0" , 0 },
    { "shawn3"  , 0 },
    { "openin"  , 0 },
    { "evil\0\0", 0 },
    { "ultima"  , 0 },
    { "read_m"  , 0 },
    { "dm2ttl"  , 0 },
    { "dm2int"  , 0 },
// heretic stuff
        { "MUS_E1M1", 0 }, // 1-1
        { "MUS_E1M2", 0 },
        { "MUS_E1M3", 0 },
        { "MUS_E1M4", 0 },
        { "MUS_E1M5", 0 },
        { "MUS_E1M6", 0 },
        { "MUS_E1M7", 0 },
        { "MUS_E1M8", 0 },
        { "MUS_E1M9", 0 },

        { "MUS_E2M1", 0 }, // 2-1
        { "MUS_E2M2", 0 },
        { "MUS_E2M3", 0 },
        { "MUS_E2M4", 0 },
        { "MUS_E1M4", 0 },
        { "MUS_E2M6", 0 },
        { "MUS_E2M7", 0 },
        { "MUS_E2M8", 0 },
        { "MUS_E2M9", 0 },

        { "MUS_E1M1", 0 }, // 3-1
        { "MUS_E3M2", 0 },
        { "MUS_E3M3", 0 },
        { "MUS_E1M6", 0 },
        { "MUS_E1M3", 0 },
        { "MUS_E1M2", 0 },
        { "MUS_E1M5", 0 },
        { "MUS_E1M9", 0 },
        { "MUS_E2M6", 0 },

        { "MUS_E1M6", 0 }, // 4-1
        { "MUS_E1M2", 0 },
        { "MUS_E1M3", 0 },
        { "MUS_E1M4", 0 },
        { "MUS_E1M5", 0 },
        { "MUS_E1M1", 0 },
        { "MUS_E1M7", 0 },
        { "MUS_E1M8", 0 },
        { "MUS_E1M9", 0 },

        { "MUS_E2M1", 0 }, // 5-1
        { "MUS_E2M2", 0 },
        { "MUS_E2M3", 0 },
        { "MUS_E2M4", 0 },
        { "MUS_E1M4", 0 },
        { "MUS_E2M6", 0 },
        { "MUS_E2M7", 0 },
        { "MUS_E2M8", 0 },
        { "MUS_E2M9", 0 },

        { "MUS_E3M2", 0 }, // 6-1
        { "MUS_E3M3", 0 }, // 6-2
        { "MUS_E1M6", 0 }, // 6-3

        { "MUS_TITL", 0 },
        { "MUS_CPTD", 0 }
};


//
// Information about all the sfx
//

sfxinfo_t S_sfx[NUMSFX] =
{
  // S_sfx[0] needs to be a dummy for odd reasons.
//         singularity(U)        pitch      skinsound   (U) for UNUSED
//               |      priority(U) volume  |
//               |        |  link   |   data|
  { "none"     , false,   0, 0, -1, -1, 0, -1},

  { "pistol"   , false,  64, 0, -1, -1, 0, -1},
  { "shotgn"   , false,  64, 0, -1, -1, 0, -1},
  { "sgcock"   , false,  64, 0, -1, -1, 0, -1},
  { "dshtgn"   , false,  64, 0, -1, -1, 0, -1},
  { "dbopn\0"  , false,  64, 0, -1, -1, 0, -1},
  { "dbcls\0"  , false,  64, 0, -1, -1, 0, -1},
  { "dbload"   , false,  64, 0, -1, -1, 0, -1},
  { "plasma"   , false,  64, 0, -1, -1, 0, -1},
  { "bfg\0\0\0", false,  64, 0, -1, -1, 0, -1},
  { "sawup\0"  , false,  64, 0, -1, -1, 0, -1},
  { "sawidl"   , false, 118, 0, -1, -1, 0, -1},
  { "sawful"   , false,  64, 0, -1, -1, 0, -1},
  { "sawhit"   , false,  64, 0, -1, -1, 0, -1},
  { "rlaunc"   , false,  64, 0, -1, -1, 0, -1},
  { "rxplod"   , false,  70, 0, -1, -1, 0, -1},
  { "firsht"   , false,  70, 0, -1, -1, 0, -1},
  { "firxpl"   , false,  70, 0, -1, -1, 0, -1},
  { "pstart"   , false, 100, 0, -1, -1, 0, -1},
  { "pstop\0"  , false, 100, 0, -1, -1, 0, -1},
  { "doropn"   , false, 100, 0, -1, -1, 0, -1},
  { "dorcls"   , false, 100, 0, -1, -1, 0, -1},
  { "stnmov"   , false, 119, 0, -1, -1, 0, -1},
  { "swtchn"   , false,  78, 0, -1, -1, 0, -1},
  { "swtchx"   , false,  78, 0, -1, -1, 0, -1},
  { "plpain"   , false,  96, 0, -1, -1, 0, SKSPLPAIN},
  { "dmpain"   , false,  96, 0, -1, -1, 0, -1},
  { "popain"   , false,  96, 0, -1, -1, 0, -1},
  { "vipain"   , false,  96, 0, -1, -1, 0, -1},
  { "mnpain"   , false,  96, 0, -1, -1, 0, -1},
  { "pepain"   , false,  96, 0, -1, -1, 0, -1},
  { "slop\0\0" , false,  78, 0, -1, -1, 0, SKSSLOP},
  { "itemup"   ,  true,  78, 0, -1, -1, 0, -1},
  { "wpnup"    ,  true,  78, 0, -1, -1, 0, -1},
  { "oof\0\0\0", false,  96, 0, -1, -1, 0, SKSOOF},
  { "telept"   , false,  32, 0, -1, -1, 0, -1},
  { "posit1"   ,  true,  98, 0, -1, -1, 0, -1},
  { "posit2"   ,  true,  98, 0, -1, -1, 0, -1},
  { "posit3"   ,  true,  98, 0, -1, -1, 0, -1},
  { "bgsit1"   ,  true,  98, 0, -1, -1, 0, -1},
  { "bgsit2"   ,  true,  98, 0, -1, -1, 0, -1},
  { "sgtsit"   ,  true,  98, 0, -1, -1, 0, -1},
  { "cacsit"   ,  true,  98, 0, -1, -1, 0, -1},
  { "brssit"   ,  true,  94, 0, -1, -1, 0, -1},
  { "cybsit"   ,  true,  92, 0, -1, -1, 0, -1},
  { "spisit"   ,  true,  90, 0, -1, -1, 0, -1},
  { "bspsit"   ,  true,  90, 0, -1, -1, 0, -1},
  { "kntsit"   ,  true,  90, 0, -1, -1, 0, -1},
  { "vilsit"   ,  true,  90, 0, -1, -1, 0, -1},
  { "mansit"   ,  true,  90, 0, -1, -1, 0, -1},
  { "pesit\0"  ,  true,  90, 0, -1, -1, 0, -1},
  { "sklatk"   , false,  70, 0, -1, -1, 0, -1},
  { "sgtatk"   , false,  70, 0, -1, -1, 0, -1},
  { "skepch"   , false,  70, 0, -1, -1, 0, -1},
  { "vilatk"   , false,  70, 0, -1, -1, 0, -1},
  { "claw\0\0" , false,  70, 0, -1, -1, 0, -1},
  { "skeswg"   , false,  70, 0, -1, -1, 0, -1},
  { "pldeth"   , false,  32, 0, -1, -1, 0, SKSPLDETH},
  { "pdiehi"   , false,  32, 0, -1, -1, 0, SKSPDIEHI},
  { "podth1"   , false,  70, 0, -1, -1, 0, -1},
  { "podth2"   , false,  70, 0, -1, -1, 0, -1},
  { "podth3"   , false,  70, 0, -1, -1, 0, -1},
  { "bgdth1"   , false,  70, 0, -1, -1, 0, -1},
  { "bgdth2"   , false,  70, 0, -1, -1, 0, -1},
  { "sgtdth"   , false,  70, 0, -1, -1, 0, -1},
  { "cacdth"   , false,  70, 0, -1, -1, 0, -1},
  { "skldth"   , false,  70, 0, -1, -1, 0, -1},
  { "brsdth"   , false,  32, 0, -1, -1, 0, -1},
  { "cybdth"   , false,  32, 0, -1, -1, 0, -1},
  { "spidth"   , false,  32, 0, -1, -1, 0, -1},
  { "bspdth"   , false,  32, 0, -1, -1, 0, -1},
  { "vildth"   , false,  32, 0, -1, -1, 0, -1},
  { "kntdth"   , false,  32, 0, -1, -1, 0, -1},
  { "pedth\0"  , false,  32, 0, -1, -1, 0, -1},
  { "skedth"   , false,  32, 0, -1, -1, 0, -1},
  { "posact"   ,  true, 120, 0, -1, -1, 0, -1},
  { "bgact\0"  ,  true, 120, 0, -1, -1, 0, -1},
  { "dmact\0"  ,  true, 120, 0, -1, -1, 0, -1},
  { "bspact"   ,  true, 100, 0, -1, -1, 0, -1},
  { "bspwlk"   ,  true, 100, 0, -1, -1, 0, -1},
  { "vilact"   ,  true, 100, 0, -1, -1, 0, -1},
  { "noway\0"  , false,  78, 0, -1, -1, 0, SKSNOWAY},
  { "barexp"   , false,  60, 0, -1, -1, 0, -1},
  { "punch\0"  , false,  64, 0, -1, -1, 0, SKSPUNCH},
  { "hoof\0\0" , false,  70, 0, -1, -1, 0, -1},
  { "metal\0"  , false,  70, 0, -1, -1, 0, -1},
  { "chgun\0"  , false,  64, &S_sfx[sfx_pistol], 150, 0, 0, -1},
  { "tink\0\0" , false,  60, 0, -1, -1, 0, -1},
  { "bdopn\0"  , false, 100, 0, -1, -1, 0, -1},
  { "bdcls\0"  , false, 100, 0, -1, -1, 0, -1},
  { "itmbk\0"  , false, 100, 0, -1, -1, 0, -1},
  { "flame\0"  , false,  32, 0, -1, -1, 0, -1},
  { "flamst"   , false,  32, 0, -1, -1, 0, -1},
  { "getpow"   , false,  60, 0, -1, -1, 0, -1},
  { "bospit"   , false,  70, 0, -1, -1, 0, -1},
  { "boscub"   , false,  70, 0, -1, -1, 0, -1},
  { "bossit"   , false,  70, 0, -1, -1, 0, -1},
  { "bospn\0"  , false,  70, 0, -1, -1, 0, -1},
  { "bosdth"   , false,  70, 0, -1, -1, 0, -1},
  { "manatk"   , false,  70, 0, -1, -1, 0, -1},
  { "mandth"   , false,  70, 0, -1, -1, 0, -1},
  { "sssit\0"  , false,  70, 0, -1, -1, 0, -1},
  { "ssdth\0"  , false,  70, 0, -1, -1, 0, -1},
  { "keenpn"   , false,  70, 0, -1, -1, 0, -1},
  { "keendt"   , false,  70, 0, -1, -1, 0, -1},
  { "skeact"   , false,  70, 0, -1, -1, 0, -1},
  { "skesit"   , false,  70, 0, -1, -1, 0, -1},
  { "skeatk"   , false,  70, 0, -1, -1, 0, -1},
  { "radio\0"  , false,  60, 0, -1, -1, 0, SKSRADIO},

  //added:22-02-98: sound when the player avatar jumps in air 'hmpf!'
  { "jump\0\0" , false,  60, 0, -1, -1, 0, SKSJUMP},
  { "ouch\0\0" , false,  64, 0, -1, -1, 0, SKSOUCH},

  //added:09-08-98:test water sounds
  { "gloop\0"  , false,  60, 0, -1, -1, 0, -1},
  { "splash"   , false,  64, 0, -1, -1, 0, -1},
  { "floush"   , false,  64, 0, -1, -1, 0, -1},

// heretic sounds

  { "gldhit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "gntful",  false, 32, NULL, -1, -1, NULL, -1 },
  { "gnthit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "gntpow",  false, 32, NULL, -1, -1, NULL, -1 },
//  { "gntact",  false, 32, NULL, -1, -1, NULL, -1 },
  { "gntuse",  false, 32, NULL, -1, -1, NULL, -1 },
  { "phosht",  false, 32, NULL, -1, -1, NULL, -1 },
  { "phohit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "-phopow", false, 32, &S_sfx[sfx_hedat1], -1, -1, NULL, -1 },
  { "lobsht",  false, 20, NULL, -1, -1, NULL, -1 },
  { "lobhit",  false, 20, NULL, -1, -1, NULL, -1 },
  { "lobpow",  false, 20, NULL, -1, -1, NULL, -1 },
  { "hrnsht",  false, 32, NULL, -1, -1, NULL, -1 },
  { "hrnhit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "hrnpow",  false, 32, NULL, -1, -1, NULL, -1 },
  { "ramphit", false, 32, NULL, -1, -1, NULL, -1 },
  { "ramrain", false, 10, NULL, -1, -1, NULL, -1 },
  { "bowsht",  false, 32, NULL, -1, -1, NULL, -1 },
  { "stfhit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "stfpow",  false, 32, NULL, -1, -1, NULL, -1 },
  { "stfcrk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "impsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "impat1",  false, 32, NULL, -1, -1, NULL, -1 },
  { "impat2",  false, 32, NULL, -1, -1, NULL, -1 },
  { "impdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "-impact", false, 20, &S_sfx[sfx_impsit], -1, -1, NULL, -1 },
  { "imppai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "mumsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "mumat1",  false, 32, NULL, -1, -1, NULL, -1 },
  { "mumat2",  false, 32, NULL, -1, -1, NULL, -1 },
  { "mumdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "-mumact", false, 20, &S_sfx[sfx_mumsit], -1, -1, NULL, -1 },
  { "mumpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "mumhed",  false, 32, NULL, -1, -1, NULL, -1 },
  { "bstsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "bstatk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "bstdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "bstact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "bstpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "clksit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "clkatk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "clkdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "clkact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "clkpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "snksit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "snkatk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "snkdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "snkact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "snkpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "kgtsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "kgtatk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "kgtat2",  false, 32, NULL, -1, -1, NULL, -1 },
  { "kgtdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "-kgtact", false, 20, &S_sfx[sfx_kgtsit], -1, -1, NULL, -1 },
  { "kgtpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "wizsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "wizatk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "wizdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "wizact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "wizpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "minsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "minat1",  false, 32, NULL, -1, -1, NULL, -1 },
  { "minat2",  false, 32, NULL, -1, -1, NULL, -1 },
  { "minat3",  false, 32, NULL, -1, -1, NULL, -1 },
  { "mindth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "minact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "minpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "hedsit",  false, 32, NULL, -1, -1, NULL, -1 },
  { "hedat1",  false, 32, NULL, -1, -1, NULL, -1 },
  { "hedat2",  false, 32, NULL, -1, -1, NULL, -1 },
  { "hedat3",  false, 32, NULL, -1, -1, NULL, -1 },
  { "heddth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "hedact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "hedpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "sorzap",  false, 32, NULL, -1, -1, NULL, -1 },
  { "sorrise", false, 32, NULL, -1, -1, NULL, -1 },
  { "sorsit",  false, 200,NULL, -1, -1, NULL, -1 },
  { "soratk",  false, 32, NULL, -1, -1, NULL, -1 },
  { "soract",  false, 200,NULL, -1, -1, NULL, -1 },
  { "sorpai",  false, 200,NULL, -1, -1, NULL, -1 },
  { "sordsph", false, 200,NULL, -1, -1, NULL, -1 },
  { "sordexp", false, 200,NULL, -1, -1, NULL, -1 },
  { "sordbon", false, 200,NULL, -1, -1, NULL, -1 },
  { "-sbtsit", false, 32, &S_sfx[sfx_bstsit], -1, -1, NULL, -1 },
  { "-sbtatk", false, 32, &S_sfx[sfx_bstatk], -1, -1, NULL, -1 },
  { "sbtdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "sbtact",  false, 20, NULL, -1, -1, NULL, -1 },
  { "sbtpai",  false, 32, NULL, -1, -1, NULL, -1 },
//  { "plroof",  false, 32, NULL, -1, -1, NULL, -1 },
  { "plrpai",  false, 32, NULL, -1, -1, NULL, -1 },
  { "plrdth",  false, 80, NULL, -1, -1, NULL, -1 },
  { "gibdth",  false, 100,NULL, -1, -1, NULL, -1 },
  { "plrwdth", false, 80, NULL, -1, -1, NULL, -1 },
  { "plrcdth", false, 100,NULL, -1, -1, NULL, -1 },
  { "itemup",  false, 32, NULL, -1, -1, NULL, -1 },
  { "wpnup",   false, 32, NULL, -1, -1, NULL, -1 },
//  { "telept",  false, 50, NULL, -1, -1, NULL, -1 },
  { "doropn",  false, 40, NULL, -1, -1, NULL, -1 },
  { "dorcls",  false, 40, NULL, -1, -1, NULL, -1 },
  { "dormov",  false, 40, NULL, -1, -1, NULL, -1 },
  { "artiup",  false, 32, NULL, -1, -1, NULL, -1 },
//  { "switch",  false, 40, NULL, -1, -1, NULL, -1 },
  { "pstart",  false, 40, NULL, -1, -1, NULL, -1 },
  { "pstop",   false, 40, NULL, -1, -1, NULL, -1 },
  { "stnmov",  false, 40, NULL, -1, -1, NULL, -1 },
  { "chicpai", false, 32, NULL, -1, -1, NULL, -1 },
  { "chicatk", false, 32, NULL, -1, -1, NULL, -1 },
  { "chicdth", false, 40, NULL, -1, -1, NULL, -1 },
  { "chicact", false, 32, NULL, -1, -1, NULL, -1 },
  { "chicpk1", false, 32, NULL, -1, -1, NULL, -1 },
  { "chicpk2", false, 32, NULL, -1, -1, NULL, -1 },
  { "chicpk3", false, 32, NULL, -1, -1, NULL, -1 },
  { "keyup"  , false, 50, NULL, -1, -1, NULL, -1 },
  { "ripslop", false, 16, NULL, -1, -1, NULL, -1 },
  { "newpod" , false, 16, NULL, -1, -1, NULL, -1 },
  { "podexp" , false, 40, NULL, -1, -1, NULL, -1 },
  { "bounce" , false, 16, NULL, -1, -1, NULL, -1 },
  { "-volsht", false, 16, &S_sfx[sfx_bstatk], -1, -1, NULL, -1 },
  { "-volhit", false, 16, &S_sfx[sfx_lobhit], -1, -1, NULL, -1 },
  { "burn"   , false, 10, NULL, -1, -1, NULL, -1 },
  { "splash" , false, 10, NULL, -1, -1, NULL, -1 },
  { "gloop"  , false, 10, NULL, -1, -1, NULL, -1 },
//  { "respawn", false, 10, NULL, -1, -1, NULL, -1 },
  { "blssht" , false, 32, NULL, -1, -1, NULL, -1 },
  { "blshit" , false, 32, NULL, -1, -1, NULL, -1 },
//  { "chat"   , false, 100,NULL, -1, -1, NULL, -1 },
  { "artiuse", false, 32, NULL, -1, -1, NULL, -1 },
  { "gfrag"  , false, 100,NULL, -1, -1, NULL, -1 },
  { "waterfl", false, 16, NULL, -1, -1, NULL, -1 },

  // Monophonic sounds

  { "wind"   , false, 16, NULL, -1, -1, NULL, -1 },
  { "amb1"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb2"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb3"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb4"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb5"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb6"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb7"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb8"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb9"   , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb10"  , false,  1, NULL, -1, -1, NULL, -1 },
  { "amb11"  , false,  1, NULL, -1, -1, NULL, -1 }
  // skin sounds free slots to add sounds at run time (Boris HACK!!!)
  // initialized to NULL
};


// Prepare free sfx slots to add sfx at run time
void S_InitRuntimeSounds (void)
{
    int  i;

    for (i=sfx_freeslot0; i<=sfx_lastfreeslot; i++)
        S_sfx[i].name = NULL;
}

// Add a new sound fx into a free sfx slot.
//
int S_AddSoundFx (char *name,int singularity)
{
    int i;

    for(i=sfx_freeslot0;i<NUMSFX;i++)
    {
        if(!S_sfx[i].name)
        {
            S_sfx[i].name=(char *)Z_Malloc(7,PU_STATIC,NULL);
            strncpy(S_sfx[i].name,name,6);
            S_sfx[i].name[6]='\0';
            S_sfx[i].singularity=singularity;
            S_sfx[i].priority=60;
            S_sfx[i].link=0;
            S_sfx[i].pitch=-1;
            S_sfx[i].volume=-1;
            S_sfx[i].lumpnum=-1;
            S_sfx[i].skinsound=-1;
            S_sfx[i].usefulness=-1;

            // if precache load it here ! todo !
            S_sfx[i].data=0;
            return i;
        }
    }
    CONS_Printf("\2No more free sound slots\n");
    return 0;
}


void S_RemoveSoundFx (int id)
{
    if (id>=sfx_freeslot0 &&
        id<=sfx_lastfreeslot &&
        S_sfx[id].name)
    {
        Z_Free(S_sfx[id].name);
        S_sfx[id].name=NULL;
        S_sfx[id].lumpnum=-1;
        I_FreeSfx(&S_sfx[id]);
    }
}


//
// S_AddMusic
// Adds a single song to the runtime songs.
int S_AddMusic(char *name)
{
  int    i;
  char   lumpname[9];

  sprintf(lumpname, "d_%.6s", name);

  for(i = mus_firstfreeslot; i < mus_lastfreeslot; i++)
  {
    if(S_music[i].name == NULL)
    {
      S_music[i].name = Z_Strdup(name, PU_STATIC, 0);
      S_music[i].lumpnum = W_GetNumForName(lumpname);
      S_music[i].data = 0;
      return i;
    }
  }

  CONS_Printf("All music slots are full!\n");
  return 0;
}


int S_FindMusic(char *name)
{
  int   i;

  for(i = 0; i < NUMMUSIC; i++)
  {
    if(!S_music[i].name)
      continue;
    if(!stricmp(name, S_music[i].name)) return i;
  }

  return S_AddMusic(name);
}
