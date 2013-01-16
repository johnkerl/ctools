// ================================================================
// Copyright (c) 2004 John Kerl.
// kerl.john.r@gmail.com
//
// This code and information is provided as is without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.
//
// No restrictions are placed on copy or reuse of this code, as long
// as these paragraphs are included in the code.
// ================================================================

#include <stdio.h>
#include "myx8tabs.h"

typedef unsigned char  u8;
typedef unsigned short u16;

#define REDUCTOR   ((u16)0x01c3)
#define GENERATOR  ((u8 )0x02)

// ----------------------------------------------------------------
// If multiplication is done using a table instead:
// a * b = g^(log_g(a * b))
//       = g^(log_g(a) ^ log_g(b))

u8 fe_slow_mul(u8 a, u8 b)
{
	int i;
	int j;
	u16 m1 = a;
	u16 m2 = b;
	u16 pprod;
	u16 prod;
	u16 biti;

	prod = 0;
	for (i = 0; i < 8; i++) {
		biti = (m2 >> i) & 1;
		if (!biti)
			continue;
		pprod = m1;
		for (j = 0; j < i; j++) {
			pprod <<= 1;
			if (pprod & 0x100)
				pprod ^= REDUCTOR;
		}
		prod ^= pprod;
	}
	return (u8)prod;
}

u8 fe_fast_mul(u8 a, u8 b)
{
	if (a && b)
		return myx8_exp_table[myx8_log_table[a] + myx8_log_table[b]];
	else
		return 0;
}

// ----------------------------------------------------------------
void check_table(void)
{
	int i, j;
	u8 a, b, s, f;
	int num_good = 0;
	int num_bad  = 0;

	for (i = 0; i < 256; i++) {
		a = (u8) i;
		for (j = 0; j < 256; j++) {
			b = (u8) j;
			s = fe_slow_mul(a, b);
			f = fe_fast_mul(a, b);
			if (s == f) {
				num_good++;
			}
			else {
				num_bad++;
			}
			//printf("0x%02x * 0x%02x = 0x%02x\n",
				//(unsigned) a, (unsigned) b, (unsigned) c);
		}
		//printf("\n");
	}
	printf("good %d bad %d\n", num_good, num_bad);
}

// ----------------------------------------------------------------
int main(void)
{
	check_table();
	return 0;
}
