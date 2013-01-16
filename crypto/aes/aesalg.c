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
#include "aesalg.h"

// ----------------------------------------------------------------
#define SC	((BC - 4) >> 1)

#include "boxes-ref.dat"

static word8 shifts[3][4][2] = {
   0, 0,
   1, 3,
   2, 2,
   3, 1,

   0, 0,
   1, 5,
   2, 4,
   3, 3,

   0, 0,
   1, 7,
   3, 5,
   4, 4
};


// ----------------------------------------------------------------
static word8 mul(
	word8 a,
	word8 b)
{
	// Multiply two elements of GF(2^m).
	// Needed for mix_column and inv_mix_column.
	if (a && b)
		return Alogtable[Logtable[a] + Logtable[b]];
	else
		return 0;
}

// ----------------------------------------------------------------
// XOR corresponding text input and round key input bytes

static void key_addition(
	word8 a [4][MAXBC],
	word8 rk[4][MAXBC],
	int   BC)
{
	if (BC != 8) {
		int j;

		for (j = 0; j < BC; j++) {
			a[0][j] ^= rk[0][j];
			a[1][j] ^= rk[1][j];
			a[2][j] ^= rk[2][j];
			a[3][j] ^= rk[3][j];
		}
		return;
	}

	a[0][0]^=rk[0][0];a[1][0]^=rk[1][0];a[2][0]^=rk[2][0];a[3][0]^=rk[3][0];
	a[0][1]^=rk[0][1];a[1][1]^=rk[1][1];a[2][1]^=rk[2][1];a[3][1]^=rk[3][1];
	a[0][2]^=rk[0][2];a[1][2]^=rk[1][2];a[2][2]^=rk[2][2];a[3][2]^=rk[3][2];
	a[0][3]^=rk[0][3];a[1][3]^=rk[1][3];a[2][3]^=rk[2][3];a[3][3]^=rk[3][3];
	a[0][4]^=rk[0][4];a[1][4]^=rk[1][4];a[2][4]^=rk[2][4];a[3][4]^=rk[3][4];
	a[0][5]^=rk[0][5];a[1][5]^=rk[1][5];a[2][5]^=rk[2][5];a[3][5]^=rk[3][5];
	a[0][6]^=rk[0][6];a[1][6]^=rk[1][6];a[2][6]^=rk[2][6];a[3][6]^=rk[3][6];
	a[0][7]^=rk[0][7];a[1][7]^=rk[1][7];a[2][7]^=rk[2][7];a[3][7]^=rk[3][7];

}

// ----------------------------------------------------------------
// Row 0 remains unchanged.
// The other three rows are shifted a variable amount.

static void shift_row(
	word8 a[4][MAXBC],
	word8 d,
	int   BC)
{
	word8 tmp[MAXBC];
	int i, j;

	if (BC != 8) {
		for (i = 1; i < 4; i++) {
			int scid = shifts[SC][i][d];
			for (j = 0; j < BC; j++)
				tmp[j] = a[i][(j + scid) % BC];
			for (j = 0; j < BC; j++)
				a[i][j] = tmp[j];
		}
		return;
	}

	if (d == 0) {
		tmp[0] = a[1][1];
		tmp[1] = a[1][2];
		tmp[2] = a[1][3];
		tmp[3] = a[1][4];
		tmp[4] = a[1][5];
		tmp[5] = a[1][6];
		tmp[6] = a[1][7];
		tmp[7] = a[1][0];
	} else {
		tmp[0] = a[1][7];
		tmp[1] = a[1][0];
		tmp[2] = a[1][1];
		tmp[3] = a[1][2];
		tmp[4] = a[1][3];
		tmp[5] = a[1][4];
		tmp[6] = a[1][5];
		tmp[7] = a[1][6];
	}

	a[1][0] = tmp[0];
	a[1][1] = tmp[1];
	a[1][2] = tmp[2];
	a[1][3] = tmp[3];
	a[1][4] = tmp[4];
	a[1][5] = tmp[5];
	a[1][6] = tmp[6];
	a[1][7] = tmp[7];

	if (d == 0) {
		tmp[0] = a[2][3];
		tmp[1] = a[2][4];
		tmp[2] = a[2][5];
		tmp[3] = a[2][6];
		tmp[4] = a[2][7];
		tmp[5] = a[2][0];
		tmp[6] = a[2][1];
		tmp[7] = a[2][2];
	} else {
		tmp[0] = a[2][5];
		tmp[1] = a[2][6];
		tmp[2] = a[2][7];
		tmp[3] = a[2][0];
		tmp[4] = a[2][1];
		tmp[5] = a[2][2];
		tmp[6] = a[2][3];
		tmp[7] = a[2][4];
	}

	a[2][0] = tmp[0];
	a[2][1] = tmp[1];
	a[2][2] = tmp[2];
	a[2][3] = tmp[3];
	a[2][4] = tmp[4];
	a[2][5] = tmp[5];
	a[2][6] = tmp[6];
	a[2][7] = tmp[7];

	tmp[0]  = a[3][4];
	tmp[1]  = a[3][5];
	tmp[2]  = a[3][6];
	tmp[3]  = a[3][7];
	tmp[4]  = a[3][0];
	tmp[5]  = a[3][1];
	tmp[6]  = a[3][2];
	tmp[7]  = a[3][3];

	a[3][0] = tmp[0];
	a[3][1] = tmp[1];
	a[3][2] = tmp[2];
	a[3][3] = tmp[3];
	a[3][4] = tmp[4];
	a[3][5] = tmp[5];
	a[3][6] = tmp[6];
	a[3][7] = tmp[7];
}

// ----------------------------------------------------------------
// Replace every byte of the input by the byte at that place in the
// nonlinear S-box.

static void substitution(
	word8 a[4][MAXBC],
	word8 box[256],
	int   BC)
{
	if (BC != 8) {
		int j;

		for (j = 0; j < BC; j++) {
			a[0][j] = box[a[0][j]];
			a[1][j] = box[a[1][j]];
			a[2][j] = box[a[2][j]];
		a[3][j] = box[a[3][j]];
		}
		return;
	}

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

	a[0][4] = box[a[0][4]];
	a[1][4] = box[a[1][4]];
	a[2][4] = box[a[2][4]];
	a[3][4] = box[a[3][4]];

	a[0][5] = box[a[0][5]];
	a[1][5] = box[a[1][5]];
	a[2][5] = box[a[2][5]];
	a[3][5] = box[a[3][5]];

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
	word8 a[4][MAXBC],
	int   BC)
{
	word8 b[4][MAXBC];

	if (BC != 8) {
		int j;

		for (j = 0; j < BC; j++) {
			b[0][j] =
			  	M2[a[0][j]] ^ M3[a[1][j]] ^ a[2][j] ^ a[3][j];
			b[1][j] =
			  	M2[a[1][j]] ^ M3[a[2][j]] ^ a[3][j] ^ a[0][j];
			b[2][j] =
			  	M2[a[2][j]] ^ M3[a[3][j]] ^ a[0][j] ^ a[1][j];
			b[3][j] =
			  	M2[a[3][j]] ^ M3[a[0][j]] ^ a[1][j] ^ a[2][j];
		}

		for (j = 0; j < BC; j++) {
			a[0][j] = b[0][j];
			a[1][j] = b[1][j];
			a[2][j] = b[2][j];
			a[3][j] = b[3][j];
		}

		return;
	}

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

	b[0][4] = M2[a[0][4]] ^ M3[a[1][4]] ^ a[2][4] ^ a[3][4];
	b[1][4] = M2[a[1][4]] ^ M3[a[2][4]] ^ a[3][4] ^ a[0][4];
	b[2][4] = M2[a[2][4]] ^ M3[a[3][4]] ^ a[0][4] ^ a[1][4];
	b[3][4] = M2[a[3][4]] ^ M3[a[0][4]] ^ a[1][4] ^ a[2][4];

	b[0][5] = M2[a[0][5]] ^ M3[a[1][5]] ^ a[2][5] ^ a[3][5];
	b[1][5] = M2[a[1][5]] ^ M3[a[2][5]] ^ a[3][5] ^ a[0][5];
	b[2][5] = M2[a[2][5]] ^ M3[a[3][5]] ^ a[0][5] ^ a[1][5];
	b[3][5] = M2[a[3][5]] ^ M3[a[0][5]] ^ a[1][5] ^ a[2][5];

	b[0][6] = M2[a[0][6]] ^ M3[a[1][6]] ^ a[2][6] ^ a[3][6];
	b[1][6] = M2[a[1][6]] ^ M3[a[2][6]] ^ a[3][6] ^ a[0][6];
	b[2][6] = M2[a[2][6]] ^ M3[a[3][6]] ^ a[0][6] ^ a[1][6];
	b[3][6] = M2[a[3][6]] ^ M3[a[0][6]] ^ a[1][6] ^ a[2][6];

	b[0][7] = M2[a[0][7]] ^ M3[a[1][7]] ^ a[2][7] ^ a[3][7];
	b[1][7] = M2[a[1][7]] ^ M3[a[2][7]] ^ a[3][7] ^ a[0][7];
	b[2][7] = M2[a[2][7]] ^ M3[a[3][7]] ^ a[0][7] ^ a[1][7];
	b[3][7] = M2[a[3][7]] ^ M3[a[0][7]] ^ a[1][7] ^ a[2][7];


	a[0][0]=b[0][0];a[1][0]=b[1][0];a[2][0]=b[2][0];a[3][0]=b[3][0];
	a[0][1]=b[0][1];a[1][1]=b[1][1];a[2][1]=b[2][1];a[3][1]=b[3][1];
	a[0][2]=b[0][2];a[1][2]=b[1][2];a[2][2]=b[2][2];a[3][2]=b[3][2];
	a[0][3]=b[0][3];a[1][3]=b[1][3];a[2][3]=b[2][3];a[3][3]=b[3][3];
	a[0][4]=b[0][4];a[1][4]=b[1][4];a[2][4]=b[2][4];a[3][4]=b[3][4];
	a[0][5]=b[0][5];a[1][5]=b[1][5];a[2][5]=b[2][5];a[3][5]=b[3][5];
	a[0][6]=b[0][6];a[1][6]=b[1][6];a[2][6]=b[2][6];a[3][6]=b[3][6];
	a[0][7]=b[0][7];a[1][7]=b[1][7];a[2][7]=b[2][7];a[3][7]=b[3][7];

}

// ----------------------------------------------------------------
// Mix the four bytes of every column in a linear way.
// This is the opposite operation of Mixcolumn.

static void inv_mix_column(
	word8 a[4][MAXBC],
	int   BC)
{
	word8 b[4][MAXBC];

	if (BC != 8) {
		int j;

		for (j = 0; j < BC; j++) {
			b[0][j] =
			  	Me[a[0][j]]
				^ Mb[a[1][j]]
				^ Md[a[2][j]]
				^ M9[a[3][j]];

			b[1][j] =
			  	Me[a[1][j]]
				^ Mb[a[2][j]]
				^ Md[a[3][j]]
				^ M9[a[0][j]];

			b[2][j] =
			  	Me[a[2][j]]
				^ Mb[a[3][j]]
				^ Md[a[0][j]]
				^ M9[a[1][j]];

			b[3][j] =
			  	Me[a[3][j]]
				^ Mb[a[0][j]]
				^ Md[a[1][j]]
				^ M9[a[2][j]];
		}

		for (j = 0; j < BC; j++) {
			a[0][j] = b[0][j];
			a[1][j] = b[1][j];
			a[2][j] = b[2][j];
			a[3][j] = b[3][j];
		}
		return;
	}

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

	b[0][4] = Me[a[0][4]] ^ Mb[a[1][4]] ^ Md[a[2][4]] ^ M9[a[3][4]];
	b[1][4] = Me[a[1][4]] ^ Mb[a[2][4]] ^ Md[a[3][4]] ^ M9[a[0][4]];
	b[2][4] = Me[a[2][4]] ^ Mb[a[3][4]] ^ Md[a[0][4]] ^ M9[a[1][4]]; 
	b[3][4] = Me[a[3][4]] ^ Mb[a[0][4]] ^ Md[a[1][4]] ^ M9[a[2][4]]; 

	b[0][5] = Me[a[0][5]] ^ Mb[a[1][5]] ^ Md[a[2][5]] ^ M9[a[3][5]];
	b[1][5] = Me[a[1][5]] ^ Mb[a[2][5]] ^ Md[a[3][5]] ^ M9[a[0][5]];
	b[2][5] = Me[a[2][5]] ^ Mb[a[3][5]] ^ Md[a[0][5]] ^ M9[a[1][5]]; 
	b[3][5] = Me[a[3][5]] ^ Mb[a[0][5]] ^ Md[a[1][5]] ^ M9[a[2][5]]; 

	b[0][6] = Me[a[0][6]] ^ Mb[a[1][6]] ^ Md[a[2][6]] ^ M9[a[3][6]];
	b[1][6] = Me[a[1][6]] ^ Mb[a[2][6]] ^ Md[a[3][6]] ^ M9[a[0][6]];
	b[2][6] = Me[a[2][6]] ^ Mb[a[3][6]] ^ Md[a[0][6]] ^ M9[a[1][6]]; 
	b[3][6] = Me[a[3][6]] ^ Mb[a[0][6]] ^ Md[a[1][6]] ^ M9[a[2][6]]; 

	b[0][7] = Me[a[0][7]] ^ Mb[a[1][7]] ^ Md[a[2][7]] ^ M9[a[3][7]];
	b[1][7] = Me[a[1][7]] ^ Mb[a[2][7]] ^ Md[a[3][7]] ^ M9[a[0][7]];
	b[2][7] = Me[a[2][7]] ^ Mb[a[3][7]] ^ Md[a[0][7]] ^ M9[a[1][7]]; 
	b[3][7] = Me[a[3][7]] ^ Mb[a[0][7]] ^ Md[a[1][7]] ^ M9[a[2][7]]; 


	a[0][0]=b[0][0];a[1][0]=b[1][0];a[2][0]=b[2][0];a[3][0]=b[3][0];
	a[0][1]=b[0][1];a[1][1]=b[1][1];a[2][1]=b[2][1];a[3][1]=b[3][1];
	a[0][2]=b[0][2];a[1][2]=b[1][2];a[2][2]=b[2][2];a[3][2]=b[3][2];
	a[0][3]=b[0][3];a[1][3]=b[1][3];a[2][3]=b[2][3];a[3][3]=b[3][3];
	a[0][4]=b[0][4];a[1][4]=b[1][4];a[2][4]=b[2][4];a[3][4]=b[3][4];
	a[0][5]=b[0][5];a[1][5]=b[1][5];a[2][5]=b[2][5];a[3][5]=b[3][5];
	a[0][6]=b[0][6];a[1][6]=b[1][6];a[2][6]=b[2][6];a[3][6]=b[3][6];
	a[0][7]=b[0][7];a[1][7]=b[1][7];a[2][7]=b[2][7];a[3][7]=b[3][7];

}

// ----------------------------------------------------------------
// Calculate the necessary round keys.
// The number of calculations depends on key_length_in_bits and
// block_length_in_bits.

int AES_key_schedule (
	word8 k[4][MAXKC],
	int   key_length_in_bits,
	int   block_length_in_bits,
	word8 W[MAXROUNDS+1][4][MAXBC])
{
	int KC, BC, ROUNDS;
	int i, j, t, rconpointer = 0;
	word8 tk[4][MAXKC];

	switch (key_length_in_bits) {
	case 128: KC = 4; break;
	case 192: KC = 6; break;
	case 256: KC = 8; break;
	default : return -1;
	}

	switch (block_length_in_bits) {
	case 128: BC = 4; break;
	case 192: BC = 6; break;
	case 256: BC = 8; break;
	default : return -2;
	}

	switch
		((key_length_in_bits >= block_length_in_bits)
			? key_length_in_bits
			: block_length_in_bits)
	{
	case 128: ROUNDS = 10; break;
	case 192: ROUNDS = 12; break;
	case 256: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}


	for (j = 0; j < KC; j++)
		for (i = 0; i < 4; i++)
			tk[i][j] = k[i][j];
	t = 0;
	// Copy values into round key array
	for (j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
		for (i = 0; i < 4; i++)
			W[t / BC][i][t % BC] = tk[i][j];

	while (t < (ROUNDS+1)*BC) { // while not enough round key material calculated
		// calculate new values
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

	return 0;
}

// ----------------------------------------------------------------
int AES_encrypt_block (
	word8 a[4][MAXBC],
	int   key_length_in_bits,
	int   block_length_in_bits,
	word8 rk[MAXROUNDS+1][4][MAXBC])
{
	// Encryption of one block.
	int r, BC, ROUNDS;

	switch (block_length_in_bits) {
	case 128: BC = 4; break;
	case 192: BC = 6; break;
	case 256: BC = 8; break;
	default : return -2;
	}

	switch
		((key_length_in_bits >= block_length_in_bits)
			? key_length_in_bits
			: block_length_in_bits)
	{
	case 128: ROUNDS = 10; break;
	case 192: ROUNDS = 12; break;
	case 256: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	// Begin with a key addition
	key_addition(a, rk[0], BC);

	// ROUNDS-1 ordinary rounds
	for (r = 1; r < ROUNDS; r++) {
		substitution(a, S, BC);
		shift_row(a, 0, BC);
		mix_column(a, BC);
		key_addition(a, rk[r], BC);
	}

	// Last round is special: there is no mix_column
	substitution(a, S, BC);
	shift_row(a, 0, BC);
	key_addition(a, rk[ROUNDS], BC);

	return 0;
}

// ----------------------------------------------------------------
int AES_decrypt_block (
	word8 a[4][MAXBC],
	int key_length_in_bits,
	int block_length_in_bits,
	word8 rk[MAXROUNDS+1][4][MAXBC])
{
	int r, BC, ROUNDS;

	switch (block_length_in_bits) {
	case 128: BC = 4; break;
	case 192: BC = 6; break;
	case 256: BC = 8; break;
	default : return -2;
	}

	switch
		((key_length_in_bits >= block_length_in_bits)
			? key_length_in_bits
			: block_length_in_bits)
	{
	case 128: ROUNDS = 10; break;
	case 192: ROUNDS = 12; break;
	case 256: ROUNDS = 14; break;
	default : return -3; // this cannot happen
	}

	// To decrypt: apply the inverse operations of the encrypt routine,
	// in opposite order.
	//
	// (key_addition is an involution: it's equal to its inverse.)
	// (The inverse of substitution with table S is substitution with the inverse table of S.)
	// (The inverse of shift_row is shift_row over a suitable distance.)

	// First the special round:
	// * Without inv_mix_column
	// * With extra key_addition
	key_addition(a, rk[ROUNDS], BC);
	substitution(a, Si, BC);
	shift_row(a, 1, BC);

	// ROUNDS-1 ordinary rounds
	for (r = ROUNDS-1; r > 0; r--) {
		key_addition(a, rk[r], BC);
		inv_mix_column(a, BC);
		substitution(a, Si, BC);
		shift_row(a, 1, BC);
	}

	// End with the extra key addition
	key_addition(a, rk[0], BC);

	return 0;
}
