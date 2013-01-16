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

//                table
//              ./    \.
//en: KA       (SU SR MC KA) SU SR KA
//de: KA SU SR (KA MC SU SR) KA
//                .\   /.
//                 table

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "aesfalg.h"
#include "util.h"
#include "blkcipht.h"

#include "aesf_tabs.h"

#define S4(a,b) { \
	((u8 *)(&a))[0] = S[((u8 *)(&b))[0]]; \
	((u8 *)(&a))[1] = S[((u8 *)(&b))[1]]; \
	((u8 *)(&a))[2] = S[((u8 *)(&b))[2]]; \
	((u8 *)(&a))[3] = S[((u8 *)(&b))[3]]; \
}

#define Si4(a,b) { \
	((u8 *)(&a))[0] = Si[((u8 *)(&b))[0]]; \
	((u8 *)(&a))[1] = Si[((u8 *)(&b))[1]]; \
	((u8 *)(&a))[2] = Si[((u8 *)(&b))[2]]; \
	((u8 *)(&a))[3] = Si[((u8 *)(&b))[3]]; \
}

//----------------------------------------------------------------
static u32 func_S4(
	u32 a,
	u8 box[256])
{
	u32 b;
	u8  a0, a1, a2, a3;
	u8  b0, b1, b2, b3;

	AESF_word_to_bytes(a, &a0, &a1, &a2, &a3);
	b0 = box[a0];
	b1 = box[a1];
	b2 = box[a2];
	b3 = box[a3];
	AESF_bytes_to_word(b0, b1, b2, b3, &b);

	return b;
}


//// ----------------------------------------------------------------
//void AESF_print_block(
//	char  * msg,
//	u32  a[MAXBC],
//	int     BC)
//{
//	int j;
//
//	fprintf(stderr, "%s:\n", msg);
//	for (j = 0; j < BC; j++)
//		fprintf(stderr, "%08x ", (unsigned)a[j]);
//	fprintf(stderr, "\n");
//	fprintf(stderr, "\n");
//}

// ----------------------------------------------------------------
void AESF_print_block(
	char * msg,
	u32 a[MAXBC],
	int BC)
{
	int i, j;
	u8 * pa = (u8 *)&a[0];

	fprintf(stderr, "%s:\n", msg);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < BC; j++)
			fprintf(stderr, "%02x ", (unsigned) pa[3-i+4*j]);
		fprintf(stderr, "{");
		for (j = 0; j < BC; j++)
			fprintf(stderr, "%c",
				isprint(pa[3-i+4*j]) ? pa[3-i+4*j] : '.');
		fprintf(stderr, "}\n");
	}
	fprintf(stderr, "\n");
}

//----------------------------------------------------------------
#ifdef DEBUG_AESFALG
#define DEBUG_BLOCK(msg,blk,bc) AESF_print_block(msg,blk,bc)
//#define DEBUG_BLOCK(msg,blk,bc) hex_dump_region(msg, &blk[0][0], bc * 4, bc)

#define DEBUG_TEXT(x) fprintf(stderr, "%s\n", x);
#define DEBUG_SEP fprintf(stderr, "\n")

#else
#define DEBUG_BLOCK(msg,blk,bc)
#define DEBUG_TEXT(x)
#define DEBUG_SEP
#endif

//----------------------------------------------------------------
void AESF_bytes_to_word(
	u8    b0,
	u8    b1,
	u8    b2,
	u8    b3,
	u32 * pw)
{
	*pw = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

void AESF_bytes_to_block(
	int               num_bytes,
	char            * input,
	u32            block[MAXBC])
{
	int bi;
	int wi;

	for (bi = 0, wi = 0; bi < num_bytes; bi+=4, wi++) {
		AESF_bytes_to_word(
			input[bi],
			input[bi+1],
			input[bi+2],
			input[bi+3],
			&block[wi]);
	}
}

//----------------------------------------------------------------
void AESF_word_to_bytes(
	u32   w,
	u8  * pb0,
	u8  * pb1,
	u8  * pb2,
	u8  * pb3)
{
	*pb0 = w >> 24;
	*pb1 = w >> 16;
	*pb2 = w >>  8;
	*pb3 = w;
}

void AESF_block_to_bytes(
	int               num_bytes,
	u32            block[MAXBC],
	char            * output)
{
	int bi;
	int wi;

	for (bi = 0, wi = 0; bi < num_bytes; bi+=4, wi++) {
		AESF_word_to_bytes(
			block[wi],
			&output[bi],
			&output[bi+1],
			&output[bi+2],
			&output[bi+3]);
	}
}

//----------------------------------------------------------------
static u32 rot8(
	u32 a)
{
	u32 b;
	u8  a0, a1, a2, a3;
	u8  b0, b1, b2, b3;

	AESF_word_to_bytes(a, &a0, &a1, &a2, &a3);
	b0 = a1;
	b1 = a2;
	b2 = a3;
	b3 = a0;
	AESF_bytes_to_word(b0, b1, b2, b3, &b);

	return b;
}

//----------------------------------------------------------------
//Calculate the necessary round keys.
//The number of calculations depends on kl and
//bl.

int AESF_key_schedule (
	u32 k[MAXKC],
	int kl,
	int bl,
	u32 W[MAXROUNDS+1][MAXKC])
{
	int KC, BC, ROUNDS;
	int j;
	int t;
	int rconindex = 0;
	u32 tk[MAXKC];

	switch (kl) {
	case 16: KC = 4; break;
	case 24: KC = 6; break;
	case 32: KC = 8; break;
	default : return -1;
	}

	switch (bl) {
	case 16: BC = 4; break;
	case 24: BC = 6; break;
	case 32: BC = 8; break;
	default : return -2;
	}

	switch ((kl >= bl) ? kl : bl) {
	case 16: ROUNDS = 10; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	for (j = 0; j < KC; j++)
		tk[j] = k[j];
	t = 0;
	for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
		W[t / BC][t % BC] = tk[j];

	while (t < (ROUNDS+1)*BC) {
		tk[0] ^= func_S4(rot8(tk[KC-1]), S);
		tk[0] ^= rcon[rconindex++] << 24;

		if (KC != 8) {
			for (j = 1; j < KC; j++)
				tk[j] ^= tk[j-1];
		}
		else {
			for (j = 1; j < KC/2; j++)
				tk[j] ^= tk[j-1];
			tk[KC/2] ^= func_S4(tk[KC/2 - 1], S);
			for (j = KC/2 + 1; j < KC; j++)
				tk[j] ^= tk[j-1];
		}
		for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
			W[t / BC][t % BC] = tk[j];
	}

	return 1;
}

//================================================================
int AESF_encrypt_block_bl_128 (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	int ROUNDS;
	int r;
	u8 * pa = (u8 *) a;
	u8 * pb = (u8 *) b;

	switch (kl) {
	case 16: ROUNDS = 10; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	DEBUG_BLOCK("e in", a, 4);

	a[0] ^= rk[0][0];
	a[1] ^= rk[0][1];
	a[2] ^= rk[0][2];
	a[3] ^= rk[0][3];

	DEBUG_BLOCK("e rk0", rk[0], 4);
	DEBUG_BLOCK("e ka", a, 4);

	for (r = 1; r < ROUNDS; r++) {
		DEBUG_BLOCK("e ri", a, 4);

		// Shift row:
		// b00 b01 b02 b03   a00 a01 a02 a03
		// b10 b11 b12 b13 = a11 a12 a13 a10
		// b20 b21 b22 b23   a22 a23 a20 a21
		// b30 b31 b32 b33   a33 a30 a31 a32

		// Column 0 indices: 00, 11, 22, 33
		// Column 1 indices: 01, 12, 23, 30
		// Column 2 indices: 02, 13, 20, 31
		// Column 3 indices: 03, 10, 21, 32

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00 | a01 | a02 | a03   3  7 11 15
		// a10 | a11 | a12 | a13   2  6 10 14
		// a20 | a21 | a22 | a23   1  5  9 13
		// a30 | a31 | a32 | a33   0  4  8 12

		// Column 0 indices:  3,  6,  9, 12
		// Column 1 indices:  7, 10, 13,  0
		// Column 2 indices: 11, 14,  1,  4
		// Column 3 indices: 15,  2,  5,  8

		b[0] = T0[pa[ 3]] ^ T1[pa[ 6]] ^ T2[pa[ 9]] ^ T3[pa[12]];
		b[1] = T0[pa[ 7]] ^ T1[pa[10]] ^ T2[pa[13]] ^ T3[pa[ 0]];
		b[2] = T0[pa[11]] ^ T1[pa[14]] ^ T2[pa[ 1]] ^ T3[pa[ 4]];
		b[3] = T0[pa[15]] ^ T1[pa[ 2]] ^ T2[pa[ 5]] ^ T3[pa[ 8]];

		DEBUG_BLOCK("e T", b, 4);

		a[0] = b[0] ^ rk[r][0];
		a[1] = b[1] ^ rk[r][1];
		a[2] = b[2] ^ rk[r][2];
		a[3] = b[3] ^ rk[r][3];

		DEBUG_BLOCK("e rkr", rk[r], 4);
		DEBUG_BLOCK("e ro", a, 4);
		DEBUG_TEXT("--------------------------------\n");
	}

	S4(a[0], a[0]);
	S4(a[1], a[1]);
	S4(a[2], a[2]);
	S4(a[3], a[3]);

	// Shift row:
	// b00 b01 b02 b03   a00 a01 a02 a03
	// b10 b11 b12 b13 = a11 a12 a13 a10
	// b20 b21 b22 b23   a22 a23 a20 a21
	// b30 b31 b32 b33   a33 a30 a31 a32

	//                        Byte offsets
	//                     !!!LITTLE ENDIAN!!!
	// a00 | a01 | a02 | a03   3  7 11 15
	// a10 | a11 | a12 | a13   2  6 10 14
	// a20 | a21 | a22 | a23   1  5  9 13
	// a30 | a31 | a32 | a33   0  4  8 12

	// Shift row:
	// pb 3 pb 7 pb11 pb15   pa 3 pa 7 pa11 pa15
	// pb 2 pb 6 pb10 pb14 = pa 6 pa10 pa14 pa 2
	// pb 1 pb 5 pb 9 pb13   pa 9 pa13 pa 1 pa 5
	// pb 0 pb 4 pb 8 pb12   pa12 pa 0 pa 4 pa 8

	pb[ 3] = pa[ 3]; pb[ 7] = pa[ 7]; pb[11] = pa[11]; pb[15] = pa[15];
	pb[ 2] = pa[ 6]; pb[ 6] = pa[10]; pb[10] = pa[14]; pb[14] = pa[ 2];
	pb[ 1] = pa[ 9]; pb[ 5] = pa[13]; pb[ 9] = pa[ 1]; pb[13] = pa[ 5];
	pb[ 0] = pa[12]; pb[ 4] = pa[ 0]; pb[ 8] = pa[ 4]; pb[12] = pa[ 8];

	DEBUG_BLOCK("e sr", b, 4);

	b[0] ^= rk[ROUNDS][0];
	b[1] ^= rk[ROUNDS][1];
	b[2] ^= rk[ROUNDS][2];
	b[3] ^= rk[ROUNDS][3];
	DEBUG_BLOCK("e rkl", rk[ROUNDS], 4);
	DEBUG_BLOCK("e out", b, 4);

	DEBUG_SEP;
	return 1;
}

//----------------------------------------------------------------
int AESF_encrypt_block_bl_192 (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	int ROUNDS;
	int r;
	u8 * pa = (u8 *) a;
	u8 * pb = (u8 *) b;

	switch (kl) {
	case 16: ROUNDS = 12; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	DEBUG_BLOCK("e in", a, 6);

	a[0] ^= rk[0][0];
	a[1] ^= rk[0][1];
	a[2] ^= rk[0][2];
	a[3] ^= rk[0][3];
	a[4] ^= rk[0][4];
	a[5] ^= rk[0][5];

	DEBUG_BLOCK("e rk0", rk[0], 6);
	DEBUG_BLOCK("e ka", a, 6);

	for (r = 1; r < ROUNDS; r++) {
		DEBUG_BLOCK("e ri", a, 6);

		// Shift row:
		// b00 b01 b02 b03 b04 b05   a00 a01 a02 a03 a04 a05
		// b10 b11 b12 b13 b14 b15 = a11 a12 a13 a14 a15 a10
		// b20 b21 b22 b23 b24 b25   a22 a23 a24 a25 a20 a21
		// b30 b31 b32 b33 b34 b35   a33 a34 a35 a30 a31 a32

		// Column 0 indices: 00, 11, 22, 33
		// Column 1 indices: 01, 12, 23, 34
		// Column 2 indices: 02, 13, 24, 35
		// Column 3 indices: 03, 14, 25, 30
		// Column 4 indices: 04, 15, 20, 31
		// Column 5 indices: 05, 10, 21, 32

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00 | a01 | a02 | a03 | a04 | a05   3  7 11 15 19 23
		// a10 | a11 | a12 | a13 | a14 | a15   2  6 10 14 18 22
		// a20 | a21 | a22 | a23 | a24 | a25   1  5  9 13 17 21
		// a30 | a31 | a32 | a33 | a34 | a35   0  4  8 12 16 20

		// Column 0 indices:  3,  6,  9, 12
		// Column 1 indices:  7, 10, 13, 16
		// Column 2 indices: 11, 14, 17, 20
		// Column 3 indices: 15, 18, 21,  0
		// Column 4 indices: 19, 22,  1,  4
		// Column 5 indices: 23,  2,  5,  8

		b[0] = T0[pa[ 3]] ^ T1[pa[ 6]] ^ T2[pa[ 9]] ^ T3[pa[12]];
		b[1] = T0[pa[ 7]] ^ T1[pa[10]] ^ T2[pa[13]] ^ T3[pa[16]];
		b[2] = T0[pa[11]] ^ T1[pa[14]] ^ T2[pa[17]] ^ T3[pa[20]];
		b[3] = T0[pa[15]] ^ T1[pa[18]] ^ T2[pa[21]] ^ T3[pa[ 0]];
		b[4] = T0[pa[19]] ^ T1[pa[22]] ^ T2[pa[ 1]] ^ T3[pa[ 4]];
		b[5] = T0[pa[23]] ^ T1[pa[ 2]] ^ T2[pa[ 5]] ^ T3[pa[ 8]];

		DEBUG_BLOCK("e T", b, 6);

		a[0] = b[0] ^ rk[r][0];
		a[1] = b[1] ^ rk[r][1];
		a[2] = b[2] ^ rk[r][2];
		a[3] = b[3] ^ rk[r][3];
		a[4] = b[4] ^ rk[r][4];
		a[5] = b[5] ^ rk[r][5];

		DEBUG_BLOCK("e rkr", rk[r], 6);
		DEBUG_BLOCK("e ro", a, 6);
		DEBUG_TEXT("--------------------------------\n");
	}

	S4(a[0], a[0]);
	S4(a[1], a[1]);
	S4(a[2], a[2]);
	S4(a[3], a[3]);
	S4(a[4], a[4]);
	S4(a[5], a[5]);

	// Shift row:
	// b00 b01 b02 b03 b04 b05   a00 a01 a02 a03 a04 a05
	// b10 b11 b12 b13 b14 b15 = a11 a12 a13 a14 a15 a10
	// b20 b21 b22 b23 b24 b25   a22 a23 a24 a25 a20 a21
	// b30 b31 b32 b33 b34 b35   a33 a34 a35 a30 a31 a32

	//                        Byte offsets
	//                     !!!LITTLE ENDIAN!!!
	// a00 | a01 | a02 | a03 | a04 | a05   3  7 11 15 19 23
	// a10 | a11 | a12 | a13 | a14 | a15   2  6 10 14 18 22
	// a20 | a21 | a22 | a23 | a24 | a25   1  5  9 13 17 21
	// a30 | a31 | a32 | a33 | a34 | a35   0  4  8 12 16 20

	// Shift row:
	// pb 3 pb 7 pb11 pb15 pb19 pb23   pa 3 pa 7 pa11 pa15 pa19 pa23
	// pb 2 pb 6 pb10 pb14 pb18 pb22 = pa 6 pa10 pa14 pa18 pa22 pa 2
	// pb 1 pb 5 pb 9 pb13 pb17 pb21   pa 9 pa13 pa17 pa21 pa 1 pa 5
	// pb 0 pb 4 pb 8 pb12 pb16 pb20   pa12 pa16 pa20 pa 0 pa 4 pa 8

	pb[ 3] = pa[ 3]; pb[ 2] = pa[ 6]; pb[ 1] = pa[ 9]; pb[ 0] = pa[12];
	pb[ 7] = pa[ 7]; pb[ 6] = pa[10]; pb[ 5] = pa[13]; pb[ 4] = pa[16];
	pb[11] = pa[11]; pb[10] = pa[14]; pb[ 9] = pa[17]; pb[ 8] = pa[20];
	pb[15] = pa[15]; pb[14] = pa[18]; pb[13] = pa[21]; pb[12] = pa[ 0];
	pb[19] = pa[19]; pb[18] = pa[22]; pb[17] = pa[ 1]; pb[16] = pa[ 4];
	pb[23] = pa[23]; pb[22] = pa[ 2]; pb[21] = pa[ 5]; pb[20] = pa[ 8];

	DEBUG_BLOCK("e sr", b, 6);

	b[0] ^= rk[ROUNDS][0];
	b[1] ^= rk[ROUNDS][1];
	b[2] ^= rk[ROUNDS][2];
	b[3] ^= rk[ROUNDS][3];
	b[4] ^= rk[ROUNDS][4];
	b[5] ^= rk[ROUNDS][5];
	DEBUG_BLOCK("e rkl", rk[ROUNDS], 6);
	DEBUG_BLOCK("e out", b, 6);

	DEBUG_SEP;
	return 1;
}

//----------------------------------------------------------------
int AESF_encrypt_block_bl_256 (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	int ROUNDS = 14;
	int r;
	u8 * pa = (u8 *) a;
	u8 * pb = (u8 *) b;

	DEBUG_BLOCK("e in", a, 8);

	a[0] ^= rk[0][0];
	a[1] ^= rk[0][1];
	a[2] ^= rk[0][2];
	a[3] ^= rk[0][3];
	a[4] ^= rk[0][4];
	a[5] ^= rk[0][5];
	a[6] ^= rk[0][6];
	a[7] ^= rk[0][7];

	DEBUG_BLOCK("e rk0", rk[0], 8);
	DEBUG_BLOCK("e ka", a, 8);

	for (r = 1; r < ROUNDS; r++) {
		DEBUG_BLOCK("e ri", a, 8);

	// Shift row:

	// b00 b01 b02 b03 b04 b05 b06 b07   a00 a01 a02 a03 a04 a05 a06 a07
	// b10 b11 b12 b13 b14 b15 b16 b17   a11 a12 a13 a14 a15 a16 a17 a10
	// b20 b21 b22 b23 b24 b25 b26 b27   a23 a24 a25 a26 a27 a20 a21 a22
	// b30 b31 b32 b33 b34 b35 b36 b37   a34 a35 a36 a37 a30 a31 a32 a33

		// Column 0 indices: 00, 11, 23, 34
		// Column 1 indices: 01, 12, 24, 35
		// Column 2 indices: 02, 13, 25, 36
		// Column 3 indices: 03, 14, 26, 37
		// Column 4 indices: 04, 15, 27, 30
		// Column 5 indices: 05, 16, 20, 31
		// Column 6 indices: 06, 17, 21, 32
		// Column 7 indices: 07, 10, 22, 33

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00|a01|a02|a03|a04|a05|a06|a07   3  7 11 15 19 23 27 31
		// a10|a11|a12|a13|a14|a15|a16|a17   2  6 10 14 18 22 26 30
		// a20|a21|a22|a23|a24|a25|a26|a27   1  5  9 13 17 21 25 29
		// a30|a31|a32|a33|a34|a35|a36|a37   0  4  8 12 16 20 24 28

		// Column 0 indices:  3,  6, 13, 16
		// Column 1 indices:  7, 10, 17, 20
		// Column 2 indices: 11, 14, 21, 24
		// Column 3 indices: 15, 18, 25, 28
		// Column 4 indices: 19, 22, 29,  0
		// Column 5 indices: 23, 26,  1,  4
		// Column 6 indices: 27, 30,  5,  8
		// Column 7 indices: 31,  2,  9, 12

		b[0] = T0[pa[ 3]] ^ T1[pa[ 6]] ^ T2[pa[13]] ^ T3[pa[16]];
		b[1] = T0[pa[ 7]] ^ T1[pa[10]] ^ T2[pa[17]] ^ T3[pa[20]];
		b[2] = T0[pa[11]] ^ T1[pa[14]] ^ T2[pa[21]] ^ T3[pa[24]];
		b[3] = T0[pa[15]] ^ T1[pa[18]] ^ T2[pa[25]] ^ T3[pa[28]];
		b[4] = T0[pa[19]] ^ T1[pa[22]] ^ T2[pa[29]] ^ T3[pa[ 0]];
		b[5] = T0[pa[23]] ^ T1[pa[26]] ^ T2[pa[ 1]] ^ T3[pa[ 4]];
		b[6] = T0[pa[27]] ^ T1[pa[30]] ^ T2[pa[ 5]] ^ T3[pa[ 8]];
		b[7] = T0[pa[31]] ^ T1[pa[ 2]] ^ T2[pa[ 9]] ^ T3[pa[12]];

		DEBUG_BLOCK("e T", b, 8);

		a[0] = b[0] ^ rk[r][0];
		a[1] = b[1] ^ rk[r][1];
		a[2] = b[2] ^ rk[r][2];
		a[3] = b[3] ^ rk[r][3];
		a[4] = b[4] ^ rk[r][4];
		a[5] = b[5] ^ rk[r][5];
		a[6] = b[6] ^ rk[r][6];
		a[7] = b[7] ^ rk[r][7];

		DEBUG_BLOCK("e rkr", rk[r], 8);
		DEBUG_BLOCK("e ro", a, 8);
		DEBUG_TEXT("--------------------------------\n");
	}

	S4(a[0], a[0]);
	S4(a[1], a[1]);
	S4(a[2], a[2]);
	S4(a[3], a[3]);
	S4(a[4], a[4]);
	S4(a[5], a[5]);
	S4(a[6], a[6]);
	S4(a[7], a[7]);

	// b00 b01 b02 b03 b04 b05 b06 b07   a00 a01 a02 a03 a04 a05 a06 a07
	// b10 b11 b12 b13 b14 b15 b16 b17   a11 a12 a13 a14 a15 a16 a17 a10
	// b20 b21 b22 b23 b24 b25 b26 b27   a23 a24 a25 a26 a27 a20 a21 a22
	// b30 b31 b32 b33 b34 b35 b36 b37   a34 a35 a36 a37 a30 a31 a32 a33

	//                        Byte offsets
	//                     !!!LITTLE ENDIAN!!!
	// a00|a01|a02|a03|a04|a05|a06|a07   3  7 11 15 19 23 27 31
	// a10|a11|a12|a13|a14|a15|a16|a17   2  6 10 14 18 22 26 30
	// a20|a21|a22|a23|a24|a25|a26|a27   1  5  9 13 17 21 25 29
	// a30|a31|a32|a33|a34|a35|a36|a37   0  4  8 12 16 20 24 28

	pb[ 3] = pa[ 3]; pb[ 2] = pa[ 6]; pb[ 1] = pa[13]; pb[ 0] = pa[16];
	pb[ 7] = pa[ 7]; pb[ 6] = pa[10]; pb[ 5] = pa[17]; pb[ 4] = pa[20];
	pb[11] = pa[11]; pb[10] = pa[14]; pb[ 9] = pa[21]; pb[ 8] = pa[24];
	pb[15] = pa[15]; pb[14] = pa[18]; pb[13] = pa[25]; pb[12] = pa[28];
	pb[19] = pa[19]; pb[18] = pa[22]; pb[17] = pa[29]; pb[16] = pa[ 0];
	pb[23] = pa[23]; pb[22] = pa[26]; pb[21] = pa[ 1]; pb[20] = pa[ 4];
	pb[27] = pa[27]; pb[26] = pa[30]; pb[25] = pa[ 5]; pb[24] = pa[ 8];
	pb[31] = pa[31]; pb[30] = pa[ 2]; pb[29] = pa[ 9]; pb[28] = pa[12];

	DEBUG_BLOCK("e sr", b, 8);

	b[0] ^= rk[ROUNDS][0];
	b[1] ^= rk[ROUNDS][1];
	b[2] ^= rk[ROUNDS][2];
	b[3] ^= rk[ROUNDS][3];
	b[4] ^= rk[ROUNDS][4];
	b[5] ^= rk[ROUNDS][5];
	b[6] ^= rk[ROUNDS][6];
	b[7] ^= rk[ROUNDS][7];
	DEBUG_BLOCK("e rkl", rk[ROUNDS], 8);
	DEBUG_BLOCK("e out", b, 8);

	DEBUG_SEP;
	return 1;
}

//----------------------------------------------------------------
int AESF_encrypt_block (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	int bl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	if (bl == 16) return AESF_encrypt_block_bl_128(a, b, kl, rk);
	if (bl == 24) return AESF_encrypt_block_bl_192(a, b, kl, rk);
	if (bl == 32) return AESF_encrypt_block_bl_256(a, b, kl, rk);

	return -10;
}

//================================================================
int AESF_decrypt_block_bl_128 (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	int ROUNDS;
	int r;
	u8 * pa = (u8 *) a;
	u8 * pb = (u8 *) b;

	switch (kl) {
	case 16: ROUNDS = 10; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	DEBUG_BLOCK("d in", a, 4);

	a[0] ^= rk[ROUNDS][0];
	a[1] ^= rk[ROUNDS][1];
	a[2] ^= rk[ROUNDS][2];
	a[3] ^= rk[ROUNDS][3];

	DEBUG_BLOCK("d rkl", rk[ROUNDS], 4);
	DEBUG_BLOCK("d ka", a, 4);

	Si4(a[0], a[0]);
	Si4(a[1], a[1]);
	Si4(a[2], a[2]);
	Si4(a[3], a[3]);
	DEBUG_BLOCK("d su", a, 4);

	// Shift row:
	// b00 b01 b02 b03   a00 a01 a02 a03
	// b10 b11 b12 b13 = a13 a10 a11 a12
	// b20 b21 b22 b23   a22 a23 a20 a21
	// b30 b31 b32 b33   a31 a32 a33 a30

	//                        Byte offsets
	//                     !!!LITTLE ENDIAN!!!
	// a00 | a01 | a02 | a03   3  7 11 15
	// a10 | a11 | a12 | a13   2  6 10 14
	// a20 | a21 | a22 | a23   1  5  9 13
	// a30 | a31 | a32 | a33   0  4  8 12

	// Shift row:
	// pb 3 pb 7 pb11 pb15   pa 3 pa 7 pa11 pa15
	// pb 2 pb 6 pb10 pb14 = pa14 pa 2 pa 6 pa10
	// pb 1 pb 5 pb 9 pb13   pa 9 pa13 pa 1 pa 5
	// pb 0 pb 4 pb 8 pb12   pa 4 pa 8 pa12 pa 0

	pb[ 3] = pa[ 3]; pb[ 7] = pa[ 7]; pb[11] = pa[11]; pb[15] = pa[15];
	pb[ 2] = pa[14]; pb[ 6] = pa[ 2]; pb[10] = pa[ 6]; pb[14] = pa[10];
	pb[ 1] = pa[ 9]; pb[ 5] = pa[13]; pb[ 9] = pa[ 1]; pb[13] = pa[ 5];
	pb[ 0] = pa[ 4]; pb[ 4] = pa[ 8]; pb[ 8] = pa[12]; pb[12] = pa[ 0];

	DEBUG_BLOCK("d sr", b, 4);

	for (r = ROUNDS - 1; r >= 1; r--) {
		DEBUG_BLOCK("d ri", b, 4);

		a[0] = b[0] ^ rk[r][0];
		a[1] = b[1] ^ rk[r][1];
		a[2] = b[2] ^ rk[r][2];
		a[3] = b[3] ^ rk[r][3];

		DEBUG_BLOCK("d rkr", rk[r], 4);
		DEBUG_BLOCK("d ka", a, 4);

		// Column 0 indices: 00, 10, 20, 30
		// Column 1 indices: 01, 11, 21, 31
		// Column 2 indices: 02, 12, 22, 32
		// Column 3 indices: 03, 13, 23, 33

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00 | a01 | a02 | a03   3  7 11 15
		// a10 | a11 | a12 | a13   2  6 10 14
		// a20 | a21 | a22 | a23   1  5  9 13
		// a30 | a31 | a32 | a33   0  4  8 12

	//U tables:
	//S of:
	//e00^b10^d20^930 e01^b11^d21^931 e02^b12^d22^932 e03^b13^d23^933
	//900^e10^b20^d30 901^e11^b21^d31 902^e12^b22^d32 903^e13^b23^d33
	//d00^910^e20^b30 d01^911^e21^b31 d02^912^e22^b32 d03^913^e23^b33
	//b00^d10^920^e30 b01^d11^921^e31 b02^d12^922^e32 b03^d13^923^e33

	//S of:
	//e  ^b  ^d  ^9   e  ^b  ^d  ^9   e  ^b  ^d  ^9   e  ^b  ^d  ^9
	//900^e10^b20^d30 901^e11^b21^d31 902^e12^b22^d32 903^e13^b23^d33
	//d  ^9  ^e  ^b   d  ^9  ^e  ^b   d  ^9  ^e  ^b   d  ^9  ^e  ^b
	//b  ^d  ^9  ^e   b  ^d  ^9  ^e   b  ^d  ^9  ^e   b  ^d  ^9  ^e

	//S of:
	//U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3
	//  3^  2^  1^  0   7^  6^  5^  4  11^ 10^  9^  8  15^ 14^ 13^ 12
	// 00^ 10^ 20^ 30  01^ 11^ 21^ 31  02^ 12^ 22^ 32  03^ 13^ 23^ 33

		b[0] = U0[pa[ 3]] ^ U1[pa[ 2]] ^ U2[pa[ 1]] ^ U3[pa[ 0]];
		b[1] = U0[pa[ 7]] ^ U1[pa[ 6]] ^ U2[pa[ 5]] ^ U3[pa[ 4]];
		b[2] = U0[pa[11]] ^ U1[pa[10]] ^ U2[pa[ 9]] ^ U3[pa[ 8]];
		b[3] = U0[pa[15]] ^ U1[pa[14]] ^ U2[pa[13]] ^ U3[pa[12]];
		DEBUG_BLOCK("d U", b, 4);

		Si4(a[0], b[0]);
		Si4(a[1], b[1]);
		Si4(a[2], b[2]);
		Si4(a[3], b[3]);

		pb[ 3]=pa[ 3]; pb[ 7]=pa[ 7]; pb[11]=pa[11]; pb[15]=pa[15];
		pb[ 2]=pa[14]; pb[ 6]=pa[ 2]; pb[10]=pa[ 6]; pb[14]=pa[10];
		pb[ 1]=pa[ 9]; pb[ 5]=pa[13]; pb[ 9]=pa[ 1]; pb[13]=pa[ 5];
		pb[ 0]=pa[ 4]; pb[ 4]=pa[ 8]; pb[ 8]=pa[12]; pb[12]=pa[ 0];

		DEBUG_BLOCK("d sr", b, 4);

		DEBUG_TEXT("--------------------------------\n");
	}

	b[0] ^= rk[0][0];
	b[1] ^= rk[0][1];
	b[2] ^= rk[0][2];
	b[3] ^= rk[0][3];
	DEBUG_BLOCK("d rk0", rk[0], 4);
	DEBUG_BLOCK("d out", b, 4);

	DEBUG_SEP;
	return 1;
}

//----------------------------------------------------------------
int AESF_decrypt_block_bl_192 (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	int  ROUNDS;
	int  r;
	u8 * pa = (u8 *) a;
	u8 * pb = (u8 *) b;

	switch (kl) {
	case 16: ROUNDS = 12; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	DEBUG_BLOCK("d in", a, 6);

	a[0] ^= rk[ROUNDS][0];
	a[1] ^= rk[ROUNDS][1];
	a[2] ^= rk[ROUNDS][2];
	a[3] ^= rk[ROUNDS][3];
	a[4] ^= rk[ROUNDS][4];
	a[5] ^= rk[ROUNDS][5];

	DEBUG_BLOCK("d rkl", rk[ROUNDS], 6);
	DEBUG_BLOCK("d ka", a, 6);

	Si4(a[0], a[0]);
	Si4(a[1], a[1]);
	Si4(a[2], a[2]);
	Si4(a[3], a[3]);
	Si4(a[4], a[4]);
	Si4(a[5], a[5]);
	DEBUG_BLOCK("d su", a, 6);

	// Shift row:
	// b00 b01 b02 b03 b04 b05   a00 a01 a02 a03 a04 a05
	// b10 b11 b12 b13 b14 b15 = a13 a10 a11 a12 a14 a15
	// b20 b21 b22 b23 b24 b25   a22 a23 a20 a21 a24 a25
	// b30 b31 b32 b33 b34 b35   a31 a32 a33 a30 a34 a35

	//                        Byte offsets
	//                     !!!LITTLE ENDIAN!!!
	// a00 | a01 | a02 | a03 | a04 | a05   3  7 11 15 19 23
	// a10 | a11 | a12 | a13 | a14 | a15   2  6 10 14 18 22
	// a20 | a21 | a22 | a23 | a24 | a25   1  5  9 13 17 21
	// a30 | a31 | a32 | a33 | a34 | a35   0  4  8 12 16 20

	// Shift row:
	// pb 3 pb 7 pb11 pb15 pb19 pb23   pa 3 pa 7 pa11 pa15 pa19 pa23
	// pb 2 pb 6 pb10 pb14 pb18 pb22 = pa22 pa 2 pa 6 pa10 pa14 pa18
	// pb 1 pb 5 pb 9 pb13 pb17 pb21   pa17 pa21 pa 1 pa 5 pa 9 pa13
	// pb 0 pb 4 pb 8 pb12 pb16 pb20   pa12 pa16 pa20 pa 0 pa 4 pa 8

	pb[ 3] = pa[ 3]; pb[ 2] = pa[22]; pb[ 1] = pa[17]; pb[ 0] = pa[12];
	pb[ 7] = pa[ 7]; pb[ 6] = pa[ 2]; pb[ 5] = pa[21]; pb[ 4] = pa[16];
	pb[11] = pa[11]; pb[10] = pa[ 6]; pb[ 9] = pa[ 1]; pb[ 8] = pa[20];
	pb[15] = pa[15]; pb[14] = pa[10]; pb[13] = pa[ 5]; pb[12] = pa[ 0];
	pb[19] = pa[19]; pb[18] = pa[14]; pb[17] = pa[ 9]; pb[16] = pa[ 4];
	pb[23] = pa[23]; pb[22] = pa[18]; pb[21] = pa[13]; pb[20] = pa[ 8];

	DEBUG_BLOCK("d sr", b, 6);

	for (r = ROUNDS - 1; r >= 1; r--) {
		DEBUG_BLOCK("d ri", b, 6);

		a[0] = b[0] ^ rk[r][0];
		a[1] = b[1] ^ rk[r][1];
		a[2] = b[2] ^ rk[r][2];
		a[3] = b[3] ^ rk[r][3];
		a[4] = b[4] ^ rk[r][4];
		a[5] = b[5] ^ rk[r][5];

		DEBUG_BLOCK("d rkr", rk[r], 6);
		DEBUG_BLOCK("d ka", a, 6);

		// Column 0 indices: 00, 10, 20, 30
		// Column 1 indices: 01, 11, 21, 31
		// Column 2 indices: 02, 12, 22, 32
		// Column 3 indices: 03, 13, 23, 33

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00 | a01 | a02 | a03   3  7 11 15
		// a10 | a11 | a12 | a13   2  6 10 14
		// a20 | a21 | a22 | a23   1  5  9 13
		// a30 | a31 | a32 | a33   0  4  8 12

	//U tables:
	//S of:
	//e00^b10^d20^930 e01^b11^d21^931 e02^b12^d22^932 e03^b13^d23^933
	//900^e10^b20^d30 901^e11^b21^d31 902^e12^b22^d32 903^e13^b23^d33
	//d00^910^e20^b30 d01^911^e21^b31 d02^912^e22^b32 d03^913^e23^b33
	//b00^d10^920^e30 b01^d11^921^e31 b02^d12^922^e32 b03^d13^923^e33

	//S of:
	//e  ^b  ^d  ^9   e  ^b  ^d  ^9   e  ^b  ^d  ^9   e  ^b  ^d  ^9
	//900^e10^b20^d30 901^e11^b21^d31 902^e12^b22^d32 903^e13^b23^d33
	//d  ^9  ^e  ^b   d  ^9  ^e  ^b   d  ^9  ^e  ^b   d  ^9  ^e  ^b
	//b  ^d  ^9  ^e   b  ^d  ^9  ^e   b  ^d  ^9  ^e   b  ^d  ^9  ^e

	//S of:
	//U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3
	//  3^  2^  1^  0   7^  6^  5^  4  11^ 10^  9^  8  15^ 14^ 13^ 12
	// 00^ 10^ 20^ 30  01^ 11^ 21^ 31  02^ 12^ 22^ 32  03^ 13^ 23^ 33

		b[0] = U0[pa[ 3]] ^ U1[pa[ 2]] ^ U2[pa[ 1]] ^ U3[pa[ 0]];
		b[1] = U0[pa[ 7]] ^ U1[pa[ 6]] ^ U2[pa[ 5]] ^ U3[pa[ 4]];
		b[2] = U0[pa[11]] ^ U1[pa[10]] ^ U2[pa[ 9]] ^ U3[pa[ 8]];
		b[3] = U0[pa[15]] ^ U1[pa[14]] ^ U2[pa[13]] ^ U3[pa[12]];
		b[4] = U0[pa[19]] ^ U1[pa[18]] ^ U2[pa[17]] ^ U3[pa[16]];
		b[5] = U0[pa[23]] ^ U1[pa[22]] ^ U2[pa[21]] ^ U3[pa[20]];
		DEBUG_BLOCK("d U", b, 6);

		Si4(a[0], b[0]);
		Si4(a[1], b[1]);
		Si4(a[2], b[2]);
		Si4(a[3], b[3]);
		Si4(a[4], b[4]);
		Si4(a[5], b[5]);

		pb[ 3]=pa[ 3];pb[ 2]=pa[22];pb[ 1]=pa[17];pb[ 0]=pa[12];
		pb[ 7]=pa[ 7];pb[ 6]=pa[ 2];pb[ 5]=pa[21];pb[ 4]=pa[16];
		pb[11]=pa[11];pb[10]=pa[ 6];pb[ 9]=pa[ 1];pb[ 8]=pa[20];
		pb[15]=pa[15];pb[14]=pa[10];pb[13]=pa[ 5];pb[12]=pa[ 0];
		pb[19]=pa[19];pb[18]=pa[14];pb[17]=pa[ 9];pb[16]=pa[ 4];
		pb[23]=pa[23];pb[22]=pa[18];pb[21]=pa[13];pb[20]=pa[ 8];

		DEBUG_BLOCK("d sr", b, 6);

		DEBUG_TEXT("--------------------------------\n");
	}

	b[0] ^= rk[0][0];
	b[1] ^= rk[0][1];
	b[2] ^= rk[0][2];
	b[3] ^= rk[0][3];
	b[4] ^= rk[0][4];
	b[5] ^= rk[0][5];
	DEBUG_BLOCK("d rk0", rk[0], 6);
	DEBUG_BLOCK("d out", b, 6);

	DEBUG_SEP;
	return 1;
}

//----------------------------------------------------------------
int AESF_decrypt_block_bl_256 (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	int  ROUNDS;
	int  r;
	u8 * pa = (u8 *) a;
	u8 * pb = (u8 *) b;

	ROUNDS = 14;

	DEBUG_BLOCK("d in", a, 8);

	a[0] ^= rk[ROUNDS][0];
	a[1] ^= rk[ROUNDS][1];
	a[2] ^= rk[ROUNDS][2];
	a[3] ^= rk[ROUNDS][3];
	a[4] ^= rk[ROUNDS][4];
	a[5] ^= rk[ROUNDS][5];
	a[6] ^= rk[ROUNDS][6];
	a[7] ^= rk[ROUNDS][7];

	DEBUG_BLOCK("d rkl", rk[ROUNDS], 8);
	DEBUG_BLOCK("d ka", a, 8);

	Si4(a[0], a[0]);
	Si4(a[1], a[1]);
	Si4(a[2], a[2]);
	Si4(a[3], a[3]);
	Si4(a[4], a[4]);
	Si4(a[5], a[5]);
	Si4(a[6], a[6]);
	Si4(a[7], a[7]);
	DEBUG_BLOCK("d su", a, 8);

	// Shift row:

	// b00 b01 b02 b03 b04 b05 b06 b07   a00 a01 a02 a03 a04 a05 a06 a07
	// b10 b11 b12 b13 b14 b15 b16 b17   a17 a10 a11 a12 a13 a14 a15 a16
	// b20 b21 b22 b23 b24 b25 b26 b27   a25 a26 a27 a20 a21 a22 a23 a24
	// b30 b31 b32 b33 b34 b35 b36 b37   a34 a35 a36 a37 a30 a31 a32 a33

		// Column 0 indices: 00, 17, 25, 34
		// Column 1 indices: 01, 10, 26, 35
		// Column 2 indices: 02, 11, 27, 36
		// Column 3 indices: 03, 12, 20, 37
		// Column 4 indices: 04, 13, 21, 30
		// Column 5 indices: 05, 14, 22, 31
		// Column 6 indices: 06, 15, 23, 32
		// Column 7 indices: 07, 16, 24, 33

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00|a01|a02|a03|a04|a05|a06|a07   3  7 11 15 19 23 27 31
		// a10|a11|a12|a13|a14|a15|a16|a17   2  6 10 14 18 22 26 30
		// a20|a21|a22|a23|a24|a25|a26|a27   1  5  9 13 17 21 25 29
		// a30|a31|a32|a33|a34|a35|a36|a37   0  4  8 12 16 20 24 28

		// Column 0 indices:  3, 30, 21, 16
		// Column 1 indices:  7,  2, 25, 20
		// Column 2 indices: 11,  6, 29, 24
		// Column 3 indices: 15, 10,  1, 28
		// Column 4 indices: 19, 14,  5,  0
		// Column 5 indices: 23, 18,  9,  4
		// Column 6 indices: 27, 22, 13,  8
		// Column 7 indices: 31, 26, 17, 12

	pb[ 3] = pa[ 3]; pb[ 2] = pa[30]; pb[ 1] = pa[21]; pb[ 0] = pa[16];
	pb[ 7] = pa[ 7]; pb[ 6] = pa[ 2]; pb[ 5] = pa[25]; pb[ 4] = pa[20];
	pb[11] = pa[11]; pb[10] = pa[ 6]; pb[ 9] = pa[29]; pb[ 8] = pa[24];
	pb[15] = pa[15]; pb[14] = pa[10]; pb[13] = pa[ 1]; pb[12] = pa[28];
	pb[19] = pa[19]; pb[18] = pa[14]; pb[17] = pa[ 5]; pb[16] = pa[ 0];
	pb[23] = pa[23]; pb[22] = pa[18]; pb[21] = pa[ 9]; pb[20] = pa[ 4];
	pb[27] = pa[27]; pb[26] = pa[22]; pb[25] = pa[13]; pb[24] = pa[ 8];
	pb[31] = pa[31]; pb[30] = pa[26]; pb[29] = pa[17]; pb[28] = pa[12];

	DEBUG_BLOCK("d sr", b, 8);

	for (r = ROUNDS - 1; r >= 1; r--) {
		DEBUG_BLOCK("d ri", b, 8);

		a[0] = b[0] ^ rk[r][0];
		a[1] = b[1] ^ rk[r][1];
		a[2] = b[2] ^ rk[r][2];
		a[3] = b[3] ^ rk[r][3];
		a[4] = b[4] ^ rk[r][4];
		a[5] = b[5] ^ rk[r][5];
		a[6] = b[6] ^ rk[r][6];
		a[7] = b[7] ^ rk[r][7];

		DEBUG_BLOCK("d rkr", rk[r], 8);
		DEBUG_BLOCK("d ka", a, 8);

		// Column 0 indices: 00, 10, 20, 30
		// Column 1 indices: 01, 11, 21, 31
		// Column 2 indices: 02, 12, 22, 32
		// Column 3 indices: 03, 13, 23, 33

		//                        Byte offsets
		//                     !!!LITTLE ENDIAN!!!
		// a00 | a01 | a02 | a03   3  7 11 15
		// a10 | a11 | a12 | a13   2  6 10 14
		// a20 | a21 | a22 | a23   1  5  9 13
		// a30 | a31 | a32 | a33   0  4  8 12

	//U tables:
	//S of:
	//e00^b10^d20^930 e01^b11^d21^931 e02^b12^d22^932 e03^b13^d23^933
	//900^e10^b20^d30 901^e11^b21^d31 902^e12^b22^d32 903^e13^b23^d33
	//d00^910^e20^b30 d01^911^e21^b31 d02^912^e22^b32 d03^913^e23^b33
	//b00^d10^920^e30 b01^d11^921^e31 b02^d12^922^e32 b03^d13^923^e33

	//S of:
	//e  ^b  ^d  ^9   e  ^b  ^d  ^9   e  ^b  ^d  ^9   e  ^b  ^d  ^9
	//900^e10^b20^d30 901^e11^b21^d31 902^e12^b22^d32 903^e13^b23^d33
	//d  ^9  ^e  ^b   d  ^9  ^e  ^b   d  ^9  ^e  ^b   d  ^9  ^e  ^b
	//b  ^d  ^9  ^e   b  ^d  ^9  ^e   b  ^d  ^9  ^e   b  ^d  ^9  ^e

	//S of:
	//U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3  U0 ^U1 ^U2 ^U3
	//  3^  2^  1^  0   7^  6^  5^  4  11^ 10^  9^  8  15^ 14^ 13^ 12
	// 00^ 10^ 20^ 30  01^ 11^ 21^ 31  02^ 12^ 22^ 32  03^ 13^ 23^ 33

		b[0] = U0[pa[ 3]] ^ U1[pa[ 2]] ^ U2[pa[ 1]] ^ U3[pa[ 0]];
		b[1] = U0[pa[ 7]] ^ U1[pa[ 6]] ^ U2[pa[ 5]] ^ U3[pa[ 4]];
		b[2] = U0[pa[11]] ^ U1[pa[10]] ^ U2[pa[ 9]] ^ U3[pa[ 8]];
		b[3] = U0[pa[15]] ^ U1[pa[14]] ^ U2[pa[13]] ^ U3[pa[12]];
		b[4] = U0[pa[19]] ^ U1[pa[18]] ^ U2[pa[17]] ^ U3[pa[16]];
		b[5] = U0[pa[23]] ^ U1[pa[22]] ^ U2[pa[21]] ^ U3[pa[20]];
		b[6] = U0[pa[27]] ^ U1[pa[26]] ^ U2[pa[25]] ^ U3[pa[24]];
		b[7] = U0[pa[31]] ^ U1[pa[30]] ^ U2[pa[29]] ^ U3[pa[28]];
		DEBUG_BLOCK("d U", b, 8);

		Si4(a[0], b[0]);
		Si4(a[1], b[1]);
		Si4(a[2], b[2]);
		Si4(a[3], b[3]);
		Si4(a[4], b[4]);
		Si4(a[5], b[5]);
		Si4(a[6], b[6]);
		Si4(a[7], b[7]);

		pb[ 3]=pa[ 3];pb[ 2]=pa[30];pb[ 1]=pa[21];pb[ 0]=pa[16];
		pb[ 7]=pa[ 7];pb[ 6]=pa[ 2];pb[ 5]=pa[25];pb[ 4]=pa[20];
		pb[11]=pa[11];pb[10]=pa[ 6];pb[ 9]=pa[29];pb[ 8]=pa[24];
		pb[15]=pa[15];pb[14]=pa[10];pb[13]=pa[ 1];pb[12]=pa[28];
		pb[19]=pa[19];pb[18]=pa[14];pb[17]=pa[ 5];pb[16]=pa[ 0];
		pb[23]=pa[23];pb[22]=pa[18];pb[21]=pa[ 9];pb[20]=pa[ 4];
		pb[27]=pa[27];pb[26]=pa[22];pb[25]=pa[13];pb[24]=pa[ 8];
		pb[31]=pa[31];pb[30]=pa[26];pb[29]=pa[17];pb[28]=pa[12];

		DEBUG_BLOCK("d sr", b, 8);

		DEBUG_TEXT("--------------------------------\n");
	}

	b[0] ^= rk[0][0];
	b[1] ^= rk[0][1];
	b[2] ^= rk[0][2];
	b[3] ^= rk[0][3];
	b[4] ^= rk[0][4];
	b[5] ^= rk[0][5];
	b[6] ^= rk[0][6];
	b[7] ^= rk[0][7];
	DEBUG_BLOCK("d rk0", rk[0], 8);
	DEBUG_BLOCK("d out", b, 8);

	DEBUG_SEP;
	return 1;
}
//----------------------------------------------------------------
int AESF_decrypt_block (
	u32 a[MAXBC],
	u32 b[MAXBC],
	int kl,
	int bl,
	u32 rk[MAXROUNDS+1][MAXKC])
{
	if (bl == 16) return AESF_decrypt_block_bl_128(a, b, kl, rk);
	if (bl == 24) return AESF_decrypt_block_bl_192(a, b, kl, rk);
	if (bl == 32) return AESF_decrypt_block_bl_256(a, b, kl, rk);

	return -10;
}
