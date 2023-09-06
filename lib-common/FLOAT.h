#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "trap.h"

typedef int FLOAT;

static inline int F2int(FLOAT a) {
	int sf = 0;
	if(a & 0x80000000) {//(a>>31)&1
		sf = 1;
		a = ~a + 1;
	}
	a >>= 16;
	if(sf) a = ~a + 1;
	return a;
}

static inline FLOAT int2F(int a) {
	int sf = 0;
	if(a & 0x80000000) {
		sf = 1;
		a = ~a + 1;
	}
	a <<= 16;
	if(sf) a = ~a + 1;
	return a;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
	return a * b;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
	return a / b;
}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT sqrt(FLOAT);
FLOAT pow(FLOAT, FLOAT);

// used when calling printf/sprintf to format a FLOAT argument
#define FLOAT_ARG(f) (long long)f

void init_FLOAT_vfprintf(void);

#endif
