#ifndef __TLB_H__
#define __TLB_H__

#include "common.h"

#define TLB_SIZE 64

typedef struct{
    bool valid;
    uint32_t tag,data;//tag为虚拟也号，data为物理页号
}TLB;

TLB tlb[TLB_SIZE];
void init_tlb();

int read_tlb(lnaddr_t addr);
void write_tlb(lnaddr_t addr, hwaddr_t hwaddr);
#endif