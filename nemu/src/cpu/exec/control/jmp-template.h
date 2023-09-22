#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	cpu.eip += op_src->val;
	print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)
#if DATA_BYTE == 4
make_helper(jmp_rm_l) {
	int len = decode_rm_l(eip + 1);
	cpu.eip = op_src->val - (len + 1);
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}

make_helper(ljmp) {
	cpu.eip=instr_fetch(cpu.eip+1,4)-7;
	cpu.CS.selector=instr_fetch(cpu.eip+5,2);
	sreg_set(R_CS);
	print_asm("ljmp 0x%x 0x%x",instr_fetch(cpu.eip+5,2),instr_fetch(cpu.eip+1,4));
	return 7;
}

#endif
#include "cpu/exec/template-end.h"
