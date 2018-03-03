#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args)
{
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args)
{
	return -1;
}

static int cmd_help(char *args);

static int cmd_n(char *args)
{
	int n = 1;
	if (args != NULL)
		n = atoi(args);

	cpu_exec(n);
	return 0;
}

static int cmd_info(char *args)
{
	if (args == NULL) {
		cmd_help("info");
		return 0;
	}

	if (strcmp(args, "r") == 0) {
		printf("%%eip: 0x%08x\n", cpu.eip);
		printf("%%eax: 0x%08x\n", cpu.eax);
		printf("%%ebx: 0x%08x\n", cpu.ebx);
		printf("%%ecx: 0x%08x\n", cpu.ecx);
		printf("%%edx: 0x%08x\n", cpu.edx);
		printf("%%esp: 0x%08x\n", cpu.esp);
		printf("%%ebp: 0x%08x\n", cpu.ebp);
		printf("%%esi: 0x%08x\n", cpu.esi);
		printf("%%edi: 0x%08x\n", cpu.edi);
	}
    else if (strcmp(args, "w") == 0) {
		show_wp();
	}
    else {
		cmd_help("info");
	}

	return 0;
}

static int cmd_x(char *args)
{
	// parse args
	Assert(args != NULL, "too few argument");
	char *sLen = strtok(args, " ");
	Assert(sLen != NULL, "too few argument");
	char *sAddr = strtok(NULL, " ");
	Assert(sAddr != NULL, "too few argument");
	Assert(strtok(NULL, " ") == NULL, "too many argument");

	// convert
	int len = atoi(sLen);
	vaddr_t nAddr = expr(sAddr);

	// print
	int i;
	for (i = 0; i < len; i += 4) {
		int loop = len - i < 4 ? len - i : 4;
		uint32_t data = vaddr_read(nAddr + i, loop);
        printf("[0x%08x]: ", nAddr + i);
		int j;
		for (j = 0; j < loop; j++) {
			printf("0x%02x ", data & 0xff);
			data = data >> 8;
		}
		printf("\n");
	}

	return 0;
}

static int cmd_p(char *args)
{
	int res = expr(args);
	printf("result = %d | %u | 0x%x\n", res, res, res);

	return 0;
}

static int cmd_w(char *args)
{
	if (args == NULL) {
		cmd_help("w");
		return 0;
	}

	WP *w = new_wp(args, 0);
	printf("[+] set wp [%d], %s = 0x%08x\n", w->NO, w->str, w->value);

	return 0;
}

static int cmd_b(char *args)
{
	if (args == NULL) {
		cmd_help("b");
		return 0;
	}

    sprintf(args, "$eip==%s", args);
	WP *w = new_wp(args, 0);
	printf("[+] set bp [%d], %s\n", w->NO, w->str);

	return 0;
}

static int cmd_d(char *args)
{
	if (args == NULL) {
		cmd_help("d");
		return 0;
	}

	int id = 0;
	sscanf(args, "%d", &id);
	free_wp(id);

	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table[] = {
	{
	"help", "Display informations about all supported commands", cmd_help}, {
	"c", "   c,         // Continue", cmd_c}, {
	"q", "   q,         // Quit", cmd_q},
	    /* TODO: Add more commands */
	{
	"n", "   n [N],     // Next N instr, default to 1", cmd_n}, {
	"info", "info r|w,  // Infos of reg or watchpoint", cmd_info}, {
	"x", "   x N EXPR,  // eXamine memory, N bytes", cmd_x}, {
	"p", "   p EXPR,    // Print expr", cmd_p}, {
	"w", "   w EXPR,    // set Watchpoint", cmd_w}, {
    "d", "   d N,       // Delete num N watchpoint", cmd_d}, {
    "b", "   b EXPR,    // set Breakpoint", cmd_b}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL) {
		/* no argument given */
		for (i = 0; i < NR_CMD; i++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	} else {
		for (i = 0; i < NR_CMD; i++) {
			if (strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop(int is_batch_mode)
{
	if (is_batch_mode) {
		cmd_c(NULL);
		return;
	}

	while (1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL) {
			continue;
		}

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if (args >= str_end) {
			args = NULL;
		}
#ifdef HAS_IOE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for (i = 0; i < NR_CMD; i++) {
			if (strcmp(cmd, cmd_table[i].name) == 0) {
				// 返回 0 则结束程序
				if (cmd_table[i].handler(args) < 0) {
					return;
				}
				break;
			}
		}

		if (i == NR_CMD) {
			printf("Unknown command '%s'\n", cmd);
		}
	}
}
