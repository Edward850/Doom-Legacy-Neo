// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: byteptr.h,v 1.6 2003/05/04 02:26:39 sburke Exp $
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
// $Log: byteptr.h,v $
// Revision 1.6  2003/05/04 02:26:39  sburke
// Fix problems in adapting to __BIG_ENDIAN__ machines.
//
// Revision 1.5  2000/10/21 08:43:28  bpereira
// no message
//
// Revision 1.4  2000/04/16 18:38:06  bpereira
// no message
//
//
// DESCRIPTION:
//    Macro to read/write from/to a char*, used for packet cration and such...
//
//-----------------------------------------------------------------------------

#ifndef __BIG_ENDIAN__
//
// Little-endian machines
//
#define writeshort(p,b)     *(short*)  (p)   = b
#define writelong(p,b)      *(long *)  (p)   = b
#define WRITEBYTE(p,b)      *((byte   *)p)++ = b
#define WRITECHAR(p,b)      *((char   *)p)++ = b
#define WRITESHORT(p,b)     *((short  *)p)++ = b
#define WRITEUSHORT(p,b)    *((USHORT *)p)++ = b
#define WRITELONG(p,b)      *((long   *)p)++ = b
#define WRITEULONG(p,b)     *((ULONG  *)p)++ = b
#define WRITEFIXED(p,b)     *((fixed_t*)p)++ = b
#define WRITEANGLE(p,b)     *((angle_t*)p)++ = b
#define WRITESTRING(p,b)    { int tmp_i=0; do { WRITECHAR(p,b[tmp_i]); } while(b[tmp_i++]); }
#define WRITESTRINGN(p,b,n) { int tmp_i=0; do { WRITECHAR(p,b[tmp_i]); if(!b[tmp_i]) break;tmp_i++; } while(tmp_i<n); }
#define WRITEMEM(p,s,n)     memcpy(p, s, n);p+=n

#define readshort(p)	    *((short  *)p)
#define readlong(p)	    *((long   *)p)
#define READBYTE(p)         *((byte   *)p)++
#define READCHAR(p)         *((char   *)p)++
#define READSHORT(p)        *((short  *)p)++
#define READUSHORT(p)       *((USHORT *)p)++
#define READLONG(p)         *((long   *)p)++
#define READULONG(p)        *((ULONG  *)p)++
#define READFIXED(p)        *((fixed_t*)p)++
#define READANGLE(p)        *((angle_t*)p)++
#define READSTRING(p,s)     { int tmp_i=0; do { s[tmp_i]=READBYTE(p);  } while(s[tmp_i++]); }
#define SKIPSTRING(p)       while(READBYTE(p))
#define READMEM(p,s,n)      memcpy(s, p, n);p+=n
#else 
//
// definitions for big-endian machines with alignment constraints.
//
// Write a value to a little-endian, unaligned destination.
//
static inline void writeshort(void * ptr, int val)
{
  char * cp = ptr;
  cp[0] = val ;  val >>= 8;
  cp[1] = val ;
}

static inline void writelong(void * ptr, int val)
{
  char * cp = ptr;
  cp[0] = val ;  val >>= 8;
  cp[1] = val ;  val >>= 8;
  cp[2] = val ;  val >>= 8;
  cp[3] = val ;
}

#define WRITEBYTE(p,b)      *((byte   *)p)++ = (b)
#define WRITECHAR(p,b)      *((char   *)p)++ = (b)
#define WRITESHORT(p,b)     writeshort(((short *)p)++,  (b))
#define WRITEUSHORT(p,b)    writeshort(((u_short*)p)++, (b))
#define WRITELONG(p,b)      writelong (((long  *)p)++,  (b))
#define WRITEULONG(p,b)     writelong (((u_long *)p)++, (b))
#define WRITEFIXED(p,b)     writelong (((fixed_t*)p)++,  (b))
#define WRITEANGLE(p,b)     writelong (((angle_t*)p)++, (long) (b))
#define WRITESTRING(p,b)    { int tmp_i=0; do { WRITECHAR(p,b[tmp_i]); } while(b[tmp_i++]); }
#define WRITESTRINGN(p,b,n) { int tmp_i=0; do { WRITECHAR(p,b[tmp_i]); if(!b[tmp_i]) break;tmp_i++; } while(tmp_i<n); }
#define WRITEMEM(p,s,n)     memcpy(p, s, n);p+=n

// Read a signed quantity from little-endian, unaligned data.
// 
static inline short readshort(void * ptr)
{
  char   *cp  = ptr;
  u_char *ucp = ptr;
  return (cp[1] << 8)  |  ucp[0] ;
}

static inline u_short readushort(void * ptr)
{
  u_char *ucp = ptr;
  return (ucp[1] << 8) |  ucp[0] ;
}

static inline long readlong(void * ptr)
{
  char   *cp  = ptr;
  u_char *ucp = ptr;
  return (cp[3] << 24) | (ucp[2] << 16) | (ucp[1] << 8) | ucp[0] ;
}

static inline u_long readulong(void * ptr)
{
  u_char *ucp = ptr;
  return (ucp[3] << 24) | (ucp[2] << 16) | (ucp[1] << 8) | ucp[0] ;
}


#define READBYTE(p)         *((byte   *)p)++
#define READCHAR(p)         *((char   *)p)++
#define READSHORT(p)        readshort ( ((short*) p)++)
#define READUSHORT(p)       readushort(((USHORT*) p)++)
#define READLONG(p)         readlong  (  ((long*) p)++)
#define READULONG(p)        readulong ( ((ULONG*) p)++)
#define READFIXED(p)        readlong  (  ((long*) p)++)
#define READANGLE(p)        readulong ( ((ULONG*) p)++)
#define READSTRING(p,s)     { int tmp_i=0; do { s[tmp_i]=READBYTE(p);  } while(s[tmp_i++]); }
#define SKIPSTRING(p)       while(READBYTE(p))
#define READMEM(p,s,n)      memcpy(s, p, n);p+=n
#endif //__BIG_ENDIAN__
