#include "cpu/exec.h"

make_EHelper(test)
{
	TODO();

	print_asm_template2(test);
}

make_EHelper(and)
{
	rtl_and(&id_dest->addr, &id_dest->val, &id_src->val);

	print_asm_template2(and);
}

make_EHelper(xor)
{
#ifdef FUNC_DEBUG
	printf("[[ xor ]]\n");
	if (id_dest->type == OP_TYPE_MEM)
		printf("mem: 0x%08x\n", id_dest->addr);
	else if (id_dest->type == OP_TYPE_REG)
		printf("reg: %%%s\n", reg_name(id_dest->reg, id_dest->width));
	printf("\n");
#endif
	// 按位异或
	if (id_dest->type == OP_TYPE_MEM)
		rtl_xor(guest_to_host(id_dest->addr), &id_dest->val, &id_dest->val);
	else if (id_dest->type == OP_TYPE_REG) {
		rtlreg_t temp;
		rtl_xor(&temp, &id_dest->val, &id_dest->val);
		rtl_sr(id_dest->reg, id_dest->width, &temp);
	}

	print_asm_template2(xor);
}

make_EHelper(or)
{
	TODO();

	print_asm_template2(or);
}

make_EHelper(sar)
{
	TODO();
	// unnecessary to update CF and OF in NEMU

	print_asm_template2(sar);
}

make_EHelper(shl)
{
	TODO();
	// unnecessary to update CF and OF in NEMU
	print_asm_template2(shl);
}

make_EHelper(shr)
{
	TODO();
	// unnecessary to update CF and OF in NEMU

	print_asm_template2(shr);
}

make_EHelper(setcc)
{
	uint8_t subcode = decoding.opcode & 0xf;
	rtl_setcc(&t2, subcode);
	operand_write(id_dest, &t2);

	print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not)
{
	TODO();

	print_asm_template1(not);
}
