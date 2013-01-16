// ================================================================
// Copyright (c) 2004 John Kerl.
// kerl at math dot arizona dot edu
//
// This code and information is provided as is without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.
//
// No restrictions are placed on copy or reuse of this code, as long
// as these paragraphs are included in the code.
// ================================================================
// This is a command-line interface to a quick-and-dirty PRNG
// from Numerical Recipes.

#include <stdio.h>
#include <stdlib.h>
#include "qrand.h"

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr, "Usage: %s {seed} [# iterations]\n", argv0);
	exit(1);
}

// ----------------------------------------------------------------
int main(int argc, char ** argv)
{
	unsigned rand, i, n = 16, skip = 0;
	if (argc < 2)
		usage(argv[0]);
	if (argc > 4)
		usage(argv[0]);
	if (sscanf(argv[1], "%08x", &rand) != 1)
		usage(argv[0]);
	if (argc >= 3) {
		if (sscanf(argv[2], "%u", &n) != 1)
			usage(argv[0]);
	}
	if (argc == 4) {
		if (sscanf(argv[3], "%u", &skip) != 1)
			usage(argv[0]);
	}

	for (i = 0; i < skip; i++) {
		STEP_RAND(rand);
	}

	for (i = 0; i < n; i++) {
		printf("%08x", rand);
		STEP_RAND(rand);
		if ((i & 3) == 3)
			printf("\n");
		else
			printf(" ");
	}
	if ((n & 3) != 0)
		printf("\n");

	return 0;
}
