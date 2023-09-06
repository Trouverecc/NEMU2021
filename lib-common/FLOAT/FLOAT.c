#include "FLOAT.h"

typedef union{
	long long a;
	struct{
		int b :16;
		int c :32;
		int d :16;
	};
}fmulf;
typedef union{
	float a;
	int b;
}fbi;

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	fmulf ans;
	ans.a = 1ll * a * b;
	return ans.c;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */

	FLOAT ans, temp;
	asm volatile ("idiv %2" : "=a"(ans), "=d"(temp) : "r"(b), "a"(a << 16), "d"(a >> 16));
	return ans;
}

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */

	//return (FLOAT)(a * 65536.0);
	fbi aa;
	aa.a = a;
	int s = (aa.b >> 31) & 1;
	int zeros = 0;
	int exp = (aa.b >> 23) & 0xff, E;
	FLOAT ans = aa.b & 0x7fffff;
	if(exp == 0xff) ans = 0x80000000;
	else {
		E = -126;
		if(exp) {
			ans |= 1 << 23;
			E = exp - 0xff;
		}
		E -= 23;
		if(E <= -32) ans >>= 32;
		else if(E <= 0) ans >>= E;
		else if(E <= 32) ans <<= E;
		else ans <<= 32;
	}
	/*if(s == zeros) return ans;
		ans = ~ans;
		++ans;*/
	ans = (ans ^ ((s << 31) >> 31)) + s;
	return ans;
}

FLOAT Fabs(FLOAT a) {
	FLOAT ans = a;
	if((a >> 31) & 1) ans = ~ans + 1;
	return ans;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

