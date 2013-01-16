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

void apply_maps(
	char * alpha_map,
	char * numer_map,
	char * file_name)
{
	FILE * fp;
	int ci;
	char c;
	char p;
	int i;

	if (strcmp(file_name, "-") == 0)
		fp = stdin;
	else {
		fp = fopen(file_name, "r");
		if (fp == NULL) {
			fprintf(stderr, "Couldn't open \"%s\" for read.\n",
				file_name);
			exit(1);
		}
	}

	while ( (ci=fgetc(fp)) != EOF) {
		c = ci;
		if (isdigit(c)) {
			c = /*'0' + */ numer_map[c - '0'];
		}
		else if (islower(c)) {
			c = /*'a'+ */ alpha_map[c - 'a'];
		}
		else if (isupper(c)) {
			c = toupper(/*'a' + */ alpha_map[c - 'A']);
		}
		fputc(c, stdout);
	}

	if (fp != stdin)	
		fclose(fp);
}

static void usage(char * argv0)
{
	fprintf(stderr,
	"Usage: %s {alpha map} {numer map} {file name}\n",
		argv0);
	exit(1);
}

int main(int argc, char ** argv)
{
	char * file_name;
	char * alpha_map;
	char * numer_map;
	int i;

	if (argc != 4)
		usage(argv[0]);
	alpha_map = argv[1];
	numer_map = argv[2];
	file_name = argv[3];

	if (strlen(alpha_map) != 26)
		usage(argv[0]);
	if (strlen(numer_map) != 10)
		usage(argv[0]);

	apply_maps(alpha_map, numer_map, file_name);

	return 0;
}
