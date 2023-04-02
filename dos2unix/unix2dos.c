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
// This is an NT workalike for the familiar Unix program.

// ================================================================
// John Kerl
// 2001-02-02
// ================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ----------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s {input files ... }\n", argv0);
	exit(1);
}
// ----------------------------------------------------------------
int main(int argc, char **argv)
{
	int    argi;
	int    num_fail = 0;
	char * input_file_name;
	char   temp_file_name[256];
	FILE * input_fp;
	FILE * temp_fp;
	int    c;

	if (argc < 2)
		usage(argv[0]);

	for (argi = 1; argi < argc; argi++) {
		input_file_name = argv[argi];
		sprintf(temp_file_name, "%s.__d2u__", input_file_name);

		input_fp  = fopen(input_file_name, "rb");  // Open in binary mode for read.
		if (input_fp == 0) {
			fprintf(stderr, "Couldn't open input file \"%s\" for read.  Exiting.\n",
				input_file_name);
			num_fail++;
			continue;
		}

		temp_fp = fopen(temp_file_name, "wb"); // Open in binary mode for write.
			if (temp_fp == 0) {
			fprintf(stderr, "Couldn't open temp file \"%s\" for write.  Exiting.\n",
				temp_file_name);
			fclose(input_fp);
			num_fail++;
			continue;
		}

		while ((c = fgetc(input_fp)) != EOF) {
			if (c == 0x0a)
				fputc(0x0d, temp_fp);
			fputc(c, temp_fp);
		}

		fclose(input_fp);
		fclose(temp_fp);

		if (unlink(input_file_name) < 0) {
			fprintf(stderr, "Could not unlink %s",
				input_file_name);
			perror(": ");
			num_fail++;
			continue;
		}

		if (rename(temp_file_name, input_file_name) < 0) {
			fprintf(stderr, "Could not rename %s to %s",
				temp_file_name, input_file_name);
			perror(": ");
			num_fail++;
		}

	}

	return num_fail;
}
