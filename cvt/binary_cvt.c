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

#include "binary_cvt.h"

char * ulong_to_binary_string(
	unsigned long input,
	char output[])
{
	int num_bits = sizeof(unsigned long) * 8;
	int i;
	char *p;

	p = output;
	for (i = num_bits - 1; i >= 0; i--) {
		*p++ = ((input >> i) & 1) + '0';

		if ( ((i % 8) == 0) && (i > 0) )
			*p++ = '_';
	}
	*p++ = 0;

	return output;
}

int ulong_from_binary_string(
	char input[],
	unsigned long * poutput)
{
	unsigned long u = 0;
	char * p;

	for (p = input; *p; p++) {
		if (*p == '_')
			continue;
		else if (*p == '1') {
			u <<= 1;
			u++;
		}
		else if (*p == '0') {
			u <<= 1;
		}
		else {
			return 0;
		}
	}

	*poutput = u;
	return 1;
}
