#include "cpu/exec/template-start.h"

#define instr jne

static void do_execute()
{
	DATA_TYPE_S result=op_src->val;
    print_asm(str(instr) " %x",cpu.eip+1+DATA_BYTE);
    if(cpu.eflags.ZF==0)
    cpu.eip+=result;
    
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"