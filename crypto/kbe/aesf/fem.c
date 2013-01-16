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
// A command-line driven element multiplier over F_256.
// For a much more general tool, please see
// https://github.com/johnkerl/ruffl.

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
	u32 t;
	u8 a;
	u8 p = 1;
	int argi;

	if (argc < 2)
		usage(argv[0]);
	for (argi = 1; argi < argc; argi++) {
		if (sscanf(argv[argi], "%x", &t) != 1)
			usage(argv[0]);
		a = t;
		p = mul(p, a);
	}
	printf("0x%02x\n", p);

	return 0;
}
