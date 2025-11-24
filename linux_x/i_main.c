// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: i_main.c,v 1.4 2002/09/10 19:30:11 hurdler Exp $
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
// $Log: i_main.c,v $
// Revision 1.4  2002/09/10 19:30:11  hurdler
// Add log file under Linux
//
// Revision 1.3  2000/04/23 16:19:52  bpereira
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
//      Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"

#ifdef LOGMESSAGES
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int logstream;
#endif

int main(int argc, char **argv)
{ 
    myargc = argc; 
    myargv = argv;

#ifdef LOGMESSAGES
    //Hurdler: only write log if we have the permission in the legacy executable directory
    logstream = open("log.txt", O_CREAT | O_WRONLY);
    if (logstream < 0)
    {
        logstream = INVALID_HANDLE_VALUE; // so we haven't to change the current source code
    }
#endif
 
    D_DoomMain (); 
    D_DoomLoop ();
    return 0;
} 
