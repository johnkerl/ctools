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
#include "gasdev.h"

typedef struct _opts_t {
	int   seed0;
	int   seed1;
	int   do_seed;
	float mean;
	float stddev;
	int num_samples;
} opts_t;

static void set_defaults(opts_t * popts);
static int parse_command_line(int argc, char ** argv, opts_t * popts);

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr,
		"Usage: %s [-s {seed}] [-m mean] [-t std dev] {num samples}\n",
		argv0);
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

	for (i = 0; i < opts.num_samples; i++) {
		float rand = opts.mean + opts.stddev * gasdev();
		printf("%11.7f\n", rand);
	}

	return 0;
}

// ----------------------------------------------------------------
static void set_defaults(opts_t * popts)
{
	popts->seed0       = 0;
	popts->seed1       = 0;
	popts->do_seed     = 0;
	popts->mean        = 0.0;
	popts->stddev      = 1.0;
	popts->num_samples = 1;
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

		else if (strcmp(argv[argi], "-m") == 0) {
			argi++;
			if ((argc - argi) < 1)
				return 0;
			if (sscanf(argv[argi], "%f", &popts->mean) != 1)
				return 0;
		}

		else if (strcmp(argv[argi], "-t") == 0) {
			argi++;
			if ((argc - argi) < 1)
				return 0;
			if (sscanf(argv[argi], "%f", &popts->stddev) != 1)
				return 0;
		}

		else {
			return 0;
		}
	}
	if ((argc - argi) == 0) {
		; // Use default num_bytes.
	}
	else if ((argc - argi) == 1) {
		if (sscanf(argv[argi], "%d", &popts->num_samples) != 1)
			return 0;
	}
	else {
			return 0;
	}
	return 1;
}
