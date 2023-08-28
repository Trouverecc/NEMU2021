#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
static WP* new_wp(){
	WP* temp;
	if(free_ == NULL){
		printf("Error: free_ is full.\n");
		return NULL;
	}
	temp = free_;
	free_ = free_->next;
	return temp;
	
}

static bool free_wp(WP *wp){
	if(wp < wp_pool || wp > wp_pool + NR_WP){
		return false;
	}
	wp->next = free_;
	free_ = wp;
	return true;
}

int set_watchpoints(char* new_str){
	uint32_t val;
	bool success;
	success = true;
	val = expr(new_str, &success);
	if(success == false){
		return -1;
	}
	WP* temp;
	temp = new_wp();
	if(temp == NULL)
		return -1;
	strcpy(temp->str, new_str);
	temp->old_val = val;
	temp->next = head;
	head = temp;
	return temp->NO;
	
}



bool delete_watchpoints(int NO){
	WP *p;
	WP *pre;
	pre = NULL;
	for(p = head; p != NULL; pre = p, p = p->next){
		if(p->NO == NO)
			break;
	}
	if(p == NULL)
		return false;
	if(pre == NULL)
		head = p->next;
	else{
		pre->next = p->next;
	}
	bool ifmake;
	ifmake = free_wp(p);
	if(ifmake == false){
		return false;
	}
	return true;
}

WP* ask_head(){
	return head;
}

void print_watchpoints(){
	if(head == NULL){
		printf("There are no watchpoints.\n");
	}
	else{
		WP *p;
		for(p = head; p != NULL; p = p->next){
			printf("watchpoint %d: %s\n", p->NO, p->str);
			printf("        the value now is: %d(0x%08x).\n", p->old_val, p->old_val);
		}
	}
}


