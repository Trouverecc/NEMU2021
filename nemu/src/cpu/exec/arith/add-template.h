#include "cpu/exec/template-start.h"

#define instr add

static void do_execute(){
	DATA_TYPE r=op_dest->val+op_src->val;
	OPERAND_W(op_dest,r);
	int len=(DATA_BYTE<<3)-1;
	cpu.eflags.CF=(r<op_dest->val);
	cpu.eflags.SF=r>>len;
	cpu.eflags.ZF=!r;
	int s1=op_dest->val>>len;
	int s2=op_src->val>>len;
	cpu.eflags.OF=(s1==s2&&s1!=cpu.eflags.SF);
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.eflags.PF=!(r&1);
	print_asm_template2();
}

#if DATA_BYTE==2 || DATA_BYTE==4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"