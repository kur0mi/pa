#include "cpu/exec.h"

/*	控制跳跃指令
 */

make_EHelper(jmp)
{
	// the target address is calculated at the decode stage
	decoding.is_jmp = 1;

	print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc)
{
	// the target address is calculated at the decode stage
	uint8_t subcode = decoding.opcode & 0xf;
	rtl_setcc(&t2, subcode);
	decoding.jmp_eip = decoding.seq_eip + id_dest->val;
	decoding.is_jmp = t2;

	print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm)
{
	decoding.jmp_eip = id_dest->val;
	decoding.is_jmp = 1;

	print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call)
{
	// the target address is calculated at the decode stage
	rtl_push(&decoding.seq_eip, id_src->width);	
	decoding.jmp_eip = decoding.seq_eip + id_src->val;
	decoding.is_jmp = 1;

#ifdef EXT_DEBUG
	printf("[[ call ]]\n");
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif
	
	print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret)
{
	rtlreg_t temp = cpu.eax;
	rtlreg_t id = 0;
	rtl_pop(true, &id, id_src->width);
	decoding.jmp_eip = cpu.eax;
	decoding.is_jmp = 1;
	cpu.eax = temp;

#ifdef EXT_DEBUG
	printf("[[ ret ]]\n");
	printf("pop %%esp\n");
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif
	
	print_asm("ret");
}

make_EHelper(call_rm)
{
	TODO();

	print_asm("call *%s", id_dest->str);
}
