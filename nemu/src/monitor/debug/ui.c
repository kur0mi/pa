#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  int n;
  if (args == NULL)
 	n = 1;
  else
	n = atoi(args);

  cpu_exec(n);

  return 0;
}

static int cmd_info(char *args){
  if (args == NULL){
  	cmd_help("info");
	return 0;
  }

  if (strcmp(args, "r") == 0){
	char eip[9], eax[9], ebx[9], ecx[9], edx[9], esp[9], ebp[9], esi[9], edi[9];
	sprintf(eip, "%08x", cpu.eip);
	sprintf(eax, "%08x", cpu.eax);
	sprintf(ebx, "%08x", cpu.ebx);
	sprintf(ecx, "%08x", cpu.ecx);
	sprintf(edx, "%08x", cpu.edx);
	sprintf(esp, "%08x", cpu.esp);
	sprintf(ebp, "%08x", cpu.ebp);
	sprintf(esi, "%08x", cpu.esi);
	sprintf(edi, "%08x", cpu.edi);
	printf("%%eip: 0x%s\n", eip);	
	printf("%%eax: 0x%s\n", eax);	
	printf("%%ebx: 0x%s\n", ebx);	
	printf("%%ecx: 0x%s\n", ecx);	
	printf("%%edx: 0x%s\n", edx);	
	printf("%%esp: 0x%s\n", esp);	
	printf("%%ebp: 0x%s\n", ebp);	
	printf("%%esi: 0x%s\n", esi);	
	printf("%%edi: 0x%s\n", edi);	
  }
  else if (strcmp(args, "w") == 0){
	/* TODO: print watch points */
  }
  else{
  	cmd_help("info");
	return 0;
  }

  return 0;
}

static int cmd_x(char *args){
  /* TODO: fetch memory */
  // parse args
  Assert(args != NULL, "too few argument");
  char *sLen = strtok(args, " ");
  Assert(sLen != NULL, "too few argument");
  char *sAddr = strtok(NULL, " ");
  Assert(sAddr != NULL, "too few argument");
  Assert(strtok(NULL, " ") == NULL, "too many argument");
  
  // convert
  int len = atoi(sLen);
  vaddr_t nAddr;
  sscanf(sAddr + 2, "%x", &nAddr);
  
  uint32_t nn = vaddr_read(nAddr, len);
  uint8_t *p_nn = (void *)&nn;
  int i;
  for (i = 0; i < len; i+=4){
	if (len - i < 4){
		printf("%07x: ", host_to_guest(&p_nn[i]));
		int k;
		for (k = 0; k < len - i; k++){
			printf("0x%02x ", p_nn[i+k]);
		}
		printf("\n");
	}
	else{
		printf("%07x: 0x%02x 0x%02x 0x%02x 0x%02x\n", host_to_guest(&p_nn[i]), p_nn[i], p_nn[i+1], p_nn[i+2], p_nn[i+3]);
	}
  }
  
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */ 
  { "si", "si [N], Exec next N instr, default to 1", cmd_si },
  { "info", "info r|w, Print infos of register or watchpoint", cmd_info },
  { "x", "x N EXPR, Print memory by N bytes", cmd_x },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

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
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
