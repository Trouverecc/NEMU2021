#include "cpu/exec/template-start.h"

#define instr lgdt

static void do_execute(){
    cpu.GDTR.limit=swaddr_read(op_src->addr,2,R_DS);
    if(op_src->size==2)
        cpu.GDTR.base=swaddr_read(op_src->addr+2,3,R_DS);
    else if(op_src->size==4)
        cpu.GDTR.base=swaddr_read(op_src->addr+2,4,R_DS);
    print_asm_template1();
}

make_instr_helper(rm);

#include "cpu/exec/template-end.h"