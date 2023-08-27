#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>
//#include <stulib.h>

enum {
        NOTYPE = 256, EQ, NEQ, LESSEQ, GREATEREQ, NOTEQUAL,
        AND, OR, HEXNUM, NUM, REGNAME, NEG, REF,

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},		
	{"==", EQ},
	{"!=", NEQ},
	{">=", LESSEQ},
	{"<=", GREATEREQ},
	{">", '>'},	
	{"<", '<'},
	{"!=", NOTEQUAL},
	{"\\&\\&", AND},
	{"\\|\\|", OR},
	{"\\!", '!'},
	{"0x[0-9a-f]+", HEXNUM},
	{"[0-9]+", NUM},
	{"\\$[a-z]{2,3}",REGNAME},
	{"\\+", '+'},					
	{"\\-", '-'},
	{"\\*", '*'},
	{"\\/", '/'},
	{"\\%", '%'},
	{"\\(", '('},
	{"\\)", ')'},	

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				
				switch(rules[i].token_type) {
					case NOTYPE: 
						break;
					case NUM:
					case HEXNUM:
					case REGNAME:
						tokens[nr_token].type = rules[i].token_type;
						sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
						nr_token++;
						break;				
					//default: panic("please implement me");
					default:
					    tokens[nr_token++].type = rules[i].token_type;
					break;
				}
                
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true; 
}

static bool ifmatch;

static bool check_if_match(int p, int q){
	int lecounter, i;
	lecounter = 0;
	for(i = p; i <= q; i++){
		if(tokens[i].type == '(')
			lecounter++;
		else if(tokens[i].type == ')')
			lecounter--;
		if(lecounter < 0){
			return false;
		}
	}
	if(lecounter == 0)
        return true;
	else 
		return false;
}
static int op_value(int op_type){
	switch(op_type){
		case '!':
		case NEG:
		case REF:
			return 0;
		case '*':
		case '/':
		case '%':
			return 1;
		case '+':
		case '-':
			return 2;
		case EQ:
		case NEQ:
			return 4;
		case AND:
			return 7;
		case OR:
			return 8;
		default: 
			ifmatch = false;
			return -1;
			
	}
}

static int op_cmp(int op_type1, int op_type2){
	return op_value(op_type1) - op_value(op_type2);
}

static bool check_parentheses(int p, int q){
	if(ifmatch == false){
		return false;
	}
	int lecounter, i;
	lecounter = 0;
	bool res;
	res = true;
	if(tokens[p].type != '(' || tokens[q].type != ')')
		return false;
	ifmatch = check_if_match(p, q);
	for(i = p + 1; i < q; i++){
		if(tokens[i].type == '('){
			lecounter++;
		}
		else if(tokens[i].type == ')'){
			lecounter--;
		}
		if(lecounter < 0){
			res = false;
			
		}
	}
	if(lecounter == 0 && res == true)
		res = true;
	else if(lecounter != 0 || res == false)
		res = false;
	return res;
}

static int find_dominant_op(int p, int q){
	if(ifmatch == false){
		return 0;
	}
	int lbrac_count;
	int i;
	int tmp_op;
	lbrac_count = 0;
	tmp_op = -1;
	for(i = p; i <= q; i++){
		switch(tokens[i].type){
			case REGNAME:
			case NUM:
			case HEXNUM:
			case '!':
			case NEG:
			case REF:
				break;
			case '(':
				lbrac_count++;
				break;
			case ')':
				lbrac_count--;
				if(lbrac_count < 0){
					ifmatch = false;
					return 0;
				}
				break;
			default:
				if(lbrac_count == 0){
					if(tmp_op == -1 || op_cmp(tokens[tmp_op].type, tokens[i].type) < 0 ||op_cmp(tokens[tmp_op].type, tokens[i].type) == 0){
						tmp_op = i;
					}
				}
				break;
		}
	}
	return tmp_op;
} 

uint32_t eval(int p, int q){
	if(ifmatch == true){
		if(p > q){
			ifmatch = false;
		}
		else if(p == q){
			uint32_t res;
			switch(tokens[p].type){
				case NUM:
					sscanf(tokens[p].str, "%d", &res);
					return res;
				case HEXNUM:
					sscanf(tokens[p].str + 2, "%x", &res);
					return res;	
				case REGNAME:
					if(strcmp(tokens[p].str, "$eip") == 0){
						return cpu.eip;
					}	
					int i;
					for(i = R_EAX; i <= R_EDI; i++){
						if(strcmp(tokens[p].str + 1, regsl[i]) == 0){
							return cpu.gpr[i]._32;
						}
					}
					for(i = R_AX; i <= R_DI; i++){
						if(strcmp(tokens[p].str + 1, regsw[i]) == 0)
							return cpu.gpr[i]._16;
					}
					for(i = R_AL; i <= R_BH; i++){
						if(strcmp(tokens[p].str + 1, regsb[i]) == 0)
							return cpu.gpr[i]._8[1];
					}
			}
		}        	
		else if(check_parentheses(p, q) == true){
			uint32_t ans;
			ans = eval(p + 1, q - 1);
			return ans;
		}
		else{
			int op;
			op = find_dominant_op(p, q);
			if(op == -1){
				int p_type;
				p_type = tokens[p].type;
				ifmatch = true;
				if(p_type == NEG || p_type == REF || p_type == '!'){
					uint32_t val3;
					val3 = eval(p + 1, q);
					switch(p_type){
						case NEG:
							return -val3;
						case REF:
							return swaddr_read(val3, 4);
						case '!':
							return !val3;
						default:
							ifmatch = false;
					}
				}
			}
			uint32_t val1, val2;
			val1 = eval(p, op - 1);
			val2 = eval(op + 1, q);
			switch(tokens[op].type){
				case '+': 
					return val1 + val2;
				case '-': 
					return val1 - val2;
				case '*': 
					return val1 * val2;
				case '/': 
					return val1 / val2;
				case '%':
					 return val1 % val2;
				case EQ: 
					return val1 == val2;
				case NEQ: 
					return val1 != val2;
				case AND:
					 return val1 && val2;
				case OR:
					 return val1 || val2;
				default: 
					ifmatch = false;
			}
		}
	}
	return -1;
}
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
    int i;
	int pre_type;
	for(i = 0; i <= nr_token - 1; i++){
		if(i == 0 && tokens[i].type == '-'){
			tokens[i].type = NEG;
		}
		else if(i != 0 && tokens[i].type == '-'){
			pre_type = tokens[i - 1].type;
			if(pre_type == NUM || pre_type == HEXNUM || pre_type == ')' || pre_type == REGNAME){
				tokens[i].type = '-';
			}
			else {
				tokens[i].type = NEG;
			}
		}
		if(i == 0 && tokens[i].type == '*'){
			tokens[i].type = REF;
		}
		else if(i != 0 && tokens[i].type == '*'){
			pre_type = tokens[i - 1].type;
			if(pre_type == NUM || pre_type == HEXNUM || pre_type == ')' || pre_type == REGNAME){
				tokens[i].type = '*';
			}
			else{
				tokens[i].type = REF;
			}
		}
	}
	uint32_t ans;
	ifmatch=true;
	ans=eval(0,nr_token-1);
	*success=ifmatch;
	return ans;
}


