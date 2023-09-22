#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute(){
	OPERAND_W(op_src,MEM_R(REG(R_ESP),2));
    swaddr_write(reg_l(R_ESP), 4, 0, R_SS); 
	reg_l(R_ESP) += 4; 
	print_asm_template1();
}

#if DATA_BYTE == 2||DATA_BYTE==4
make_instr_helper(r)
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"