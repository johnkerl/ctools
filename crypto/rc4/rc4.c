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

#include "rc4.h"

// ----------------------------------------------------------------
void RC4_stream_init(
	RC4_stream_t  * ps,
	unsigned char * key,
	unsigned        key_length)
{
	int i, j;
	unsigned char K[256];
	unsigned char temp;

	// Initialize S-box to be the identity permutation.
	for (i = 0; i <= 255; i++)
		ps->S[i] = i;

	// Copy key material (repeatedly if necessary) to fill out
	// a 256-byte array.
	for (i = 0; i <= 255; i++)
		K[i] = key[i % key_length];

	// Do pseudo-random, key-dependent swaps within the S-box.
	// The i index walks through each element in the S-box, ensuring
	// each is touched; the j index jumps around less regularly,
	// ensuring that the swaps are random.
	j = 0;
	for (i = 0; i <= 255; i++) {
		j = (j + ps->S[i] + K[i]) & 0xff;

		// Swap S[i], S[j]:
		temp = ps->S[i];
		ps->S[i] = ps->S[j];
		ps->S[j] = temp;
	}

	// Initialize stream counters.
	ps->si = 0;
	ps->sj = 0;
}

// ----------------------------------------------------------------
unsigned char RC4_stream_step(
	RC4_stream_t  * ps)
{
	unsigned char t;
	unsigned char temp;

	// Increment si, mod 256 (implicitly since si, sj are 8-bit ints).
	ps->si = ps->si + 1;
	// Update sj, mod 256.
	ps->sj = ps->sj + ps->S[ps->si];

	// Swap S[si] and S[sj].
	temp          = ps->S[ps->si];
	ps->S[ps->si] = ps->S[ps->sj];
	ps->S[ps->sj] = temp;

	// Addition mod 256 (again, implicitly since si, sj are 8-bit ints).
	t = ps->S[ps->si] + ps->S[ps->sj];

	return ps->S[t];
}
