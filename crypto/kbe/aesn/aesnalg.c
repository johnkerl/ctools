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
#include <string.h>
#include <ctype.h>
#include "aesnalg.h"
#include "util.h"
#include "blkcipht.h"

#include "boxes-ref.dat"

static void pack4(
	word32 * pw,
	word8    b0,
	word8    b1,
	word8    b2,
	word8    b3)
{
	*pw = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

static void unpack4(
	word32   w,
	word8  * pb0,
	word8  * pb1,
	word8  * pb2,
	word8  * pb3)
{
	*pb0 = w >> 24;
	*pb1 = w >> 16;
	*pb2 = w >>  8;
	*pb3 = w >>  0;
}


// // ----------------------------------------------------------------
// // XOR corresponding text input and round key input bytes
// 
// static void key_addition(
// 	word8 * a,
// 	word8 * rk,
// 	int   BC)
// {
// 	a[ 0]^=rk[ 0];a[ 1]^=rk[ 1];a[ 2]^=rk[ 2];a[ 3]^=rk[ 3];
// 	a[ 4]^=rk[ 4];a[ 5]^=rk[ 5];a[ 6]^=rk[ 6];a[ 7]^=rk[ 7];
// 	a[ 8]^=rk[ 8];a[ 9]^=rk[ 9];a[10]^=rk[10];a[11]^=rk[11];
// 	a[12]^=rk[12];a[13]^=rk[13];a[14]^=rk[14];a[15]^=rk[15];
// 	if (BC == 4)
// 		return;
// 	a[16]^=rk[16];a[17]^=rk[17];a[18]^=rk[18];a[19]^=rk[19];
// 	a[20]^=rk[20];a[21]^=rk[21];a[22]^=rk[22];a[23]^=rk[23];
// 	if (BC == 6)
// 		return;
// 	a[24]^=rk[24];a[25]^=rk[25];a[26]^=rk[26];a[27]^=rk[27];
// 	a[28]^=rk[28];a[29]^=rk[29];a[30]^=rk[30];a[31]^=rk[31];
// }
// 
// // ----------------------------------------------------------------
// // Row 0 remains unchanged.
// // The other three rows are shifted a variable amount.
// 
// static void shift_row_4(
// 	word8 * a,
// 	word8 * b,
// 	word8 d)
// {
// 	b[ 0] = a[ 0];
// 	b[ 4] = a[ 4];
// 	b[ 8] = a[ 8];
// 	b[12] = a[12];
// 
// 	if (d == 0) {
// 		b[ 1] = a[ 5];
// 		b[ 5] = a[ 9];
// 		b[ 9] = a[13];
// 		b[13] = a[ 1];
// 	} else {
// 		b[ 1] = a[13];
// 		b[ 5] = a[ 1];
// 		b[ 9] = a[ 5];
// 		b[13] = a[ 9];
// 	}
// 
// 	b[ 2] = a[10];
// 	b[ 6] = a[14];
// 	b[10] = a[ 2];
// 	b[14] = a[ 6];
// 
// 	if (d == 0) {
// 		b[ 3] = a[15];
// 		b[ 7] = a[ 3];
// 		b[11] = a[ 7];
// 		b[15] = a[11];
// 	} else {
// 		b[ 3] = a[ 7];
// 		b[ 7] = a[11];
// 		b[11] = a[15];
// 		b[15] = a[ 3];
// 	}
// }
// 
// static void shift_row_6(
// 	word8 * a,
// 	word8 * b,
// 	word8 d)
// {
// 	b[ 0] = a[ 0];
// 	b[ 4] = a[ 4];
// 	b[ 8] = a[ 8];
// 	b[12] = a[12];
// 	b[16] = a[16];
// 	b[20] = a[20];
// 
// 	if (d == 0) {
// 		b[ 1] = a[ 5];
// 		b[ 5] = a[ 9];
// 		b[ 9] = a[13];
// 		b[13] = a[17];
// 		b[17] = a[21];
// 		b[21] = a[ 1];
// 	} else {
// 		b[ 1] = a[21];
// 		b[ 5] = a[ 1];
// 		b[ 9] = a[ 5];
// 		b[13] = a[ 9];
// 		b[17] = a[13];
// 		b[21] = a[17];
// 	}
// 
// 	if (d == 0) {
// 		b[ 2] = a[10];
// 		b[ 6] = a[14];
// 		b[10] = a[18];
// 		b[14] = a[22];
// 		b[18] = a[ 2];
// 		b[22] = a[ 6];
// 	} else {
// 		b[ 2] = a[18];
// 		b[ 6] = a[22];
// 		b[10] = a[ 2];
// 		b[14] = a[ 6];
// 		b[18] = a[10];
// 		b[22] = a[14];
// 	}
// 
// 	b[ 3] = a[15];
// 	b[ 7] = a[19];
// 	b[11] = a[23];
// 	b[15] = a[ 3];
// 	b[19] = a[ 7];
// 	b[23] = a[11];
// }
// 
// static void shift_row_8(
// 	word8 * a,
// 	word8 * b,
// 	word8 d)
// {
// 	b[ 0]  = a[ 0];
// 	b[ 4]  = a[ 4];
// 	b[ 8]  = a[ 8];
// 	b[12]  = a[12];
// 	b[16]  = a[16];
// 	b[20]  = a[20];
// 	b[24]  = a[24];
// 	b[28]  = a[28];
// 
// 	if (d == 0) {
// 		b[ 1] = a[ 5];
// 		b[ 5] = a[ 9];
// 		b[ 9] = a[13];
// 		b[13] = a[17];
// 		b[17] = a[21];
// 		b[21] = a[25];
// 		b[25] = a[29];
// 		b[29] = a[ 1];
// 	} else {
// 		b[ 1] = a[29];
// 		b[ 5] = a[ 1];
// 		b[ 9] = a[ 5];
// 		b[13] = a[ 9];
// 		b[17] = a[13];
// 		b[21] = a[17];
// 		b[25] = a[21];
// 		b[29] = a[25];
// 	}
// 
// 	if (d == 0) {
// 		b[ 2] = a[14];
// 		b[ 6] = a[18];
// 		b[10] = a[22];
// 		b[14] = a[26];
// 		b[18] = a[30];
// 		b[22] = a[ 2];
// 		b[26] = a[ 6];
// 		b[30] = a[10];
// 	} else {
// 		b[ 2] = a[22];
// 		b[ 6] = a[26];
// 		b[10] = a[30];
// 		b[14] = a[ 2];
// 		b[18] = a[ 6];
// 		b[22] = a[10];
// 		b[26] = a[14];
// 		b[30] = a[18];
// 	}
// 
// 	b[ 3]  = a[19];
// 	b[ 7]  = a[23];
// 	b[11]  = a[27];
// 	b[15]  = a[31];
// 	b[19]  = a[ 3];
// 	b[23]  = a[ 7];
// 	b[27]  = a[11];
// 	b[31]  = a[15];
// }
// 
// static void shift_row(
// 	word8 * a,
// 	word8 * b,
// 	word8 d,
// 	int   BC)
// {
// 	switch (BC) {
// 	case 4: shift_row_4(a, b, d); break;
// 	case 6: shift_row_6(a, b, d); break;
// 	case 8: shift_row_8(a, b, d); break;
// 	}
// }
// 
// // ----------------------------------------------------------------
// // Replace every byte of the input by the byte at that place in the
// // nonlinear S-box.
// 
// static void substitution(
// 	word8 * a,
// 	word8 * box,
// 	int   BC)
// {
// 	a[ 0] = box[a[ 0]];
// 	a[ 1] = box[a[ 1]];
// 	a[ 2] = box[a[ 2]];
// 	a[ 3] = box[a[ 3]];
// 
// 	a[ 4] = box[a[ 4]];
// 	a[ 5] = box[a[ 5]];
// 	a[ 6] = box[a[ 6]];
// 	a[ 7] = box[a[ 7]];
// 
// 	a[ 8] = box[a[ 8]];
// 	a[ 9] = box[a[ 9]];
// 	a[10] = box[a[10]];
// 	a[11] = box[a[11]];
// 
// 	a[12] = box[a[12]];
// 	a[13] = box[a[13]];
// 	a[14] = box[a[14]];
// 	a[15] = box[a[15]];
// 
// 	if (BC == 4)
// 		return;
// 
// 	a[16] = box[a[16]];
// 	a[17] = box[a[17]];
// 	a[18] = box[a[18]];
// 	a[19] = box[a[19]];
// 
// 	a[20] = box[a[20]];
// 	a[21] = box[a[21]];
// 	a[22] = box[a[22]];
// 	a[23] = box[a[23]];
// 
// 	if (BC == 6)
// 		return;
// 
// 	a[24] = box[a[24]];
// 	a[25] = box[a[25]];
// 	a[26] = box[a[26]];
// 	a[27] = box[a[27]];
// 
// 	a[28] = box[a[28]];
// 	a[29] = box[a[29]];
// 	a[30] = box[a[30]];
// 	a[31] = box[a[31]];
// 
// }
// 
// // ----------------------------------------------------------------
// // Mix the four bytes of every column in a linear way.
// 
// static void mix_column(
// 	word8 * a,
// 	word8 * b,
// 	int   BC)
// {
// 	b[ 0] = M2[a[ 0]] ^ M3[a[ 1]] ^ a[ 2] ^ a[ 3];
// 	b[ 1] = M2[a[ 1]] ^ M3[a[ 2]] ^ a[ 3] ^ a[ 0];
// 	b[ 2] = M2[a[ 2]] ^ M3[a[ 3]] ^ a[ 0] ^ a[ 1];
// 	b[ 3] = M2[a[ 3]] ^ M3[a[ 0]] ^ a[ 1] ^ a[ 2];
// 
// 	b[ 4] = M2[a[ 4]] ^ M3[a[ 5]] ^ a[ 6] ^ a[ 7];
// 	b[ 5] = M2[a[ 5]] ^ M3[a[ 6]] ^ a[ 7] ^ a[ 4];
// 	b[ 6] = M2[a[ 6]] ^ M3[a[ 7]] ^ a[ 4] ^ a[ 5];
// 	b[ 7] = M2[a[ 7]] ^ M3[a[ 4]] ^ a[ 5] ^ a[ 6];
// 
// 	b[ 8] = M2[a[ 8]] ^ M3[a[ 9]] ^ a[10] ^ a[11];
// 	b[ 9] = M2[a[ 9]] ^ M3[a[10]] ^ a[11] ^ a[ 8];
// 	b[10] = M2[a[10]] ^ M3[a[11]] ^ a[ 8] ^ a[ 9];
// 	b[11] = M2[a[11]] ^ M3[a[ 8]] ^ a[ 9] ^ a[10];
// 
// 	b[12] = M2[a[12]] ^ M3[a[13]] ^ a[14] ^ a[15];
// 	b[13] = M2[a[13]] ^ M3[a[14]] ^ a[15] ^ a[12];
// 	b[14] = M2[a[14]] ^ M3[a[15]] ^ a[12] ^ a[13];
// 	b[15] = M2[a[15]] ^ M3[a[12]] ^ a[13] ^ a[14];
// 
// 	if (BC == 4)
// 		return;
// 
// 	b[16] = M2[a[16]] ^ M3[a[17]] ^ a[18] ^ a[19];
// 	b[17] = M2[a[17]] ^ M3[a[18]] ^ a[19] ^ a[16];
// 	b[18] = M2[a[18]] ^ M3[a[19]] ^ a[16] ^ a[17];
// 	b[19] = M2[a[19]] ^ M3[a[16]] ^ a[17] ^ a[18];
// 
// 	b[20] = M2[a[20]] ^ M3[a[21]] ^ a[22] ^ a[23];
// 	b[21] = M2[a[21]] ^ M3[a[22]] ^ a[23] ^ a[20];
// 	b[22] = M2[a[22]] ^ M3[a[23]] ^ a[20] ^ a[21];
// 	b[23] = M2[a[23]] ^ M3[a[20]] ^ a[21] ^ a[22];
// 
// 	if (BC == 6)
// 		return;
// 
// 	b[24] = M2[a[24]] ^ M3[a[25]] ^ a[26] ^ a[27];
// 	b[25] = M2[a[25]] ^ M3[a[26]] ^ a[27] ^ a[24];
// 	b[26] = M2[a[26]] ^ M3[a[27]] ^ a[24] ^ a[25];
// 	b[27] = M2[a[27]] ^ M3[a[24]] ^ a[25] ^ a[26];
// 
// 	b[28] = M2[a[28]] ^ M3[a[29]] ^ a[30] ^ a[31];
// 	b[29] = M2[a[29]] ^ M3[a[30]] ^ a[31] ^ a[28];
// 	b[30] = M2[a[30]] ^ M3[a[31]] ^ a[28] ^ a[29];
// 	b[31] = M2[a[31]] ^ M3[a[28]] ^ a[29] ^ a[30];
// }
// 
// // ----------------------------------------------------------------
// // Mix the four bytes of every column in a linear way.
// // This is the opposite operation of Mixcolumn.
// 
// static void inv_mix_column(
// 	word8 * a,
// 	word8 * b,
// 	int   BC)
// {
// 	b[ 0] = Me[a[ 0]] ^ Mb[a[ 1]] ^ Md[a[ 2]] ^ M9[a[ 3]];
// 	b[ 1] = Me[a[ 1]] ^ Mb[a[ 2]] ^ Md[a[ 3]] ^ M9[a[ 0]];
// 	b[ 2] = Me[a[ 2]] ^ Mb[a[ 3]] ^ Md[a[ 0]] ^ M9[a[ 1]]; 
// 	b[ 3] = Me[a[ 3]] ^ Mb[a[ 0]] ^ Md[a[ 1]] ^ M9[a[ 2]]; 
// 
// 	b[ 4] = Me[a[ 4]] ^ Mb[a[ 5]] ^ Md[a[ 6]] ^ M9[a[ 7]];
// 	b[ 5] = Me[a[ 5]] ^ Mb[a[ 6]] ^ Md[a[ 7]] ^ M9[a[ 4]];
// 	b[ 6] = Me[a[ 6]] ^ Mb[a[ 7]] ^ Md[a[ 4]] ^ M9[a[ 5]]; 
// 	b[ 7] = Me[a[ 7]] ^ Mb[a[ 4]] ^ Md[a[ 5]] ^ M9[a[ 6]]; 
// 
// 	b[ 8] = Me[a[ 8]] ^ Mb[a[ 9]] ^ Md[a[10]] ^ M9[a[11]];
// 	b[ 9] = Me[a[ 9]] ^ Mb[a[10]] ^ Md[a[11]] ^ M9[a[ 8]];
// 	b[10] = Me[a[10]] ^ Mb[a[11]] ^ Md[a[ 8]] ^ M9[a[ 9]]; 
// 	b[11] = Me[a[11]] ^ Mb[a[ 8]] ^ Md[a[ 9]] ^ M9[a[10]]; 
// 
// 	b[12] = Me[a[12]] ^ Mb[a[13]] ^ Md[a[14]] ^ M9[a[15]];
// 	b[13] = Me[a[13]] ^ Mb[a[14]] ^ Md[a[15]] ^ M9[a[12]];
// 	b[14] = Me[a[14]] ^ Mb[a[15]] ^ Md[a[12]] ^ M9[a[13]]; 
// 	b[15] = Me[a[15]] ^ Mb[a[12]] ^ Md[a[13]] ^ M9[a[14]]; 
// 
// 	if (BC == 4)
// 		return;
// 
// 	b[16] = Me[a[16]] ^ Mb[a[17]] ^ Md[a[18]] ^ M9[a[19]];
// 	b[17] = Me[a[17]] ^ Mb[a[18]] ^ Md[a[19]] ^ M9[a[16]];
// 	b[18] = Me[a[18]] ^ Mb[a[19]] ^ Md[a[16]] ^ M9[a[17]]; 
// 	b[19] = Me[a[19]] ^ Mb[a[16]] ^ Md[a[17]] ^ M9[a[18]]; 
// 
// 	b[20] = Me[a[20]] ^ Mb[a[21]] ^ Md[a[22]] ^ M9[a[23]];
// 	b[21] = Me[a[21]] ^ Mb[a[22]] ^ Md[a[23]] ^ M9[a[20]];
// 	b[22] = Me[a[22]] ^ Mb[a[23]] ^ Md[a[20]] ^ M9[a[21]]; 
// 	b[23] = Me[a[23]] ^ Mb[a[20]] ^ Md[a[21]] ^ M9[a[22]]; 
// 
// 	if (BC == 6)
// 		return;
// 
// 	b[24] = Me[a[24]] ^ Mb[a[25]] ^ Md[a[26]] ^ M9[a[27]];
// 	b[25] = Me[a[25]] ^ Mb[a[26]] ^ Md[a[27]] ^ M9[a[24]];
// 	b[26] = Me[a[26]] ^ Mb[a[27]] ^ Md[a[24]] ^ M9[a[25]]; 
// 	b[27] = Me[a[27]] ^ Mb[a[24]] ^ Md[a[25]] ^ M9[a[26]]; 
// 
// 	b[28] = Me[a[28]] ^ Mb[a[29]] ^ Md[a[30]] ^ M9[a[31]];
// 	b[29] = Me[a[29]] ^ Mb[a[30]] ^ Md[a[31]] ^ M9[a[28]];
// 	b[30] = Me[a[30]] ^ Mb[a[31]] ^ Md[a[28]] ^ M9[a[29]]; 
// 	b[31] = Me[a[31]] ^ Mb[a[28]] ^ Md[a[29]] ^ M9[a[30]]; 
// }

// ----------------------------------------------------------------
#ifdef DEBUG_AESNALG
#define DEBUG_BLOCK(msg,blk,bc) print_block(msg,blk,bc)

#define DEBUG_SEP fprintf(stderr, "\n")

static void print_block(
	char * msg,
	word32 * a,
	int BC)
{
	int j;

	fprintf(stderr, "%s:\n", msg);
	for (j = 0; j < BC; j++)
		fprintf(stderr, "%08x ", (unsigned) a[j]);
	fprintf(stderr, "\n");
}

#else
#define DEBUG_BLOCK(msg,blk,bc)
#define DEBUG_SEP
#endif

// // ----------------------------------------------------------------
// static word32 rot8(
// 	word32 a)
// {
// 	return (a >> 8) | (a << 24);
// }
// 
// // ----------------------------------------------------------------
// static word32 sub4(
// 	word32 a,
// 	word8 box[256])
// {
// 	word8 b0, b1, b2, b3;
// 	unpack4(a, b0, b1, b2, b3);
// 	b0 = box[b0];
// 	b1 = box[b1];
// 	b2 = box[b2];
// 	b3 = box[b3];
// 	pack4(a, b0, b1, b2, b3);
// 	return a;
// }

// ----------------------------------------------------------------
// Calculate the necessary round keys.
// The number of calculations depends on kl and
// bl.

int AESN_key_schedule (
	word32 k[MAXKC],
	int    kl,
	int    bl,
	word32 W[MAXROUNDS+1][MAXKC])
{
	int KC, BC, ROUNDS;
	int j, t, rconpointer = 0;
	word32 tk[MAXKC];
	word8 tka[4];
	word8 tkb[4];

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

DEBUG_BLOCK("ks.0", k, KC);
	for (j = 0; j < KC; j++)
		tk[j] = k[j];
	t = 0;
DEBUG_BLOCK("ks.1", tk, KC);
	for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
		W[t / BC][t % BC] = tk[j];

	while (t < (ROUNDS+1)*BC) {
DEBUG_BLOCK("ks.2", tk, KC);

		unpack4(tk[KC-1], &tka[0], &tka[1], &tka[2], &tka[3]);
		tkb[0] = tka[0] ^ S[tka[1]];
		tkb[1] = tka[1] ^ S[tka[2]];
		tkb[2] = tka[2] ^ S[tka[3]];
		tkb[3] = tka[3] ^ S[tka[0]];

fprintf(stderr, "tk[KC-1] 0x%08x\n", tk[KC-1]);
fprintf(stderr, "aa %02x %02x %02x %02x\n",
	(unsigned)tka[0], (unsigned)tka[1], (unsigned)tka[2], (unsigned)tka[3]);
fprintf(stderr, "bb %02x %02x %02x %02x\n",
	(unsigned)tkb[0], (unsigned)tkb[1], (unsigned)tkb[2], (unsigned)tkb[3]);
		pack4(&tk[0], tkb[0], tkb[1], tkb[2], tkb[3]);
DEBUG_BLOCK("ks.3", tk, KC);

		tkb[0] ^= rcon[rconpointer++];
		pack4(&tk[0], tkb[0], tkb[1], tkb[2], tkb[3]);

DEBUG_BLOCK("ks.4", tk, KC);

		if (KC != 8) {
			for (j = 1; j < KC; j++)
				tk[j] ^= tk[j-1];
		}
		else {
			for (j = 1; j < KC/2; j++)
				tk[j] ^= tk[j-1];
			tk[KC/2] ^= S[tk[KC/2 - 1]];
			for (j = KC/2 + 1; j < KC; j++)
				tk[j] ^= tk[j-1];
		}
		// Copy values into round key array
		for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
			W[t / BC][t % BC] = tk[j];
	}

	return 1;
}

// ----------------------------------------------------------------
int AESN_encrypt_block (
	word32 * atemp,
	word32 * b,
	int   kl,
	int   bl,
	word32 rk[MAXROUNDS+1][MAXBC])
{
	int r;
	int BC, ROUNDS;
	word32 a[MAXBC];

	switch (bl) {
	case 16: BC = 4; break;
	case 24: BC = 6; break;
	case 32: BC = 8; break;
	default : return -2;
	}

	memcpy(a, atemp, bl);
	switch ((kl >= bl) ? kl : bl) {
	case 16: ROUNDS = 10; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	                                      DEBUG_BLOCK("e in", a, BC);
	                                      DEBUG_BLOCK("e rk0", rk[0], BC);
// 	key_addition(a, rk[0], BC);           DEBUG_BLOCK("e ka", a, BC);
// 
	for (r = 1; r < ROUNDS; r++) {
// 		substitution(a, S, BC);       DEBUG_BLOCK("e sb", a, BC);
// 		shift_row(a, b, 0, BC);       DEBUG_BLOCK("e sr", b, BC);
// 		mix_column(b, a, BC);         DEBUG_BLOCK("e mc", a, BC);
	                                      DEBUG_BLOCK("e rkr", rk[r], BC);
// 		key_addition(a, rk[r], BC);   DEBUG_BLOCK("e ka", a, BC);
	}
// 
// 	substitution(a, S, BC);               DEBUG_BLOCK("e sb", a, BC);
// 	shift_row(a, b, 0, BC);               DEBUG_BLOCK("e sr", b, BC);
	                                  DEBUG_BLOCK("e rkl", rk[ROUNDS], BC);
// 	key_addition(b, rk[ROUNDS], BC);      DEBUG_BLOCK("e ka", b, BC);
// 
	                                      DEBUG_BLOCK("e out", b, BC);
	DEBUG_SEP;
	return 1;
}

// ----------------------------------------------------------------
int AESN_decrypt_block (
	word32 * atemp,
	word32 * b,
	int kl,
	int bl,
	word32 rk[MAXROUNDS+1][MAXKC])
{
	int r;
	int BC, ROUNDS;
	word32 a[MAXBC];

	switch (bl) {
	case 16: BC = 4; break;
	case 24: BC = 6; break;
	case 32: BC = 8; break;
	default : return -2;
	}

	memcpy(a, atemp, bl);
	switch ((kl >= bl) ? kl : bl) {
	case 16: ROUNDS = 10; break;
	case 24: ROUNDS = 12; break;
	case 32: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	                                    DEBUG_BLOCK("d in", a, BC);
// 	key_addition(a, rk[ROUNDS], BC);    DEBUG_BLOCK("d ka",  a, BC);
                               DEBUG_BLOCK("d rkl", rk[ROUNDS], BC);
// 	substitution(a, Si, BC);            DEBUG_BLOCK("d sb",  a, BC);
// 	shift_row(a, b, 1, BC);             DEBUG_BLOCK("d sr", b, BC);
// 
// 	// ROUNDS-1 ordinary rounds
	for (r = ROUNDS-1; r > 0; r--) {
// 		key_addition(b, rk[r], BC); DEBUG_BLOCK("d ka",   b, BC);
	                                    DEBUG_BLOCK("d rkr", rk[r], BC);
// 		inv_mix_column(b, a, BC);   DEBUG_BLOCK("d im", a, BC);
// 		substitution(a, Si, BC);    DEBUG_BLOCK("d sb",  a, BC);
// 		shift_row(a, b, 1, BC);     DEBUG_BLOCK("d sr", b, BC);
	}
// 
// 	// End with the extra key addition
// 	key_addition(b, rk[0], BC);         DEBUG_BLOCK("d ka", b, BC);
	                                    DEBUG_BLOCK("d rk0", rk[0], BC);

	                                    DEBUG_BLOCK("d out", b, BC);
	DEBUG_SEP;
	return 1;
}
