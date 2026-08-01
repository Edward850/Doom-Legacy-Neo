// Copyright (C) 2026 Edward Richardson
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

#include "z_zonehash.h"
#include <unordered_map>
#include <list>

static constexpr int PU_CACHESTART = 101;

struct ptrChain
{
    void* ptr;
    ptrChain* next;
    ptrChain* prev;
    ptrChain* mnext;
    ptrChain* mprev;
    ptrChain(void* p) : ptr(p), next(nullptr), prev(nullptr), mnext(nullptr), mprev(nullptr) {}
};

static std::unordered_map<size_t, ptrChain*> cacheMap[2];
static ptrChain* globalCacheHead = nullptr;

void Z_AddToCache(void* ptr, const size_t size, const int tag)
{
    ptrChain* newNode = new ptrChain(ptr);
    std::unordered_map<size_t, ptrChain*>* map = &cacheMap[tag - PU_CACHESTART];
    auto it = map->find(size);
    if(it == map->end())
    {
        (*map)[size] = newNode;

        // This constructs a circular doubly linked list, so we can easily add to the end.
        newNode->next = newNode;
        newNode->prev = newNode;
    }
    else
    {
        // Got to the tail simply by stepping back from the head (it->second), and then insert.
        newNode->next = it->second; // Head of the list
        newNode->prev = it->second->prev; // New entry gets the previous tail as its prev
        newNode->prev->next = newNode; // Old tail's next points to the new node
        it->second->prev = newNode; // Head's prev points to the new node
    }

    if (globalCacheHead == nullptr)
    {
        globalCacheHead = newNode;
        newNode->mnext = newNode;
        newNode->mprev = newNode;
    }
    else
    {
        // Insert into the global circular list
        newNode->mnext = globalCacheHead; // New node's next is the current head
        newNode->mprev = globalCacheHead->mprev; // New node's prev is the current tail
        globalCacheHead->mprev->mnext = newNode; // Old tail's next points to the new node
        globalCacheHead->mprev = newNode; // Head's prev points to the new node
    }
}

static void Z_UnlinkFromCache(ptrChain* current)
{
    current->prev->next = current->next;
    current->next->prev = current->prev;
    current->mprev->mnext = current->mnext;
    current->mnext->mprev = current->mprev;
    // Remove from the global circular list
    if (globalCacheHead == current)
    {
        if (current->mnext == current) // If it's the only node, set globalCacheHead to nullptr
        {
            globalCacheHead = nullptr;
        }
        else
        {
            globalCacheHead = current->mnext; // Move head to next node
        }
    }
}

void Z_RemoveFromCache(void* ptr, const size_t size, const int tag)
{
    std::unordered_map<size_t, ptrChain*>* map = &cacheMap[tag - PU_CACHESTART];
    auto it = map->find(size);
    if (it != map->end())
    {
        ptrChain* current = it->second;
        while (current)
        {
            if (current->ptr == ptr)
            {
                Z_UnlinkFromCache(current);
                if (current == it->second) // If we're removing the head, move the head pointer
                {
                    if (current->next == current) // If it's the only node, remove the entry from the map
                    {
                        map->erase(it);
                    }
                    else
                    {
                        it->second = current->next; // Move head to next node
                    }
                }
                delete current;
                break;
            }
            current = current->next;
            if (current == it->second) // If we've looped back to the head, break
                break;
        }
    }
}

void* Z_FindFreeBlock(const size_t size, const int tag)
{
    std::unordered_map<size_t, ptrChain*>* map = &cacheMap[tag - PU_CACHESTART];
    auto it = map->find(size);
    if (it != map->end())
    {
        // Grab the first node as it's the oldest one (FIFO).
        void* ptr = it->second->ptr;
        ptrChain* current = it->second;
        Z_UnlinkFromCache(current);
        if (current == it->second) // If we're removing the head, move the head pointer
        {
            if (current->next == current) // If it's the only node, remove the entry from the map
            {
                map->erase(it);
            }
            else
            {
                it->second = current->next; // Move head to next node
            }
        }
        delete current;
        return ptr;
    }
    return nullptr;
}

void* Z_GetFirstGlobalBlock()
{
    return globalCacheHead ? globalCacheHead->ptr : nullptr;
}
