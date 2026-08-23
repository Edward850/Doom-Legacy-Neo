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

#pragma once
#include "tables.h"

#ifndef __BIG_ENDIAN__
//
// Little-endian machines
//
#define writeshort(p,b)     *(short*)  (p)   = b
#define writelong(p,b)      *(long *)  (p)   = b
static inline void writebyte_inc(void **ptr, byte value)
{
  *((byte *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(byte);
}
static inline void writechar_inc(void **ptr, char value)
{
  *((char *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(char);
}
static inline void writeshort_inc(void **ptr, short value)
{
  *((short *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(short);
}
static inline void writeushort_inc(void **ptr, USHORT value)
{
  *((USHORT *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(USHORT);
}
static inline void writelong_inc(void **ptr, long value)
{
  *((long *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(long);
}
static inline void writeulong_inc(void **ptr, ULONG value)
{
  *((ULONG *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(ULONG);
}
static inline void writefixed_inc(void **ptr, fixed_t value)
{
  *((fixed_t *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(fixed_t);
}
static inline void writeangle_inc(void **ptr, angle_t value)
{
  *((angle_t *)*ptr) = value;
  *ptr = (char *)*ptr + sizeof(angle_t);
}
#define WRITEBYTE(p,b)      writebyte_inc((void **)&(p), (b))
#define WRITECHAR(p,b)      writechar_inc((void **)&(p), (b))
#define WRITESHORT(p,b)     writeshort_inc((void **)&(p), (b))
#define WRITEUSHORT(p,b)    writeushort_inc((void **)&(p), (b))
#define WRITELONG(p,b)      writelong_inc((void **)&(p), (b))
#define WRITEULONG(p,b)     writeulong_inc((void **)&(p), (b))
#define WRITEFIXED(p,b)     writefixed_inc((void **)&(p), (b))
#define WRITEANGLE(p,b)     writeangle_inc((void **)&(p), (b))
#define WRITESTRING(p,b)    { int tmp_i=0; do { WRITECHAR(p,b[tmp_i]); } while(b[tmp_i++]); }
#define WRITESTRINGN(p,b,n) { int tmp_i=0; do { WRITECHAR(p,b[tmp_i]); if(!b[tmp_i]) break;tmp_i++; } while(tmp_i<n); }
#define WRITEMEM(p,s,n)     memcpy(p, s, n);p+=n

#define readshort(p)	    *((short  *)p)
#define readlong(p)	    *((long   *)p)
static inline byte readbyte_inc(void **ptr)
{
  byte value = *((byte *)*ptr);
  *ptr = (char *)*ptr + sizeof(byte);
  return value;
}
static inline char readchar_inc(void **ptr)
{
  char value = *((char *)*ptr);
  *ptr = (char *)*ptr + sizeof(char);
  return value;
}
static inline short readshort_inc(void **ptr)
{
  short value = *((short *)*ptr);
  *ptr = (char *)*ptr + sizeof(short);
  return value;
}
static inline USHORT readushort_inc(void **ptr)
{
  USHORT value = *((USHORT *)*ptr);
  *ptr = (char *)*ptr + sizeof(USHORT);
  return value;
}
static inline long readlong_inc(void **ptr)
{
  long value = *((long *)*ptr);
  *ptr = (char *)*ptr + sizeof(long);
  return value;
}
static inline ULONG readulong_inc(void **ptr)
{
  ULONG value = *((ULONG *)*ptr);
  *ptr = (char *)*ptr + sizeof(ULONG);
  return value;
}
static inline fixed_t readfixed_inc(void **ptr)
{
  fixed_t value = *((fixed_t *)*ptr);
  *ptr = (char *)*ptr + sizeof(fixed_t);
  return value;
}
static inline angle_t readangle_inc(void **ptr)
{
  angle_t value = *((angle_t *)*ptr);
  *ptr = (char *)*ptr + sizeof(angle_t);
  return value;
}
#define READBYTE(p)         readbyte_inc((void **)&(p))
#define READCHAR(p)         readchar_inc((void **)&(p))
#define READSHORT(p)        readshort_inc((void **)&(p))
#define READUSHORT(p)       readushort_inc((void **)&(p))
#define READLONG(p)         readlong_inc((void **)&(p))
#define READULONG(p)        readulong_inc((void **)&(p))
#define READFIXED(p)        readfixed_inc((void **)&(p))
#define READANGLE(p)        readangle_inc((void **)&(p))
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
