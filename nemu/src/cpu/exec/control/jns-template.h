#include "cpu/exec/template-start.h"

#define instr jns

static void do_execute()
{
	DATA_TYPE_S result=op_src->val;
	if(cpu.eflags.SF==0)
		cpu.eip+=result;
	print_asm(str(instr) " %x",cpu.eip+1+DATA_BYTE);
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"