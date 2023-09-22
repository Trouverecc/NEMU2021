#include "common.h"
#include "burst.h"
#include <stdlib.h>
#include "memory/cache.h"

uint32_t dram_read(hwaddr_t addr, size_t len);
void call_ddr3_read(hwaddr_t, void *);
void call_ddr3_write(hwaddr_t, void *, uint8_t *);

void init_cache()
{
    int i;
    for (i = 0; i < CACHE_SIZE_L1 / CACHE_BLOCK_SIZE; i++)
    {
        cache[i].valid = false;
        cache[i].tag = 0;
        memset(cache[i].data, 0, CACHE_BLOCK_SIZE);
    }
    for (i = 0; i < CACHE_SIZE_L2 / CACHE_BLOCK_SIZE; i++)
    {
        cache2[i].valid = false;
        cache2[i].dirty = false;
        cache2[i].tag = 0;
        memset(cache2[i].data, 0, CACHE_BLOCK_SIZE);
    }
    tol_time = 0;
}

uint32_t cache_read(hwaddr_t addr)
{
    uint32_t gid = (addr >> 6) & 0x7f; 
    bool hit = false;
    int i;
    for (i = gid * EIGHT_WAY; i < (gid + 1) * EIGHT_WAY; i++)
    {
        if (cache[i].tag == (addr >> 13) && cache[i].valid)
        {
            hit = true; 
            //printf("hit!");
            tol_time += 2;
            break;
        }
    }
    if (!hit)
    { 
        int j = second_cache_read(addr);
        for (i = gid * EIGHT_WAY; i < (gid + 1) * EIGHT_WAY; i++)
        {
            if (!cache[i].valid)
                break; 
        }
        if (i == (gid + 1) * EIGHT_WAY)
        {
            //ȡֵ
            srand(0);
            i = gid * EIGHT_WAY + rand() % EIGHT_WAY;
        }
        cache[i].valid = true;
        cache[i].tag = addr >> 13;
        memcpy(cache[i].data, cache2[j].data, CACHE_BLOCK_SIZE);
    }
    return i;
}

uint32_t second_cache_read(hwaddr_t addr)
{
    uint32_t gid = (addr >> 6) & ((1 << 12) - 1); //
    uint32_t block = (addr >> 6) << 6;
    int i;
    bool hit = false;
    for (i = gid * SIXTEEN_WAY; i < (gid + 1) * SIXTEEN_WAY; i++)
    {
        if (cache2[i].tag == (addr >> 18) && cache2[i].valid)
        {
            //hit
            hit = true;
            //printf("hit!");
            tol_time += 20;
            break;
        }
    }
    if (!hit)
    {
        //not hit
        int j;
        for (i = gid * SIXTEEN_WAY; i < (gid + 1) * SIXTEEN_WAY; i++)
        {
            if (!cache2[i].valid)
                break;
        }
        if (i == (gid + 1) * SIXTEEN_WAY)
        {
            srand(0);
            i = gid * SIXTEEN_WAY + rand() % SIXTEEN_WAY;
            if (cache2[i].dirty)
            {
                uint8_t mask[BURST_LEN * 2];
                memset(mask, 1, BURST_LEN * 2);
                for (j = 0; j < CACHE_BLOCK_SIZE / BURST_LEN; j++)
                {
                    call_ddr3_write(block + j * BURST_LEN, cache2[i].data + j * BURST_LEN, mask);
                }
            }
        }
        cache2[i].valid = true;
        cache2[i].tag = addr >> 18;
        cache2[i].dirty = false;
        for (j = 0; j < BURST_LEN; j++)
        {
            call_ddr3_read(block + j * BURST_LEN, cache2[i].data + j * BURST_LEN);
        }
        tol_time += 200;
    }
    return i;
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data)
{
    uint32_t gid = (addr >> 6) & 0x7f;
    uint32_t offset = addr & (CACHE_BLOCK_SIZE - 1); //displacement addr
    int i;
    bool hit = false;
    for (i = gid * EIGHT_WAY; i < (gid + 1) * EIGHT_WAY; i++)
    {
        if (cache[i].tag == (addr >> 13) && cache[i].valid)
        {
            hit = true;
            break;
        }
    }
    if (hit)
    {
        memcpy(cache[i].data + offset, &data, len);
    }
    second_cache_write(addr, len, data);
}

void second_cache_write(hwaddr_t addr, size_t len, uint32_t data)
{
    uint32_t gid = (addr >> 6) & ((1 << 12) - 1);
    uint32_t offset = addr & (CACHE_BLOCK_SIZE - 1);
    bool hit = false;
    int i;
    for (i = gid * SIXTEEN_WAY; i < (gid + 1) * SIXTEEN_WAY; i++)
    {
        if (cache2[i].tag == (addr >> 13) && cache2[i].valid)
        {
            hit = true;
            break;
        }
    }
    if (!hit)
    {
        i = second_cache_read(addr);
    }
    cache2[i].dirty = true;
    memcpy(cache2[i].data + offset, &data, len);
}