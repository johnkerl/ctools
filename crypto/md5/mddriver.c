/* MDDRIVER.C - test driver for MD2, MD4 and MD5 */

/* Copyright(C) 1990-2, RSA Data Security, Inc. Created 1990. All
rights reserved.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "global.h"
#include "md5.h"

/* Length of test block, number of test blocks.  */
#define TEST_BLOCK_LEN 10000
#define TEST_BLOCK_COUNT 10000

static void MDString(char *);
static void MDTimeTrial(void);
static void MDTestSuite(void);
static void MDFile(char *);
static void MDFilter(void);
static void MDPrint(unsigned char [16]);

#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final

// ----------------------------------------------------------------
static void usage(
	char * argv0)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  %s -s{string}:     digest specified string.\n",
		argv0);
	fprintf(stderr, "  %s -t:             run time trial.\n",
		argv0);
	fprintf(stderr, "  %s -x:             run test suite\n",
		argv0);
	fprintf(stderr, "  %s {one or more filenames ...}\n",
		argv0);
	fprintf(stderr, "  %s with no arguments: digest stdin.\n",
		argv0);
	exit(1);
}

// ----------------------------------------------------------------
/* Main driver.

Arguments(may be any combination):
  -s{string} - digests string
  -t       - runs time trial
  -x       - runs test script
  filename - digests file
(none)   - digests standard input
*/

int main(
	int argc,
	char *argv[])
{
	int argi;

	if (argc > 1) {
		for (argi = 1; argi < argc; argi++) {
			if (argv[argi][0] == '-' && argv[argi][1] == 's')
				MDString(argv[argi] + 2);
			else if (strcmp(argv[argi], "-t") == 0)
				MDTimeTrial();
			else if (strcmp(argv[argi], "-x") == 0)
				MDTestSuite();
			else if (strcmp(argv[argi], "--help") == 0)
				usage(argv[0]);
			else
				MDFile(argv[argi]);
		}
	}
	else {
		MDFilter();
	}

	return 0;
}

// ----------------------------------------------------------------
/* Digests a string and prints the result. */
static void MDString(
	char *string)
{
	MD_CTX context;
	unsigned char digest[16];
	unsigned int len = strlen(string);

	MDInit(&context);
	MDUpdate(&context, string, len);
	MDFinal(digest, &context);

	printf("MD5(\"%s\") = ", string);
	MDPrint(digest);
	printf("\n");
}

// ----------------------------------------------------------------
/* Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte blocks. */
static void MDTimeTrial(void)
{
	MD_CTX context;
	time_t endTime, startTime;
	unsigned char block[TEST_BLOCK_LEN], digest[16];
	unsigned int i;

	printf("MD5 time trial. Digesting %d %d-byte blocks ...",
		TEST_BLOCK_LEN, TEST_BLOCK_COUNT);

	/* Initialize block */
	for (i = 0; i < TEST_BLOCK_LEN; i++)
		block[i] =(unsigned char)(i & 0xff);

	/* Start timer */
	time(&startTime);

	/* Digest blocks */
	MDInit(&context);
	for (i = 0; i < TEST_BLOCK_COUNT; i++)
		MDUpdate(&context, block, TEST_BLOCK_LEN);
	MDFinal(digest, &context);

	/* Stop timer */
	time(&endTime);

	printf(" done\n");
	printf("Digest = ");
	MDPrint(digest);
	printf("\nTime = %ld seconds\n",(long)(endTime-startTime));
	printf("Speed = %ld bytes/second\n",
		(long)TEST_BLOCK_LEN *
		(long)TEST_BLOCK_COUNT/(endTime-startTime));
}

// ----------------------------------------------------------------
/* Digests a reference suite of strings and prints the results. */
static void MDTestSuite(void)
{
	printf("MD5 test suite:\n");

	MDString("");
	MDString("a");
	MDString("abc");
	MDString("message digest");
	MDString("abcdefghijklmnopqrstuvwxyz");
	MDString(
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"0123456789");
	MDString(
		"1234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890");
}

// ----------------------------------------------------------------
/* Digests a file and prints the result. */
static void MDFile(
	char *filename)
{
	FILE *file;
	MD_CTX context;
	int len;
	unsigned char buffer[1024], digest[16];

	if ((file = fopen(filename, "rb")) == NULL)
		printf("%s can't be opened\n", filename);

	else {
		MDInit(&context);
		while ((len = fread(buffer, 1, 1024, file)) > 0) {
			MDUpdate(&context, buffer, len);
			if (feof(file))
				break;
		}
		MDFinal(digest, &context);

		fclose(file);

		printf("MD5(%s) = ", filename);
		MDPrint(digest);
		printf("\n");
	}
}

// ----------------------------------------------------------------
/* Digests the standard input and prints the result. */
static void MDFilter(void)
{
	MD_CTX context;
	int len;
	unsigned char buffer[16], digest[16];

	MDInit(&context);
	while ((len = fread(buffer, 1, 16, stdin)) > 0) {
		printf("len %d\n", len);
		MDUpdate(&context, buffer, len);
		if (feof(stdin))
			break;
	}
	MDFinal(digest, &context);

	MDPrint(digest);
	printf("\n");
}

// ----------------------------------------------------------------
/* Prints a message digest in hexadecimal. */
static void MDPrint(
	unsigned char digest[16])
{
	unsigned int i;

	for (i = 0; i < 16; i++)
		printf("%02x", digest[i]);
}
