#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#define TestCorrect(x) if(x){printf("Invalid Command!\n");return 0;}
void cpu_exec(uint32_t);

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

static int cmd_si(char *args) {
	int step;
	char *arg = strtok(NULL," ");
	if (args == NULL) step = 1;
	else sscanf(arg, "%d", &step);
	cpu_exec(step);
	return 0;
}

static int cmd_info(char *args)  {  
    char *arg=strtok(NULL," "); 
	
    if(strcmp(arg,"r") == 0){  
		int i=0; 
        while(i<8){
            printf("%s \t%x \t%d\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32); 
			i++;
		}  
         
            printf("eip \t%x \t%d\n", cpu.eip, cpu.eip); 
			

    }  
    return 0;  
} 


static int cmd_x(char *args){  
    char *N = strtok(NULL," ");  
    char *EXPR = strtok(NULL," ");  
    int len;  
    lnaddr_t address;  
      
    sscanf(N, "%d", &len);  
    sscanf(EXPR, "%x", &address);  
      
    printf("0x%x:",address);  
    int i=0;
    while(i < len){  
        printf("%08x ",lnaddr_read(address,4));  
        address += 4;  
		i++;
    }  
    printf("\n");  
    return 0;  
}

static int cmd_p(char *args) {
	TestCorrect(args == NULL);
	uint32_t ans;
	int i=0;
	bool flag;
	ans = expr(args, &flag);
	TestCorrect(!flag) 
	else {
		char s[100];
		while(ans){
			if(ans %16>=10)
			    s[i]=ans %16+55;
			else
			    s[i]=ans %16+48;
			i++;
			ans=ans/16;	
		}
		for(i=i-1;i>=0;i--)
		    printf("%c",s[i]);
		printf("(");
		printf("%d", ans);
		printf(")");
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
    { "si", "Step into implementation of N instructions after the suspension of execution.When N is notgiven,the default is 1.", cmd_si},
	{ "info", "r for print register state\nw for print watchpoint information", cmd_info},
	{ "x", "Calculate the value of the expression and regard the result as the starting memory address.", cmd_x},
    { "p", "Calculate expressions", cmd_p},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
