#include "memory/tlb.h"
#include "burst.h"
#include <time.h>
#include <stdlib.h>

void init_tlb()
{
    int i = 0;
    for(i = 0; i < TLB_SIZE; i++) tlb[i].valid = 0;
    srand(clock());
}

int read_tlb(lnaddr_t addr)
{
    int tag = addr >> 12;
    int i = 0;
    for(i = 0; i < TLB_SIZE; i++)
    {
        if(tlb[i].valid && tlb[i].tag == tag)
            return i;
    }
    return -1;
}

void write_tlb(lnaddr_t addr, hwaddr_t hwaddr)
{
    int tag = addr >> 12;
    int i = 0;
    hwaddr >>= 12;
    for(i = 0; i < TLB_SIZE; i++)
    {
        if(!tlb[i].valid)
        {
            tlb[i].valid = 1;
            tlb[i].tag = tag;
            tlb[i].data = hwaddr;
            return;
        }
    }
    i = rand() % TLB_SIZE;
    tlb[i].valid = 1;
    tlb[i].data = hwaddr;
    tlb[i].tag = tag;
    return;
}