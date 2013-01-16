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
//
// Examples:
// $ jot 1 10
// 1
// 2
// 3
// 4
// 5
// 6
// 7
// 8
// 9
// 10
//
// $ jot 10 4 5
// 10
// 15
// 20
// 25
//
// $ d2h $(jot 0 128) | unhex | hex
// 00000000: 00 01 02 03  04 05 06 07  08 09 0a 0b  0c 0d 0e 0f |................|
// 00000010: 10 11 12 13  14 15 16 17  18 19 1a 1b  1c 1d 1e 1f |................|
// 00000020: 20 21 22 23  24 25 26 27  28 29 2a 2b  2c 2d 2e 2f | !"#$%&'()*+,-./|
// 00000030: 30 31 32 33  34 35 36 37  38 39 3a 3b  3c 3d 3e 3f |0123456789:;<=>?|
// 00000040: 40 41 42 43  44 45 46 47  48 49 4a 4b  4c 4d 4e 4f |@ABCDEFGHIJKLMNO|
// 00000050: 50 51 52 53  54 55 56 57  58 59 5a 5b  5c 5d 5e 5f |PQRSTUVWXYZ[\]^_|
// 00000060: 60 61 62 63  64 65 66 67  68 69 6a 6b  6c 6d 6e 6f |`abcdefghijklmno|
// 00000070: 70 71 72 73  74 75 76 77  78 79 7a 7b  7c 7d 7e 7f |pqrstuvwxyz{|}~.|
//
// ================================================================

#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------------*/
static void usage(char * prog)
{
	fprintf(stderr, "Usage: %s start [reps [stride]].\n", prog);
	exit(1);
}

int main(
	int argc,
	char **argv)
{
	int walker  = 1;
	int reps    = 1;
	int stride  = 1;

	if (argc < 2 || argc > 4)
		usage(argv[0]);
	if (argc >= 2) {
		if (sscanf(argv[1], "%d", &walker) != 1)
		usage(argv[0]);
	}
	if (argc >= 3) {
		if (sscanf(argv[2], "%d", &reps) != 1)
		usage(argv[0]);
	}
	if (argc >= 4) {
		if (sscanf(argv[3], "%d", &stride) != 1)
		usage(argv[0]);
	}

	while (reps > 0) {
		printf("%d\n", walker);
		walker += stride;
		reps --;
	}

	return 0;
}
