#include "cpu/exec.h"

/*	数据传送指令
 *
 *	不影响标志位
 */

make_EHelper(mov)
{
#ifdef EXEC_DEBUG
	DebugText("******* [[ mov ]] *******\n");
	DebugText("[mov data]: 0x%08x\n", id_src->val);
	if (id_dest->type == OP_TYPE_REG)
		DebugText("\t[to reg]: %%%s\n", reg_name(id_dest->reg, id_dest->width));
	else if (id_dest->type == OP_TYPE_MEM)
		DebugText("\t[to mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec error. ");
	DebugText("\n");
#endif

	operand_write(id_dest, &id_src->val);
	print_asm_template2(mov);
}

make_EHelper(push)
{
#ifdef EXEC_DEBUG
	DebugText("******* [[ push ]] *******\n");
	DebugText("[push data]: 0x%08x\n", id_dest->val);
	if (id_dest->type == OP_TYPE_REG)
		DebugText("\t[from reg]: %%%s\n", reg_name(id_dest->reg, id_dest->width));
	else if (id_dest->type == OP_TYPE_MEM)
		DebugText("\t[from mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec error. ");
	DebugText("\n");
#endif

	rtl_push(&id_dest->val, id_dest->width);
	print_asm_template1(push);
}

make_EHelper(pop)
{
#ifdef EXEC_DEBUG
	DebugText("******* [[ pop ]] *******\n");
	DebugText("[pop data]: 0x%08x\n", vaddr_read(cpu.esp, id_dest->width));
	if (id_dest->type == OP_TYPE_REG)
		DebugText("\t[to reg]: %%%s\n", reg_name(id_dest->reg, id_dest->width));
	else if (id_dest->type == OP_TYPE_MEM)
		DebugText("\t[to mem]: 0x%08x\n", id_dest->addr);
	else
		panic("exec error. ");
	DebugText("\n");
#endif

	operand_write(id_dest, guest_to_host(cpu.esp));
	rtl_addi(&cpu.esp, &cpu.esp, id_dest->width);
/*
	if (id_dest->type == OP_TYPE_MEM)
		rtl_pop(false, &id_dest->addr, id_dest->width);
	else if (id_dest->type == OP_TYPE_REG)
		rtl_pop(true, &id_dest->reg, id_dest->width);
*/
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
#ifdef DEBUG
    assert(cpu.eax == vaddr_read(cpu.esp-4, 4));
    assert(cpu.ecx == vaddr_read(cpu.esp-8, 4));
    assert(cpu.edx == vaddr_read(cpu.esp-12, 4));
    assert(cpu.ebx == vaddr_read(cpu.esp-16, 4));
    // ignore cpu.esp
    assert(cpu.ebp == vaddr_read(cpu.esp-24, 4));
    assert(cpu.esi == vaddr_read(cpu.esp-28, 4));
    assert(cpu.edi == vaddr_read(cpu.esp-32, 4));
#endif
}

make_EHelper(popa)
{
	rtlreg_t i;
	for (i = 7; (int32_t) i >= 0; i--) {
		if (i != 4)
			rtl_sr(i, id_dest->width, guest_to_host(cpu.esp));
		rtl_subi(&cpu.esp, &cpu.esp, id_dest->width);
	}

	print_asm("popa");
#ifdef DEBUG
    assert(cpu.eax == vaddr_read(cpu.esp-4, 4));
    assert(cpu.ecx == vaddr_read(cpu.esp-8, 4));
    assert(cpu.edx == vaddr_read(cpu.esp-12, 4));
    assert(cpu.ebx == vaddr_read(cpu.esp-16, 4));
    // ignore cpu.esp
    assert(cpu.ebp == vaddr_read(cpu.esp-24, 4));
    assert(cpu.esi == vaddr_read(cpu.esp-28, 4));
    assert(cpu.edi == vaddr_read(cpu.esp-32, 4));
#endif
}

make_EHelper(leave)
{
	/*	esp <= ebp
	 *	ebp <= pop()
	 */
	rtl_mv(&cpu.esp, &cpu.ebp);
	rtl_sr(R_EBP, id_dest->width, guest_to_host(cpu.esp));
	rtl_addi(&cpu.esp, &cpu.esp, id_src->width);

	print_asm("leave");
#ifdef DEBUG
    assert(cpu.ebp == vaddr_read(cpu.esp - 4, 4));
#endif
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
#ifdef DEBUG
    DebugText("[lea] effictive address: 0x%08x\n", id_src->addr);
#endif
}
