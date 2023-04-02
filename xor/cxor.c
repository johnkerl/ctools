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

// Command-line XOR.

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	char line[256];
	unsigned  l, r, x;
	int lno = 0;

	while (fgets(line, sizeof(line), stdin)) {
		lno++;
		if (sscanf(line, "%x %x", &l, &r) != 2)  {
			fprintf(stderr, "%s:  parse error at line %d.\n", argv[0], lno);
			exit(1);
		}
		x = l ^ r;
		printf("0x%08x ^ 0x%08x = 0x%08x\n", l, r, x);
	}
	return 0;
}
