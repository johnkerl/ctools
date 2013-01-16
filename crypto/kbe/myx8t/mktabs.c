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

typedef unsigned char  u8;
typedef unsigned short u16;

#define REDUCTOR   ((u16)0x01c3)
#define GENERATOR  ((u8 )0x02)

// ----------------------------------------------------------------
// If multiplication is done using a table instead:
// a * b = g^(log_g(a * b))
//       = g^(log_g(a) ^ log_g(b))

u8 fe_slow_mul(u8 arg1, u8 arg2)
{
	int i;
	int j;
	u16 m1 = arg1;
	u16 m2 = arg2;
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

// ----------------------------------------------------------------
void do_all_mults(void)
{
	int i, j;
	u8 a, b, c;

	for (i = 0; i < 256; i++) {
		a = (u8) i;
		for (j = 0; j < 256; j++) {
			b = (u8) j;
			c = fe_slow_mul(a, b);
			printf("0x%02x * 0x%02x = 0x%02x\n",
				(unsigned) a, (unsigned) b, (unsigned) c);
		}
		printf("\n");
	}
}

// ----------------------------------------------------------------
void make_table(void)
{
	int i;
	int x, n;
	u8 p = 0x01;
	u8 exp_tab[510];
	u8 log_g;

	printf("#ifndef MYX8TTABS_H\n");
	printf("#define MYX8TTABS_H\n");
	printf("\n");
	printf("// Exp table for generator g = 0x%02x, reductor 0x%03x:\n",
		(unsigned)GENERATOR, (unsigned)REDUCTOR);
	printf("// Contains g^n, indexed by n.\n");
	printf("// Contents are duplicated for ease of indexing.\n");
	printf("\n");

	printf("static unsigned char myx8t_exp_table[] = {\n");

	for (i = 0; i < 255; i++) {
		exp_tab[i] = p;
		printf("%s0x%02x%s",
			(i % 8) == 0 ? "\t" : "",
			(unsigned) p,
			(i % 8) == 7 ? ",\n" : ", ");
		p = fe_slow_mul(p, GENERATOR);
	}
	printf("\n");
	printf("\n");
	for (i = 0; i < 255; i++) {
		printf("%s0x%02x%s",
			(i % 8) == 0 ? "\t" : "",
			(unsigned) exp_tab[i],
			(i % 8) == 7 ? ",\n" : ", ");
	}
	printf("\n");

	printf("};\n");

	printf("\n");
	printf("// Log table for generator g = 0x%02x, reductor 0x%03x:\n",
		(unsigned)GENERATOR, (unsigned)REDUCTOR);
	printf("// Contains log_g(x), indexed by x (treated as an\n");
	printf("// integer rather than as a polynomial).\n");
	printf("\n");

	printf("static unsigned char myx8t_log_table[] = {\n");
	printf("\t0xff,\n");
	printf("\t// Dummy entry, to make indexing convenient.\n");
	printf("\t// Log of zero of course doesn't exist.\n");
	printf("\t      ");

	// For each x, find n such that g^n = x

	for (x = 0; x < 256; x++) {
		for (n = 0; n < 255; n++) {
			if (exp_tab[n] == x)
				printf("%s0x%02x%s",
					(x % 8) == 0 ? "\t" : "",
					(unsigned) n,
					(x % 8) == 7 ? ",\n" : ", ");
		}
		
	}
	printf("};\n");


	printf("\n");
	printf("#endif // MYX8TTABS_H\n");
}

// ----------------------------------------------------------------
int main(void)
{
	make_table();
	return 0;
}
