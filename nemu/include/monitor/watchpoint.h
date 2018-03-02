#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char str[32];
	int oldvalue;
	int value;
} WP;

WP *new_wp(char *str, int value);
void free_wp(int id);
void show_wp();
void show_free();
WP *check_wp(WP * w);

#endif
