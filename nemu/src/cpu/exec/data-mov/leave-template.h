#include "cpu/exec/template-start.h"

#define instr leave

static void do_execute(){
    swaddr_t i;
    for (i = cpu.esp;i < cpu.ebp; i+=DATA_BYTE)
    MEM_W (i,0,2);
    cpu.esp = cpu.ebp;
    REG(R_EBP)=swaddr_read(REG(R_ESP),DATA_BYTE,2);
    MEM_W(REG(R_ESP),0,2);
    reg_l(R_ESP)+=DATA_BYTE;
    print_asm("leave");
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"