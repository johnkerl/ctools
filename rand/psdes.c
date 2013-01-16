// ================================================================
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

#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "psdes.h"

// ================================================================
static unsigned non_reentrant_state0 = 0;
static unsigned non_reentrant_state1 = 0;
static unsigned non_reentrant_seeded = 0;

// ----------------------------------------------------------------
unsigned iran32(void)
{
	if (!non_reentrant_seeded)
		sran32_tod();
	return iran32_r(&non_reentrant_state0, &non_reentrant_state1);
}

// ----------------------------------------------------------------
void iran64(unsigned * pout0, unsigned * pout1)
{
	if (!non_reentrant_seeded)
		sran32_tod();
	return iran64_r(pout0, pout1,
		&non_reentrant_state0, &non_reentrant_state1);
}

// ----------------------------------------------------------------
float fran32(void)
{
	if (!non_reentrant_seeded)
		sran32_tod();
	return fran32_r(&non_reentrant_state0, &non_reentrant_state1);
}

// ---------------------------------------------------------------;-
void sran32(unsigned s)
{
	non_reentrant_state0 = 0;
	non_reentrant_state1 = s;
	non_reentrant_seeded = 1;
}

// ---------------------------------------------------------------;-
void sran32b(unsigned s0, unsigned s1)
{
	non_reentrant_state0 = s0;
	non_reentrant_state1 = s1;
	non_reentrant_seeded = 1;
}

// ----------------------------------------------------------------
void sran32_tod(void)
{
	struct timeval tod;
	(void)gettimeofday(&tod, 0);
	non_reentrant_state0 = getpid() ^ tod.tv_usec;
	non_reentrant_state1 = tod.tv_sec ^ (tod.tv_usec * tod.tv_usec + 1);
	non_reentrant_seeded = 1;
}

// ================================================================
unsigned iran32_r(unsigned * pstate0, unsigned * pstate1)
{
	unsigned word0  = *pstate0;
	unsigned word1  = *pstate1;
	psdes_hash_64(&word0, &word1);

	(*pstate1)++;
	if (*pstate1 == 0)
		(*pstate0)++;

	// Return low 32 bits of hash output; discard high 32 bits.
	return word1;
}

// ----------------------------------------------------------------
void iran64_r(unsigned * pout0, unsigned * pout1,
	unsigned * pstate0, unsigned * pstate1)
{
	*pout0 = *pstate0;
	*pout1 = *pstate1;
	psdes_hash_64(pout0, pout1);

	(*pstate1)++;
	if (*pstate1 == 0)
		(*pstate0)++;
}

// ----------------------------------------------------------------
float fran32_r(unsigned * pstate0, unsigned * pstate1)
{
	unsigned word = iran32_r(pstate0, pstate1);
	return (float)( (double)word / (double)4294967296.0 );
}

// ----------------------------------------------------------------
void sran32_tod_r(unsigned * pstate0, unsigned * pstate1)
{
	struct timeval tod;
	(void)gettimeofday(&tod, 0);
	*pstate0 = getpid() ^ tod.tv_usec;
	*pstate1 = tod.tv_sec ^ (tod.tv_usec * tod.tv_usec + 1);
}

// ----------------------------------------------------------------
// A 64-bit in-place hash, loosely inspired by DES.
// From _Numerical Recipes in C_.
//
// This is a 4-round Feistel network with the following function on
// the right-hand member R:
//
// * XOR one constant into R.
// * Split into high and low 16-bit halves; remember these as iah and ial
// * Square each, forming two 32-bit products
// * Invert the bits in the high square
// * Add the inverted high square to the lower square, forming a
//   32-bit sum
// * Swap high and low 16-bit halves
// * XOR in another constant
// * Add the 32-bit product of iah and ial from above

#define NITER 4

void psdes_hash_64(
	unsigned * pword0,
	unsigned * pword1)
{
	unsigned i;  // Round counter
	unsigned iswap, ia, iah, ial, ib, ic; // Intermediate values
	static unsigned c1[NITER] = {
		0xbaa96887, 0x1e17d32c, 0x03bcdc3c, 0x0f33d1b2 };
	static unsigned c2[NITER] = {
		0x4b0f3b58, 0xe874f0c3, 0x6955c5a6, 0x55a7ca46 };

	for (i = 0; i < NITER; i++) {
		iswap = *pword1;

		ia  = iswap ^ c1[i];
		ial = ia & 0xffff;
		iah = ia >> 16;
		ib  = ial * ial + ~(iah * iah);
		ic  = (ib >> 16) | ((ib & 0xffff) << 16);
		*pword1 = (*pword0) ^ ((ic ^ c2[i]) + ial * iah);

		*pword0 = iswap;
	}
}
