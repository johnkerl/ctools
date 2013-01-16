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

// ----------------------------------------------------------------
int main(
	int     argc,
	char ** argv)
{
	int argi;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s {one or more file names ...}\n",
			argv[0]);
		exit(1);
	}

	for (argi = 1; argi < argc; argi++) {
		char first_four[4];
		FILE * fp;

		fp = fopen(argv[argi], "rb");
		if (fp == 0) {
			printf("Could not open file \"%s\" for binary read.\n",
				argv[argi]);
			continue;
		}
		if (fread(first_four, sizeof(char), 4, fp) != 4) {
			printf("Could not read first four bytes of file \"%s\".\n",
				argv[argi]);
		}
		else {
			if ((first_four[0] == 'A')
			&&  (first_four[1] == 'E')
			&&  (first_four[2] == 'S')
			&&  (first_four[3] ==  0 ))
			{
				printf("AES file:      %s\n", argv[argi]);
			}
			else {
				printf("Non-AES file:  %s\n", argv[argi]);
			}
		}


		fclose(fp);
	}

	return 0;
}
