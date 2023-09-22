#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute () {
	DATA_TYPE_S imm = op_src -> val;
    if (op_src -> type == OP_TYPE_IMM){
        cpu.eip += imm;
    }else {
        int len = concat(decode_rm_, SUFFIX)(cpu.eip + 1);
        cpu.eip = imm - len - 1;
    }
    print_asm_template1();
}

make_instr_helper(i)
make_instr_helper(rm)



#if DATA_BYTE == 4
make_helper(ljmp){
    cpu.eip = instr_fetch(cpu.eip + 1, 4) - 7;                 
    cpu.CS.selector = instr_fetch(cpu.eip + 1 + 4, 2);          
    sreg_set(R_CS);                                            
    print_asm("ljmp 0x%x 0x%x",instr_fetch(cpu.eip + 1 + 4, 2), instr_fetch(cpu.eip+1, 4));
    return 7;
}
#endif

#include "cpu/exec/template-end.h"
