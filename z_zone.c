// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: z_zone.c,v 1.17 2002/07/29 21:52:25 hurdler Exp $
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
// $Log: z_zone.c,v $
// Revision 1.17  2002/07/29 21:52:25  hurdler
// Someone want to have a look at this bugs
//
// Revision 1.16  2001/06/30 15:06:01  bpereira
// fixed wronf next level name in intermission
//
// Revision 1.15  2001/03/13 22:14:20  stroggonmeth
// Long time no commit. 3D floors, FraggleScript, portals, ect.
//
// Revision 1.14  2001/01/25 22:15:44  bpereira
// added heretic support
//
// Revision 1.13  2000/11/06 20:52:16  bpereira
// no message
//
// Revision 1.12  2000/11/03 13:15:13  hurdler
// Some debug comments, please verify this and change what is needed!
//
// Revision 1.11  2000/11/02 17:50:10  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.10  2000/10/14 18:33:34  hurdler
// sorry, I forgot to put an #ifdef for hw memory report
//
// Revision 1.9  2000/10/14 18:32:16  hurdler
// sorry, I forgot to put an #ifdef for hw memory report
//
// Revision 1.8  2000/10/04 16:33:54  hurdler
// Implement hardware texture memory stats
//
// Revision 1.7  2000/10/02 18:25:45  bpereira
// no message
//
// Revision 1.6  2000/08/31 14:30:56  bpereira
// no message
//
// Revision 1.5  2000/07/01 09:23:49  bpereira
// no message
//
// Revision 1.4  2000/04/30 10:30:10  bpereira
// no message
//
// Revision 1.3  2000/04/24 20:24:38  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Zone Memory Allocation. Neat.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "z_zone.h"
#include "i_system.h"
#include "command.h"
#include "m_argv.h"
#include "i_video.h"
#include "doomstat.h"
#ifdef HWRENDER
#include "hardware/hw_drv.h" // for hardware memory stats
#endif

// =========================================================================
//                        ZONE MEMORY ALLOCATION
// =========================================================================
//
// There is never any space between memblocks,
//  and there will never be two contiguous free memblocks.
// The rover can be left pointing at a non-empty block.
//
// It is of no value to free a cachable block,
//  because it will get overwritten automatically if needed.
//

#define ZONEID  0x1d4a11

// use malloc so when go over malloced region do a sigsegv
//#define DEBUGMEMCLACH

typedef struct
{
    // total bytes malloced, including header
    int         size;

    // start / end cap for linked list
    memblock_t  blocklist;

    memblock_t* rover;

} memzone_t;


static memzone_t* mainzone;

void Command_Memfree_f( void );

//
// Z_ClearZone
//
// UNUSE at this time 14 nov 98
void Z_ClearZone (memzone_t* zone)
{
    memblock_t*         block;

    // set the entire zone to one free block
    zone->blocklist.next =
        zone->blocklist.prev =
        block = (memblock_t *)( (byte *)zone + sizeof(memzone_t) );

    zone->blocklist.user = (void *)zone;
    zone->blocklist.tag = PU_STATIC;
    zone->rover = block;

    block->prev = block->next = &zone->blocklist;

    // NULL indicates a free block.
    block->user = NULL;

    block->size = zone->size - sizeof(memzone_t);
}


static byte mb_used = 6;

//
// Z_Init
//
void Z_Init (void)
{
#ifndef DEBUGMEMCLACH
    memblock_t* block;
    int         size;
    ULONG       free,total;

    if( M_CheckParm ("-mb") )
    {
        if( M_IsNextParm() )
            mb_used = atoi (M_GetNextParm());
        else
            I_Error("usage : -mb <numbers of megabyte fot the heap>");
    }
    else
    {
        free = I_GetFreeMem(&total)>>20;
        CONS_Printf("system memory %dMb free %dMb\n",total>>20,free);
        // we assume that systeme use a lot of memory for disk cache
        if( free<6 )
            free=total>>21;
        mb_used = min(max(free, mb_used), 20); // min 6Mb max 20Mb
    }
    CONS_Printf ("%d megabytes requested for Z_Init.\n", mb_used);
    size = mb_used<<20;
    mainzone = (memzone_t *)malloc(size);
    if( !mainzone )
         I_Error("Could not allocate %d megabytes.\n"
                 "Please use -mb parameter and specify a lower value.\n", mb_used);

    // tuch memory to stop swaping
    memset(mainzone, 0, size);
//    if( M_checkParm("-lock") )
//        I_LockMemory(mainzone);

    mainzone->size = size;

    // set the entire zone to one free block
    // block is the only free block in the zone
    mainzone->blocklist.next =
        mainzone->blocklist.prev =
        block = (memblock_t *)( (byte *)mainzone + sizeof(memzone_t) );

    mainzone->blocklist.user = (void *)mainzone;
    mainzone->blocklist.tag = PU_STATIC;
    mainzone->rover = block;

    block->prev = block->next = &mainzone->blocklist;

    // NULL indicates a free block.
    block->user = NULL;

    block->size = mainzone->size - sizeof(memzone_t);

    COM_AddCommand ("memfree", Command_Memfree_f);
#endif
}


//
// Z_Free
//
#ifdef ZDEBUG
void Z_Free2(void* ptr ,char *file,int line)
#else
void Z_Free (void* ptr)
#endif
{
    memblock_t*         block;
    memblock_t*         other;

    block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
#ifdef DEBUGMEMCLACH
    if (block->user > (void **)0x100)
        *block->user = 0;
    free(block);
    return;
#endif


#ifdef ZDEBUG
   // SoM: HARDERCORE debuging
   // Write all Z_Free's to a debug file
   if(debugfile)
     fprintf(debugfile, "ZFREE@File: %s, line: %i\n", file, line);
   //BP: hardcore debuging
   // check if there is not a user in this zone
for (other = mainzone->blocklist.next ; other->next != &mainzone->blocklist; other = other->next)
{
   if((other!=block) &&
      (other->user>(void **)0x100) &&
      ((other->user)>=(void **)block) &&
      ((other->user)<=(void **)((byte *)block)+block->size) )
   {
       //I_Error("Z_Free: Pointer in zone\n");
       I_Error("Z_Free: Pointer %s:%d in zone at %s:%i",other->ownerfile,other->ownerline,file,line);
   }
}
#endif

    if (block->id != ZONEID)
        I_Error ("Z_Free: freed a pointer without ZONEID");
#ifdef PARANOIA
    // get direct a segv when using a pointer that isn't right
    memset(ptr,0,block->size-sizeof(memblock_t));
#endif
    if (block->user > (void **)0x100)
    {
        // smaller values are not pointers
        // Note: OS-dependend?

        // clear the user's mark
        *block->user = 0;
    }

    // mark as free
    block->user = NULL;
    block->tag = 0;
    block->id = 0;

    other = block->prev;

    if (!other->user)
    {
        // merge with previous free block
        other->size += block->size;
        other->next = block->next;
        other->next->prev = other;

        if (block == mainzone->rover)
            mainzone->rover = other;

        block = other;
    }

    other = block->next;
    if (!other->user)
    {
        // merge the next free block onto the end
        block->size += other->size;
        block->next = other->next;
        block->next->prev = block;

        if (other == mainzone->rover)
            mainzone->rover = block;
    }
}



//
// Z_Malloc
// You can pass a NULL user if the tag is < PU_PURGELEVEL.
//
#define MINFRAGMENT             sizeof(memblock_t)

#ifdef ZDEBUG
void*   Z_Malloc2 (int size, int tag, void *user,int alignbits, char *file,int line)
#else
void* Z_MallocAlign( int           size,
                     int           tag,
                     void*         user,
                     int           alignbits)
#endif
{
    ULONG alignmask=(1<<alignbits)-1;
#define ALIGN(a) (((ULONG)a+alignmask) & ~alignmask)
    int   extra;
    int   basedata;
    memblock_t* start;
    memblock_t* rover;
    memblock_t* newblock;
    memblock_t* base;

    size = (size + 3) & ~3;

    // scan through the block list,
    // looking for the first free block
    // of sufficient size,
    // throwing out any purgable blocks along the way.

    // account for size of block header
    size += sizeof(memblock_t);

#ifdef DEBUGMEMCLACH
    newblock=malloc(size);
    newblock->id=ZONEID;
    newblock->tag = tag;
    newblock->user = user;
    ((byte *)newblock) += sizeof(memblock_t);
    if(user) *(void **)user=newblock;
    return newblock;
#endif    

    // if there is a free block behind the rover,
    //  back up over them
    // added comment : base is used to point at the begin of a region in case
    //                 when there is two (or more) adjacent purgable block
    base = mainzone->rover;

    if (!base->prev->user)
        base = base->prev;

    rover = base;
    start = base->prev;

    if (base->id && base->id != ZONEID) //Hurdler: this shouldn't happen
    {
        printf("WARNING: Legacy may crash in a few time. This is a known bug, sorry.\n");
    }

    do
    {
        if (rover == start)
        {
            // scanned all the way around the list
            //faB: debug to see if problems of memory fragmentation..
            Command_Memfree_f();

            I_Error ("Z_Malloc: failed on allocation of %i bytes\n"
                     "Try to increase heap size using -mb parameter (actual heap size : %d Mb)\n", size, mb_used);
        }

        if (rover->user)
        {
            if (rover->tag < PU_PURGELEVEL)
            {
                // hit a block that can't be purged,
                //  so move base past it

                //Hurdler: FIXME: this is where the crashing problem seem to come from
                base = rover = rover->next;
            }
            else
            {
                // free the rover block (adding the size to base)

                // the rover can be the base block
                base = base->prev;
                Z_Free ((byte *)rover+sizeof(memblock_t));
                base = base->next;
                rover = base->next;
            }
        }
        else
            rover = rover->next;
        basedata = ALIGN((ULONG)base + sizeof(memblock_t));

    } while (base->user ||
     //Hurdler: huh? it crashed on my system :( (with 777.wad and 777.deh, only software mode)
     //         same problem with MR.ROCKET's wad -> it's probably not a problem with the wad !?
     //         this is because base doesn't point to something valid (and it's not NULL)
             ((ULONG)base)+base->size < basedata+size-sizeof(memblock_t));

    // aligning can leave free space in current block so make it realy free
    if( alignbits )
    {
        memblock_t *newbase=((memblock_t*)basedata) - 1;
        int sizediff = (byte*)newbase - (byte*)base;

        if( sizediff > MINFRAGMENT )
        {
            newbase->prev = base;
            newbase->next = base->next;
            newbase->next->prev = newbase;

            newbase->size = base->size - sizediff;
            base->next = newbase;
            base->size = sizediff;
        }
        else
        {
            // ajuste size of preview block if adjacent (not cycling)
            if( base->prev<base )
                base->prev->size += sizediff;
            base->prev->next = newbase;
            base->next->prev = newbase;
            base->size -= sizediff;
            memcpy(newbase,base,sizeof(memblock_t));
        }
        base = newbase;
    }

    // found a block big enough
    extra = base->size - size;

    if (extra >  MINFRAGMENT)
    {
        // there will be a free fragment after the allocated block
        newblock = (memblock_t *) ((byte *)base + size );
        newblock->size = extra;

        // NULL indicates free block.
        newblock->user = NULL;
        newblock->tag = 0;
        newblock->id = 0;
        newblock->prev = base;
        newblock->next = base->next;
        newblock->next->prev = newblock;

        base->next = newblock;
        base->size = size;
    }

    if (user)
    {
        // mark as an in use block
        base->user = user;
        *(void **)user = (void *) ((byte *)base + sizeof(memblock_t));
    }
    else
    {
        if (tag >= PU_PURGELEVEL)
            I_Error ("Z_Malloc: an owner is required for purgable blocks");

        // mark as in use, but unowned
        base->user = (void *)2;
    }
    base->tag = tag;

#ifdef ZDEBUG
    base->ownerfile = file;
    base->ownerline = line;
#endif

    // next allocation will start looking here
    mainzone->rover = base->next;

    base->id = ZONEID;

    return (void *) ((byte *)base + sizeof(memblock_t));
}



//
// Z_FreeTags
//
void Z_FreeTags( int           lowtag,
                 int           hightag )
{
    memblock_t* block;
    memblock_t* next;
#ifdef DEBUGMEMCLACH
    return;
#endif
    for (block = mainzone->blocklist.next ;
         block != &mainzone->blocklist ;
         block = next)
    {
        // get link before freeing
        next = block->next;

        // free block?
        if (!block->user)
            continue;

        if (block->tag >= lowtag && block->tag <= hightag)
            Z_Free ( (byte *)block+sizeof(memblock_t));
    }
}



//
// Z_DumpHeap
// Note: TFileDumpHeap( stdout ) ?
//
void Z_DumpHeap ( int           lowtag,
                  int           hightag )
{
    memblock_t* block;

    CONS_Printf ("zone size: %i  location: %p\n",
            mainzone->size,mainzone);

    CONS_Printf ("tag range: %i to %i\n",
            lowtag, hightag);

    for (block = mainzone->blocklist.next ; ; block = block->next)
    {
        if (block->tag >= lowtag && block->tag <= hightag)
            CONS_Printf ("block:%p    size:%7i    user:%p    tag:%3i prev:%p next:%p\n",
                    block, block->size, block->user, block->tag, block->next, block->prev);

        if (block->next == &mainzone->blocklist)
        {
            // all blocks have been hit
            break;
        }

        if ( (byte *)block + block->size != (byte *)block->next)
            CONS_Printf ("ERROR: block size does not touch the next block\n");

        if ( block->next->prev != block)
            CONS_Printf ("ERROR: next block doesn't have proper back link\n");

        if (!block->user && !block->next->user)
            CONS_Printf ("ERROR: two consecutive free blocks\n");
    }
}


//
// Z_FileDumpHeap
//
void Z_FileDumpHeap (FILE* f)
{
    memblock_t* block;
    int i=0;

    fprintf (f, "zone size: %i     location: %p\n",mainzone->size,mainzone);

    for (block = mainzone->blocklist.next ; ; block = block->next)
    {
        i++;
        fprintf (f,"block:%p size:%7i user:%7x tag:%3i prev:%p next:%p id:%7i\n",
                 block, block->size, (int)block->user, block->tag, block->prev, block->next, block->id);

        if (block->next == &mainzone->blocklist)
        {
            // all blocks have been hit
            break;
        }

        if ( (block->user > (void **)0x100) && 
             ((int)(*(block->user))!=((int)block)+(int)sizeof(memblock_t)))
            fprintf (f,"ERROR: block don't have a proper user\n");

        if ( (byte *)block + block->size != (byte *)block->next)
            fprintf (f,"ERROR: block size does not touch the next block\n");

        if ( block->next->prev != block)
            fprintf (f,"ERROR: next block doesn't have proper back link\n");

        if (!block->user && !block->next->user)
            fprintf (f,"ERROR: two consecutive free blocks\n");
    }
    fprintf (f,"Total : %d blocks\n"
               "===============================================================================\n\n",i);
}



//
// Z_CheckHeap
//
void Z_CheckHeap (int i)
{
    memblock_t* block;
#ifdef DEBUGMEMCLACH
    return;
#endif
    for (block = mainzone->blocklist.next ; ; block = block->next)
    {
        if (block->next == &mainzone->blocklist)
        {
            // all blocks have been hit
            break;
        }

        if ( (block->user > (void **)0x100) && 
             ((int)(*(block->user))!=((int)block)+(int)sizeof(memblock_t)))
            I_Error ("Z_CheckHeap: block don't have a proper user %d\n",i);

        if ( (byte *)block + block->size != (byte *)block->next)
            I_Error ("Z_CheckHeap: block size does not touch the next block %d\n",i);

        if ( block->next->prev != block)
            I_Error ("Z_CheckHeap: next block doesn't have proper back link %d\n",i);

        if (!block->user && !block->next->user)
            I_Error ("Z_CheckHeap: two consecutive free blocks %d\n",i);
    }
}




//
// Z_ChangeTag
//
void Z_ChangeTag2 ( void* ptr, int tag )
{
    memblock_t* block;
#ifdef DEBUGMEMCLACH
// can't free because the most pu_cache allocated is to use juste after
//    if(tag>=PU_PURGELEVEL)
//        Z_Free(ptr);
    return;
#endif
    block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));

    if (block->id != ZONEID)
        I_Error ("Z_ChangeTag: freed a pointer without ZONEID");

    if (tag >= PU_PURGELEVEL && (unsigned)block->user < 0x100)
        I_Error ("Z_ChangeTag: an owner is required for purgable blocks");

    block->tag = tag;
}



//
// Z_FreeMemory
//
void Z_FreeMemory (int *realfree,int *cachemem,int *usedmem,int *largefreeblock)
{
    memblock_t*         block;
    int freeblock=0;
#ifdef DEBUGMEMCLACH
    return;
#endif

    *realfree = 0;
    *cachemem = 0;
    *usedmem  = 0;
    *largefreeblock = 0;

    for (block = mainzone->blocklist.next ;
         block != &mainzone->blocklist;
         block = block->next)
    {
        if (block->user==0)
        {
            // free memory
            *realfree += block->size;
            freeblock += block->size;
            if(freeblock>*largefreeblock)
                *largefreeblock = freeblock;
        }
        else
            if(block->tag >= PU_PURGELEVEL)
            {
                // purgable memory (cache)
                *cachemem += block->size;
                freeblock += block->size;
                if(freeblock>*largefreeblock)
                    *largefreeblock = freeblock;
            }
            else
            {
                // used block
                *usedmem += block->size;
                freeblock=0;
            }
    }
}


//
// Z_TagUsage
// - return number of bytes currently allocated in the heap for the given tag
int Z_TagUsage (int tagnum)
{
    memblock_t*     block;
    int             bytes = 0;

    for (block = mainzone->blocklist.next ;
         block != &mainzone->blocklist;
         block = block->next)
    {
        if (block->user!=0 && block->tag == tagnum)
            bytes += block->size;
    }

    return bytes;
}


void Command_Memfree_f( void )
{
    int   free,cache,used,largefreeblock;
    ULONG freebytes, totalbytes;

    Z_CheckHeap (-1);
    Z_FreeMemory(&free,&cache,&used,&largefreeblock);
    CONS_Printf("\2Memory Heap Info\n");
    CONS_Printf("Total heap size    : %7d kb\n", mb_used<<10);
    CONS_Printf("used  memory       : %7d kb\n", used>>10);
    CONS_Printf("free  memory       : %7d kb\n", free>>10);
    CONS_Printf("cache memory       : %7d kb\n", cache>>10);
    CONS_Printf("largest free block : %7d kb\n", largefreeblock>>10);
#ifdef HWRENDER
    if( rendermode != render_soft )
    {
    CONS_Printf("Patch info headers : %7d kb\n", Z_TagUsage(PU_HWRPATCHINFO)>>10);
    CONS_Printf("HW Texture cache   : %7d kb\n", Z_TagUsage(PU_HWRCACHE)>>10);
    CONS_Printf("Plane polygone     : %7d kb\n", Z_TagUsage(PU_HWRPLANE)>>10);
    CONS_Printf("HW Texture used    : %7d kb\n", HWD.pfnGetTextureUsed()>>10);
    }
#endif

    CONS_Printf("\2System Memory Info\n");
    freebytes = I_GetFreeMem(&totalbytes);
    CONS_Printf("Total     physical memory: %6d kb\n", totalbytes>>10);
    CONS_Printf("Available physical memory: %6d kb\n", freebytes>>10);
}





char *Z_Strdup(const char *s, int tag, void **user)
{
  return strcpy(Z_Malloc(strlen(s)+1, tag, user), s);
}
