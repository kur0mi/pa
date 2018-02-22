#include "cpu/exec.h"

/*	数据传送指令
 */

make_EHelper(mov)
{
#ifdef EXT_DEBUG
	printf("******* [[ mov ]] *******\n");
	printf("[mov data]: 0x%08x\n", id_src->val);
	if (id_dest->type == OP_TYPE_REG)
		printf("[to reg]: %d\n", id_dest->reg);
	else if (id_dest->type == OP_TYPE_MEM)
		printf("[to mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec_mov error. ");
	printf("\n");
#endif
	operand_write(id_dest, &id_src->val);
	print_asm_template2(mov);
}

make_EHelper(push)
{
	rtl_push(&id_dest->val, id_dest->width);
	
	print_asm_template1(push);
}

make_EHelper(pop)
{
	rtl_pop(id_dest);

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
	Operand temp;
	temp.type = OP_TYPE_REG;
	temp.width = id_dest->width;
	int i;
	for (i = 0; i < 8; i++){
		temp.reg = 7 - i;
		rtl_pop(&temp);
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
	id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
	rtl_sext(&t2, &id_src->val, id_src->width);
	operand_write(id_dest, &t2);
	print_asm_template2(movsx);
}

make_EHelper(movzx)
{
	id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
	operand_write(id_dest, &id_src->val);
	print_asm_template2(movzx);
}

make_EHelper(lea)
{
	rtl_li(&t2, id_src->addr);
	operand_write(id_dest, &t2);
	print_asm_template2(lea);
}

