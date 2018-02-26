#include "cpu/exec.h"

// data-mov
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);

// control
make_EHelper(call);

// arith
make_EHelper(add);
make_EHelper(sub);

// logic
make_EHelper(xor);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
