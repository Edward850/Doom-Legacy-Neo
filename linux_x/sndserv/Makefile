##########################################################
#
# $Id: Makefile,v 1.6 2002/12/17 19:56:51 bock Exp $
#
# $Log: Makefile,v $
# Revision 1.6  2002/12/17 19:56:51  bock
# Add esound support without SDL.
# Sorry about this, this code is disabled by default.
# You should build with HAVE_ESD=1 flag.
#
# Revision 1.5  2001/05/29 22:18:41  bock
# Small BSD commit: build r_opengl.so, sndserver
#
# Revision 1.4  2000/04/30 19:50:37  metzgermeister
# no message
#
# Revision 1.3  2000/03/12 22:34:07  linuxcub
# Fixed makefile (leading spaces converted to tabs)
#
# Revision 1.2  2000/02/27 00:42:12  hurdler
# fix CR+LF problem
#
# Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
# Initial import into CVS (v1.29 pr3)
#
#
#

CC=gcc
CFLAGS=-O2 -m486 -Wall -DNORMALUNIX -DLINUX
ifdef FREEBSD
CFLAGS:=${CFLAGS} -DFREEBSD
endif
LDFLAGS=
LIBS=-lm

O=linux

HWOUTOBJ	:=	linux.o
ifdef HAVE_ESD
ESDCFLAGS	:=	$(shell esd-config --cflags)
ESDLDFLAGS	:=	$(shell esd-config --libs)
LIBS		+=	$(ESDLDFLAGS)
HWOUTOBJ	:=	esd.o
CFLAGS		+=	$(ESDCFLAGS)
endif


all:     $(O)/llsndserv

clean:
	rm -f $(O)/*

# Target
$(O)/llsndserv: \
	$(O)/soundsrv.o \
	$(O)/sounds.o \
	$(O)/$(HWOUTOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) \
	$(O)/soundsrv.o \
	$(O)/sounds.o \
	$(O)/$(HWOUTOBJ) -o $(O)/llsndserv $(LIBS)
	echo make complete.

# Rule
$(O)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

