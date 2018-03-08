#ifndef __RTL_H__
#define __RTL_H__

/*	RTL (Register Transfer Language)
 *
 *	这里实现寄存器级别的封装
 */

#include "nemu.h"

// extern 关键字表明 该变量 在其他文件中声明
extern rtlreg_t t0, t1, t2, t3;	// 临时寄存器
extern const rtlreg_t tzero;	// 0 寄存器

/* RTL basic instructions */

// 装载立即数
// imm --> dest
static inline void rtl_li(rtlreg_t * dest, uint32_t imm)
{
	*dest = imm;
}

#define c_add(a, b) ((a) + (b))	// 加
#define c_sub(a, b) ((a) - (b))	// 减
#define c_and(a, b) ((a) & (b))	// 按位与
#define c_or(a, b)  ((a) | (b))	// 按位或
#define c_xor(a, b) ((a) ^ (b))	// 按位异或
#define c_shl(a, b) ((a) << (b))	// 逻辑左移
#define c_shr(a, b) ((a) >> (b))	// 逻辑右移
#define c_sar(a, b) ((int32_t)(a) >> (b))	// 算术右移
#define c_slt(a, b) ((int32_t)(a) < (int32_t)(b))	// 小于
#define c_sltu(a, b) ((a) < (b))	// 小于(无符号比较)

/*****
static inline void rtl_add (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) {
    *dest = c_add(*src1, *src2);
}
static inline void rtl_addi (rtlreg_t* dest, const rtlreg_t* src1, int imm) {
    *dest = c_add(*src1, imm);
}
*/
#define make_rtl_arith_logic(name) \
  static inline void concat(rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  static inline void concat3(rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }

make_rtl_arith_logic(add)	// rtl_add(dest, src1, src2)
    // rtl_addi(dest, src1, imm)
    make_rtl_arith_logic(sub)
    make_rtl_arith_logic(and)
    make_rtl_arith_logic(or)
    make_rtl_arith_logic(xor)
    make_rtl_arith_logic(shl)
    make_rtl_arith_logic(shr)
    make_rtl_arith_logic(sar)
    make_rtl_arith_logic(slt)
    make_rtl_arith_logic(sltu)

/*	rtl_mul		无符号数乘法
 *	rtl_imul	有符号数乘法
 *	rtl_div		无符号数除法
 *	rtl_idiv	有符号数除法
 */
static inline void rtl_mul(rtlreg_t * dest_hi, rtlreg_t * dest_lo, const rtlreg_t * src1, const rtlreg_t * src2)
{
	asm volatile ("mul %3":"=d" (*dest_hi), "=a"(*dest_lo):"a"(*src1), "r"(*src2));
}

static inline void rtl_imul(rtlreg_t * dest_hi, rtlreg_t * dest_lo, const rtlreg_t * src1, const rtlreg_t * src2)
{
	asm volatile ("imul %3":"=d" (*dest_hi), "=a"(*dest_lo):"a"(*src1), "r"(*src2));
}

static inline void rtl_div(rtlreg_t * q, rtlreg_t * r, const rtlreg_t * src1_hi, const rtlreg_t * src1_lo, const rtlreg_t * src2)
{
	asm volatile ("div %4":"=a" (*q), "=d"(*r):"d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static inline void rtl_idiv(rtlreg_t * q, rtlreg_t * r, const rtlreg_t * src1_hi, const rtlreg_t * src1_lo, const rtlreg_t * src2)
{
	asm volatile ("idiv %4":"=a" (*q), "=d"(*r):"d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

// 装载内存
// mem --> dest
static inline void rtl_lm(rtlreg_t * dest, const rtlreg_t * addr, int len)
{
#ifdef RTL_DEBUG
	DebugText("******* [[ rtl_lm ]] *******\n");
	DebugText("[read mem]: 0x%08x\n", *addr);
	DebugText("[to addr]: 0x%08x\n", host_to_guest(dest));
	DebugText("\n");
#endif

	*dest = vaddr_read(*addr, len);
}

// 写回内存
// dest --> mem
static inline void rtl_sm(rtlreg_t * addr, int len, const rtlreg_t * src1)
{
#ifdef RTL_DEBUG
	DebugText("******* [[ rtl_sm ]] *******\n");
	DebugText("[write mem]: 0x%08x\n", *addr);
	DebugText("[value]: 0x%08x\n", *src1);
	DebugText("\n");
#endif

	vaddr_write(*addr, len, *src1);
}

static inline void rtl_lr_b(rtlreg_t * dest, int r)
{
	*dest = reg_b(r);
}

static inline void rtl_lr_w(rtlreg_t * dest, int r)
{
	*dest = reg_w(r);
}

static inline void rtl_lr_l(rtlreg_t * dest, int r)
{
	*dest = reg_l(r);
}

static inline void rtl_sr_b(int r, const rtlreg_t * src1)
{
	reg_b(r) = *src1;
}

static inline void rtl_sr_w(int r, const rtlreg_t * src1)
{
	reg_w(r) = *src1;
}

static inline void rtl_sr_l(int r, const rtlreg_t * src1)
{
	reg_l(r) = *src1;
}

/* RTL psuedo instructions */
/*
 * 	RTL 伪指令
 *	进一步封装
 */

// 装载寄存器
// register --> dest
static inline void rtl_lr(rtlreg_t * dest, int r, int width)
{
#ifdef RTL_DEBUG
	DebugText("******* [[ rtl_lr ]] *******\n");
	DebugText("[read reg]: %%%s\n", reg_name(r, width));
	DebugText("[to addr]: 0x%08x\n", *dest);
	DebugText("\n");
#endif

	switch (width) {
	case 4:
		rtl_lr_l(dest, r);
		return;
	case 1:
		rtl_lr_b(dest, r);
		return;
	case 2:
		rtl_lr_w(dest, r);
		return;
	default:
		assert(0);
	}
}

// 写回寄存器
// src1 --> register
static inline void rtl_sr(int r, int width, const rtlreg_t * src1)
{
#ifdef RTL_DEBUG
	DebugText("******* [[ rtl_sr ]] *******\n");
	DebugText("[write reg]: %%%s\n", reg_name(r, width));
	DebugText("[value]: 0x%08x\n", *src1);
	DebugText("\n");
#endif

	switch (width) {
	case 4:
		rtl_sr_l(r, src1);
		return;
	case 1:
		rtl_sr_b(r, src1);
		return;
	case 2:
		rtl_sr_w(r, src1);
		return;
	default:
		assert(0);
	}
}

/**************
static inline void rtl_set_CF(const rtlreg_t* src) {
    cpu.eflags.CF = *src;
}
static inline void rtl_get_CF(rtlreg_t* dest) {
    *dest = cpu.eflags.CF;
}
*/
#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags.f = (*src) != 0; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = cpu.eflags.f; \
  }

// 设置标志位
make_rtl_setget_eflags(CF)
    make_rtl_setget_eflags(OF)
    make_rtl_setget_eflags(ZF)
    make_rtl_setget_eflags(SF)

// 数据传送
// dest <- src1
static inline void rtl_mv(rtlreg_t * dest, const rtlreg_t * src1)
{
	*dest = *src1;
}

// 按位取反
// dest <- (~dest)
static inline void rtl_not(rtlreg_t * dest)
{
	*dest = ~(*dest);
}

// 逻辑非
// dest <- (-dest)
static inline void rtl_neg(rtlreg_t * dest)
{
	*dest = -(*dest);
}

// 符号位扩展
// dest <- signext(src1[(width * 8 - 1) .. 0])
static inline void rtl_sext(rtlreg_t * dest, const rtlreg_t * src1, int width)
{
#ifdef RTL_DEBUG
	rtlreg_t temp = *src1;
#endif
	*dest = c_sar(c_shl(*src1, 32 - width * 8), 32 - width * 8);
#ifdef RTL_DEBUG
	DebugText("[[ rtl_sext ]]\n");
	DebugText("src: 0x%08x\n", temp);
	DebugText("width: %d\n", width);
	DebugText("dest: 0x%08x\n", *dest);
	DebugText("\n");
#endif
}

// 压栈
// esp <- esp - 4
// M[esp] <- src1
static inline void rtl_push(const rtlreg_t * data, int width)
{
#ifdef RTL_DEBUG
	DebugText("******* [[ push ]] *******\n");
	DebugText("[push data]: 0x%08x\n", *data);
	DebugText("[width]: %d\n", width);
	DebugText("[to esp]: 0x%08x\n", cpu.esp - width);
	DebugText("\n");
#endif
	rtl_subi(&cpu.esp, &cpu.esp, width);
	rtl_sm(&cpu.esp, width, data);
}

// 出栈
// dest <- M[esp]
// esp <- esp + 4
static inline void rtl_pop(bool is_reg, rtlreg_t * tt, int width)
{
#ifdef RTL_DEBUG
	DebugText("******* [[ pop ]] *******\n");
	DebugText("[from esp]: 0x%08x\n", cpu.esp);
	if (is_reg)
		DebugText("[to reg]: %%%s\n", reg_name(*tt, width));
	else
		DebugText("[to mem]: 0x%08x", *tt);
	DebugText("\n");
#endif
	if (is_reg)
		rtl_sr(*tt, width, guest_to_host(cpu.esp));
	else
		rtl_sm(tt, width, guest_to_host(cpu.esp));
	cpu.esp += width;
}

// RTL 指令 - 等于 0
static inline void rtl_eq0(rtlreg_t * dest, const rtlreg_t * src1)
{
	// dest <- (src1 == 0 ? 1 : 0)
	*dest = (*src1 == 0);
}

// RTL 指令 - 等于 imm
static inline void rtl_eqi(rtlreg_t * dest, const rtlreg_t * src1, int imm)
{
	// dest <- (src1 == imm ? 1 : 0)
	*dest = (*src1 == imm);
}

// RTL 指令 - 不等于 0
static inline void rtl_neq0(rtlreg_t * dest, const rtlreg_t * src1)
{
	// dest <- (src1 != 0 ? 1 : 0)
	*dest = (*src1 != 0);
}

// RTL 指令 - 最高有效位
static inline void rtl_msb(rtlreg_t * dest, const rtlreg_t * src1, int width)
{
	// dest <- src1[width * 8 - 1]
	*dest = (*src1) >> (width * 8 - 1);
}

// RTL 指令 - 更新 标志位 ZF
static inline void rtl_update_ZF(const rtlreg_t * result, int width)
{
	// eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
	rtlreg_t temp;
	rtl_eq0(&temp, result);
	rtl_set_ZF(&temp);
}

// RTL 指令 - 更新 标志位 SF
static inline void rtl_update_SF(const rtlreg_t * result, int width)
{
	// eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
	rtlreg_t temp;
	rtl_shri(&temp, result, 8 * width - 1);
	rtl_set_SF(&temp);
}

// RTL 指令 - 更新标志位 ZF & SF
static inline void rtl_update_ZFSF(const rtlreg_t * result, int width)
{
	rtl_update_ZF(result, width);
	rtl_update_SF(result, width);
}

#ifndef MY_RELEASE
// 检查标志位
static inline void rtl_check_eflags()
{
	DebugText("ZF: %d\tSF: %d\tOF: %d\tCF: %d\n", cpu.eflags.ZF, cpu.eflags.SF, cpu.eflags.OF, cpu.eflags.CF);
}
#endif

#endif
