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
	fprintf(stderr, "Usage: %s {key} {file}\n", argv0);
	exit(1);
}

// ----------------------------------------------------------------
int main(int argc, char **argv)
{
	char   * filename;
	HANDLE   hFileHandle;
	HANDLE   hMapHandle;
	DWORD    dwFileSize;
	void   * pFileContents;
	unsigned char * pWalker;
	DWORD    dwByteCounter;
	char   * key;
	int      key_length;
	RC4_stream_t stream;

	if (argc != 3)
		usage(argv[0]);
	key      = argv[1];
	key_length = strlen(key);
	if (key_length == 0)
		key_length = 1;
	filename = argv[2];

	RC4_stream_init(&stream, (unsigned char *) key, key_length);

	pFileContents = my_mmap(
		filename,
		&hFileHandle,
		&hMapHandle,
		&dwFileSize);

	if (pFileContents == 0) {
		fprintf(stderr,
			"Couldn't open input file \"%s\" for read/write.\n",
			filename);
		exit(1);
	}


	pWalker = (unsigned char *)pFileContents;
	for (dwByteCounter = 0; dwByteCounter < dwFileSize; dwByteCounter++) {
		*pWalker ^= RC4_stream_step(&stream);
		pWalker++;
	}

	my_munmap(
		&pFileContents,
		&hFileHandle,
		&hMapHandle);

	return 0;
}
