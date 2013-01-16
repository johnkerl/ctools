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
#include <string.h>
#include <windows.h>
#include "rc4.h"
#include "my_mmap.h"

// ----------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s +{key} {file}\n", argv0);
	fprintf(stderr, "    or %s -{key} {file}\n", argv0);
	exit(1);
}

static void do_stdin(
	RC4_stream_t * pstream,
	int            forward);

static void do_mmap(
	RC4_stream_t * pstream,
	int            forward,
	char         * filename);

static unsigned char rotate(
	unsigned char in,
	int      shift,
	int      forward);

// ----------------------------------------------------------------
int main(int argc, char **argv)
{
	char        * key;
	int           key_length;
	int           forward;
	RC4_stream_t  stream;

	if ((argc != 2) && (argc != 3))
		usage(argv[0]);

	key = argv[1];
	if (key[0] == '+')
		forward = 1;
	else if (key[0] == '-')
		forward = 0;
	else
		usage(argv[0]);
	key++;

	key_length = strlen(key);
	// If the empty string was provided, use the null terminator as
	// a length-one key.
	if (key_length == 0)
		key_length = 1;

	RC4_stream_init(&stream, (unsigned char *) key, key_length);

	if (argc == 2)
		do_stdin(&stream, forward);
	else
		do_mmap (&stream, forward, argv[2]);

	return 0;
}

// ----------------------------------------------------------------
static void do_stdin(
	RC4_stream_t * pstream,
	int            forward)
{
	int c;
	unsigned char u;

	while ((c = getc(stdin)) != EOF) {
		if (c == 0x0a)
			(void)RC4_stream_step(pstream);
		putc((int)rotate((unsigned char)c, RC4_stream_step(pstream), forward), stdout);
	}
}

// ----------------------------------------------------------------
static void do_mmap(
	RC4_stream_t * pstream,
	int            forward,
	char         * filename)
{
	void   * pFileContents;
	unsigned char * pWalker;
	DWORD    dwByteCounter;
	HANDLE   hFileHandle;
	HANDLE   hMapHandle;
	DWORD    dwFileSize;

	pFileContents = my_mmap(
		filename,
		&hFileHandle,
		&hMapHandle,
		&dwFileSize);

	if (pFileContents == 0) {
		fprintf(stderr, "Couldn't open input file \"%s\" for read/write.\n",
			filename);
		exit(1);
	}


	pWalker = (unsigned char *)pFileContents;
	for (dwByteCounter = 0; dwByteCounter < dwFileSize; dwByteCounter++) {
		// *pWalker ^= RC4_stream_step(pstream);
		*pWalker = rotate(*pWalker, RC4_stream_step(pstream), forward);
		pWalker++;
	}

	my_munmap(
		&pFileContents,
		&hFileHandle,
		&hMapHandle);

}

// ----------------------------------------------------------------
static unsigned char rotate(
	unsigned char in,
	int      shift,
	int      forward)
{
	unsigned char out;
	int case_offset;
	int case_length;
	int case_index_in;
	int case_index_out;
	int modular_shift;

	if (islower(in)) {
		case_offset = 'a';
		case_length = 26;
	}
	else if (isupper(in)) {
		case_offset = 'A';
		case_length = 26;
	}
	else if (isdigit(in)) {
		case_offset = '0';
		case_length = 10;
	}
	else {
		return in;
	}

	case_index_in  = (int)in - case_offset;

	modular_shift = shift % case_length;
	if (!forward) {
		modular_shift  = case_length - modular_shift;
		modular_shift %= case_length;
	}

	case_index_out = (case_index_in + modular_shift) % case_length;

	out = (unsigned char)(case_offset + case_index_out);

	// printf(
	// 	"Char in: %c (%2d in case)  shift %s%3d (%2d mod %2d)  char out: %c (%2d in case)\n",
	// 	in, case_index_in,
	// 	(forward == 1) ? "+" : "-",
	// 	shift,
	// 	modular_shift, case_length,
	// 	out, case_index_out);

	return out;
}
