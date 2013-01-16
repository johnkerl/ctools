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
#include <unistd.h>
#include <sys/time.h>
#include "psdes.h"

#define DO_BINARY   3
#define DO_HEX_32   4
#define DO_HEX_64   5
#define DO_FLOAT    6

typedef struct _opts_t {
	int seed0;
	int seed1;
	int do_seed;
	int output_type;
	int count;
} opts_t;

static void set_defaults(opts_t * popts);
static int parse_command_line(int argc, char ** argv, opts_t * popts);

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr,
		"Usage: %s [-s {seed}] [-bin] [-hex] [-hex64] [-float] {count}\n",
		argv0);
	fprintf(stderr,
		"For binary output, count is the number of bytes.\n");
	fprintf(stderr,
		"For hex output, count is the number of 32-bit words.\n");
	fprintf(stderr,
		"For hex64 output, count is the number of 64-bit words.\n");
	fprintf(stderr,
		"For float output, count is the number of 32-bit floats.\n");
	exit(1);
}

// ----------------------------------------------------------------
int main(
	int     argc,
	char ** argv)
{
	opts_t opts;
	int i;

	set_defaults(&opts);
	if (!parse_command_line(argc, argv, &opts))
		usage(argv[0]);

	if (opts.do_seed)
		sran32b(opts.seed0, opts.seed1);

	if (opts.output_type == DO_BINARY) {
		int bytes_left = opts.count;
		while (bytes_left) {
			unsigned rand = iran32();
			int num_to_write = bytes_left > sizeof(rand)
				? sizeof(rand) : bytes_left;
			(void)fwrite((char *)&rand, 1, num_to_write, stdout);
			bytes_left -= sizeof(rand);
		}
	}
	else if (opts.output_type == DO_FLOAT) {
		for (i = 0; i < opts.count; i++) {
			printf("%11.7f\n", fran32());
		}
	}
	else if (opts.output_type == DO_HEX_64) {
		unsigned out0, out1;
		for (i = 0; i < opts.count; i++) {
			iran64(&out0, &out1);
			printf("%08x%08x\n", out0, out1);
		}
	}
	else {
		for (i = 0; i < opts.count; i++) {
			printf("%08x\n", iran32());
		}
	}

	return 0;
}

// ----------------------------------------------------------------
static void set_defaults(opts_t * popts)
{
	popts->seed0     = 0;
	popts->seed1     = 0;
	popts->do_seed   = 0;
	popts->output_type = DO_HEX_32;
	popts->count     = 1;
}

// ----------------------------------------------------------------
static int parse_command_line(int argc, char ** argv, opts_t * popts)
{
	int argi;
	for (argi = 1; argi < argc; argi++) {
		if (strcmp(argv[argi], "--help") == 0)
			return 0;
		if (argv[argi][0] != '-')
			break;

		if (strcmp(argv[argi], "-s") == 0) {
			argi++;
			if ((argc - argi) < 1)
				return 0;
			if (sscanf(argv[argi], "%i:%i", &popts->seed0, &popts->seed1) == 2)
				;
			else if (sscanf(argv[argi], "%i", &popts->seed1) == 1)
				popts->seed0 = 0;
			else
				return 0;
			popts->do_seed = 1;
		}

		else if (strcmp(argv[argi], "-bin") == 0) {
			popts->output_type = DO_BINARY;
		}
		else if (strcmp(argv[argi], "-b") == 0) {
			popts->output_type = DO_BINARY;
		}

		else if (strcmp(argv[argi], "-hex") == 0) {
			popts->output_type = DO_HEX_32;
		}
		else if (strcmp(argv[argi], "-h") == 0) {
			popts->output_type = DO_HEX_32;
		}
		else if (strcmp(argv[argi], "-32") == 0) {
			popts->output_type = DO_HEX_32;
		}

		else if (strcmp(argv[argi], "-hex64") == 0) {
			popts->output_type = DO_HEX_64;
		}
		else if (strcmp(argv[argi], "-64") == 0) {
			popts->output_type = DO_HEX_64;
		}

		else if (strcmp(argv[argi], "-float") == 0) {
			popts->output_type = DO_FLOAT;
		}
		else if (strcmp(argv[argi], "-f") == 0) {
			popts->output_type = DO_FLOAT;
		}

		else {
			return 0;
		}
	}
	if ((argc - argi) == 0) {
		; // Use default count.
	}
	else if ((argc - argi) == 1) {
		if (sscanf(argv[argi], "%d", &popts->count) != 1)
			return 0;
	}
	else {
			return 0;
	}
	return 1;
}
