#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	char str[32];
	uint32_t old_val;
	uint32_t new_val;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */


} WP;
int set_watchpoints(char* expr);
bool delete_watchpoints(int NO);
WP* ask_head();
void print_watchpoints();

#endif
