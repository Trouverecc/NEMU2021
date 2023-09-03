#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX))
{
	cpu.eip = swaddr_read(cpu.esp, DATA_BYTE);
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	return 0;
}
make_helper(concat(ret_i_, SUFFIX))
{ 
	uint32_t x;
	x = instr_fetch(cpu.eip + 1, 2);
	cpu.eip = swaddr_read(cpu.esp, DATA_BYTE);
	cpu.esp = cpu.esp + DATA_BYTE + x;
	print_asm("ret 0x%u", x);
	return 0;
}

#include "cpu/exec/template-end.h"