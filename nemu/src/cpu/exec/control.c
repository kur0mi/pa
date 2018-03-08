#include "cpu/exec.h"

/*	控制跳跃指令
 */

// 相对跳转
make_EHelper(jmp)
{
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
	// the target address is calculated at the decode stage
	decoding.jmp_eip = decoding.seq_eip + id_dest->val;
	decoding.is_jmp = 1;

#ifdef EXEC_DEBUG
	printf("[[ jmp ]]\n");
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif

	print_asm("jmp %x", decoding.jmp_eip);
}

// 相对条件跳转
make_EHelper(jcc)
{
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
	// the target address is calculated at the decode stage
	uint8_t subcode = decoding.opcode & 0xf;
	rtl_setcc(&t2, subcode);
	decoding.jmp_eip = decoding.seq_eip + id_dest->val;
	decoding.is_jmp = t2;

#ifdef EXEC_DEBUG
	printf("[[ jcc ]]\n");
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif

	print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

// 绝对跳转
make_EHelper(jmp_rm)
{
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

	decoding.jmp_eip = id_dest->val;
	decoding.is_jmp = 1;

#ifdef EXEC_DEBUG
	printf("[[ jmp_rm ]]\n");
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif

	print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call)
{
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
	// the target address is calculated at the decode stage
	rtl_push(&decoding.seq_eip, id_dest->width);
	decoding.jmp_eip = decoding.seq_eip + id_dest->val;
	decoding.is_jmp = 1;

#ifdef EXEC_DEBUG
	printf("[[ call ]]\n");
	printf("push current addr: 0x%08x\n", vaddr_read(cpu.esp, id_dest->width));
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif

	print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret)
{
	rtlreg_t temp = vaddr_read(cpu.esp, id_dest->width);
	vaddr_write(host_to_guest(&decoding.jmp_eip), id_dest->width, temp);
	rtl_addi(&cpu.esp, &cpu.esp, id_dest->width);
	decoding.jmp_eip = temp;
	decoding.is_jmp = 1;

#ifdef EXEC_DEBUG
	printf("[[ ret ]]\n");
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif

	print_asm("ret");
}

make_EHelper(call_rm)
{
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

	rtl_push(&decoding.seq_eip, id_dest->width);
	decoding.jmp_eip = id_dest->val;
	decoding.is_jmp = 1;

#ifdef EXEC_DEBUG
	printf("[[ call_rm ]]\n");
	printf("push current addr: 0x%08x\n", vaddr_read(cpu.esp, id_dest->width));
	printf("jmp to: 0x%08x\n", decoding.jmp_eip);
	printf("\n");
#endif

	print_asm("call *%s", id_dest->str);
}
