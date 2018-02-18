#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__

#include "common.h"

#include "rtl.h"

enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM };

#define OP_STR_SIZE 40

/****************  操作数(Operand) 类型  ***************

    type = OP_TYPE_IMM
    imm
    val <- imm
    val <- simm

    type = OP_TYPE_REG
    reg
    val <- reg

    type = OP_TYPE_MEM
    addr
*/
typedef struct {
	uint32_t type;
	int width;
	union {
		uint32_t reg;
		rtlreg_t addr;
		uint32_t imm;
		int32_t simm;
	};
	rtlreg_t val;
	char str[OP_STR_SIZE];
} Operand;

/*****************  译码(DecodeInfo) 类型  ****************
*/
typedef struct {
	uint32_t opcode;
	vaddr_t seq_eip;	// sequential eip
	bool is_operand_size_16;
	uint8_t ext_opcode;
	bool is_jmp;
	vaddr_t jmp_eip;
	Operand src, dest, src2;
#ifdef DEBUG
	char assembly[80];
	char asm_buf[128];
	char *p;
#endif
} DecodeInfo;

/***************  ModR_M 字节  ************
 *  value
 *  0b xx      xxx      xxx
 *     mod  reg/opcode  R_M
*/
typedef union {
	struct {
		uint8_t R_M:3;
		uint8_t reg:3;
		uint8_t mod:2;
	};
	struct {
		uint8_t dont_care:3;
		uint8_t opcode:3;
	};
	uint8_t val;
} ModR_M;

/*******************  SIB 字节  *****************
 *  value
 *  0b xx xxx xxx
 *     S   I   B
*/
typedef union {
	struct {
		uint8_t base:3;
		uint8_t index:3;
		uint8_t ss:2;
	};
	uint8_t val;
} SIB;

void load_addr(vaddr_t *, ModR_M *, Operand *);
void read_ModR_M(vaddr_t *, Operand *, bool, Operand *, bool);

void operand_write(Operand *, rtlreg_t *);

/* shared by all helper functions */
extern DecodeInfo decoding;

#define id_src (&decoding.src)
#define id_src2 (&decoding.src2)
#define id_dest (&decoding.dest)

/****************************************
 *  void decode_name(vaddr_t *eip)
*/
#define make_DHelper(name) void concat(decode_, name) (vaddr_t *eip)

/*****************************************
 *  DHelper 是一种函数类型， 返回值为 void， 参数为 vaddr_t *
*/
typedef void (*DHelper) (vaddr_t *);

make_DHelper(I2E);
make_DHelper(I2a);
make_DHelper(I2r);
make_DHelper(SI2E);
make_DHelper(SI_E2G);
make_DHelper(I_E2G);
make_DHelper(I_G2E);
make_DHelper(I);
make_DHelper(r);
make_DHelper(E);
make_DHelper(gp7_E);
make_DHelper(test_I);
make_DHelper(SI);
make_DHelper(G2E);
make_DHelper(E2G);

make_DHelper(mov_I2r);
make_DHelper(mov_I2E);
make_DHelper(mov_G2E);
make_DHelper(mov_E2G);
make_DHelper(lea_M2G);

make_DHelper(gp2_1_E);
make_DHelper(gp2_cl2E);
make_DHelper(gp2_Ib2E);

make_DHelper(O2a);
make_DHelper(a2O);

make_DHelper(J);

make_DHelper(push_SI);

make_DHelper(in_I2a);
make_DHelper(in_dx2a);
make_DHelper(out_a2I);
make_DHelper(out_a2dx);

/************************* my function **************/
//make_DHelper(push_o);

#endif
