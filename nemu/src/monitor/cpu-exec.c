#include "nemu.h"
#include "monitor/monitor.h"
#include "monitor/watchpoint.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 1000

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n)
{
	if (nemu_state == NEMU_END) {
		printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
		return;
	}
	nemu_state = NEMU_RUNNING;

	bool print_flag = (int64_t) n < MAX_INSTR_TO_PRINT;

	for (; n > 0; n--) {
		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
		exec_wrapper(print_flag);

		/* TODO: check watchpoints here. */
		WP *t = check_wp(NULL);
		while (t != NULL) {
			if (nemu_state != NEMU_END)
				nemu_state = NEMU_STOP;
			printf("[+] wp [%d], %s = %d ==> %d\n", t->NO, t->str, t->oldvalue, t->value);
			t = check_wp(t);
		}

#ifdef HAS_IOE
		extern void device_update();
		device_update();
#endif

		if (nemu_state != NEMU_RUNNING) {
			return;
		}
	}

	if (nemu_state == NEMU_RUNNING) {
		nemu_state = NEMU_STOP;
	}
}
