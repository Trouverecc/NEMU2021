#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX)) {
	int len = concat(decode_i_, SUFFIX)(eip + 1);
    reg_l(R_ESP) -= 4;
    swaddr_write(reg_l(R_ESP), 4, cpu.eip + len , R_SS);
    DATA_TYPE_S m = op_src -> val;
    print_asm("call 0x%x",cpu.eip + 1 + len + m);
    cpu.eip += m;
    return len + 1;
} 

make_helper(concat(call_rm_, SUFFIX)){
    int len = concat(decode_rm_, SUFFIX)(eip + 1);
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP) , 4, cpu.eip + len , R_SS);
	DATA_TYPE_S m = op_src -> val;
	print_asm("call 0x%x",m);
	cpu.eip = m - len - 1;
	return len + 1;
}

#include "cpu/exec/template-end.h"
