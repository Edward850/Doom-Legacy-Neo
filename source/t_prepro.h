// Emacs style mode select -*- C++ -*-
//----------------------------------------------------------------------------
//
// $Id: t_prepro.h,v 1.4 2003/05/04 04:19:06 sburke Exp $
//
// Copyright(C) 2000 Simon Howard
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Log: t_prepro.h,v $
// Revision 1.4  2003/05/04 04:19:06  sburke
// Don't typedef label_t on Solaris.
//
// Revision 1.3  2003/01/19 21:24:26  bock
// Make sources buildable on FreeBSD 5-CURRENT.
//
// Revision 1.2  2001/05/16 22:33:34  bock
// Initial FreeBSD support.
//
// Revision 1.1  2000/11/02 17:57:28  stroggonmeth
// FraggleScript files...
//
//
//--------------------------------------------------------------------------


#ifndef __PREPRO_H__
#define __PREPRO_H__

#ifdef FREEBSD
#if __FreeBSD__ < 5
#include <machine/types.h>
#endif
#endif
typedef struct section_s section_t;
#if !defined(FREEBSD) && !defined(SOLARIS)
typedef struct label_s label_t;
#elif __FreeBSD__ > 4
typedef struct label_s label_t;
#endif
#define SECTIONSLOTS 17
#define LABELSLOTS 17

#include "t_parse.h"

void preprocess(script_t *script);

/***** {} sections **********/

section_t *find_section_start(char *brace);
section_t *find_section_end(char *brace);

struct section_s
{
  char *start;    // offset of starting brace {
    char *end;      // offset of ending brace   }
  int type;       // section type: for() loop, while() loop etc
  
  union
  {
    struct
    {
      char *loopstart;  // positioned before the while()
    } data_loop;
  } data; // data for section
  
  section_t *next;        // for hashing
};

enum    // section types
{
  st_empty,       // none: empty {} braces
  st_if,          // if() statement
  st_elseif,      // elseif() statement
  st_else,        // else() statement
  st_loop,        // loop
};

/****** goto labels ***********/

label_t *labelforname(char *labelname);

#endif

//---------------------------------------------------------------------------
//
// $Log: t_prepro.h,v $
// Revision 1.4  2003/05/04 04:19:06  sburke
// Don't typedef label_t on Solaris.
//
// Revision 1.3  2003/01/19 21:24:26  bock
// Make sources buildable on FreeBSD 5-CURRENT.
//
// Revision 1.2  2001/05/16 22:33:34  bock
// Initial FreeBSD support.
//
// Revision 1.1  2000/11/02 17:57:28  stroggonmeth
// FraggleScript files...
//
// Revision 1.1.1.1  2000/04/30 19:12:09  fraggle
// initial import
//
//
//---------------------------------------------------------------------------

