// Emacs style mode select -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: t_func.h,v 1.2 2003/05/30 22:44:07 hurdler Exp $
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
// $Log: t_func.h,v $
// Revision 1.2  2003/05/30 22:44:07  hurdler
// add checkcvar function to FS
//
// Revision 1.1  2000/11/02 17:57:28  stroggonmeth
// FraggleScript files...
//
//
//--------------------------------------------------------------------------


#ifndef __FUNC_H__
#define __FUNC_H__

#include "t_parse.h"

#include "p_local.h"

extern camera_t script_camera;
extern boolean  script_camera_on;

void init_functions();

#define AngleToFixed(x)  (((double) x) / ((double) ANG45/45)) * FRACUNIT
#define FixedToAngle(x)  (((double) x) / FRACUNIT) * ANG45/45;

#endif
