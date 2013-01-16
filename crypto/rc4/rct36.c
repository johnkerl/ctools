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


// ----------------------------------------------------------------
void RCT36_stream_init(
	unsigned char   alpha[26],
	unsigned char   numer[10],
	unsigned char * key,
	unsigned        key_length)
{
	int i, j;
	unsigned char K[26];
	unsigned char temp;

	// Initialize S-boxes to be identity permutations.
	for (i = 0; i < 26; i++)
		alpha[i] = i;
	for (i = 0; i < 10; i++)
		numer[i] = i;

	// Copy key material (repeatedly if necessary) to fill out
	// a 26-byte array.
	for (i = 0; i < 26; i++)
		K[i] = key[i % key_length];

	// Do pseudo-random, key-dependent swaps within the S-box.
	// The i index walks through each element in the S-box, ensuring
	// each is touched; the j index jumps around less regularly,
	// ensuring that the swaps are random.
	j = 0;
	for (i = 0; i < 26; i++) {
		j = (j + alpha[i] + K[i]) % 26;

		// Swap S[i], S[j]:
		temp     = alpha[i];
		alpha[i] = alpha[j];
		alpha[j] = temp;
	}
}
