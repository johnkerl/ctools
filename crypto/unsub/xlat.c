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
#include <ctype.h>

void xlat_maps(
	char * alpha_map_p,
	char * numer_map_p,
	char * alpha_map_c,
	char * numer_map_c)
{
	int ci;
	char c;
	char p;
	char alpha_map[26];
	char numer_map[26];
	int i;

	for (i = 0; i < 26; i++) {
		c = alpha_map_c[i];
		p = alpha_map_p[i];
		alpha_map[c - 'a'] = p;
	}

	for (i = 0; i < 10; i++) {
		c = numer_map_c[i];
		p = numer_map_p[i];
		numer_map[c - '0'] = p;
	}

	for (i = 0; i < 26; i++)
		printf("%c", alpha_map[i]);
	printf(" ");
	for (i = 0; i < 10; i++)
		printf("%c", numer_map[i]);
	printf("\n");

}

static void usage(char * argv0)
{
	fprintf(stderr,
	"Usage: %s {alpha p} {numer p} {alpha c} {numer c}\n",
		argv0);
	exit(1);
}

int main(int argc, char ** argv)
{
	char * alpha_map_p;
	char * numer_map_p;
	char * alpha_map_c;
	char * numer_map_c;

	if (argc != 5)
		usage(argv[0]);
	alpha_map_p = argv[1];
	numer_map_p = argv[2];
	alpha_map_c = argv[3];
	numer_map_c = argv[4];

	if (strlen(alpha_map_p) != 26)
		usage(argv[0]);
	if (strlen(numer_map_p) != 10)
		usage(argv[0]);
	if (strlen(alpha_map_c) != 26)
		usage(argv[0]);
	if (strlen(numer_map_c) != 10)
		usage(argv[0]);

	xlat_maps(alpha_map_p, numer_map_p,
		alpha_map_c, numer_map_c);

	return 0;
}
