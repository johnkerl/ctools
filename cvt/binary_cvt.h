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

#ifndef BINARY_CVT_H
#define BINARY_CVT_H

char * ulong_to_binary_string(
	unsigned long input,
	char output[]);

int ulong_from_binary_string(
	char input[],
	unsigned long * poutput);

#endif /* BINARY_CVT_H */
