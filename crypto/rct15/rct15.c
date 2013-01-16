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
static void dump_K(
	unsigned char K[26])
{
	int i;
	printf("K: ");
	for (i = 0; i < 26; i++)
		printf("%c", K[i]);
	printf("\n");
}

// ----------------------------------------------------------------
static void dump_S(
	unsigned char * S,
	int    S_len)
{
	int i;
	printf("S:");
	for (i = 0; i < S_len; i++)
		printf(" %02x", ((unsigned)S[i]) & 0xff);
	printf("\n");
}

// ----------------------------------------------------------------
void init_permutations(
	unsigned char   alpha [26],
	unsigned char   ialpha[26],
	unsigned char   numer [10],
	unsigned char   inumer[10],
	unsigned char * key,
	int    key_length)
{
	unsigned char   K[26];
	unsigned char   temp;
	int    i;
	int    j;

	for (i = 0; i < 26; i++)
		alpha[i] = i;
	for (i = 0; i < 10; i++)
		numer[i] = i;

	for (i = 0; i < 26; i++) {
		K[i] = (key[i % key_length] + i*0xe7) % 26;
	}
	//dump_K(K);

	//dump_S(alpha, 26);
	for (i = 0; i < 26; i++) {
		j = (i + alpha[i] + K[i]) % 26;
		temp     = alpha[i];
		alpha[i] = alpha[j];
		alpha[j] = temp;
	}
	//dump_S(alpha, 26);
	for (i = 0; i < 26; i++)
		ialpha[alpha[i]] = i;
	//dump_S(ialpha, 26);

	//printf("\n");

	//dump_S(numer, 10);
	for (i = 0; i < 10; i++) {
		j = (i + numer[i] + K[i]) % 10;
		temp     = numer[i];
		numer[i] = numer[j];
		numer[j] = temp;
	}
	//dump_S(numer, 10);
	for (i = 0; i < 10; i++)
		inumer[numer[i]] = i;
	//dump_S(inumer, 10);

	//printf("\n");
}

// ----------------------------------------------------------------
static unsigned char apply_permutations(
	unsigned char   alpha [26],
	unsigned char   numer [10],
	unsigned char   c)

{
	if (isupper(c)) {
		c -= 'A';
		c  = alpha[c];
		c += 'A';
	}
	else if (islower(c)) {
		c -= 'a';
		c  = alpha[c];
		c += 'a';
	}
	else if (isdigit(c)) {
		c -= '0';
		c  = numer[c];
		c += '0';
	}
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
	unsigned char   alpha [26];
	unsigned char   ialpha[26];
	unsigned char   numer [10];
	unsigned char   inumer[10];
	unsigned char * key;
	int    key_length;
	unsigned ic;
	unsigned char   c;
	int    forward;

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

	init_permutations(alpha, ialpha, numer, inumer, key, key_length);

	while ( (ic=getc(stdin)) != EOF) {
		c = ic;
		if (forward)
			c = apply_permutations(alpha, numer, c);
		else
			c = apply_permutations(ialpha, inumer, c);
		(void)putc((int)c, stdout);
	}

	return 0;
}
