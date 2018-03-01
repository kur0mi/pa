#include "cpu/exec.h"

/*	数据传送指令
 */

make_EHelper(mov)
{
#ifdef EXT_DEBUG
	printf("******* [[ mov ]] *******\n");
	printf("[mov data]: 0x%08x\n", id_src->val);
	if (id_dest->type == OP_TYPE_REG)
		printf("\t[to reg]: %d\n", id_dest->reg);
	else if (id_dest->type == OP_TYPE_MEM)
		printf("\t[to mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec error. ");
	printf("\n");
#endif

	operand_write(id_dest, &id_src->val);
	print_asm_template2(mov);
}

make_EHelper(push)
{
#ifdef EXT_DEBUG
	printf("******* [[ push ]] *******\n");
	printf("[push data]: 0x%08x\n", id_dest->val);
	if (id_dest->type == OP_TYPE_REG)
		printf("\t[from reg]: %%%s\n", reg_name(id_dest->reg, id_dest->width));
	else if (id_dest->type == OP_TYPE_MEM)
		printf("\t[from mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec error. ");
	printf("\n");
#endif

	rtl_push(&id_dest->val, id_dest->width);
	print_asm_template1(push);
}

make_EHelper(pop)
{
#ifdef EXT_DEBUG
	printf("******* [[ pop ]] *******\n");
	printf("[pop data]: 0x%08x\n", vaddr_read(cpu.esp, id_dest->width));
	if (id_dest->type == OP_TYPE_REG)
		printf("\t[to reg]: %%%s\n", reg_name(id_dest->reg, id_dest->width));
	else if (id_dest->type == OP_TYPE_MEM)
		printf("\t[to mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec error. ");
	printf("\n");
#endif

	if (id_dest->type == OP_TYPE_MEM)
		rtl_pop(false, &id_dest->addr, id_dest->width);
	else if (id_dest->type == OP_TYPE_REG)
		rtl_pop(true, &id_dest->reg, id_dest->width);

	print_asm_template1(pop);
}

make_EHelper(pusha)
{
	rtl_push(&cpu.eax, id_dest->width);
	rtl_push(&cpu.ecx, id_dest->width);
	rtl_push(&cpu.edx, id_dest->width);
	rtl_push(&cpu.ebx, id_dest->width);
	rtl_push(&tzero, id_dest->width);
	rtl_push(&cpu.ebp, id_dest->width);
	rtl_push(&cpu.esi, id_dest->width);
	rtl_push(&cpu.edi, id_dest->width);

	print_asm("pusha");
}

make_EHelper(popa)
{
	rtlreg_t i;
	for (i = 7; (int32_t)i >= 0; i--) {
		if (i != 4)
			rtl_pop(true, &i, id_dest->width);
		else {
			rtlreg_t temp = cpu.eax;
			rtlreg_t id = 0;
			rtl_pop(true, &id, id_dest->width);
			cpu.eax = temp;
		}
	}

	print_asm("popa");
}

make_EHelper(leave)
{
	TODO();

	print_asm("leave");
}

make_EHelper(cltd)
{
	if (decoding.is_operand_size_16) {
		TODO();
	} else {
		TODO();
	}

	print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl)
{
	if (decoding.is_operand_size_16) {
		TODO();
	} else {
		TODO();
	}

	print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx)
{
	//id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
	rtl_sext(&t2, &id_src->val, id_src->width);
	operand_write(id_dest, &t2);
	print_asm_template2(movsx);
}

make_EHelper(movzx)
{
	//id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
	operand_write(id_dest, &id_src->val);
	print_asm_template2(movzx);
}

make_EHelper(lea)
{
	operand_write(id_dest, &id_src->addr);
	print_asm_template2(lea);
}

