#include "common.h"
#include "memory/cache.h"
#include "burst.h"
#include <stdlib.h>
#include "cpu/reg.h"
#include "memory/tlb.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
int is_mmio(hwaddr_t);
uint32_t mmio_read(hwaddr_t, size_t, int);
void mmio_write(hwaddr_t, size_t, uint32_t, int);
extern uint8_t current_sreg;

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	//redirect cache_read()
	int index = is_mmio(addr);
	if (index >= 0)
	{
		return mmio_read(addr, len, index);
	}
	uint32_t offset = addr & (CACHE_BLOCK_SIZE - 1);
	uint32_t block = cache_read(addr);
	uint8_t temp[4];
	memset(temp, 0, sizeof(temp));
	if (offset + len >= CACHE_BLOCK_SIZE) //addr too long && cache_read again
	{
		uint32_t _block = cache_read(addr + len);
		memcpy(temp, cache[block].data + offset, CACHE_BLOCK_SIZE - offset);
		memcpy(temp + CACHE_BLOCK_SIZE - offset, cache[_block].data, len - (CACHE_BLOCK_SIZE - offset));
	}
	else
	{
		memcpy(temp, cache[block].data + offset, len);
	}
	int zero = 0;
	uint32_t cnt = unalign_rw(temp + zero, 4) & (~0u >> ((4 - len) << 3));
	//printf("time: %d\n", tol_time);
	return cnt;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	//dram_write(addr, len, data); redirect
	int index = is_mmio(addr);
	if (index >= 0)
	{
		mmio_write(addr, len, data, index);
		return;
	}
	cache_write(addr, len, data);
}

hwaddr_t page_translate(lnaddr_t addr){
    if(!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr;
    uint32_t dicionary = addr >> 22;
    uint32_t page = (addr >> 12) & 0x3ff;
    uint32_t offset = addr & 0xfff;

    int index = read_tlb(addr);
    if(index != -1) return ((tlb[index].data << 12) + offset);

   
    uint32_t tmp = (cpu.cr3.page_directory_base << 12) + dicionary * 4;
    Page_info dictionary_, page_;
    dictionary_.val = hwaddr_read(tmp, 4);
    
    tmp = (dictionary_.addr << 12) + page * 4;
    page_.val = hwaddr_read(tmp, 4);
#ifdef DEBUG_page_p
	printf("eip:0x%x\taddr 0x%x\n", cpu.eip, addr);
#endif
	Assert(dictionary_.p == 1, "dirctionary present != 1");
	Assert(page_.p == 1, "second page table present != 1");
#ifdef DEBUG_page
	printf("0x%x\n", (page_.addr << 12) + offset);
#endif
	hwaddr_t addr_ = (page_.addr << 12) + offset;
	write_tlb(addr, addr_);
	return addr_;
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
    uint32_t offset = addr & 0xfff;
    
    if((int64_t)(offset + len) > 0x1000){
        size_t l = 0xfff - offset + 1;
        uint32_t down_val = lnaddr_read(addr, l);  //low bit
        uint32_t up_val = lnaddr_read(addr + l, len - 1);  //high bit
        return (up_val << (l * 8)) | down_val;
    }
    else{
        hwaddr_t hwaddr = page_translate(addr);
        return hwaddr_read(hwaddr, len);
    }
}
void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	assert(len==1||len==2||len==4);
	uint32_t offset=addr&0xfff;
	if((int64_t)(offset+len)>0x1000)
	{
		size_t l=0xfff-offset+1;
		lnaddr_write(addr,l,data&((1<<(l*8))-1));//写低位
		lnaddr_write(addr+l,len-l,data>>(l*8));//写高位
	}
	else{
		hwaddr_t hwaddr=page_translate(addr);
		hwaddr_write(hwaddr,len,data);
	}
}

lnaddr_t seg_translate(swaddr_t addr,size_t len,uint8_t sreg) {
	if(cpu.cr0.protect_enable==0) return addr; //实模式
	else { //turn to IA-32 mode
		lnaddr_t ans=cpu.sreg[sreg].base+addr;
		return ans;
	}
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {

	assert(len==1||len==2||len==4);

	lnaddr_t lnaddr=seg_translate(addr,len,sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
	assert(len == 1 || len == 2 || len == 4);
	lnaddr_t lnaddr=seg_translate(addr,len,sreg);
	lnaddr_write(lnaddr, len, data);
}

hwaddr_t cmd_page_translate(lnaddr_t addr){
    if(!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr;
    uint32_t dicionary = addr >> 22;
    uint32_t page = (addr >> 12) & 0x3ff;
    uint32_t offset = addr & 0xfff;


    // 页目录基地址 + 页目录号 * 页表项大小
    uint32_t tmp = (cpu.cr3.page_directory_base << 12) + dicionary * 4;
    Page_info dictionary_, page_;
    dictionary_.val = hwaddr_read(tmp, 4);
    // 二级页表基地址 + 页号 + 页表项大小
    tmp = (dictionary_.addr << 12) + page * 4;
    page_.val = hwaddr_read(tmp, 4);
    if(dictionary_.p != 1){
        printf("dictionary present != 1!\n");
        return 0;
    }
    if(page_.p != 1){
        printf("second page table present != 1!\n");
        return 0;
    }
    return (page_.addr << 12) + offset;
}

//线性地址->物理地址
