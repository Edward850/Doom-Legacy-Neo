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

#include <stdlib.h>
#include <string.h>

#include "doomdef.h"
#include "z_zone.h"
#include "i_system.h"
#include "command.h"
#include "i_video.h"
#include "doomstat.h"
#include "z_zonehash.h"
#ifdef HWRENDER
#include "hardware/hw_drv.h"
#endif

#define ZONEID 0x1d4a11

static memblock_t z_blocklist;
static int z_ready = 0;

void Command_Memfree_f(void);

static void Z_LinkBlock(memblock_t* block)
{
    block->next = z_blocklist.next;
    block->prev = &z_blocklist;
    z_blocklist.next->prev = block;
    z_blocklist.next = block;
}

static void Z_UnlinkBlock(memblock_t* block)
{
    block->prev->next = block->next;
    block->next->prev = block->prev;
}

void Z_Init(void)
{
    if (z_ready)
        return;

    z_blocklist.next = &z_blocklist;
    z_blocklist.prev = &z_blocklist;
    z_blocklist.user = (void**)&z_blocklist;
    z_blocklist.tag = PU_STATIC;
    z_blocklist.id = ZONEID;
    z_blocklist.size = 0;

    z_ready = 1;
    COM_AddCommand("memfree", Command_Memfree_f);
}

#ifdef ZDEBUG
void Z_Free2(void* ptr, char* file, int line)
#else
void Z_Free(void* ptr)
#endif
{
    memblock_t* block;
    void* raw;

#ifdef ZDEBUG
    (void)file;
    (void)line;
#endif

    if (!ptr)
        return;

    block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

    // Move this block instead of freeing it if it's not yet purgable, so something else could allocate it later (better matching Doom's behavior).
    if (block->tag < PU_PURGELEVEL)
    {
        block->tag = PU_CACHE;
        Z_AddToCache(block, block->size, block->tag);
        return;
    }
    else
    {
        Z_RemoveFromCache(block, block->size, block->tag);
    }

    if (block->id != ZONEID)
        I_Error("Z_Free: freed a pointer without ZONEID");

    if (block->user > (void**)0x100)
        *block->user = 0;

    Z_UnlinkBlock(block);

    block->id = 0;
    block->user = NULL;
    block->tag = 0;

    raw = *(((void**)block) - 1);
    free(raw);
}

#ifdef ZDEBUG
void* Z_Malloc2(int size, int tag, void* user, int alignbits, char* file, int line)
#else
void* Z_MallocAlign(int size, int tag, void* user, int alignbits)
#endif
{
    const size_t total = sizeof(void*) + sizeof(memblock_t) + size;
    void* raw = NULL;
    byte* userptr = NULL;
    memblock_t* block = NULL;

#ifdef ZDEBUG
    (void)file;
    (void)line;
#endif

    if (!z_ready)
        Z_Init();

    if (size < 0)
        I_Error("Z_Malloc: negative size");

    
    if (tag < PU_PURGELEVEL)
    {
        // Don't look for cache entries if tag is already a purgable block as this can cause a weird feedback loop where a block is allocated, then immediately reallocated, and so on.
        block = Z_FindFreeBlock(size + sizeof(memblock_t), PU_CACHE);
        if (block)
        {
            if (block->user > (void**)0x100)
                *block->user = 0;
            raw = *(((void**)block) - 1);
            userptr = (byte*)raw + sizeof(void*) + sizeof(memblock_t);
        }
    }
    if (!raw)
    {
        raw = malloc(total);
        if (!raw)
        {
            I_Error("Z_Malloc: failed on allocation of %i bytes", size);
            return NULL;
        }
        userptr = (byte*)raw + sizeof(void*) + sizeof(memblock_t);
        block = (memblock_t*)(userptr - sizeof(memblock_t));
        Z_LinkBlock(block);
    }

    ((void**)block)[-1] = raw;  /* store original malloc pointer */

    block->size = size + sizeof(memblock_t);
    block->tag = tag;
    block->id = ZONEID;

    if (user)
    {
        block->user = (void**)user;
        *(void**)user = (void*)userptr;
    }
    else
    {
        if (tag >= PU_PURGELEVEL)
            I_Error("Z_Malloc: an owner is required for purgable blocks");
        block->user = (void**)2;
    }
    if (tag >= PU_PURGELEVEL)
    {
        Z_AddToCache(block, block->size, block->tag);
    }

#ifdef ZDEBUG
    block->ownerfile = file;
    block->ownerline = line;
#endif

    memset(userptr, 0, size);

    return (void*)userptr;
}

void Z_FreeSomeCache()
{
    if (!z_ready)
        return;

    size_t target, cache, used;
    memblock_t* block;

    Z_CheckHeap(-1);
    Z_FreeMemory(&target, &cache, &used, &target);

    target = used / 2; /* target is half of the current memory usage */

    while(cache > target)
    {
        block = Z_GetFirstGlobalBlock();
        if (!block)
            break;

        cache -= block->size;
        Z_Free((byte*)block + sizeof(memblock_t));
    }
}

void Z_FreeTags(int lowtag, int hightag)
{
    memblock_t* block;
    memblock_t* next;

    if (!z_ready)
        return;

    // Free purgable blocks until we are at half of the current memory usage.
    if (hightag < PU_PURGELEVEL)
    {
        Z_FreeSomeCache();
    }

    for (block = z_blocklist.next; block != &z_blocklist; block = next)
    {
        next = block->next;
        if (block->tag >= lowtag && block->tag <= hightag)
            Z_Free((byte*)block + sizeof(memblock_t));
    }
}

void Z_DumpHeap(int lowtag, int hightag)
{
    memblock_t* block;

    CONS_Printf("dynamic OS allocator mode\n");
    CONS_Printf("tag range: %i to %i\n", lowtag, hightag);

    for (block = z_blocklist.next; block != &z_blocklist; block = block->next)
    {
        if (block->tag >= lowtag && block->tag <= hightag)
            CONS_Printf("block:%p size:%7i user:%p tag:%3i prev:%p next:%p\n",
                block, block->size, block->user, block->tag, block->prev, block->next);
    }
}

void Z_FileDumpHeap(FILE* f)
{
    memblock_t* block;
    int i = 0;

    fprintf(f, "dynamic OS allocator mode\n");
    for (block = z_blocklist.next; block != &z_blocklist; block = block->next)
    {
        i++;
        fprintf(f, "block:%p size:%7i user:%7x tag:%3i prev:%p next:%p id:%7i\n",
            block, block->size, (int)block->user, block->tag, block->prev, block->next, block->id);
    }
    fprintf(f, "Total : %d blocks\n", i);
}

void Z_CheckHeap(int i)
{
    memblock_t* block;

    for (block = z_blocklist.next; block != &z_blocklist; block = block->next)
    {
        if (block->id != ZONEID)
            I_Error("Z_CheckHeap: invalid block id %d\n", i);

        if (block->user > (void**)0x100 &&
            (*(block->user)) != ((void*)((byte*)block + sizeof(memblock_t))))
            I_Error("Z_CheckHeap: block doesn't have a proper user %d\n", i);
    }
}

void Z_ChangeTag2(void* ptr, int tag)
{
    memblock_t* block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

    if (block->id != ZONEID)
        I_Error("Z_ChangeTag: pointer without ZONEID");

    if(block->tag == tag)
        return;

    if (tag >= PU_PURGELEVEL && (unsigned)block->user < 0x100)
        I_Error("Z_ChangeTag: an owner is required for purgable blocks");

    if (block->tag >= PU_PURGELEVEL)
        Z_RemoveFromCache(block, block->size, block->tag);

    block->tag = tag;

    if (block->tag >= PU_PURGELEVEL)
        Z_AddToCache(block, block->size, block->tag);
}

void Z_FreeMemory(size_t* realfree, size_t* cachemem, size_t* usedmem, size_t* largefreeblock)
{
    memblock_t* block;
    size_t freebytes, totalbytes;

    *cachemem = 0;
    *usedmem = 0;

    for (block = z_blocklist.next; block != &z_blocklist; block = block->next)
    {
        if (block->tag >= PU_PURGELEVEL)
            *cachemem += block->size;
        else
            *usedmem += block->size;
    }

    freebytes = I_GetFreeMem(&totalbytes);
    if (freebytes == 0 && totalbytes != 0)
    {
        freebytes = totalbytes - (*usedmem + *cachemem);
    }
    *realfree = freebytes;
    *largefreeblock = *realfree; /* Approximation in OS allocator mode */
}

int Z_TagUsage(int tagnum)
{
    memblock_t* block;
    int bytes = 0;

    for (block = z_blocklist.next; block != &z_blocklist; block = block->next)
    {
        if (block->tag == tagnum)
            bytes += block->size;
    }

    return bytes;
}

void Command_Memfree_f(void)
{
    size_t freeb, cache, used, largest;
    size_t freebytes, totalbytes;

    Z_CheckHeap(-1);
    Z_FreeMemory(&freeb, &cache, &used, &largest);

    CONS_Printf("\2Memory Heap Info (OS allocator mode)\n");
    CONS_Printf("used  memory       : %7zu kb\n", used >> 10);
    CONS_Printf("cache memory       : %7zu kb\n", cache >> 10);
    CONS_Printf("available memory   : %7zu kb\n", freeb >> 10);

#ifdef HWRENDER
    if (rendermode != render_soft)
    {
        CONS_Printf("Patch info headers : %7zu kb\n", Z_TagUsage(PU_HWRPATCHINFO) >> 10);
        CONS_Printf("HW Texture cache   : %7zu kb\n", Z_TagUsage(PU_HWRCACHE) >> 10);
        CONS_Printf("Plane polygone     : %7zu kb\n", Z_TagUsage(PU_HWRPLANE) >> 10);
        CONS_Printf("HW Texture used    : %7zu kb\n", HWD.pfnGetTextureUsed() >> 10);
    }
#endif

    CONS_Printf("\2System Memory Info\n");
    freebytes = totalbytes = 0;
    freebytes = I_GetFreeMem(&totalbytes);
    // If the freebytes is 0, calculate the free memory based on the total memory and used memory
    if (totalbytes != 0 && freebytes == 0)
    {
        freebytes = totalbytes - (used + cache);
    }
    CONS_Printf("Total     physical memory: %6zu kb\n", totalbytes >> 10);
    CONS_Printf("Available physical memory: %6zu kb\n", freebytes >> 10);
}

char* Z_Strdup(const char* s, int tag, void** user)
{
    return strcpy(Z_Malloc(strlen(s) + 1, tag, user), s);
}
