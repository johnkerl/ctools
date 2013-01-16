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

// XORs two files together to produce a third.

#include <stdio.h>

//----------------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s {input file 1} {input file 2} {output file}\n", argv0);
	exit(1);
}

//----------------------------------------------------------------------
int main(int argc, char **argv)
{
	char * filename1;
	char * filename2;
	char * filename3;
	FILE * fp1;
	FILE * fp2;
	FILE * fp3;
	int n1;
	int n2;
	unsigned char c1;
	unsigned char c2;
	unsigned char c3;

	if (argc != 4)
		usage(argv[0]);
	filename1 = argv[1];
	filename2 = argv[2];
	filename3 = argv[3];

	fp1 = fopen(filename1, "rb");
	if (fp1 == NULL) {
		fprintf(stderr,
			"Couldn't open output file \"%s\" for read.\n",
			filename1);
		exit(1);
	}

	fp2 = fopen(filename2, "rb");
	if (fp2 == NULL) {
		fprintf(stderr,
			"Couldn't open output file \"%s\" for read.\n",
			filename2);
		fclose(fp1);
		exit(1);
	}

	fp3 = fopen(filename3, "wb");
	if (fp3 == NULL) {
		fprintf(stderr,
			"Couldn't open output file \"%s\" for write.\n",
			filename3);
		fclose(fp1);
		fclose(fp2);
		exit(1);
	}

	n1 = fread(&c1, 1, 1, fp1);
	n2 = fread(&c2, 1, 1, fp2);

	while ((n1 == 1) && (n2 == 1)) {
		c3 = c1 ^ c2;
		(void) fwrite(&c3, 1, 1, fp3);
		n1 = fread(&c1, 1, 1, fp1);
		n2 = fread(&c2, 1, 1, fp2);
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	return 0;
}
