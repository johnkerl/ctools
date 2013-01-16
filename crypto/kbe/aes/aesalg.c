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
#include <ctype.h>
#include "aesalg.h"
#include "util.h"
#include "blkcipht.h"

#include "boxes-ref.dat"

// ----------------------------------------------------------------
// XOR corresponding text input and round key input bytes

static void key_addition(
	u8 a [4][MAXBC],
	u8 rk[4][MAXBC],
	int   BC)
{
	a[0][0]^=rk[0][0];a[1][0]^=rk[1][0];a[2][0]^=rk[2][0];a[3][0]^=rk[3][0];
	a[0][1]^=rk[0][1];a[1][1]^=rk[1][1];a[2][1]^=rk[2][1];a[3][1]^=rk[3][1];
	a[0][2]^=rk[0][2];a[1][2]^=rk[1][2];a[2][2]^=rk[2][2];a[3][2]^=rk[3][2];
	a[0][3]^=rk[0][3];a[1][3]^=rk[1][3];a[2][3]^=rk[2][3];a[3][3]^=rk[3][3];
	if (BC == 4)
		return;
	a[0][4]^=rk[0][4];a[1][4]^=rk[1][4];a[2][4]^=rk[2][4];a[3][4]^=rk[3][4];
	a[0][5]^=rk[0][5];a[1][5]^=rk[1][5];a[2][5]^=rk[2][5];a[3][5]^=rk[3][5];
	if (BC == 6)
		return;
	a[0][6]^=rk[0][6];a[1][6]^=rk[1][6];a[2][6]^=rk[2][6];a[3][6]^=rk[3][6];
	a[0][7]^=rk[0][7];a[1][7]^=rk[1][7];a[2][7]^=rk[2][7];a[3][7]^=rk[3][7];
}

// ----------------------------------------------------------------
// Row 0 remains unchanged.
// The other three rows are shifted a variable amount.

static void shift_row_4(
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	u8 d)
{
	b[0][0] = a[0][0];
	b[0][1] = a[0][1];
	b[0][2] = a[0][2];
	b[0][3] = a[0][3];

	if (d == 0) {
		b[1][0] = a[1][1];
		b[1][1] = a[1][2];
		b[1][2] = a[1][3];
		b[1][3] = a[1][0];
	} else {
		b[1][0] = a[1][3];
		b[1][1] = a[1][0];
		b[1][2] = a[1][1];
		b[1][3] = a[1][2];
	}

	b[2][0] = a[2][2];
	b[2][1] = a[2][3];
	b[2][2] = a[2][0];
	b[2][3] = a[2][1];

	if (d == 0) {
		b[3][0] = a[3][3];
		b[3][1] = a[3][0];
		b[3][2] = a[3][1];
		b[3][3] = a[3][2];
	} else {
		b[3][0] = a[3][1];
		b[3][1] = a[3][2];
		b[3][2] = a[3][3];
		b[3][3] = a[3][0];
	}
}

static void shift_row_6(
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	u8 d)
{
	b[0][0] = a[0][0];
	b[0][1] = a[0][1];
	b[0][2] = a[0][2];
	b[0][3] = a[0][3];
	b[0][4] = a[0][4];
	b[0][5] = a[0][5];

	if (d == 0) {
		b[1][0] = a[1][1];
		b[1][1] = a[1][2];
		b[1][2] = a[1][3];
		b[1][3] = a[1][4];
		b[1][4] = a[1][5];
		b[1][5] = a[1][0];
	} else {
		b[1][0] = a[1][5];
		b[1][1] = a[1][0];
		b[1][2] = a[1][1];
		b[1][3] = a[1][2];
		b[1][4] = a[1][3];
		b[1][5] = a[1][4];
	}

	if (d == 0) {
		b[2][0] = a[2][2];
		b[2][1] = a[2][3];
		b[2][2] = a[2][4];
		b[2][3] = a[2][5];
		b[2][4] = a[2][0];
		b[2][5] = a[2][1];
	} else {
		b[2][0] = a[2][4];
		b[2][1] = a[2][5];
		b[2][2] = a[2][0];
		b[2][3] = a[2][1];
		b[2][4] = a[2][2];
		b[2][5] = a[2][3];
	}

	b[3][0] = a[3][3];
	b[3][1] = a[3][4];
	b[3][2] = a[3][5];
	b[3][3] = a[3][0];
	b[3][4] = a[3][1];
	b[3][5] = a[3][2];
}

static void shift_row_8(
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	u8 d)
{
	b[0][0]  = a[0][0];
	b[0][1]  = a[0][1];
	b[0][2]  = a[0][2];
	b[0][3]  = a[0][3];
	b[0][4]  = a[0][4];
	b[0][5]  = a[0][5];
	b[0][6]  = a[0][6];
	b[0][7]  = a[0][7];

	if (d == 0) {
		b[1][0] = a[1][1];
		b[1][1] = a[1][2];
		b[1][2] = a[1][3];
		b[1][3] = a[1][4];
		b[1][4] = a[1][5];
		b[1][5] = a[1][6];
		b[1][6] = a[1][7];
		b[1][7] = a[1][0];
	} else {
		b[1][0] = a[1][7];
		b[1][1] = a[1][0];
		b[1][2] = a[1][1];
		b[1][3] = a[1][2];
		b[1][4] = a[1][3];
		b[1][5] = a[1][4];
		b[1][6] = a[1][5];
		b[1][7] = a[1][6];
	}

	if (d == 0) {
		b[2][0] = a[2][3];
		b[2][1] = a[2][4];
		b[2][2] = a[2][5];
		b[2][3] = a[2][6];
		b[2][4] = a[2][7];
		b[2][5] = a[2][0];
		b[2][6] = a[2][1];
		b[2][7] = a[2][2];
	} else {
		b[2][0] = a[2][5];
		b[2][1] = a[2][6];
		b[2][2] = a[2][7];
		b[2][3] = a[2][0];
		b[2][4] = a[2][1];
		b[2][5] = a[2][2];
		b[2][6] = a[2][3];
		b[2][7] = a[2][4];
	}

	b[3][0]  = a[3][4];
	b[3][1]  = a[3][5];
	b[3][2]  = a[3][6];
	b[3][3]  = a[3][7];
	b[3][4]  = a[3][0];
	b[3][5]  = a[3][1];
	b[3][6]  = a[3][2];
	b[3][7]  = a[3][3];
}

static void shift_row(
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	u8 d,
	int   BC)
{
	switch (BC) {
	case 4: shift_row_4(a, b, d); break;
	case 6: shift_row_6(a, b, d); break;
	case 8: shift_row_8(a, b, d); break;
	}
}

// ----------------------------------------------------------------
// Replace every byte of the input by the byte at that place in the
// nonlinear S-box.

static void substitution(
	u8 a[4][MAXBC],
	u8 box[256],
	int   BC)
{
	a[0][0] = box[a[0][0]];
	a[1][0] = box[a[1][0]];
	a[2][0] = box[a[2][0]];
	a[3][0] = box[a[3][0]];

	a[0][1] = box[a[0][1]];
	a[1][1] = box[a[1][1]];
	a[2][1] = box[a[2][1]];
	a[3][1] = box[a[3][1]];

	a[0][2] = box[a[0][2]];
	a[1][2] = box[a[1][2]];
	a[2][2] = box[a[2][2]];
	a[3][2] = box[a[3][2]];

	a[0][3] = box[a[0][3]];
	a[1][3] = box[a[1][3]];
	a[2][3] = box[a[2][3]];
	a[3][3] = box[a[3][3]];

	if (BC == 4)
		return;

	a[0][4] = box[a[0][4]];
	a[1][4] = box[a[1][4]];
	a[2][4] = box[a[2][4]];
	a[3][4] = box[a[3][4]];

	a[0][5] = box[a[0][5]];
	a[1][5] = box[a[1][5]];
	a[2][5] = box[a[2][5]];
	a[3][5] = box[a[3][5]];

	if (BC == 6)
		return;

	a[0][6] = box[a[0][6]];
	a[1][6] = box[a[1][6]];
	a[2][6] = box[a[2][6]];
	a[3][6] = box[a[3][6]];

	a[0][7] = box[a[0][7]];
	a[1][7] = box[a[1][7]];
	a[2][7] = box[a[2][7]];
	a[3][7] = box[a[3][7]];
}

// ----------------------------------------------------------------
// Mix the four bytes of every column in a linear way.

static void mix_column(
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	int   BC)
{
	b[0][0] = M2[a[0][0]] ^ M3[a[1][0]] ^ a[2][0] ^ a[3][0];
	b[1][0] = M2[a[1][0]] ^ M3[a[2][0]] ^ a[3][0] ^ a[0][0];
	b[2][0] = M2[a[2][0]] ^ M3[a[3][0]] ^ a[0][0] ^ a[1][0];
	b[3][0] = M2[a[3][0]] ^ M3[a[0][0]] ^ a[1][0] ^ a[2][0];

	b[0][1] = M2[a[0][1]] ^ M3[a[1][1]] ^ a[2][1] ^ a[3][1];
	b[1][1] = M2[a[1][1]] ^ M3[a[2][1]] ^ a[3][1] ^ a[0][1];
	b[2][1] = M2[a[2][1]] ^ M3[a[3][1]] ^ a[0][1] ^ a[1][1];
	b[3][1] = M2[a[3][1]] ^ M3[a[0][1]] ^ a[1][1] ^ a[2][1];

	b[0][2] = M2[a[0][2]] ^ M3[a[1][2]] ^ a[2][2] ^ a[3][2];
	b[1][2] = M2[a[1][2]] ^ M3[a[2][2]] ^ a[3][2] ^ a[0][2];
	b[2][2] = M2[a[2][2]] ^ M3[a[3][2]] ^ a[0][2] ^ a[1][2];
	b[3][2] = M2[a[3][2]] ^ M3[a[0][2]] ^ a[1][2] ^ a[2][2];

	b[0][3] = M2[a[0][3]] ^ M3[a[1][3]] ^ a[2][3] ^ a[3][3];
	b[1][3] = M2[a[1][3]] ^ M3[a[2][3]] ^ a[3][3] ^ a[0][3];
	b[2][3] = M2[a[2][3]] ^ M3[a[3][3]] ^ a[0][3] ^ a[1][3];
	b[3][3] = M2[a[3][3]] ^ M3[a[0][3]] ^ a[1][3] ^ a[2][3];

	if (BC == 4)
		return;

	b[0][4] = M2[a[0][4]] ^ M3[a[1][4]] ^ a[2][4] ^ a[3][4];
	b[1][4] = M2[a[1][4]] ^ M3[a[2][4]] ^ a[3][4] ^ a[0][4];
	b[2][4] = M2[a[2][4]] ^ M3[a[3][4]] ^ a[0][4] ^ a[1][4];
	b[3][4] = M2[a[3][4]] ^ M3[a[0][4]] ^ a[1][4] ^ a[2][4];

	b[0][5] = M2[a[0][5]] ^ M3[a[1][5]] ^ a[2][5] ^ a[3][5];
	b[1][5] = M2[a[1][5]] ^ M3[a[2][5]] ^ a[3][5] ^ a[0][5];
	b[2][5] = M2[a[2][5]] ^ M3[a[3][5]] ^ a[0][5] ^ a[1][5];
	b[3][5] = M2[a[3][5]] ^ M3[a[0][5]] ^ a[1][5] ^ a[2][5];

	if (BC == 6)
		return;

	b[0][6] = M2[a[0][6]] ^ M3[a[1][6]] ^ a[2][6] ^ a[3][6];
	b[1][6] = M2[a[1][6]] ^ M3[a[2][6]] ^ a[3][6] ^ a[0][6];
	b[2][6] = M2[a[2][6]] ^ M3[a[3][6]] ^ a[0][6] ^ a[1][6];
	b[3][6] = M2[a[3][6]] ^ M3[a[0][6]] ^ a[1][6] ^ a[2][6];

	b[0][7] = M2[a[0][7]] ^ M3[a[1][7]] ^ a[2][7] ^ a[3][7];
	b[1][7] = M2[a[1][7]] ^ M3[a[2][7]] ^ a[3][7] ^ a[0][7];
	b[2][7] = M2[a[2][7]] ^ M3[a[3][7]] ^ a[0][7] ^ a[1][7];
	b[3][7] = M2[a[3][7]] ^ M3[a[0][7]] ^ a[1][7] ^ a[2][7];
}

// ----------------------------------------------------------------
// Mix the four bytes of every column in a linear way.
// This is the opposite operation of Mixcolumn.

static void inv_mix_column(
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	int   BC)
{
	b[0][0] = Me[a[0][0]] ^ Mb[a[1][0]] ^ Md[a[2][0]] ^ M9[a[3][0]];
	b[1][0] = Me[a[1][0]] ^ Mb[a[2][0]] ^ Md[a[3][0]] ^ M9[a[0][0]];
	b[2][0] = Me[a[2][0]] ^ Mb[a[3][0]] ^ Md[a[0][0]] ^ M9[a[1][0]];
	b[3][0] = Me[a[3][0]] ^ Mb[a[0][0]] ^ Md[a[1][0]] ^ M9[a[2][0]];

	b[0][1] = Me[a[0][1]] ^ Mb[a[1][1]] ^ Md[a[2][1]] ^ M9[a[3][1]];
	b[1][1] = Me[a[1][1]] ^ Mb[a[2][1]] ^ Md[a[3][1]] ^ M9[a[0][1]];
	b[2][1] = Me[a[2][1]] ^ Mb[a[3][1]] ^ Md[a[0][1]] ^ M9[a[1][1]];
	b[3][1] = Me[a[3][1]] ^ Mb[a[0][1]] ^ Md[a[1][1]] ^ M9[a[2][1]];

	b[0][2] = Me[a[0][2]] ^ Mb[a[1][2]] ^ Md[a[2][2]] ^ M9[a[3][2]];
	b[1][2] = Me[a[1][2]] ^ Mb[a[2][2]] ^ Md[a[3][2]] ^ M9[a[0][2]];
	b[2][2] = Me[a[2][2]] ^ Mb[a[3][2]] ^ Md[a[0][2]] ^ M9[a[1][2]];
	b[3][2] = Me[a[3][2]] ^ Mb[a[0][2]] ^ Md[a[1][2]] ^ M9[a[2][2]];

	b[0][3] = Me[a[0][3]] ^ Mb[a[1][3]] ^ Md[a[2][3]] ^ M9[a[3][3]];
	b[1][3] = Me[a[1][3]] ^ Mb[a[2][3]] ^ Md[a[3][3]] ^ M9[a[0][3]];
	b[2][3] = Me[a[2][3]] ^ Mb[a[3][3]] ^ Md[a[0][3]] ^ M9[a[1][3]];
	b[3][3] = Me[a[3][3]] ^ Mb[a[0][3]] ^ Md[a[1][3]] ^ M9[a[2][3]];

	if (BC == 4)
		return;

	b[0][4] = Me[a[0][4]] ^ Mb[a[1][4]] ^ Md[a[2][4]] ^ M9[a[3][4]];
	b[1][4] = Me[a[1][4]] ^ Mb[a[2][4]] ^ Md[a[3][4]] ^ M9[a[0][4]];
	b[2][4] = Me[a[2][4]] ^ Mb[a[3][4]] ^ Md[a[0][4]] ^ M9[a[1][4]];
	b[3][4] = Me[a[3][4]] ^ Mb[a[0][4]] ^ Md[a[1][4]] ^ M9[a[2][4]];

	b[0][5] = Me[a[0][5]] ^ Mb[a[1][5]] ^ Md[a[2][5]] ^ M9[a[3][5]];
	b[1][5] = Me[a[1][5]] ^ Mb[a[2][5]] ^ Md[a[3][5]] ^ M9[a[0][5]];
	b[2][5] = Me[a[2][5]] ^ Mb[a[3][5]] ^ Md[a[0][5]] ^ M9[a[1][5]];
	b[3][5] = Me[a[3][5]] ^ Mb[a[0][5]] ^ Md[a[1][5]] ^ M9[a[2][5]];

	if (BC == 6)
		return;

	b[0][6] = Me[a[0][6]] ^ Mb[a[1][6]] ^ Md[a[2][6]] ^ M9[a[3][6]];
	b[1][6] = Me[a[1][6]] ^ Mb[a[2][6]] ^ Md[a[3][6]] ^ M9[a[0][6]];
	b[2][6] = Me[a[2][6]] ^ Mb[a[3][6]] ^ Md[a[0][6]] ^ M9[a[1][6]];
	b[3][6] = Me[a[3][6]] ^ Mb[a[0][6]] ^ Md[a[1][6]] ^ M9[a[2][6]];

	b[0][7] = Me[a[0][7]] ^ Mb[a[1][7]] ^ Md[a[2][7]] ^ M9[a[3][7]];
	b[1][7] = Me[a[1][7]] ^ Mb[a[2][7]] ^ Md[a[3][7]] ^ M9[a[0][7]];
	b[2][7] = Me[a[2][7]] ^ Mb[a[3][7]] ^ Md[a[0][7]] ^ M9[a[1][7]];
	b[3][7] = Me[a[3][7]] ^ Mb[a[0][7]] ^ Md[a[1][7]] ^ M9[a[2][7]];
}

// ----------------------------------------------------------------
void AES_print_block(
	char * msg,
	u8 a[4][MAXBC],
	int BC)
{
	int i, j;

	fprintf(stderr, "%s:\n", msg);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < BC; j++)
			fprintf(stderr, "%02x ", (unsigned) a[i][j]);
		fprintf(stderr, "{");
		for (j = 0; j < BC; j++)
			fprintf(stderr, "%c",
				isprint(a[i][j]) ? a[i][j] : '.');
		fprintf(stderr, "}\n");
	}
	fprintf(stderr, "\n");
}

// ----------------------------------------------------------------

#ifdef XDEBUG_AESALG
#define XDEBUG_BLOCK(msg,blk,bc) AES_print_block(msg,blk,bc)
#else
#define XDEBUG_BLOCK(msg,blk,bc)
#endif

#ifdef DEBUG_AESALG
#define DEBUG_BLOCK(msg,blk,bc) AES_print_block(msg,blk,bc)
#define DEBUG_TEXT(x) fprintf(stderr, "%s\n", x);
#define DEBUG_SEP fprintf(stderr, "\n")
#else
#define DEBUG_BLOCK(msg,blk,bc)
#define DEBUG_TEXT(x)
#define DEBUG_SEP
#endif

// ----------------------------------------------------------------
// Calculate the necessary round keys.
// The number of calculations depends on kl and
// bl.

int AES_key_schedule (
	u8 k[4][MAXKC],
	int   kl,
	int   bl,
	u8 W[MAXROUNDS+1][4][MAXBC])
{
	int KC, BC, ROUNDS;
	int i, j, t, rconpointer = 0;
	u8 tk[4][MAXKC];

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
		for (i = 0; i < 4; i++)
			tk[i][j] = k[i][j];
	t = 0;
	for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
		for (i = 0; i < 4; i++)
			W[t / BC][i][t % BC] = tk[i][j];

	while (t < (ROUNDS+1)*BC) {
		for (i = 0; i < 4; i++)
			tk[i][0] ^= S[tk[(i+1)&3][KC-1]];
		tk[0][0] ^= rcon[rconpointer++];

		if (KC != 8)
			for (j = 1; j < KC; j++)
				for (i = 0; i < 4; i++)
					tk[i][j] ^= tk[i][j-1];
		else {
			for (j = 1; j < KC/2; j++)
				for (i = 0; i < 4; i++)
					tk[i][j] ^= tk[i][j-1];
			for (i = 0; i < 4; i++)
				tk[i][KC/2] ^= S[tk[i][KC/2 - 1]];
			for (j = KC/2 + 1; j < KC; j++)
				for (i = 0; i < 4; i++)
					tk[i][j] ^= tk[i][j-1];
		}
		// Copy values into round key array
		for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
			for (i = 0; i < 4; i++)
				W[t / BC][i][t % BC] = tk[i][j];
	}

	return 1;
}

// ----------------------------------------------------------------
int AES_encrypt_block (
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	int   kl,
	int   bl,
	u8 rk[MAXROUNDS+1][4][MAXBC])
{
	int r, BC, ROUNDS;

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

	                                      DEBUG_BLOCK("e in", a, BC);
	// Begin with a key addition
	                                      DEBUG_BLOCK("e rk0", rk[0], BC);
	key_addition(a, rk[0], BC);           DEBUG_BLOCK("e ka", a, BC);

	// ROUNDS-1 ordinary rounds
	for (r = 1; r < ROUNDS; r++) {
		                              DEBUG_BLOCK("e ri", a, BC);
		substitution(a, S, BC);       DEBUG_BLOCK("e su", a, BC);
		shift_row(a, b, 0, BC);       DEBUG_BLOCK("e sr", b, BC);
		mix_column(b, a, BC);         DEBUG_BLOCK("e mc", a, BC);
	                                      DEBUG_BLOCK("e rkr", rk[r], BC);
		key_addition(a, rk[r], BC);   DEBUG_BLOCK("e ro", a, BC);
		DEBUG_TEXT("--------------------------------\n");
	}

	// Last round is special: there is no mix_column
	substitution(a, S, BC);               DEBUG_BLOCK("e su", a, BC);
	shift_row(a, b, 0, BC);               DEBUG_BLOCK("e sr", b, BC);
	                                  DEBUG_BLOCK("e rkl", rk[ROUNDS], BC);
	key_addition(b, rk[ROUNDS], BC);      DEBUG_BLOCK("e out", b, BC);

	DEBUG_SEP;
	return 1;
}

// ----------------------------------------------------------------
int AES_decrypt_block (
	u8 a[4][MAXBC],
	u8 b[4][MAXBC],
	int kl,
	int bl,
	u8 rk[MAXROUNDS+1][4][MAXBC])
{
	int r, BC, ROUNDS;

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

	// To decrypt: apply the inverse operations of the encrypt routine,
	// in opposite order.
	//
	// (key_addition is an involution: it's equal to its inverse.)
	// (The inverse of substitution with table S is substitution with
	// the inverse table of S.)
	// (The inverse of shift_row is shift_row over a suitable distance.)

	// First the special round:
	// * Without inv_mix_column
	// * With extra key_addition
	                                    DEBUG_BLOCK("d in", a, BC);
	key_addition(a, rk[ROUNDS], BC);
	DEBUG_BLOCK("d rkl", rk[ROUNDS], BC);
	DEBUG_BLOCK("d ka",  a, BC);

	substitution(a, Si, BC);            DEBUG_BLOCK("d su",  a, BC);
	shift_row(a, b, 1, BC);             DEBUG_BLOCK("d sr", b, BC);

	// ROUNDS-1 ordinary rounds
	for (r = ROUNDS-1; r > 0; r--) {
		                            DEBUG_BLOCK("d ri", b, BC);
		key_addition(b, rk[r], BC);
		DEBUG_BLOCK("d rkr", rk[r], BC);
		DEBUG_BLOCK("d ka",   b, BC);
		inv_mix_column(b, a, BC);   DEBUG_BLOCK("d mc", a, BC);
		substitution(a, Si, BC);    DEBUG_BLOCK("d su",  a, BC);
		shift_row(a, b, 1, BC);     DEBUG_BLOCK("d sr", b, BC);

		DEBUG_TEXT("----------------------\n");
	}

	// End with the extra key addition
	key_addition(b, rk[0], BC);         DEBUG_BLOCK("d ka", b, BC);
	                                    DEBUG_BLOCK("d out", rk[0], BC);

	DEBUG_SEP;
	return 1;
}
