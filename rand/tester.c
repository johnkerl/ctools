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
#include "psdes.h"

// ----------------------------------------------------------------
int main(
	int     argc,
	char ** argv)
{
	char line[80];
	static char prompt[] = "> ";
	int s = 0;
	int r = 0;

	printf(prompt); fflush(stdout);
	while (gets(line)) {
		if ((line[0] == 's') && (line[1] == '=')) {
			if (sscanf(&line[2], "%i", &s) == 1) {
				printf("  s = 0x%08x\n", s);
				// s = -s;
				r = iran32(&s);
				printf("  s = 0x%08x  r = 0x%08x\n", s, r);
			}
			else {
				printf("Couldn't parse input.\n");
			}
		}
		else if (line[0] == 's') {
			printf("  s = 0x%08x\n", s);
		}
		else if (line[0] == 'r') {
			r = iran32(&s);
			printf("  s = 0x%08x  r = 0x%08x\n", s, r);
		}
		else if (line[0] == 'd') {
			printf("  s = 0x%08x  r = 0x%08x\n", s, r);
		}
		else if ((line[0] == 'h') || (line[0] == '?')) {
			printf("Commands:\n");
			printf("  s={...}:  Select sequence and step once, if s is negative;\n");
			printf("            select element number if s is non-negative.\n");
			printf("  s:        Show s.\n");
			printf("  r:        Step current sequence once.\n");
			printf("  d:        Show s and r.\n");
			printf("  h/?:      help\n");
			printf("  q/x:      quit\n");
			printf("\n");
		}
		else if (line[0] == 'q')
			break;
		else if (line[0] == 'x')
			break;
		else if (line[0] == 0) {
			;
		}
		else {
			printf("?  [Type 'h' for help]\n");
		}
		printf(prompt); fflush(stdout);
	}
	return 0;
}
