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
#include <stdlib.h>
#include "aesfalg.h"
#include "logexptbls.h"

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr, "Usage: %s {a} {b}\n", argv0);
	exit(0);
}

// ----------------------------------------------------------------
static u8 fediv(
	u8 a,
	u8 b)
{
	int idx;
	if (a && b) {
		idx = (fe_log_table[a] - fe_log_table[b] + 510) % 255;
		return fe_exp_table[idx];
	}
	else
		return 0;
}


// ----------------------------------------------------------------
int main(int argc, char ** argv)
{
	u32 t;
	u8 a;
	u8 b;
	u8 q = 1;

	if (argc != 3)
		usage(argv[0]);
	if (sscanf(argv[1], "%x", &t) != 1)
		usage(argv[0]);
	a = t;
	if (sscanf(argv[2], "%x", &t) != 1)
		usage(argv[0]);
	b = t;

	q = fediv(a, b);
	printf("0x%02x\n", q);

	return 0;
}
