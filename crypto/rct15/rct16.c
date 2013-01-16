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
#include <ctype.h>

// ----------------------------------------------------------------
void init_permutations(
	unsigned char   S[94],
	unsigned char   U[94],
	unsigned char * key,
	int             key_length)
{
	unsigned char   K[94];
	unsigned char   temp;
	int    i;
	int    j;

	for (i = 0; i < 94; i++)
		S[i] = i;

	for (i = 0; i < 94; i++) {
		K[i] = (key[i % key_length] + i*0xe7) % 94;
	}

	for (i = 0; i < 94; i++) {
		j = (i + S[i] + K[i]) % 94;
		temp     = S[i];
		S[i] = S[j];
		S[j] = temp;
	}
	for (i = 0; i < 94; i++)
		U[S[i]] = i;
}

// ----------------------------------------------------------------
static unsigned char apply_permutations(
	unsigned char   S[94],
	unsigned char   c)

{
	if (c >= 127)
		return c;
	if (c < 33)
		return c;
	c -= 33;
	c = S[c];
	c += 33;
	return c;
}

// ----------------------------------------------------------------
static void usage(char * argv0)
{
	fprintf(stderr, "Usage: %s {+key}\n", argv0);
	fprintf(stderr, "or:    %s {-key}\n", argv0);
	exit(1);
}

// ----------------------------------------------------------------
int main(int argc, char ** argv)
{
	unsigned char   S[94];
	unsigned char   U[94];
	unsigned char * key;
	int             key_length;
	unsigned        ic;
	unsigned char   c;
	int             forward;

	if (argc != 2)
		usage(argv[0]);
	if (argv[1][0] == '+')
		forward = 1;
	else if (argv[1][0] == '-')
		forward = 0;
	else
		usage(argv[0]);

	key = &argv[1][1];
	key_length = strlen(key);

	init_permutations(S, U, key, key_length);

	while ( (ic=getc(stdin)) != EOF) {
		c = ic;
		if (forward)
			c = apply_permutations(S, c);
		else
			c = apply_permutations(U, c);
		(void)putc((int)c, stdout);
	}

	return 0;
}
