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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

// ----------------------------------------------------------------
static void histo_one_file(
	char * filename);

static void usage(
	char * argv0);

static void find_mean_and_stddev(
	unsigned * counts,
	int        N,
	double   * pmean,
	double   * pstddev);

// ----------------------------------------------------------------
int main(int argc, char **argv)
{
	int argi;

	if (argc < 2)
		usage(argv[0]);

	for (argi = 1; argi < argc; argi++)
		histo_one_file(argv[argi]);

	return 0;
}

// ----------------------------------------------------------------
static void histo_one_file(
	char * filename)
{
	FILE   * fp;
	unsigned file_size;
	unsigned char line[512];
	unsigned counts[256];
	int  num_read;
	int i;
	int j;
	int n;
	unsigned min;
	unsigned max;
	double   mean;
	double   stddev;

	file_size = 0;
	for (i = 0; i < 256; i++)
		counts[i] = 0;

	fp = fopen(filename, "rb");
	if (fp == 0) {
		fprintf(stderr, "Couldn't open input file \"%s\" for binary read.\n",
			filename);
		return;
	}

	while ( (num_read=fread(line, sizeof(unsigned char), sizeof(line), fp)) > 0)  {
		for (i = 0; i < num_read; i++) {
			counts[line[i]]++;
		}
	}
	fclose(fp);

	min = 0;
	max = 0;
	for (i = 0; i < 256; i++) {
		if (i == 0) {
			min = counts[i];
			max = counts[i];
		}
		else {
			if (counts[i] < min)
				min = counts[i];
			if (counts[i] > max)
				max = counts[i];
		}
	}

	printf("----------------------------------------------------------------\n");
	printf("%s:\n", filename);
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 8; j++) {
			n = i + 32 * j;
			if (isprint(n) && (n != ' ') && (n != '\t'))
				printf("%2c", n);
			else
				printf("%02x", n);
			printf(": %-5d", counts[n]);
			if (j == 7)
				printf(" ");
		}
		printf("\n");
	}

	find_mean_and_stddev(counts, 256, &mean, &stddev);
	printf("Min: %-6d  Max: %-6d\n", min, max);
	printf("Mean: %7.4lf  Stddev: %7.4lf\n", mean, stddev);
	printf("\n");
}

// ----------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s {file(s) ...}\n", argv0);
	exit(1);
}

// ----------------------------------------------------------------
static void find_mean_and_stddev(
	unsigned * counts,
	int        N,
	double   * pmean,
	double   * pstddev)
{
	int i;
	double diff;

	*pmean   = 0.0;
	*pstddev = 0.0;

	for (i = 0; i < N; i++) {
		*pmean += (double)counts[i];
	}
	*pmean /= (double)N;

	for (i = 0; i < N; i++) {
		diff = (double)counts[i] - *pmean;
		*pstddev += diff * diff;
	}
	*pstddev /= (double)N;
	*pstddev = sqrt(*pstddev);
}
