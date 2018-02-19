#include "cpu/exec.h"
	printf("[addr]: 0x%08x\n", addr);

make_EHelper(mov)
{
#ifdef EXT_DEBUG
	printf("[exec_mov]: \n");
	printf("    mov: 0x%08x \n", id_src->val);
	if (id_dest->type == OP_TYPE_REG)
		printf("    to reg: %d \n\n", id_dest->reg);
	else if (id_dest->type == OP_TYPE_MEM)
		printf("    to mem: 0x%08x \n\n", id_dest->addr);
	else
		panic("exec_mov error. ");
#endif
	operand_write(id_dest, &id_src->val);
	print_asm_template2(mov);
}

make_EHelper(push)
{
	uint32_t data;
	int width = id_dest->width;
	if (id_dest->type == OP_TYPE_MEM || OP_TYPE_REG || OP_TYPE_IMM)
		data = id_dest->val;
	else
		panic("unknown type");
#ifdef EXT_DEBUG
	printf("[data]: 0x%08x\n", data);
	printf("[width]: %d\n", width);
	printf("[esp]: 0x%08x\n", cpu.esp);
#endif
	rtl_push(data, width);
	print_asm_template1(push);
}

//make_EHelper(pusha){}

make_EHelper(pop)
{
    uint32_t * addr;
	int width = id_dest->width;
	if (id_dest->type == OP_TYPE_MEM)// || OP_TYPE_REG || OP_TYPE_IMM)
		addr = id_dest->addr;
	else
		panic("unknown type");
#ifdef EXT_DEBUG
	//printf("[addr]: 0x%08x\n", addr);
	printf("[width]: %d\n", width);
	printf("[esp]: 0x%08x\n", cpu.esp);
#endif
	rtl_pop(addr, width);
	print_asm_template1(pop);
}

make_EHelper(pusha)
{
	TODO();

	print_asm("pusha");
}

make_EHelper(popa)
{
	TODO();

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
