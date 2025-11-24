#############################################################################
#
# $Id: makefile,v 1.74 2004/04/17 12:55:26 hurdler Exp $
#     GNU Make makefile for DOS/LINUX version
#			       of Doom LEGACY (legacy@newdoom.com)
#
# Copyright (C) 1998-2000 by DooM Legacy Team.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# $Log: makefile,v $
# Revision 1.74  2004/04/17 12:55:26  hurdler
# now compile with gcc 3.3.3 under Linux
#
# Revision 1.73  2003/08/11 13:50:01  hurdler
# go final + translucent HUD + fix spawn in net game
#
# Revision 1.72  2003/05/04 02:15:47  sburke
# Added Solaris build rules.
#
# Revision 1.71  2003/01/19 21:24:26  bock
# Make sources buildable on FreeBSD 5-CURRENT.
#
# Revision 1.70  2002/12/17 19:56:50  bock
# Add esound support without SDL.
# Sorry about this, this code is disabled by default.
# You should build with HAVE_ESD=1 flag.
#
# Revision 1.69  2002/10/30 23:50:03  bock
# Fix CR+LF
#
# Revision 1.68  2002/09/27 16:40:08  tonyd
# First commit of acbot
#
# Revision 1.67  2002/09/07 12:23:19  hurdler
# Ok, that option work on my config, should we put dga as a cvar?
#
# Revision 1.66  2002/07/26 15:21:36  hurdler
# near RC release
#
# Revision 1.65  2002/07/01 19:59:58  metzgermeister
# *** empty log message ***
#
# Revision 1.64  2002/06/30 21:37:48  hurdler
# Ready for 1.32 beta 5 release
#
# Revision 1.63  2002/01/03 19:20:06  bock
# Add FreeBSD code to I_GetFreeMem.
# Modified Files:
#     makefile linux_x/i_system.c sdl/i_system.c
#
# Revision 1.62  2001/08/20 20:40:39  metzgermeister
# *** empty log message ***
#
# Revision 1.61  2001/05/29 22:18:41  bock
# Small BSD commit: build r_opengl.so, sndserver
#
# Revision 1.60  2001/05/21 14:57:05  crashrl
# Readded directory crawling file search function
#
# Revision 1.59  2001/05/17 14:52:07  crashrl
# Fixed misplaced else concerning i_cdmus and FREEBSD
#
# Revision 1.58  2001/05/16 22:33:34  bock
# Initial FreeBSD support.
#
# Revision 1.57  2001/05/16 17:12:52  crashrl
# Added md5-sum support, removed recursiv wad search
#
# Revision 1.56  2001/05/05 20:34:08  hurdler
# restore old value again
#
# Revision 1.55  2001/05/03 21:22:25  hurdler
# remove some warnings
#
# Revision 1.53  2001/04/29 14:25:26  hurdler
# small fix
#
# Revision 1.52  2001/04/13 10:24:49  metzgermeister
# compile sndserver and musserver
#
# Revision 1.51  2001/04/01 17:35:06  bpereira
# no message
#
# Revision 1.50  2001/03/30 17:12:50  bpereira
# no message
#
# Revision 1.49  2001/03/25 18:11:23  metzgermeister
#   * SDL sound bug with swapped stereo channels fixed
#   * separate hw_trick.c now for HW_correctSWTrick(.)
#
# Revision 1.48  2001/03/19 21:18:48  metzgermeister
#   * missing textures in HW mode are replaced by default texture
#   * fixed crash bug with P_SpawnMissile(.) returning NULL
#   * deep water trick and other nasty thing work now in HW mode (tested with tnt/map02 eternal/map02)
#   * added cvar gr_correcttricks
#
# Revision 1.47  2001/03/13 22:14:19  stroggonmeth
# Long time no commit. 3D floors, FraggleScript, portals, ect.
#
# Revision 1.46  2001/03/09 21:53:55  metzgermeister
# *** empty log message ***
#
# Revision 1.44  2001/02/13 20:37:27  metzgermeister
# *** empty log message ***
#
# Revision 1.43  2001/02/10 15:24:19  hurdler
# Apply Rob's patch for Linux version
#
# Revision 1.42  2001/01/25 22:15:43  bpereira
# added heretic support
#
# Revision 1.41  2000/11/21 21:13:17  stroggonmeth
# Optimised 3D floors and fixed crashing bug in high resolutions.
#
# Revision 1.40  2000/11/09 17:56:20  stroggonmeth
# Hopefully fixed a few bugs and did a few optimizations.
#
# Revision 1.39  2000/11/06 20:52:15  bpereira
# no message
#
# Revision 1.38  2000/11/03 03:27:17  stroggonmeth
# Again with the bug fixing...
#
# Revision 1.37  2000/11/02 21:54:26  bpereira
# no message
#
# Revision 1.36  2000/11/02 19:49:35  bpereira
# no message
#
# Revision 1.35  2000/11/02 17:50:07  stroggonmeth
# Big 3Dfloors & FraggleScript commit!!
#
# Revision 1.34  2000/10/21 23:21:56  hurdler
# Minor updates
#
# Revision 1.33  2000/10/07 12:45:59  crashrl
# added -fwritable-strings to linux CFLAGS to fix teamplay activation segfault
#
# Revision 1.32  2000/10/01 10:23:59  bpereira
# no message
#
# Revision 1.31  2000/10/01 10:18:17  bpereira
# no message
#
# Revision 1.30  2000/09/28 20:57:15  bpereira
# no message
#
# Revision 1.29  2000/09/10 10:44:42  metzgermeister
# resolved SDL messup
#
# Revision 1.28  2000/08/31 14:30:55  bpereira
# no message
#
# Revision 1.27  2000/08/10 19:58:04  bpereira
# no message
#
# Revision 1.26  2000/08/10 14:57:26  ydario
# OS/2 port
#
# Revision 1.25  2000/08/03 17:57:42  bpereira
# no message
#
# Revision 1.24  2000/07/01 09:23:49  bpereira
# no message
#
# Revision 1.23  2000/05/07 08:27:57  metzgermeister
# no message
#
# Revision 1.20  2000/04/25 19:53:12  metzgermeister
# fixed the old tabs and spaces problem
#
# Revision 1.19  2000/04/24 20:24:38  bpereira
# no message
#
# Revision 1.18  2000/04/22 20:26:01  metzgermeister
# commandline compile options
#
# Revision 1.17  2000/04/21 08:23:47  emanne
# To have SDL working.
# Makefile: made the hiding by "@" optional. See the CC variable at
# the begining. Sorry, but I like to see what's going on while building
#
# qmus2mid.h: force include of qmus2mid_sdl.h when needed.
# s_sound.c: ??!
# s_sound.h: with it.
# (sorry for s_sound.* : I had problems with cvs...)
#
# Revision 1.16  2000/04/18 14:50:36  hurdler
# no message
#
# Revision 1.15  2000/04/16 18:38:07  bpereira
# no message
#
# Revision 1.14  2000/04/13 23:47:47  stroggonmeth
# See logs
#
# Revision 1.13  2000/04/12 19:32:04  metzgermeister
# added support for commandline OS switch
#
# Revision 1.12  2000/04/11 19:07:23  stroggonmeth
# Finished my logs, fixed a crashing bug.
#
# Revision 1.11  2000/04/09 02:30:57  stroggonmeth
# Fixed missing sprite def :\
#
# Revision 1.10  2000/04/08 17:29:24  stroggonmeth
# no message
#
# Revision 1.9	2000/04/07 23:10:15  metzgermeister
# fullscreen support under X in Linux
#
# Revision 1.8	2000/04/04 19:28:42  stroggonmeth
# Global colormaps working. Added a new linedef type 272.
#
# Revision 1.7	2000/04/04 00:32:46  stroggonmeth
# Initial Boom compatability plus few misc changes all around.
#
# Revision 1.6	2000/03/29 19:39:48  bpereira
# no message
#
# Revision 1.5	2000/03/07 03:32:24  hurdler
# fix linux compilation
#
# Revision 1.4	2000/03/05 17:17:03  bpereira
# no message
#
# Revision 1.3	2000/03/04 23:27:08  hurdler
# fix "missing operator" problem
#
# Revision 1.2	2000/02/27 00:42:10  hurdler
# fix CR+LF problem
#
# Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
# Initial import into CVS (v1.29 pr3)
#
#
#     -DUSEASM -> use assembly routines instead of C ones, where possible.
#     -DPC_DOS -> use dos specific code (eg:textmode stuff)...
#     -DLINUX  -> use for the linux specific
#     -D__OS2__  -> use for the OS/2 specific
#
#     Compile the Linux version with 'make LINUX=1'
#     Compile the FreeBSD version with 'gmake FREEBSD=1'
#     Compile the Solaris/SDL version with 'make SOLARIS=1'
#     Compile the OS/2 version with  'make OS2=1'
#     Compile a debug/profile version with 'make LINUX=1 PROFILEMODE=1'
#     Compile the SDL version with 'make LINUX=1 HAVE_MIXER=1'
#     Compile the SDL/FreeBSD version with 'gmake FREEBSD=1 FBSD_SDL=1'
#
#############################################################################

# gcc or g++
#CC=@gcc
CC=gcc

#if use PGCC or EGCS
PGCC=1

WFLAGS=-Wall # -W -Wno-unused -Wno-sign-compare

#uncomment if you want to use the POLL_POINTER hack
#POLL_POINTER=-DPOLL_POINTER

#WITH_DGA=1

#indicate platform and what interface use with
ifndef OS2
ifndef LINUX
ifndef SDL
ifndef FREEBSD
ifndef SOLARIS
	DJGPPDOS=1
endif
endif
endif
endif
endif

#determine the interface directory (where you put all i_*.c)
ifdef DJGPPDOS
	INTERFACE=DJGPPDOS
	NASMFORMAT=coff
else
	ifdef LINUX
		INTERFACE=linux_x
		NASMFORMAT=elf -DLINUX
	endif
	ifdef FREEBSD
		INTERFACE=linux_x
		NASMFORMAT=elf -DLINUX
	endif
	ifdef SOLARIS
		INTERFACE=sdl
		NASMFORMAT=elf -DLINUX
	endif
	ifdef OS2
		INTERFACE=os2
		NASMFORMAT=coff
	endif
	ifdef SDL
		INTERFACE=sdl
	endif
endif

ifdef DJGPPDOS
	# subdirectory for objects

	O=objs
	BIN=bin

	#use the x86 asm code
    #USEASM=1

	# options
    OPTS=-DPC_DOS -g

	DEBUGLIBS=-lalleg -lbcd -lsocket
    LIBS=-lalleg -lbcd -lsocket

	SFLAGS=

    CFLAGS=-g

	OBJS=$(O)/i_video.o $(O)/vid_vesa.o

	# name of the exefile
	EXENAME=DOOM3.exe

endif #ifdef DJGPPDOS

ifdef LINUX

	# subdirectory for objects
	O=../objs
	BIN=../bin
	MUSSERV=linux_x/musserv
	SNDSERV=linux_x/sndserv

	#use the x86 asm code
	#USEASM=1

	OPTS=-DLINUX -DOLD_SOUND_DRIVER -DMUSSERV -DSNDSERV -DHWRENDER -DDIRECTFULLSCREEN
	X=1

	ifdef X

	    OPTS := $(OPTS) -DVID_X11 $(POLL_POINTER) -I. -I/usr/X11R6/include
	    LDFLAGS=-L/usr/X11R6/lib
	    LIBS=-lXext -lX11 -lm -ldl -lXxf86vm
	    ifdef WITH_DGA
		OPTS += -DWITH_DGA
		LIBS += -lXxf86dga -lXext
	    endif #WITH_DGA
	    # name of the exefile
	    EXENAME=llxdoom
	    SFLAGS=-g $(OPTS)
	    OBJS=$(O)/i_video_xshm.o $(O)/dosstr.o $(O)/searchp.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o

	else
	    OPTS := $(OPTS) -DVID_GGI -I.
	    LDFLAGS=
	    LIBS=-lggi -lm
	    # name of the exefile
	    EXENAME=llggidoom
	    SFLAGS=-g $(OPTS)
	    OBJS=$(O)/i_video_ggi.o $(O)/dosstr.o $(O)/searchp.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o

	endif
endif

ifdef FREEBSD

	# subdirectory for objects
	O=../objs
	BIN=../bin
	MUSSERV=linux_x/musserv
	SNDSERV=linux_x/sndserv
	OSVERSION:=$(shell /sbin/sysctl -n kern.osreldate)
	OSMAJOR:=$(shell echo $(OSVERSION) | sed 's/.....$$//')
	ifeq ($(OSMAJOR), 5)
		GCC30=1
	endif

	#use the x86 asm code
	#USEASM=1
ifndef FBSD_SDL
	OPTS=-DLINUX -DFREEBSD -DOLD_SOUND_DRIVER -DSNDSERV -DHWRENDER -DDIRECTFULLSCREEN
ifndef FBSD_GGI
	X=1
endif
endif

	ifdef X

	    OPTS := $(OPTS) -DVID_X11 $(POLL_POINTER) -I. -I/usr/X11R6/include
	    LDFLAGS=-L/usr/X11R6/lib
	    LIBS=-lXext -lX11 -lm -lXxf86vm -lipx -lkvm -pthread
	    # name of the exefile
	    EXENAME=llxdoom
	    SFLAGS=-g $(OPTS)
	    OBJS=$(O)/i_video_xshm.o $(O)/dosstr.o $(O)/searchp.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o

	else #ifdef X
	ifdef FBSD_SDL
            INTERFACE=sdl
	    OPTS := -DLINUX -DFREEBSD -DHWRENDER -DDIRECTFULLSCREEN -DHAVE_MIXER -DSDL -DVID_X11 $(POLL_POINTER) -I. -I/usr/local/include/SDL11 -I/usr/X11R6/include -Wall
	    SFLAGS=-g $(OPTS)
	    OBJS=$(O)/i_video.o $(O)/dosstr.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o \
		 $(O)/r_opengl.o $(O)/ogl_sdl.o $(O)/hwsym_sdl.o

	    DEBUGLIBS=$(LIBS)

	    LDFLAGS=-L/usr/X11R6/lib -L/usr/local/lib
	    LIBS=-lm -lSDL-1.1 -lSDL_mixer -lGL -lGLU -lipx -lkvm -pthread
	    DEBUGLIBS=$(LIBS)

	    # name of the exefile
	    EXENAME=lsdldoom

	else #ifdef FBSD_SDL
	    OPTS := $(OPTS) -DVID_GGI -I. -I/usr/local/include
	    LDFLAGS=
	    LIBS=-lggi -lm -lipx
	    # name of the exefile
	    EXENAME=llggidoom
	    SFLAGS=-g $(OPTS)
	    OBJS=$(O)/i_video_ggi.o $(O)/dosstr.o $(O)/searchp.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o

	endif #ifdef FBSD_SDL
	endif #ifdef X
endif #ifdef FREEBSD

ifdef SOLARIS

	# subdirectory for objects
	O=../objs
	BIN=../bin
	OPTS := -DLINUX -DHWRENDER -DDIRECTFULLSCREEN -DHAVE_MIXER -DSDL -DVID_X11 
	OPTS := $(OPTS) -I. -I/usr/local/include -I/usr/X11R6/include -Wall
	OPTS := $(OPTS) -DBSD_COMP -DSOLARIS 
	OPTS := $(OPTS) -DDONTDEFINEBOOL -DTRUE=1 -DFALSE=0 -DINADDR_NONE=INADDR_ANY 
	OPTS := $(OPTS) -D__BIG_ENDIAN__

	SFLAGS=-g $(OPTS)

	OBJS=$(O)/i_video.o $(O)/dosstr.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o \
		 $(O)/r_opengl.o $(O)/ogl_sdl.o $(O)/hwsym_sdl.o


	LDFLAGS = -L/usr/X/lib -L/usr/local/lib
	LIBS    = -lSDL -lSDL_mixer -lGL -lGLU -lpthread -lsocket -lnsl -lm

	# name of the exefile
	EXENAME=lsdldoom


endif #ifdef SOLARIS


ifdef SDL
	# subdirectory for objects
	O=../objs
	BIN=../bin
	OPTS := $(OPTS) -I.
	SFLAGS=-g $(OPTS)
	OBJS=$(O)/i_video.o $(O)/dosstr.o $(O)/endtxt.o \
		 $(O)/hw_bsp.o $(O)/hw_draw.o $(O)/hw_light.o $(O)/hw_main.o \
		 $(O)/hw_md2.o $(O)/hw_cache.o $(O)/hw_trick.o \
		 $(O)/r_opengl.o $(O)/ogl_sdl.o $(O)/hwsym_sdl.o

	DEBUGLIBS=$(LIBS)
	OPTS := -DLINUX -DHWRENDER -DDIRECTFULLSCREEN -DHAVE_MIXER -DSDL -DVID_X11 $(POLL_POINTER) -I. -I/usr/local/include/SDL -I/usr/X11R6/include -Wall

	LDFLAGS=-L/usr/X11R6/lib -L/usr/local/lib
	LIBS=-lm -lSDL -lSDL_mixer -lpthread -lGL -lGLU
	DEBUGLIBS=$(LIBS)

	# name of the exefile
	EXENAME=lsdldoom
endif

ifdef OS2

	# subdirectory for objects
	O=objs
	BIN=bin

	#use the x86 asm code
	NASM=nasm
	#USEASM=1
	PROFILEMODE=1
	# options
	OPTS=-Zmt -D__OS2__ -I.. -I. -Ios2

	DEBUGLIBS=-lmmpm2 -lsocket -g
	LIBS= $(INTERFACE)/I_pm.def -lmmpm2 -lsocket -g

	SFLAGS=-g $(OPTS)

	OBJS=$(O)/i_video.o $(O)/i_dart.o $(O)/i_dive.o $(O)/I_pm.o \
		$(O)/printf.o $(O)/I_pm.res

	# name of the exefile
	EXENAME=Doom2.exe

endif

ifdef USEASM
   OBJS:= $(OBJS) $(O)/tmap.o $(O)/vid_copy.o
   OPTS:= $(OPTS) -DUSEASM
endif

ifndef SOLARIS
ifdef GCC30
	M5=-march=pentium
	M4=-march=486
else
	M5=-mpentium
	M4=-m486
endif
endif

ifdef PROFILEMODE

	# build with profiling information
	ifdef PGCC
		CFLAGS = -g -pg -DPARANOIA -fwritable-strings $(OPTS)
	else
		CFLAGS = -g -pg $(M4) -O3 -ffast-math -fwritable-strings $(OPTS)
	endif
	LDFLAGS = -g -pg -L/usr/X11R6/lib
else


	# build a normal optimised version
	ifdef PGCC
		CFLAGS = $(OPTS) $(M5) -O6 -ffast-math -fomit-frame-pointer -fwritable-strings
		# -fgcse -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop -ffunction-cse \
		# -fexpensive-optimizations -fforce-mem -fstrength-reduce -fthread-jumps  \
		# -fregmove -fschedule-insns -fschedule-insns2 -fmove-all-movables \
		# -fcaller-saves -finline-functions -mieee-fp -mfp-ret-in-387 -m80387 -mhard-float -finline
	else
		CFLAGS = $(M4) -O3 -ffast-math -fomit-frame-pointer -fwritable-strings $(OPTS)
	endif
endif

ifdef FREEBSD
ifndef FBSD_SDL
	i_cdmus_o=$(O)/i_cdmus_fbsd.o
else
	i_cdmus_o=$(O)/i_cdmus.o
endif
else
	i_cdmus_o=$(O)/i_cdmus.o
endif

SNDSERV_EXTRAFLAGS:=
ifdef HAVE_ESD
SNDSERV_EXTRAFLAGS:= HAVE_ESD=1
endif

# not too sophisticated dependency
OBJS :=  $(OBJS) \
		$(O)/dstrings.o  \
		$(i_cdmus_o)	 \
		$(O)/i_net.o	 \
		$(O)/i_tcp.o	 \
		$(O)/i_system.o  \
		$(O)/i_sound.o	 \
		$(O)/tables.o	 \
		$(O)/f_finale.o  \
		$(O)/f_wipe.o	 \
		$(O)/d_main.o	 \
		$(O)/d_net.o	 \
		$(O)/d_items.o	 \
		$(O)/g_game.o	 \
		$(O)/m_menu.o	 \
		$(O)/m_misc.o	 \
		$(O)/m_argv.o	 \
		$(O)/m_bbox.o	 \
		$(O)/m_fixed.o	 \
		$(O)/m_swap.o	 \
		$(O)/m_cheat.o	 \
		$(O)/m_random.o  \
		$(O)/am_map.o	 \
		$(O)/p_ceilng.o  \
		$(O)/p_doors.o	 \
		$(O)/p_enemy.o	 \
		$(O)/p_floor.o	 \
		$(O)/p_genlin.o  \
		$(O)/p_info.o	 \
		$(O)/p_inter.o	 \
		$(O)/p_lights.o  \
		$(O)/p_map.o	 \
		$(O)/p_maputl.o  \
		$(O)/p_plats.o	 \
		$(O)/p_pspr.o	 \
		$(O)/p_setup.o	 \
		$(O)/p_sight.o	 \
		$(O)/p_spec.o	 \
		$(O)/p_switch.o  \
		$(O)/p_mobj.o	 \
		$(O)/p_telept.o  \
		$(O)/p_tick.o	 \
		$(O)/p_saveg.o	 \
		$(O)/p_user.o	 \
		$(O)/r_bsp.o	 \
		$(O)/r_data.o	 \
		$(O)/r_draw.o	 \
		$(O)/r_main.o	 \
		$(O)/r_plane.o	 \
		$(O)/r_segs.o	 \
		$(O)/r_sky.o	 \
		$(O)/r_things.o  \
		$(O)/w_wad.o	 \
		$(O)/wi_stuff.o  \
		$(O)/v_video.o	 \
		$(O)/st_lib.o	 \
		$(O)/st_stuff.o  \
		$(O)/hu_stuff.o  \
		$(O)/s_sound.o	 \
		$(O)/z_zone.o	 \
		$(O)/info.o	 \
		$(O)/sounds.o	 \
		$(O)/p_fab.o	 \
		$(O)/dehacked.o  \
		$(O)/qmus2mid.o  \
		$(O)/g_input.o	 \
		$(O)/screen.o	 \
		$(O)/console.o	 \
		$(O)/command.o	 \
		$(O)/d_netcmd.o  \
		$(O)/d_clisrv.o  \
		$(O)/r_splats.o  \
		$(O)/g_state.o	 \
		$(O)/t_func.o	 \
		$(O)/t_oper.o	 \
		$(O)/t_parse.o	 \
		$(O)/t_prepro.o  \
		$(O)/t_script.o  \
		$(O)/t_spec.o	 \
		$(O)/t_vari.o	 \
		$(O)/d_netfil.o  \
		$(O)/mserv.o	 \
		$(O)/p_hsight.o  \
		$(O)/s_amb.o	 \
		$(O)/sb_bar.o	 \
		$(O)/p_heretic.o \
		$(O)/md5.o	 \
		$(O)/p_chex.o	 \
		$(O)/filesrch.o	\
		$(O)/b_game.o	\
		$(O)/b_look.o	\
		$(O)/b_node.o	\
		$(O)/b_search.o


ifdef LINUX
all:	 $(BIN)/$(EXENAME) dll musserv sndserv
else
ifdef FREEBSD
all:	 $(BIN)/$(EXENAME) dll sndserv
else
all:	 $(BIN)/$(EXENAME)
endif
endif

clean:
ifdef LINUX
	rm -f *.o *~ *.flc
	rm -f $(O)/*
	rm -f $(SNDSERV)/linux/*
	rm -f $(MUSSERV)/linux/*
else
ifdef SDL
	rm -f *.o *~ *.flc
	rm -f $(O)/*
else
ifdef FREEBSD
	rm -f *.o *~ *.flc
	rm -f $(O)/*
	rm -f $(SNDSERV)/linux/*
	rm -f $(MUSSERV)/linux/*
else
ifdef SOLARIS
	rm -f *.o *~ *.flc
	rm -f $(O)/*
	rm -f $(SNDSERV)/linux/*
	rm -f $(MUSSERV)/linux/*
else
	del $(O)\*.* >NUL
endif
endif
endif
endif

#make a big doom3.s that is the disasm of the exe (dos only ?)
asm:
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -g $(O)/i_main.o \
	-o $(O)/tmp.exe $(DEBUGLIBS)
	objdump  -d $(O)/tmp.exe --no-show-raw-insn > doom3.s
	del $(O)\tmp.exe

# executable

$(BIN)/$(EXENAME):	$(O) $(OBJS) $(O)/i_main.o
#	 @mkdir $(BIN)
	@echo Linking...
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(O)/i_main.o \
	-o $(BIN)/$(EXENAME) $(LIBS)

$(O):
	@mkdir $(O)

#dll
dll : $(O)/r_opengl.o $(O)/ogl_x11.o
	$(CC) $(M5) -O6 -o $(BIN)/r_opengl.so -shared -nostartfiles $(O)/r_opengl.o $(O)/ogl_x11.o -L/usr/X11R6/lib -L/usr/local/lib -lX11 -lXext -lGL -lGLU -lm

musserv:
	@mkdir -p $(MUSSERV)/linux
	cd $(MUSSERV) \
	&& make -f Makefile.linux

sndserv:
	@mkdir -p $(SNDSERV)/linux
	cd $(SNDSERV) \
	&& $(MAKE) $(SNDSERV_EXTRAFLAGS)

ifdef LINUX
$(O)/r_opengl.o:	hardware/r_opengl/r_opengl.c hardware/r_opengl/r_opengl.h
	$(CC) $(M5) -O6 -o $(O)/r_opengl.o	-DHWRENDER -DVID_X11 -DLINUX $(POLL_POINTER) -shared -nostartfiles -I/usr/X11R6/include -c hardware/r_opengl/r_opengl.c
endif

ifdef SDL
$(O)/r_opengl.o:	hardware/r_opengl/r_opengl.c hardware/r_opengl/r_opengl.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -I/usr/X11R6/include -c $< -o $@
endif

ifdef FREEBSD
ifdef FBSD_SDL
$(O)/r_opengl.o:	hardware/r_opengl/r_opengl.c hardware/r_opengl/r_opengl.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -I/usr/X11R6/include -c $< -o $@
else  #FBSD_SDL
$(O)/r_opengl.o:	hardware/r_opengl/r_opengl.c hardware/r_opengl/r_opengl.h
	$(CC) $(M5) -O6 -o $(O)/r_opengl.o	-DHWRENDER -DVID_X11 -DLINUX -DFREEBSD $(POLL_POINTER) -shared -nostartfiles -I/usr/X11R6/include -c hardware/r_opengl/r_opengl.c
endif #FBSD_SDL
endif #FREEBSD

ifdef SOLARIS
$(O)/r_opengl.o:	hardware/r_opengl/r_opengl.c hardware/r_opengl/r_opengl.h
	$(CC) $(CFLAGS) -Wno-unknown-pragmas $(LDFLAGS) $(WFLAGS) -I/usr/X/include -c $< -o $@
endif

ifndef FREEBSD
$(O)/ogl_x11.o:  hardware/r_opengl/ogl_x11.c hardware/r_opengl/r_opengl.h
	$(CC) $(M5) -O6 -o $(O)/ogl_x11.o -DHWRENDER -DVID_X11 -DLINUX $(POLL_POINTER) -shared -nostartfiles -I/usr/X11R6/include -c hardware/r_opengl/ogl_x11.c
else
$(O)/ogl_x11.o:  hardware/r_opengl/ogl_x11.c hardware/r_opengl/r_opengl.h
	$(CC) $(M5) -O6 -o $(O)/ogl_x11.o -DHWRENDER -DVID_X11 -DLINUX -DFREEBSD $(POLL_POINTER) -shared -nostartfiles -I/usr/X11R6/include -c hardware/r_opengl/ogl_x11.c
endif


#dependecy made by gcc itself ! (see makedep.mak for more)
$(O)/dstrings.o: dstrings.c dstrings.h d_englsh.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_sound.o: $(INTERFACE)/i_sound.c doomdef.h \
 doomtype.h g_state.h m_swap.h doomstat.h doomdata.h d_player.h \
 d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h \
 p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h command.h i_system.h \
 d_event.h i_sound.h sounds.h z_zone.h m_argv.h m_misc.h \
 w_wad.h s_sound.h qmus2mid.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_cdmus.o: $(INTERFACE)/i_cdmus.c doomdef.h \
 doomtype.h g_state.h m_swap.h	i_sound.h sounds.h command.h \
 i_system.h d_ticcmd.h d_event.h  s_sound.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_video.o: $(INTERFACE)/i_video.c doomdef.h \
 doomtype.h g_state.h m_swap.h i_system.h d_ticcmd.h d_event.h \
 v_video.h r_defs.h m_fixed.h d_think.h p_mobj.h tables.h \
 doomdata.h info.h screen.h command.h m_argv.h \
 i_video.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_net.o: $(INTERFACE)/i_net.c doomdef.h doomtype.h \
 g_state.h m_swap.h i_system.h d_ticcmd.h d_event.h d_net.h \
 m_argv.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_clisrv.h \
 d_netcmd.h command.h z_zone.h i_net.h i_tcp.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_tcp.o: i_tcp.c doomdef.h doomtype.h g_state.h m_swap.h i_system.h \
 d_ticcmd.h d_event.h i_net.h d_net.h m_argv.h doomstat.h doomdata.h \
 d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h \
 p_mobj.h d_clisrv.h d_netcmd.h command.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_system.o: $(INTERFACE)/i_system.c doomdef.h \
 doomtype.h g_state.h m_swap.h \
 m_misc.h w_wad.h i_video.h \
 i_sound.h sounds.h d_net.h \
 g_game.h doomstat.h doomdata.h \
 d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h \
 d_think.h p_mobj.h d_ticcmd.h \
 d_clisrv.h d_netcmd.h command.h \
 d_event.h d_main.h m_argv.h \
 z_zone.h g_input.h \
 console.h i_system.h \
 i_joy.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/tables.o: tables.c tables.h m_fixed.h doomtype.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/f_finale.o: f_finale.c doomdef.h doomtype.h g_state.h m_swap.h \
 am_map.h d_event.h dstrings.h d_englsh.h d_main.h w_wad.h f_finale.h \
 g_game.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h \
 d_netcmd.h command.h hu_stuff.h wi_stuff.h r_defs.h screen.h \
 r_local.h m_bbox.h r_main.h r_data.h r_state.h r_bsp.h r_segs.h \
 r_plane.h r_sky.h r_things.h sounds.h r_draw.h s_sound.h i_video.h \
 v_video.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/f_wipe.o: f_wipe.c doomdef.h doomtype.h g_state.h m_swap.h m_random.h \
 f_wipe.h i_system.h d_ticcmd.h d_event.h i_video.h v_video.h r_defs.h \
 m_fixed.h d_think.h p_mobj.h tables.h doomdata.h info.h screen.h \
 command.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/d_main.o: d_main.c doomdef.h doomtype.h g_state.h m_swap.h command.h \
 console.h d_event.h doomstat.h doomdata.h d_player.h d_items.h \
 p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h \
 d_clisrv.h d_netcmd.h am_map.h d_net.h dehacked.h dstrings.h \
 d_englsh.h f_wipe.h f_finale.h g_game.h g_input.h keys.h hu_stuff.h \
 w_wad.h wi_stuff.h r_defs.h screen.h i_sound.h sounds.h i_system.h \
 i_video.h m_argv.h m_menu.h m_misc.h p_setup.h p_fab.h r_main.h \
 r_data.h r_state.h r_local.h m_bbox.h r_bsp.h r_segs.h r_plane.h \
 r_sky.h r_things.h r_draw.h s_sound.h st_stuff.h v_video.h z_zone.h \
 d_main.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/d_net.o: d_net.c doomdef.h doomtype.h g_state.h m_swap.h d_clisrv.h \
 d_ticcmd.h d_netcmd.h command.h g_game.h doomstat.h doomdata.h \
 d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h \
 p_mobj.h d_event.h i_net.h i_system.h m_argv.h d_net.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/d_items.o: d_items.c info.h d_think.h d_items.h doomdef.h doomtype.h \
 g_state.h m_swap.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/g_game.o: g_game.c doomdef.h doomtype.h g_state.h m_swap.h command.h \
 console.h d_event.h dstrings.h d_englsh.h d_main.h w_wad.h d_net.h \
 d_netcmd.h f_finale.h p_setup.h doomdata.h p_saveg.h i_system.h \
 d_ticcmd.h wi_stuff.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h am_map.h m_random.h p_local.h \
 m_bbox.h p_tick.h r_defs.h screen.h p_maputl.h p_spec.h r_data.h \
 r_state.h r_draw.h r_main.h r_sky.h s_sound.h sounds.h g_game.h \
 doomstat.h d_clisrv.h g_input.h keys.h p_fab.h m_cheat.h m_misc.h \
 m_menu.h m_argv.h hu_stuff.h st_stuff.h z_zone.h i_video.h p_inter.h \
 byteptr.h i_joy.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_menu.o: m_menu.c am_map.h d_event.h doomtype.h g_state.h doomdef.h \
 m_swap.h dstrings.h d_englsh.h d_main.h w_wad.h console.h r_local.h \
 tables.h m_fixed.h screen.h command.h m_bbox.h r_main.h d_player.h \
 d_items.h p_pspr.h info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h \
 r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h \
 r_things.h sounds.h r_draw.h hu_stuff.h wi_stuff.h g_game.h \
 doomstat.h d_clisrv.h d_netcmd.h g_input.h keys.h m_argv.h s_sound.h \
 i_system.h m_menu.h v_video.h i_video.h z_zone.h p_local.h p_tick.h \
 p_maputl.h p_spec.h p_fab.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_misc.o: m_misc.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h m_misc.h w_wad.h hu_stuff.h wi_stuff.h r_defs.h \
 screen.h v_video.h z_zone.h g_input.h keys.h i_video.h d_main.h \
 m_argv.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_argv.o: m_argv.c doomdef.h doomtype.h g_state.h m_swap.h command.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_bbox.o: m_bbox.c doomtype.h m_bbox.h m_fixed.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_fixed.o: m_fixed.c i_system.h d_ticcmd.h doomtype.h d_event.h \
 g_state.h m_fixed.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_swap.o: m_swap.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_cheat.o: m_cheat.c doomdef.h doomtype.h g_state.h m_swap.h \
 dstrings.h d_englsh.h am_map.h d_event.h m_cheat.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h r_local.h screen.h m_bbox.h r_main.h r_data.h r_defs.h \
 r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h r_things.h sounds.h \
 r_draw.h p_local.h p_tick.h p_maputl.h p_spec.h p_inter.h i_sound.h \
 s_sound.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/m_random.o: m_random.c doomdef.h doomtype.h g_state.h m_swap.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/am_map.o: am_map.c doomdef.h doomtype.h g_state.h m_swap.h doomstat.h \
 doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h \
 d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h command.h \
 g_game.h d_event.h am_map.h g_input.h keys.h m_cheat.h p_local.h \
 m_bbox.h p_tick.h r_defs.h screen.h p_maputl.h p_spec.h v_video.h \
 st_stuff.h i_system.h i_video.h r_state.h r_data.h dstrings.h \
 d_englsh.h w_wad.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_ceilng.o: p_ceilng.c doomdef.h doomtype.h g_state.h m_swap.h \
 p_local.h command.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h \
 info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h \
 r_defs.h screen.h p_maputl.h p_spec.h r_state.h r_data.h s_sound.h \
 sounds.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_doors.o: p_doors.c doomdef.h doomtype.h g_state.h m_swap.h \
 dstrings.h d_englsh.h p_local.h command.h d_player.h d_items.h \
 p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h doomdata.h \
 d_ticcmd.h m_bbox.h p_tick.h r_defs.h screen.h p_maputl.h p_spec.h \
 r_state.h r_data.h s_sound.h sounds.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_enemy.o: p_enemy.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h r_main.h r_data.h r_state.h s_sound.h sounds.h \
 m_random.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_floor.o: p_floor.c doomdef.h doomtype.h g_state.h m_swap.h p_local.h \
 command.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h \
 d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h r_state.h r_data.h s_sound.h sounds.h \
 z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_genlin.o: p_genlin.c doomdef.h g_game.h p_local.h r_data.h m_random.h \
 m_random.h s_sound.h z_zone.h command.h d_player.h d_think.h m_fixed.h \
 m_bbox.h p_tick.h r_defs.h p_maputl.h p_mobj.h screen.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_info.o: p_info.c doomdef.h dehacked.h dstrings.h p_setup.h p_info.h \
 p_mobj.h t_script.h w_wad.h z_zone.h command.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_inter.o: p_inter.c doomdef.h doomtype.h g_state.h m_swap.h \
 i_system.h d_ticcmd.h d_event.h am_map.h dstrings.h d_englsh.h \
 g_game.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_clisrv.h d_netcmd.h \
 command.h m_random.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h p_inter.h s_sound.h sounds.h r_main.h r_data.h \
 r_state.h st_stuff.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_lights.o: p_lights.c doomdef.h doomtype.h g_state.h m_swap.h \
 p_local.h command.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h \
 info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h \
 r_defs.h screen.h p_maputl.h p_spec.h r_state.h r_data.h z_zone.h \
 m_random.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_map.o: p_map.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h m_bbox.h m_random.h p_local.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h p_inter.h r_state.h r_data.h r_main.h \
 r_sky.h s_sound.h sounds.h r_splats.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_maputl.o: p_maputl.c p_local.h command.h doomtype.h d_player.h \
 d_items.h doomdef.h g_state.h m_swap.h p_pspr.h m_fixed.h tables.h \
 info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h \
 r_defs.h screen.h p_maputl.h p_spec.h r_main.h r_data.h r_state.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_plats.o: p_plats.c doomdef.h doomtype.h g_state.h m_swap.h p_local.h \
 command.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h \
 d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h s_sound.h sounds.h r_state.h r_data.h \
 z_zone.h m_random.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_pspr.o: p_pspr.c doomdef.h doomtype.h g_state.h m_swap.h d_event.h \
 p_local.h command.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h \
 info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h \
 r_defs.h screen.h p_maputl.h p_spec.h p_inter.h s_sound.h sounds.h \
 g_game.h doomstat.h d_clisrv.h d_netcmd.h g_input.h keys.h r_main.h \
 r_data.h r_state.h m_random.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_setup.o: p_setup.c doomdef.h doomtype.h g_state.h m_swap.h d_main.h \
 d_event.h w_wad.h g_game.h doomstat.h doomdata.h d_player.h d_items.h \
 p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h \
 d_clisrv.h d_netcmd.h command.h p_local.h m_bbox.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h p_setup.h i_sound.h sounds.h r_sky.h \
 r_data.h r_state.h r_things.h s_sound.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_sight.o: p_sight.c doomdef.h doomtype.h g_state.h m_swap.h p_local.h \
 command.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h \
 d_think.h p_mobj.h doomdata.h d_ticcmd.h m_bbox.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h r_main.h r_data.h r_state.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_spec.o: p_spec.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h p_setup.h r_data.h r_state.h m_random.h s_sound.h \
 sounds.h w_wad.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_switch.o: p_switch.c doomdef.h doomtype.h g_state.h m_swap.h \
 g_game.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h \
 d_netcmd.h command.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h s_sound.h sounds.h r_main.h r_data.h \
 r_state.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_mobj.o: p_mobj.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h g_input.h keys.h st_stuff.h hu_stuff.h w_wad.h \
 wi_stuff.h r_defs.h screen.h p_local.h m_bbox.h p_tick.h p_maputl.h \
 p_spec.h p_inter.h p_setup.h r_main.h r_data.h r_state.h r_things.h \
 sounds.h r_sky.h s_sound.h z_zone.h m_random.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_telept.o: p_telept.c doomdef.h doomtype.h g_state.h m_swap.h \
 g_game.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h \
 d_netcmd.h command.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h \
 screen.h p_maputl.h p_spec.h r_state.h r_data.h s_sound.h sounds.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_tick.o: p_tick.c doomstat.h doomdata.h doomtype.h doomdef.h \
 g_state.h m_swap.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h \
 info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h command.h \
 g_game.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_saveg.o: p_saveg.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h r_state.h r_data.h z_zone.h w_wad.h p_setup.h \
 byteptr.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_user.o: p_user.c doomdef.h doomtype.h g_state.h m_swap.h d_event.h \
 g_game.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h \
 d_netcmd.h command.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h r_main.h r_data.h r_state.h s_sound.h sounds.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_bsp.o: r_bsp.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h r_local.h screen.h m_bbox.h r_main.h r_data.h \
 r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h r_things.h \
 sounds.h r_draw.h r_splats.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_data.o: r_data.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h i_video.h r_local.h screen.h m_bbox.h r_main.h \
 r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h \
 r_things.h sounds.h r_draw.h p_local.h p_tick.h p_maputl.h p_spec.h \
 w_wad.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_draw.o: r_draw.c doomdef.h doomtype.h g_state.h m_swap.h r_local.h \
 tables.h m_fixed.h screen.h command.h m_bbox.h r_main.h d_player.h \
 d_items.h p_pspr.h info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h \
 r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h \
 r_things.h sounds.h r_draw.h st_stuff.h d_event.h i_video.h \
 v_video.h w_wad.h z_zone.h r_draw8.c r_draw16.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_main.o: r_main.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h g_input.h keys.h r_local.h screen.h m_bbox.h \
 r_main.h r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h \
 r_sky.h r_things.h sounds.h r_draw.h r_splats.h st_stuff.h p_local.h \
 p_tick.h p_maputl.h p_spec.h i_video.h m_menu.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_plane.o: r_plane.c doomdef.h doomtype.h g_state.h m_swap.h console.h \
 d_event.h g_game.h doomstat.h doomdata.h d_player.h d_items.h \
 p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h \
 d_clisrv.h d_netcmd.h command.h r_data.h r_defs.h screen.h r_state.h \
 r_local.h m_bbox.h r_main.h r_bsp.h r_segs.h r_plane.h r_sky.h \
 r_things.h sounds.h r_draw.h r_splats.h v_video.h w_wad.h \
 z_zone.h p_setup.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_segs.o: r_segs.c doomdef.h doomtype.h g_state.h m_swap.h r_local.h \
 tables.h m_fixed.h screen.h command.h m_bbox.h r_main.h d_player.h \
 d_items.h p_pspr.h info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h \
 r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h \
 r_things.h sounds.h r_draw.h r_splats.h w_wad.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_sky.o: r_sky.c doomdef.h doomtype.h g_state.h m_swap.h r_sky.h \
 m_fixed.h r_local.h tables.h screen.h command.h m_bbox.h r_main.h \
 d_player.h d_items.h p_pspr.h info.h d_think.h p_mobj.h doomdata.h \
 d_ticcmd.h r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h \
 r_things.h sounds.h r_draw.h w_wad.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_things.o: r_things.c doomdef.h doomtype.h g_state.h m_swap.h \
 console.h d_event.h g_game.h doomstat.h doomdata.h d_player.h \
 d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h \
 d_ticcmd.h d_clisrv.h d_netcmd.h command.h r_local.h screen.h \
 m_bbox.h r_main.h r_data.h r_defs.h r_state.h r_bsp.h r_segs.h \
 r_plane.h r_sky.h r_things.h sounds.h r_draw.h st_stuff.h w_wad.h \
 z_zone.h i_video.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/w_wad.o: w_wad.c doomdef.h doomtype.h g_state.h m_swap.h w_wad.h \
 z_zone.h i_video.h d_netfil.h md5.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/wi_stuff.o: wi_stuff.c doomdef.h doomtype.h g_state.h m_swap.h \
 wi_stuff.h d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h \
 d_think.h p_mobj.h doomdata.h d_ticcmd.h g_game.h doomstat.h \
 d_clisrv.h d_netcmd.h command.h d_event.h hu_stuff.h w_wad.h r_defs.h \
 screen.h m_random.h r_local.h m_bbox.h r_main.h r_data.h r_state.h \
 r_bsp.h r_segs.h r_plane.h r_sky.h r_things.h sounds.h r_draw.h \
 s_sound.h st_stuff.h i_video.h v_video.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/v_video.o: v_video.c doomdef.h doomtype.h g_state.h m_swap.h r_local.h \
 tables.h m_fixed.h screen.h command.h m_bbox.h r_main.h d_player.h \
 d_items.h p_pspr.h info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h \
 r_data.h r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h \
 r_things.h sounds.h r_draw.h v_video.h hu_stuff.h d_event.h w_wad.h \
 wi_stuff.h i_video.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/st_lib.o: st_lib.c doomdef.h doomtype.h g_state.h m_swap.h st_lib.h \
 r_defs.h m_fixed.h d_think.h p_mobj.h tables.h doomdata.h info.h \
 screen.h command.h w_wad.h st_stuff.h d_event.h d_player.h d_items.h \
 p_pspr.h d_ticcmd.h v_video.h z_zone.h i_video.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/st_stuff.o: st_stuff.c doomdef.h doomtype.h g_state.h m_swap.h \
 am_map.h d_event.h g_game.h doomstat.h doomdata.h d_player.h \
 d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h \
 d_ticcmd.h d_clisrv.h d_netcmd.h command.h m_cheat.h r_local.h \
 screen.h m_bbox.h r_main.h r_data.h r_defs.h r_state.h r_bsp.h \
 r_segs.h r_plane.h r_sky.h r_things.h sounds.h r_draw.h p_local.h \
 p_tick.h p_maputl.h p_spec.h p_inter.h m_random.h st_stuff.h st_lib.h \
 w_wad.h i_video.h v_video.h keys.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/hu_stuff.o: hu_stuff.c doomdef.h doomtype.h g_state.h m_swap.h \
 hu_stuff.h d_event.h w_wad.h wi_stuff.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h doomdata.h d_ticcmd.h \
 r_defs.h screen.h command.h d_netcmd.h d_clisrv.h g_game.h doomstat.h \
 g_input.h keys.h i_video.h dstrings.h d_englsh.h st_stuff.h r_local.h \
 m_bbox.h r_main.h r_data.h r_state.h r_bsp.h r_segs.h r_plane.h \
 r_sky.h r_things.h sounds.h r_draw.h v_video.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/s_sound.o: s_sound.c doomdef.h doomtype.h g_state.h m_swap.h command.h \
 g_game.h doomstat.h doomdata.h d_player.h d_items.h p_pspr.h \
 m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h \
 d_netcmd.h d_event.h m_argv.h r_main.h r_data.h r_defs.h screen.h \
 r_state.h r_things.h sounds.h i_sound.h s_sound.h w_wad.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/z_zone.o: z_zone.c doomdef.h doomtype.h g_state.h m_swap.h z_zone.h \
 i_system.h d_ticcmd.h d_event.h command.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/info.o: info.c doomdef.h doomtype.h g_state.h m_swap.h sounds.h \
 m_fixed.h p_mobj.h tables.h d_think.h doomdata.h info.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/sounds.o: sounds.c doomtype.h sounds.h r_defs.h m_fixed.h d_think.h \
 p_mobj.h tables.h doomdata.h doomdef.h g_state.h m_swap.h info.h \
 screen.h command.h r_things.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/tmap.o: tmap.nas
	nasm -o $@ -f $(NASMFORMAT) $<

$(O)/p_fab.o: p_fab.c doomdef.h doomtype.h g_state.h m_swap.h g_game.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h d_event.h p_local.h m_bbox.h p_tick.h r_defs.h screen.h \
 p_maputl.h p_spec.h r_state.h r_data.h p_fab.h m_random.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/dehacked.o: dehacked.c doomdef.h doomtype.h g_state.h m_swap.h \
 command.h console.h d_event.h g_game.h doomstat.h doomdata.h \
 d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h \
 p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h sounds.h m_cheat.h \
 dstrings.h d_englsh.h m_argv.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/qmus2mid.o: qmus2mid.c doomdef.h doomtype.h g_state.h m_swap.h \
 i_system.h d_ticcmd.h d_event.h byteptr.h m_swap.h $(qmus2mid_h)
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/qmus2mid_sdl.o: qmus2mid_sdl.c doomdef.h doomtype.h g_state.h m_swap.h \
 i_system.h d_ticcmd.h d_event.h byteptr.h m_swap.h $(qmus2mid_h)
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/vid_copy.o: vid_copy.s
	$(CC) $(OPTS) $(SFLAGS) -x assembler-with-cpp -c $< -o $@

$(O)/vid_vesa.o: $(INTERFACE)/vid_vesa.c i_system.h \
 d_ticcmd.h doomtype.h d_event.h \
 g_state.h $(INTERFACE)/vid_vesa.h doomdef.h \
 m_swap.h screen.h command.h \
 st_stuff.h d_player.h d_items.h \
 p_pspr.h m_fixed.h tables.h \
 info.h d_think.h p_mobj.h \
 doomdata.h console.h i_video.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/g_input.o: g_input.c doomdef.h doomtype.h g_state.h m_swap.h g_input.h \
 d_event.h keys.h command.h hu_stuff.h w_wad.h wi_stuff.h d_player.h \
 d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h \
 doomdata.h d_ticcmd.h r_defs.h screen.h d_net.h console.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/screen.o: screen.c doomdef.h doomtype.h g_state.h m_swap.h screen.h \
 command.h console.h d_event.h am_map.h i_system.h d_ticcmd.h \
 i_video.h r_local.h tables.h m_fixed.h m_bbox.h r_main.h d_player.h \
 d_items.h p_pspr.h info.h d_think.h p_mobj.h doomdata.h r_data.h \
 r_defs.h r_state.h r_bsp.h r_segs.h r_plane.h r_sky.h r_things.h \
 sounds.h r_draw.h m_argv.h v_video.h st_stuff.h hu_stuff.h w_wad.h \
 wi_stuff.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/console.o: console.c doomdef.h doomtype.h g_state.h m_swap.h console.h \
 d_event.h g_game.h doomstat.h doomdata.h d_player.h d_items.h \
 p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h d_ticcmd.h \
 d_clisrv.h d_netcmd.h command.h g_input.h keys.h hu_stuff.h w_wad.h \
 wi_stuff.h r_defs.h screen.h sounds.h st_stuff.h s_sound.h v_video.h \
 i_video.h z_zone.h i_system.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/command.o: command.c doomdef.h doomtype.h g_state.h m_swap.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h console.h d_event.h z_zone.h m_misc.h w_wad.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/d_netcmd.o: d_netcmd.c doomdef.h doomtype.h g_state.h m_swap.h \
 console.h d_event.h command.h d_netcmd.h d_clisrv.h d_ticcmd.h \
 i_system.h dstrings.h d_englsh.h g_game.h doomstat.h doomdata.h \
 d_player.h d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h \
 p_mobj.h hu_stuff.h w_wad.h wi_stuff.h r_defs.h screen.h g_input.h \
 keys.h m_menu.h r_local.h m_bbox.h r_main.h r_data.h r_state.h \
 r_bsp.h r_segs.h r_plane.h r_sky.h r_things.h sounds.h r_draw.h \
 p_inter.h p_setup.h s_sound.h m_misc.h am_map.h byteptr.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/d_clisrv.o: d_clisrv.c doomdef.h doomtype.h g_state.h m_swap.h \
 command.h i_net.h i_system.h d_ticcmd.h d_event.h d_net.h d_netcmd.h \
 d_clisrv.h d_main.h w_wad.h g_game.h doomstat.h doomdata.h d_player.h \
 d_items.h p_pspr.h m_fixed.h tables.h info.h d_think.h p_mobj.h \
 hu_stuff.h wi_stuff.h r_defs.h screen.h keys.h m_argv.h m_menu.h \
 console.h d_netfil.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/r_splats.o: r_splats.c r_draw.h r_defs.h m_fixed.h doomtype.h \
 d_think.h p_mobj.h tables.h doomdata.h doomdef.h g_state.h m_swap.h \
 info.h screen.h command.h r_main.h d_player.h d_items.h p_pspr.h \
 d_ticcmd.h r_data.h r_state.h r_plane.h r_splats.h w_wad.h z_zone.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/g_state.o: g_state.c g_state.h doomtype.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/d_netfil.o: d_netfil.c doomdef.h doomtype.h g_state.h m_swap.h \
 doomstat.h doomdata.h d_player.h d_items.h p_pspr.h m_fixed.h \
 tables.h info.h d_think.h p_mobj.h d_ticcmd.h d_clisrv.h d_netcmd.h \
 command.h g_game.h d_event.h i_net.h i_system.h m_argv.h d_net.h \
 d_netfil.h z_zone.h md5.h filesrch.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/t_func.o: t_func.c command.h doomstat.h doomtype.h d_main.h \
 g_game.h hu_stuff.h info.h m_random.h p_mobj.h p_tick.h \
 p_spec.h p_inter.h r_data.h r_main.h r_segs.h s_sound.h w_wad.h \
 z_zone.h t_parse.h t_spec.h t_script.h t_oper.h t_vari.h t_func.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/t_oper.o: t_oper.c command.h doomstat.h doomtype.h z_zone.h \
 t_parse.h t_vari.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/t_parse.o: t_parse.c command.h doomtype.h doomdef.h s_sound.h \
 w_wad.h z_zone.h t_parse.h t_spec.h t_script.h t_oper.h t_vari.h t_func.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/t_prepro.o: t_prepro.c command.h doomtype.h w_wad.h z_zone.h \
 t_parse.h t_spec.h t_script.h t_oper.h t_vari.h t_func.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/t_script.o: t_script.c command.h g_game.h r_state.h p_info.h \
 p_mobj.h p_spec.h p_setup.h w_wad.h z_zone.h t_parse.h t_script.h \
 t_vari.h t_func.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/t_spec.o: t_spec.c command.h doomtype.h z_zone.h \
 t_parse.h t_spec.h t_vari.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/i_main.o: $(INTERFACE)/i_main.c doomdef.h \
 doomtype.h g_state.h m_swap.h \
 m_argv.h d_main.h d_event.h \
 w_wad.h i_system.h d_ticcmd.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/md5.o: md5.c md5.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/p_chex.o: p_chex.c p_chex.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/filesrch.o: $(INTERFACE)/filesrch.c filesrch.h d_netfil.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/b_game.o: b_game.c b_bot.h b_game.h b_look.h \
 b_node.h doomdef.h doomstat.h m_random.h p_local.h \
 z_zone.h command.h r_state.h v_video.h m_argv.h \
 p_setup.h r_main.h r_things.h g_game.h d_net.h

$(O)/b_look.o: b_look.c b_bot.h b_game.h b_look.h \
 b_node.h g_game.h r_defs.h p_local.h m_random.h \
 r_main.h z_zone.h

$(O)/b_node.o: b_node.c b_bot.h b_node.h z_zone.h \
 g_game.h r_state.h p_maputl.h r_main.h p_local.h \
 p_setup.h

$(O)/b_search.o: b_search.c g_game.h r_defs.h p_local.h \
 m_random.h r_main.h z_zone.h b_search.h b_node.h

$(O)/%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/%.o: $(INTERFACE)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(O)/%.o: hardware/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

ifdef OS2
$(O)/%.res: os2/%.rc
	rc.exe -i .\os2 -r $< $@
endif

#############################################################
#
#############################################################
