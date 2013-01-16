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

typedef unsigned __u32;

#define HASH_BUFFER_SIZE 5
#define HASH_EXTRA_SIZE 80
#define HASH_TRANSFORM SHATransform

/* Various size/speed tradeoffs are available.  Choose 0..3. */
#define SHA_CODE_SIZE 0

/*
 * SHA transform algorithm, taken from code written by Peter Gutmann,
 * and placed in the public domain.
 */

/* The SHA f()-functions.  */

#define f1(x,y,z)   ( z ^ (x & (y^z)) )		/* Rounds  0-19: x ? y : z */
#define f2(x,y,z)   (x ^ y ^ z)			/* Rounds 20-39: XOR */
#define f3(x,y,z)   ( (x & y) + (z & (x ^ y)) )	/* Rounds 40-59: majority */
#define f4(x,y,z)   (x ^ y ^ z)			/* Rounds 60-79: XOR */

/* The SHA Mysterious Constants */

#define K1  0x5A827999L			/* Rounds  0-19: sqrt(2) * 2^30 */
#define K2  0x6ED9EBA1L			/* Rounds 20-39: sqrt(3) * 2^30 */
#define K3  0x8F1BBCDCL			/* Rounds 40-59: sqrt(5) * 2^30 */
#define K4  0xCA62C1D6L			/* Rounds 60-79: sqrt(10) * 2^30 */

#define ROTL(n,X)  ( ( ( X ) << n ) | ( ( X ) >> ( 32 - n ) ) )

#define subRound(a, b, c, d, e, f, k, data) \
    ( e += ROTL( 5, a ) + f( b, c, d ) + k + data, b = ROTL( 30, b ) )


static void SHATransform(__u32 digest[85], __u32 const data[16])
{
    __u32 A, B, C, D, E;     /* Local vars */
    __u32 TEMP;
    int	i;
#define W (digest + HASH_BUFFER_SIZE)	/* Expanded data array */

    /*
     * Do the preliminary expansion of 16 to 80 words.  Doing it
     * out-of-line line this is faster than doing it in-line on
     * register-starved machines like the x86, and not really any
     * slower on real processors.
     */
    memcpy(W, data, 16*sizeof(__u32));
    for (i = 0; i < 64; i++) {
	    TEMP = W[i] ^ W[i+2] ^ W[i+8] ^ W[i+13];
	    W[i+16] = ROTL(1, TEMP);
    }

    /* Set up first buffer and local data buffer */
    A = digest[ 0 ];
    B = digest[ 1 ];
    C = digest[ 2 ];
    D = digest[ 3 ];
    E = digest[ 4 ];

    /* Heavy mangling, in 4 sub-rounds of 20 iterations each. */
#if SHA_CODE_SIZE == 0
    /*
     * Approximately 50% of the speed of the largest version, but
     * takes up 1/16 the space.  Saves about 6k on an i386 kernel.
     */
    for (i = 0; i < 80; i++) {
	if (i < 40) {
	    if (i < 20)
		TEMP = f1(B, C, D) + K1;
	    else
		TEMP = f2(B, C, D) + K2;
	} else {
	    if (i < 60)
		TEMP = f3(B, C, D) + K3;
	    else
		TEMP = f4(B, C, D) + K4;
	}
	TEMP += ROTL(5, A) + E + W[i];
	E = D; D = C; C = ROTL(30, B); B = A; A = TEMP;
    }
#elif SHA_CODE_SIZE == 1
    for (i = 0; i < 20; i++) {
	TEMP = f1(B, C, D) + K1 + ROTL(5, A) + E + W[i];
	E = D; D = C; C = ROTL(30, B); B = A; A = TEMP;
    }
    for (; i < 40; i++) {
	TEMP = f2(B, C, D) + K2 + ROTL(5, A) + E + W[i];
	E = D; D = C; C = ROTL(30, B); B = A; A = TEMP;
    }
    for (; i < 60; i++) {
	TEMP = f3(B, C, D) + K3 + ROTL(5, A) + E + W[i];
	E = D; D = C; C = ROTL(30, B); B = A; A = TEMP;
    }
    for (; i < 80; i++) {
	TEMP = f4(B, C, D) + K4 + ROTL(5, A) + E + W[i];
	E = D; D = C; C = ROTL(30, B); B = A; A = TEMP;
    }
#elif SHA_CODE_SIZE == 2
    for (i = 0; i < 20; i += 5) {
	subRound( A, B, C, D, E, f1, K1, W[ i   ] );
	subRound( E, A, B, C, D, f1, K1, W[ i+1 ] );
	subRound( D, E, A, B, C, f1, K1, W[ i+2 ] );
	subRound( C, D, E, A, B, f1, K1, W[ i+3 ] );
	subRound( B, C, D, E, A, f1, K1, W[ i+4 ] );
    }
    for (; i < 40; i += 5) {
	subRound( A, B, C, D, E, f2, K2, W[ i   ] );
	subRound( E, A, B, C, D, f2, K2, W[ i+1 ] );
	subRound( D, E, A, B, C, f2, K2, W[ i+2 ] );
	subRound( C, D, E, A, B, f2, K2, W[ i+3 ] );
	subRound( B, C, D, E, A, f2, K2, W[ i+4 ] );
    }
    for (; i < 60; i += 5) {
	subRound( A, B, C, D, E, f3, K3, W[ i   ] );
	subRound( E, A, B, C, D, f3, K3, W[ i+1 ] );
	subRound( D, E, A, B, C, f3, K3, W[ i+2 ] );
	subRound( C, D, E, A, B, f3, K3, W[ i+3 ] );
	subRound( B, C, D, E, A, f3, K3, W[ i+4 ] );
    }
    for (; i < 80; i += 5) {
	subRound( A, B, C, D, E, f4, K4, W[ i   ] );
	subRound( E, A, B, C, D, f4, K4, W[ i+1 ] );
	subRound( D, E, A, B, C, f4, K4, W[ i+2 ] );
	subRound( C, D, E, A, B, f4, K4, W[ i+3 ] );
	subRound( B, C, D, E, A, f4, K4, W[ i+4 ] );
    }
#elif SHA_CODE_SIZE == 3 /* Really large version */
    subRound( A, B, C, D, E, f1, K1, W[  0 ] );
    subRound( E, A, B, C, D, f1, K1, W[  1 ] );
    subRound( D, E, A, B, C, f1, K1, W[  2 ] );
    subRound( C, D, E, A, B, f1, K1, W[  3 ] );
    subRound( B, C, D, E, A, f1, K1, W[  4 ] );
    subRound( A, B, C, D, E, f1, K1, W[  5 ] );
    subRound( E, A, B, C, D, f1, K1, W[  6 ] );
    subRound( D, E, A, B, C, f1, K1, W[  7 ] );
    subRound( C, D, E, A, B, f1, K1, W[  8 ] );
    subRound( B, C, D, E, A, f1, K1, W[  9 ] );
    subRound( A, B, C, D, E, f1, K1, W[ 10 ] );
    subRound( E, A, B, C, D, f1, K1, W[ 11 ] );
    subRound( D, E, A, B, C, f1, K1, W[ 12 ] );
    subRound( C, D, E, A, B, f1, K1, W[ 13 ] );
    subRound( B, C, D, E, A, f1, K1, W[ 14 ] );
    subRound( A, B, C, D, E, f1, K1, W[ 15 ] );
    subRound( E, A, B, C, D, f1, K1, W[ 16 ] );
    subRound( D, E, A, B, C, f1, K1, W[ 17 ] );
    subRound( C, D, E, A, B, f1, K1, W[ 18 ] );
    subRound( B, C, D, E, A, f1, K1, W[ 19 ] );

    subRound( A, B, C, D, E, f2, K2, W[ 20 ] );
    subRound( E, A, B, C, D, f2, K2, W[ 21 ] );
    subRound( D, E, A, B, C, f2, K2, W[ 22 ] );
    subRound( C, D, E, A, B, f2, K2, W[ 23 ] );
    subRound( B, C, D, E, A, f2, K2, W[ 24 ] );
    subRound( A, B, C, D, E, f2, K2, W[ 25 ] );
    subRound( E, A, B, C, D, f2, K2, W[ 26 ] );
    subRound( D, E, A, B, C, f2, K2, W[ 27 ] );
    subRound( C, D, E, A, B, f2, K2, W[ 28 ] );
    subRound( B, C, D, E, A, f2, K2, W[ 29 ] );
    subRound( A, B, C, D, E, f2, K2, W[ 30 ] );
    subRound( E, A, B, C, D, f2, K2, W[ 31 ] );
    subRound( D, E, A, B, C, f2, K2, W[ 32 ] );
    subRound( C, D, E, A, B, f2, K2, W[ 33 ] );
    subRound( B, C, D, E, A, f2, K2, W[ 34 ] );
    subRound( A, B, C, D, E, f2, K2, W[ 35 ] );
    subRound( E, A, B, C, D, f2, K2, W[ 36 ] );
    subRound( D, E, A, B, C, f2, K2, W[ 37 ] );
    subRound( C, D, E, A, B, f2, K2, W[ 38 ] );
    subRound( B, C, D, E, A, f2, K2, W[ 39 ] );
    
    subRound( A, B, C, D, E, f3, K3, W[ 40 ] );
    subRound( E, A, B, C, D, f3, K3, W[ 41 ] );
    subRound( D, E, A, B, C, f3, K3, W[ 42 ] );
    subRound( C, D, E, A, B, f3, K3, W[ 43 ] );
    subRound( B, C, D, E, A, f3, K3, W[ 44 ] );
    subRound( A, B, C, D, E, f3, K3, W[ 45 ] );
    subRound( E, A, B, C, D, f3, K3, W[ 46 ] );
    subRound( D, E, A, B, C, f3, K3, W[ 47 ] );
    subRound( C, D, E, A, B, f3, K3, W[ 48 ] );
    subRound( B, C, D, E, A, f3, K3, W[ 49 ] );
    subRound( A, B, C, D, E, f3, K3, W[ 50 ] );
    subRound( E, A, B, C, D, f3, K3, W[ 51 ] );
    subRound( D, E, A, B, C, f3, K3, W[ 52 ] );
    subRound( C, D, E, A, B, f3, K3, W[ 53 ] );
    subRound( B, C, D, E, A, f3, K3, W[ 54 ] );
    subRound( A, B, C, D, E, f3, K3, W[ 55 ] );
    subRound( E, A, B, C, D, f3, K3, W[ 56 ] );
    subRound( D, E, A, B, C, f3, K3, W[ 57 ] );
    subRound( C, D, E, A, B, f3, K3, W[ 58 ] );
    subRound( B, C, D, E, A, f3, K3, W[ 59 ] );

    subRound( A, B, C, D, E, f4, K4, W[ 60 ] );
    subRound( E, A, B, C, D, f4, K4, W[ 61 ] );
    subRound( D, E, A, B, C, f4, K4, W[ 62 ] );
    subRound( C, D, E, A, B, f4, K4, W[ 63 ] );
    subRound( B, C, D, E, A, f4, K4, W[ 64 ] );
    subRound( A, B, C, D, E, f4, K4, W[ 65 ] );
    subRound( E, A, B, C, D, f4, K4, W[ 66 ] );
    subRound( D, E, A, B, C, f4, K4, W[ 67 ] );
    subRound( C, D, E, A, B, f4, K4, W[ 68 ] );
    subRound( B, C, D, E, A, f4, K4, W[ 69 ] );
    subRound( A, B, C, D, E, f4, K4, W[ 70 ] );
    subRound( E, A, B, C, D, f4, K4, W[ 71 ] );
    subRound( D, E, A, B, C, f4, K4, W[ 72 ] );
    subRound( C, D, E, A, B, f4, K4, W[ 73 ] );
    subRound( B, C, D, E, A, f4, K4, W[ 74 ] );
    subRound( A, B, C, D, E, f4, K4, W[ 75 ] );
    subRound( E, A, B, C, D, f4, K4, W[ 76 ] );
    subRound( D, E, A, B, C, f4, K4, W[ 77 ] );
    subRound( C, D, E, A, B, f4, K4, W[ 78 ] );
    subRound( B, C, D, E, A, f4, K4, W[ 79 ] );
#else
#error Illegal SHA_CODE_SIZE
#endif

    /* Build message digest */
    digest[ 0 ] += A;
    digest[ 1 ] += B;
    digest[ 2 ] += C;
    digest[ 3 ] += D;
    digest[ 4 ] += E;

	/* W is wiped by the caller */
#undef W
}

// ----------------------------------------------------------------
void SHA_init(__u32 out[5])
{
	out[0] = 0x67452301;
	out[1] = 0xefcdab89;
	out[2] = 0x98badcfe;
	out[3] = 0x10325476;
	out[4] = 0xc3d2e1f0;
}

// ----------------------------------------------------------------
int main(void)
{
	__u32 in[16];
	__u32 out[85];
	int i;

	SHA_init(out);
	for (i = 0; i < 16; i++)
		in[i] = 0;
	SHATransform(out, in);
	printf("%08x %08x %08x %08x %08x\n",
		out[0], out[1], out[2], out[3], out[4]);

	SHA_init(out);
	in[0]++;
	SHATransform(out, in);
	printf("%08x %08x %08x %08x %08x\n",
		out[0], out[1], out[2], out[3], out[4]);

	return 0;
}
