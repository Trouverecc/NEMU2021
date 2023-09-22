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

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

