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

int main()
{
	unsigned char a[4][4];
	int i;
	int j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("[%d][%d] %08x ", i, j, &a[i][j]);

		}
		printf("\n");
	}
	return 0;
}
