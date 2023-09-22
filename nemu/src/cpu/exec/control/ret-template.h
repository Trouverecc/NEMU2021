#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_near_, SUFFIX)) {
    cpu.eip = MEM_R(reg_l(R_SP),2);
    if(DATA_BYTE == 2) cpu.eip &= 0xffff;
    reg_l(R_ESP) += DATA_BYTE;
    print_asm("ret");
    return 1;
} 

make_helper(concat(ret_i_, SUFFIX)){

	cpu.eip = MEM_R (cpu.esp, 2);
	if (DATA_BYTE == 2) cpu.eip &= 0xffff;
	cpu.esp += DATA_BYTE;
    int val = instr_fetch(eip + 1, 2);
	int i;
	for (i = 0; i < val; i += DATA_BYTE) MEM_W (cpu.esp + i,0, 2);
	cpu.esp += val;

	print_asm("ret $0x%x",val);
	return 1;
}
#include "cpu/exec/template-end.h"