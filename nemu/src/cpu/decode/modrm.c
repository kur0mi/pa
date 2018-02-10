#include "cpu/exec.h"
#include "cpu/rtl.h"

// memory
// mod = 0, 1, 2
void load_addr(vaddr_t *eip, ModR_M *m, Operand *rm) {
  assert(m->mod != 3);

  // 初始化 disp(base_reg, index_reg, scale)
  //        4 (-1, -1, 0)
  int32_t disp = 0;
  int disp_size = 4;
  int base_reg = -1, index_reg = -1, scale = 0;
  rtl_li(&rm->addr, 0);

  // 如果 R_M 等于 R_ESP， 再读取一个字节， 得 三个参数
  // R_M == R_ESP:
  //                 4 (SIB.b, SIB.i, SIB.s)
  // R_M != R_ESP:
  //                 4 (R_M, -1, 0)
  if (m->R_M == R_ESP) {
    SIB s;
    s.val = instr_fetch(eip, 1);
    base_reg = s.base;
    scale = s.ss;
    Assert(s.index != R_ESP, "error");
    index_reg = s.index;
  }
  // 否则， 取 R_M 作为 base_reg
  else {
    /* no SIB */
    base_reg = m->R_M;
  }

  // mod == 0:
  //    breg = EBP: 4 (-1, x, x)
  //                0 ( x, x, x)
  // mod == 1:
  //                1 ( x, x, x)
  if (m->mod == 0) {
    if (base_reg == R_EBP)
         base_reg = -1;
    else
         disp_size = 0;
  }
  else if (m->mod == 1)
         disp_size = 1;

  // 读取 disp
  if (disp_size != 0) {
    /* has disp */
    disp = instr_fetch(eip, disp_size);
    if (disp_size == 1)
        disp = (int8_t)disp;
    rtl_addi(&rm->addr, &rm->addr, disp);
  }

  // 读取 base_reg
  if (base_reg != -1) {
    rtl_add(&rm->addr, &rm->addr, &reg_l(base_reg));
  }

  // 读取 index_reg
  if (index_reg != -1) {
    rtl_shli(&t0, &reg_l(index_reg), scale);
    rtl_add(&rm->addr, &rm->addr, &t0);
  }

#ifdef DEBUG
  char disp_buf[16];
  char base_buf[8];
  char index_buf[8];

  if (disp_size != 0) {
    /* has disp */
    sprintf(disp_buf, "%s%#x", (disp < 0 ? "-" : ""), (disp < 0 ? -disp : disp));
  }
  else { disp_buf[0] = '\0'; }

  if (base_reg == -1) { base_buf[0] = '\0'; }
  else {
    sprintf(base_buf, "%%%s", reg_name(base_reg, 4));
  }

  if (index_reg == -1) { index_buf[0] = '\0'; }
  else {
    sprintf(index_buf, ",%%%s,%d", reg_name(index_reg, 4), 1 << scale);
  }

  if (base_reg == -1 && index_reg == -1) {
    sprintf(rm->str, "%s", disp_buf);
  }
  else {
    sprintf(rm->str, "%s(%s%s)", disp_buf, base_buf, index_buf);
  }
#endif

  rm->type = OP_TYPE_MEM;
}

/*
    读取 modR_M 字节
*/
void read_ModR_M(vaddr_t *eip, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  ModR_M m;
  m.val = instr_fetch(eip, 1);
  decoding.ext_opcode = m.opcode;   // 解析 opcode
  if (reg != NULL) {                // 解析 reg
    reg->type = OP_TYPE_REG;
    reg->reg = m.reg;
    if (load_reg_val) {
      rtl_lr(&reg->val, reg->reg, reg->width);
    }

#ifdef DEBUG
    snprintf(reg->str, OP_STR_SIZE, "%%%s", reg_name(reg->reg, reg->width));
#endif
  }

  // 解析 mod & R_M
  // register
  if (m.mod == 3) {
    rm->type = OP_TYPE_REG;
    rm->reg = m.R_M;
    if (load_rm_val) {
      rtl_lr(&rm->val, m.R_M, rm->width);
    }

#ifdef DEBUG
    sprintf(rm->str, "%%%s", reg_name(m.R_M, rm->width));
#endif
  }
  // memory
  else {
    load_addr(eip, &m, rm);
    if (load_rm_val) {
      rtl_lm(&rm->val, &rm->addr, rm->width);
    }
  }
}
