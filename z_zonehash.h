#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    void Z_AddToCache(void* ptr, const size_t size, const int tag);
    void Z_RemoveFromCache(void* ptr, const size_t size, const int tag);
    void* Z_FindFreeBlock(const size_t size, const int tag);
    void* Z_GetFirstGlobalBlock();
#ifdef __cplusplus
}
#endif