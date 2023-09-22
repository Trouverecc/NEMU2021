#include "cpu/exec/template-start.h"
#include "memory/tlb.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX),1);

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr,1);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE==4

make_helper(mov_cr2r){
	uint8_t tmp=instr_fetch(eip+1,1);
	uint8_t cr=(tmp>>3)&7;
	uint8_t reg=tmp&7;
	if(cr==0){
		reg_l(reg)=cpu.cr0.val;
		print_asm("mov cr0 %%%s",REG_NAME(reg));
	}
	else if(cr==3){
		reg_l(reg)=cpu.cr3.val;
		print_asm("mov cr3 %%%s",REG_NAME(reg));
	}
	return 2;
}

make_helper(mov_r2cr){
	uint8_t tmp=instr_fetch(eip+1,1);
	uint8_t cr=(tmp>>3)&7;
	uint8_t reg=tmp&7;
	if(cr==0){
		cpu.cr0.val=reg_l(reg);
		print_asm("mov %%%s cr0",REG_NAME(reg));
	}
	else if(cr==3){
		init_tlb();
		cpu.cr3.val=reg_l(reg);
		print_asm("mov %%%s cr3",REG_NAME(reg));
	}
	return 2;
}

#endif

#if DATA_BYTE==2

make_helper(mov_sreg2rm){
	uint8_t modrm=instr_fetch(eip+1,1);
	uint8_t sreg=(modrm>>3)&7;
	uint8_t reg=(modrm&7);
	cpu.sreg[sreg].selector=reg_w(reg);
	sreg_set(sreg);
	print_asm("mov %s sreg%d",REG_NAME(reg),sreg);
	return 2;
}

#endif

#include "cpu/exec/template-end.h"
