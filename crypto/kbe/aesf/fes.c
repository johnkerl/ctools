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
#include <string.h>
#include "aesfalg.h"
#include "logexptbls.h"

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr, "Usage: %s {a} {b}\n", argv0);
	exit(0);
}

// ----------------------------------------------------------------
int main(int argc, char ** argv)
{
	u32 t;
	u8  a;
	u8  s = 0;
	int argi;

	if (argc < 2) {
		char line[128];

		while (fgets(line, 128, stdin)) {
			char * p = line;
			int len = strlen(line);
			if (line[len-1] == '\n')
				line[len-1] = 0;
			p = strtok(line, " \t");
			if (p) {
				do {
					if (sscanf(p, "%x", &t) != 1) {
						fprintf(stderr,
						"Not hex: <<%s>>\n", p);
						exit(1);
					}
					a = t;
					s = S[a];
					printf(" %02x", s);
				} while (p=(strtok(0, " \t")));
			}
			printf("\n");
		}
	}
	else {
		for (argi = 1; argi < argc; argi++) {
			if (sscanf(argv[argi], "%x", &t) != 1)
				usage(argv[0]);
			a = t;
			s = S[a];
			printf("0x%02x\n", s);
		}
	}

	return 0;
}
