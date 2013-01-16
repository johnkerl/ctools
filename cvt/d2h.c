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

/* --------------------------------------------------------------------*/
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s {one or more decimal numbers ...}\n", argv0);
	exit(1);
}

/* --------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int argi;
	unsigned long x;

	if (argc < 2)
		usage(argv[0]);

	for (argi = 1; argi < argc; argi++) {
		if (sscanf(argv[argi], "%lu", &x) == 1) {
			printf("0x%08lx\n", x);
		}
		else {
			fprintf(stderr,
				"Couldn't parse argument \"%s\" as decimal integer.\n",
				argv[argi]);
		}
	}

	return 0;
}
