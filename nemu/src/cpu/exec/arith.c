#include "cpu/exec.h"

make_EHelper(add)
{
	// t0 为运算结果
	rtl_add(&t0, &id_dest->val, &id_src->val);

	rtl_sltu(&t1, &t0, &id_dest->val);
#ifdef ONLY_DEBUG
	TipText("CF: 0x%08x => 0x%08x\n", cpu.eflags.CF, t1);
	TipText("0x%08x < 0x%08x\n", t0, id_dest->val);
#endif
	rtl_set_CF(&t1);
	//cpu.eflags.CF = t1;
	TipText("0x%08x\n", cpu.eflags.CF);

	rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_xor(&t1, &t1, &t0);
	rtl_xor(&t1, &t1, &t0);
	rtl_not(&t1);
	rtl_msb(&t1, &t1, id_dest->width);
	rtl_set_OF(&t1);

	operand_write(id_dest, &t0);

	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template2(add);
#ifdef EXEC_DEBUG
	//DebugText("0x%08x + 0x%08x ==> 0x%08x\n", id_dest->val, id_src->val, t0);
	//rtl_check_eflags();
#endif
}

make_EHelper(sub)
{
	if (id_src->width == 1 && id_dest->width != 1)
		rtl_sext(&id_src->val, &id_src->val, id_src->width);

	// t0 为运算结果
	rtl_sub(&t0, &id_dest->val, &id_src->val);

	rtl_sltu(&t1, &id_dest->val, &id_src->val);
	rtl_set_CF(&t1);

	rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_xor(&t1, &t1, &id_src->val);
	rtl_xor(&t1, &t1, &id_src->val);
	rtl_msb(&t1, &t1, id_dest->width);
	rtl_set_OF(&t1);

	operand_write(id_dest, &t0);

	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template2(sub);
#ifdef EXEC_DEBUG
	//DebugText("0x%x - 0x%x = 0x%x\n", id_dest->val, id_src->val, t0);
	//rtl_check_eflags();
#endif
}

make_EHelper(cmp)
{
	if (id_src->width == 1 && id_dest->width != 1)
		rtl_sext(&id_src->val, &id_src->val, id_src->width);

	// t0 为运算结果
	rtl_sub(&t0, &id_dest->val, &id_src->val);

	rtl_sltu(&t1, &id_dest->val, &id_src->val);
	rtl_set_CF(&t1);

	rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_xor(&t1, &t1, &id_src->val);
	rtl_xor(&t1, &t1, &id_src->val);
	rtl_msb(&t1, &t1, id_dest->width);
	rtl_set_OF(&t1);

	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template2(cmp);
#ifdef EXEC_DEBUG
	//DebugText("0x%x - 0x%x = 0x%x\n", id_dest->val, id_src->val, t0);
	//rtl_check_eflags();
#endif
}

make_EHelper(inc)
{
	// t0 为运算结果
	rtl_addi(&t0, &id_dest->val, 1);

	rtl_sltu(&t1, &t0, &id_dest->val);
	rtl_set_CF(&t1);

	rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_xor(&t1, &t1, &t0);
	rtl_xor(&t1, &t1, &t0);
	rtl_not(&t1);
	rtl_msb(&t1, &t1, id_dest->width);
	rtl_set_OF(&t1);

	operand_write(id_dest, &t0);

	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template1(inc);
#ifdef EXEC_DEBUG
	//DebugText("0x%x + 0x%x = 0x%x\n", id_dest->val, 1, t0);
	//rtl_check_eflags();
#endif
}

make_EHelper(dec)
{
	// t0 为运算结果
	rtl_subi(&t0, &id_dest->val, 1);

	rtl_sltu(&t1, &t0, &id_dest->val);
	rtl_set_CF(&t1);

	rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_xor(&t1, &t1, &t0);
	rtl_xor(&t1, &t1, &t0);
	rtl_not(&t1);
	rtl_msb(&t1, &t1, id_dest->width);
	rtl_set_OF(&t1);

	operand_write(id_dest, &t0);

	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template1(dec);
#ifdef EXEC_DEBUG
	//DebugText("0x%x + 0x%x = 0x%x\n", id_dest->val, 1, t0);
	//rtl_check_eflags();
#endif
}

make_EHelper(neg)
{
	rtl_neg(&id_dest->val);
	operand_write(id_dest, &id_dest->val);
	
	print_asm_template1(neg);
}

make_EHelper(adc)
{
	rtl_add(&t2, &id_dest->val, &id_src->val);
	rtl_sltu(&t3, &t2, &id_dest->val);
	rtl_get_CF(&t1);
#ifdef ONLY_DEBUG
	rtlreg_t carry = t1;
	TipText("carry: 0x%08x\n", carry);
#endif
	rtl_add(&t2, &t2, &t1);
	operand_write(id_dest, &t2);

	rtl_update_ZFSF(&t2, id_dest->width);

	rtl_sltu(&t0, &t2, &id_dest->val);
	rtl_or(&t0, &t3, &t0);
	rtl_set_CF(&t0);

	rtl_xor(&t0, &id_dest->val, &id_src->val);
	rtl_not(&t0);
	rtl_xor(&t1, &id_dest->val, &t2);
	rtl_and(&t0, &t0, &t1);
	rtl_msb(&t0, &t0, id_dest->width);
	rtl_set_OF(&t0);

#ifdef EXEC_DEBUG
	//printf("%u + %u + %u = %u\n", id_dest->val, id_src->val, carry, t0);
	//rtl_check_eflags();
#endif

	print_asm_template2(adc);
}

make_EHelper(sbb)
{
	rtl_sub(&t2, &id_dest->val, &id_src->val);
	rtl_sltu(&t3, &id_dest->val, &t2);
	rtl_get_CF(&t1);
#ifdef EXEC_DEBUG
	//rtlreg_t carry = t1;
#endif
	rtl_sub(&t2, &t2, &t1);
	operand_write(id_dest, &t2);

	rtl_update_ZFSF(&t2, id_dest->width);

	rtl_sltu(&t0, &id_dest->val, &t2);
	rtl_or(&t0, &t3, &t0);
	rtl_set_CF(&t0);

	rtl_xor(&t0, &id_dest->val, &id_src->val);
	rtl_xor(&t1, &id_dest->val, &t2);
	rtl_and(&t0, &t0, &t1);
	rtl_msb(&t0, &t0, id_dest->width);
	rtl_set_OF(&t0);

#ifdef EXEC_DEBUG
	//printf("%u + %u + %u = %u\n", id_dest->val, id_src->val, carry, t0);
	//rtl_check_eflags();
#endif

	print_asm_template2(sbb);
}

make_EHelper(mul)
{
	rtl_lr(&t0, R_EAX, id_dest->width);
	rtl_mul(&t0, &t1, &id_dest->val, &t0);

	switch (id_dest->width) {
	case 1:
		rtl_sr_w(R_AX, &t1);
		break;
	case 2:
		rtl_sr_w(R_AX, &t1);
		rtl_shri(&t1, &t1, 16);
		rtl_sr_w(R_DX, &t1);
		break;
	case 4:
		rtl_sr_l(R_EDX, &t0);
		rtl_sr_l(R_EAX, &t1);
		break;
	default:
		assert(0);
	}

	print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1)
{
	rtl_lr(&t0, R_EAX, id_dest->width);
	rtl_imul(&t0, &t1, &id_dest->val, &t0);

	switch (id_dest->width) {
	case 1:
		rtl_sr_w(R_AX, &t1);
		break;
	case 2:
		rtl_sr_w(R_AX, &t1);
		rtl_shri(&t1, &t1, 16);
		rtl_sr_w(R_DX, &t1);
		break;
	case 4:
		rtl_sr_l(R_EDX, &t0);
		rtl_sr_l(R_EAX, &t1);
		break;
	default:
		assert(0);
	}

	print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2)
{
	rtl_sext(&id_src->val, &id_src->val, id_src->width);
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

	rtl_imul(&t0, &t1, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t1);

	print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3)
{
	rtl_sext(&id_src->val, &id_src->val, id_src->width);
	rtl_sext(&id_src2->val, &id_src2->val, id_src->width);
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

	rtl_imul(&t0, &t1, &id_src2->val, &id_src->val);
	operand_write(id_dest, &t1);

	print_asm_template3(imul);
}

make_EHelper(div)
{
	switch (id_dest->width) {
	case 1:
		rtl_li(&t1, 0);
		rtl_lr_w(&t0, R_AX);
		break;
	case 2:
		rtl_lr_w(&t0, R_AX);
		rtl_lr_w(&t1, R_DX);
		rtl_shli(&t1, &t1, 16);
		rtl_or(&t0, &t0, &t1);
		rtl_li(&t1, 0);
		break;
	case 4:
		rtl_lr_l(&t0, R_EAX);
		rtl_lr_l(&t1, R_EDX);
		break;
	default:
		assert(0);
	}

	rtl_div(&t2, &t3, &t1, &t0, &id_dest->val);

	rtl_sr(R_EAX, id_dest->width, &t2);
	if (id_dest->width == 1) {
		rtl_sr_b(R_AH, &t3);
	} else {
		rtl_sr(R_EDX, id_dest->width, &t3);
	}

	print_asm_template1(div);
}

make_EHelper(idiv)
{
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

	switch (id_dest->width) {
	case 1:
		rtl_lr_w(&t0, R_AX);
		rtl_sext(&t0, &t0, 2);
		rtl_msb(&t1, &t0, 4);
		rtl_sub(&t1, &tzero, &t1);
		break;
	case 2:
		rtl_lr_w(&t0, R_AX);
		rtl_lr_w(&t1, R_DX);
		rtl_shli(&t1, &t1, 16);
		rtl_or(&t0, &t0, &t1);
		rtl_msb(&t1, &t0, 4);
		rtl_sub(&t1, &tzero, &t1);
		break;
	case 4:
		rtl_lr_l(&t0, R_EAX);
		rtl_lr_l(&t1, R_EDX);
		break;
	default:
		assert(0);
	}

	rtl_idiv(&t2, &t3, &t1, &t0, &id_dest->val);

	rtl_sr(R_EAX, id_dest->width, &t2);
	if (id_dest->width == 1) {
		rtl_sr_b(R_AH, &t3);
	} else {
		rtl_sr(R_EDX, id_dest->width, &t3);
	}

	print_asm_template1(idiv);
}
