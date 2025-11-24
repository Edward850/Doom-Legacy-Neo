// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: usleep.c,v 1.2 2000/02/27 00:42:12 hurdler Exp $
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
// $Log: usleep.c,v $
// Revision 1.2  2000/02/27 00:42:12  hurdler
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


#include <signal.h>
#include <sys/types.h>

#ifdef SCOOS5
#include <sys/itimer.h>
#endif

#if defined(SCOUW2) || defined(SCOUW7)
#include <sys/time.h>
#endif

#include "usleep.h"

extern int pause (void);
extern pid_t getpid (void);

volatile static int waiting;

static void getalrm(i)
  int i;
{
    waiting = 0;
}

void usleep(t)
  unsigned t;
{
    static struct itimerval it, ot;
    void (*oldsig)();
    long nt;

    it.it_value.tv_sec = t / 1000000;
    it.it_value.tv_usec = t % 1000000;
    oldsig = (void (*)()) signal(SIGALRM, getalrm);
    waiting = 1;
    if (setitimer(ITIMER_REAL, &it, &ot))
        return /*error*/;
    while (waiting) {
        pause();
    }
    signal(SIGALRM, oldsig);
    if (ot.it_value.tv_sec + ot.it_value.tv_usec > 0) {
      nt = ((ot.it_value.tv_sec * 1000000L) + ot.it_value.tv_usec) - t;
      if (nt <= 0) {
        kill(getpid(), SIGALRM);
      } else {
        ot.it_value.tv_sec = nt / 1000000;
        ot.it_value.tv_usec = nt % 1000000;
        setitimer(ITIMER_REAL, &ot, 0);
      }
    }
}
