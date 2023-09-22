#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "../../lib-common/x86-inc/cpu.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };
enum { R_CS, R_DS, R_ES, R_SS};

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	uint16_t selector,attribute;
	uint32_t base,limit;
}S_reg;

typedef struct {
	union{
		struct{
			uint32_t p:1, rw:1, us:1, :2, a:1, d:1, :2, ava:3, addr:20;
		};
		uint32_t val;
	};
}Page_info;

typedef struct {
     union {
		union {
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];

		/* Do NOT change the order of the GPRs' definitions. */

		struct {
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};

	swaddr_t eip;
     
     union {
		struct {
			uint32_t CF		:1;
			uint32_t pad0	:1;
			uint32_t PF		:1;
			uint32_t pad1	:1;
			uint32_t AF		:1;
			uint32_t pad2	:1;
			uint32_t ZF		:1;
			uint32_t SF		:1;
			uint32_t TF		:1;
			uint32_t IF		:1;
			uint32_t DF		:1;
			uint32_t OF		:1;
			uint32_t IOPL	:2;
			uint32_t NT		:1;
			uint32_t pad3	:1;
			uint16_t pad4;
		};
		uint32_t val;
	} eflags;


	struct {
		uint32_t base,limit;
	} GDTR;
	//保护机制开关
	CR0 cr0;
	CR3 cr3;
	//段寄存器->段选择符
	union{
		
		struct{
			
			S_reg sreg[4];
		};
		struct{
			S_reg CS,DS,SS,ES;
		};

	};

} CPU_state;

extern CPU_state cpu;

typedef struct{
	union{
		struct{
			uint16_t lim1,b1;
		};
		uint32_t p1;
	};
	union{
		struct{
			uint32_t b2:8,a:1,type:3,s:1,dpl:2,p:1,lim2:4;
			uint32_t avl:1,:1,x:1,g:1,b3:8;
		};
		uint32_t p2;
	};
}SREG_info;
SREG_info sreg_info;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

void sreg_set(uint8_t);

#endif