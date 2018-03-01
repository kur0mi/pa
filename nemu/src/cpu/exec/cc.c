#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t * dest, uint8_t subcode)
{
	bool invert = subcode & 0x1;
	enum {
		CC_O, CC_NO, CC_B, CC_NB,
		CC_E, CC_NE, CC_BE, CC_NBE,
		CC_S, CC_NS, CC_P, CC_NP,
		CC_L, CC_NL, CC_LE, CC_NLE
	};

	// TODO: Query EFLAGS to determine whether the condition code is satisfied.
	// dest <- ( cc is satisfied ? 1 : 0)
	switch (subcode & 0xe) {
	case CC_O:					// overflow
		rtl_get_OF(dest);
	case CC_B:					// below (unsigned)
		rtl_get_CF(dest);
	case CC_E:					// equal
		rtl_get_ZF(dest);
	case CC_BE:					// below or equal
		rtl_get_CF(&t0);
		rtl_get_ZF(&t1);
		rtl_or(&t0, &t0, &t1);
		rtl_andi(&t0, &t0, 0x1);
		rtl_mv(dest, &t0);
	case CC_S:					// sign
		rtl_get_SF(dest);
	case CC_L:					// less
		rtl_get_SF(&t0);
		rtl_get_OF(&t1);
		rtl_xor(&t0, &t0, &t1);
		rtl_andi(&t0, &t0, 0x1);
		rtl_mv(dest, &t0);
	case CC_LE:					// less equal
		rtl_get_SF(&t0);
		rtl_get_OF(&t1);
		rtl_xor(&t0, &t0, &t1);
		rtl_get_ZF(&t1);
		rtl_or(&t0, &t0, &t1);
		rtl_andi(&t0, &t0, 0x1);
		rtl_mv(dest, &t0);
	case CC_P:					// parity
		panic("n86 does not have PF");
	default:
		panic("should not reach here");
	}

	if (invert) {
		rtl_xori(dest, dest, 0x1);
	}
}
