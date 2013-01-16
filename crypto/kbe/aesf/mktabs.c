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

#include "aesfalg.h"
#include "boxesf-ref.dat"

u32 T0[256];
u32 T1[256];
u32 T2[256];
u32 T3[256];

u32 U0[256];
u32 U1[256];
u32 U2[256];
u32 U3[256];

// ----------------------------------------------------------------
static void print_w32_table(
	char   * name,
	u32 * T,
	int      size)
{
	int i;
	printf("static u32 %s[%d] = {\n", name, size);
	for (i = 0; i < size; i++) {
		printf("%c0x%08x/*%02x*/,%s",
			((i & 3) == 0) ? '\t' : ' ',
			T[i],
			i,
			((i & 3) == 3) ? "\n" : "");
	}
	if ((size & 3) != 0)
		printf("\n");
	printf("};\n");
	printf("\n");
}

// ----------------------------------------------------------------
static void print_w8_table(
	char  * name,
	u8 * T,
	int     size)
{
	int i;
	printf("static u8 %s[%d] = {\n", name, size);
	for (i = 0; i < size; i++) {
		printf("%c0x%02x,%s",
			((i & 7) == 0) ? '\t' : ' ',
			T[i],
			((i & 7) == 7) ? "\n" : "");
	}
	if ((size & 7) != 0)
		printf("\n");
	printf("};\n");
	printf("\n");
}

// ----------------------------------------------------------------
int main(void)
{
	int i;
	u8 v2, v3, v1;
	u8 ve, vb, vd, v9;

	for (i = 0; i <= 255; i++) {

		v2 = M2[S[i]];
		v3 = M3[S[i]];
		v1 =    S[i];

		ve = Me[i];
		vb = Mb[i];
		vd = Md[i];
		v9 = M9[i];

		T0[i] = (v2 << 24) | (v1 << 16) | (v1 << 8) | v3;
		T1[i] = (v3 << 24) | (v2 << 16) | (v1 << 8) | v1;
		T2[i] = (v1 << 24) | (v3 << 16) | (v2 << 8) | v1;
		T3[i] = (v1 << 24) | (v1 << 16) | (v3 << 8) | v2;

		U0[i] = (ve << 24) | (v9 << 16) | (vd << 8) | vb;
		U1[i] = (vb << 24) | (ve << 16) | (v9 << 8) | vd;
		U2[i] = (vd << 24) | (vb << 16) | (ve << 8) | v9;
		U3[i] = (v9 << 24) | (vd << 16) | (vb << 8) | ve;

	}

	printf("#ifndef AESF_TABS_H\n");
	printf("#define AESF_TABS_H\n");
	printf("\n");

	print_w32_table("T0", T0, 256);
	print_w32_table("T1", T1, 256);
	print_w32_table("T2", T2, 256);
	print_w32_table("T3", T3, 256);
	printf("\n");

	print_w32_table("U0", U0, 256);
	print_w32_table("U1", U1, 256);
	print_w32_table("U2", U2, 256);
	print_w32_table("U3", U3, 256);
	printf("\n");

	print_w8_table("S", S, 256);
	print_w8_table("Si", Si, 256);
	printf("\n");

	print_w32_table("rcon", rcon, 30);

	printf("#endif // AESF_TABS_H\n");

	return 0;
}
