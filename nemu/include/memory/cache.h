#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

#define CACHE_BLOCK_SIZE 64
#define CACHE_SIZE_L1 64*1024
#define CACHE_SIZE_L2 4*1024*1024
#define EIGHT_WAY 8
#define SIXTEEN_WAY 16

struct Cache{
    bool valid;
    int tag;
    uint8_t data[CACHE_BLOCK_SIZE];
} cache[CACHE_SIZE_L1/CACHE_BLOCK_SIZE];

struct SecondaryCache {
    bool valid,dirty;
    int tag;
    uint8_t data[CACHE_BLOCK_SIZE];
} cache2[CACHE_SIZE_L2/CACHE_BLOCK_SIZE];

void init_cache();
uint32_t cache_read(hwaddr_t addr);
uint32_t second_cache_read(hwaddr_t addr);
void cache_write(hwaddr_t addr, size_t len, uint32_t data);
void second_cache_write(hwaddr_t addr, size_t len, uint32_t data);

int tol_time;

#endif