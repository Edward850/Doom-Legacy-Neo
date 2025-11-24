// Emacs style mode select -*- C++ -*-
//----------------------------------------------------------------------------
//
// $Id: t_spec.h,v 1.2 2003/05/30 22:44:07 hurdler Exp $
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
// $Log: t_spec.h,v $
// Revision 1.2  2003/05/30 22:44:07  hurdler
// add checkcvar function to FS
//
// Revision 1.1  2000/11/02 17:57:28  stroggonmeth
// FraggleScript files...
//
//
//--------------------------------------------------------------------------


#ifndef __SPEC_H__
#define __SPEC_H__

void spec_brace();

int  spec_if();  //SoM: returns weather or not the if statement was true.
int  spec_elseif(boolean lastif);
void spec_else(boolean lastif);
void spec_while();
void spec_for();
void spec_goto();

// variable types

boolean spec_variable();

void spec_script();     // in t_script.c btw

#endif
