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
// A stdio-driven matrix multiplier over F_256.

#include <stdio.h>
#include <stdlib.h>
#include "aesfalg.h"
#include "logexptbls.h"

#define BC 4

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr, "Usage: %s {a} {b}\n", argv0);
	exit(0);
}

// ----------------------------------------------------------------
static u8 mul(
	u8 a,
	u8 b)
{
	if (a && b)
		return fe_exp_table[fe_log_table[a] + fe_log_table[b]];
	else
		return 0;
}


// ----------------------------------------------------------------
int main(int argc, char ** argv)
{
	u8  a[4][MAXBC];
	u8  b[4][MAXBC];
	u8  m[4][4] = {
		{ 0x02, 0x03, 0x01, 0x01 },
		{ 0x01, 0x02, 0x03, 0x01 },
		{ 0x01, 0x01, 0x02, 0x03 },
		{ 0x03, 0x01, 0x01, 0x02 },
	};
	u32 t;
	int i, j, k;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < BC; j++) {
			if (scanf("%x", &t) != 1) {
				fprintf(stderr,
				"Matrix read failure row %d column %d.\n",
					i, j);
				exit(1);
			}
			a[i][j] = t;
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < BC; j++) {
			b[i][j] = 0;
			for (k = 0; k < 4; k++) {
				b[i][j] ^= mul(m[i][k], a[k][j]);
			}
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < BC; j++)
			printf(" %02x", b[i][j]);

		printf("\n");
	}

	return 0;
}
