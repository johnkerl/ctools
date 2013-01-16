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
#include <ctype.h>

typedef struct _char_count_t {
	char  c;
	int   count;
} char_count_t;

// ----------------------------------------------------------------
int char_count_cmp(const void * pv1, const void * pv2)
{
	char_count_t * pc1 = (char_count_t *)pv1;
	char_count_t * pc2 = (char_count_t *)pv2;
	return pc2->count - pc1->count;
}

// ----------------------------------------------------------------
void char_freq(char * file_name)
{
	FILE * fp;
	int ci;
	char c;
	char_count_t alphas[26];
	char_count_t numers[10];
	int i;

	for (i = 0; i < 26; i++) {
		alphas[i].c = 'a' + i;
		alphas[i].count = 0;
	}
	for (i = 0; i < 10; i++) {
		numers[i].c = '0' + i;
		numers[i].count = 0;
	}

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
		if (isdigit(c))
			numers[c-'0'].count++;
		else if (islower(c))
			alphas[c-'a'].count++;
		else if (isupper(c))
			alphas[c-'A'].count++;
	}

	if (fp != stdin)
		fclose(fp);

	qsort(&alphas[0], 26, sizeof(char_count_t), char_count_cmp);
	qsort(&numers[0], 10, sizeof(char_count_t), char_count_cmp);

	for (i = 0; i < 26; i++) {
		//printf("%c %d\n", 'a' + i, alphas[i].count);
		printf("%c", alphas[i].c);
	}
	printf(" ");
	for (i = 0; i < 10; i++) {
		//printf("%c %d\n", '0' + i, numers[i].count);
		printf("%c", numers[i].c);
	}
	printf("\n");
}

int main(void)
{
	char_freq("-");
	return 0;
}
