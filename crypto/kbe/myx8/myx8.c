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
// This is a silly little cipher I invented after learning about finite fields.
// (See http://johnkerl.org/doc/ffcomp.pdf for theoretical background.)  There
// are no claims to its solidity as a cipher.  It was just fun.  The name "myx"
// is because it uses matrix arithmetic over F_256 to mix up the bits in the
// plaintext.
// ================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "blkcipht.h"
#include "myx8tabs.h"
#include "myx8dat.h"
#include "myx8box.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES  16 // Really, 8 + 8
#define DEFAULT_KEY_LENGTH_IN_BYTES    64 // Really, 8 * 8
#define MYX8_MAX_KEY_DIM 16

#define NUM_ROUNDS 4

typedef unsigned char u8;

typedef struct _myx8_key_schedule_t {
	u8 mat[MYX8_MAX_KEY_DIM][MYX8_MAX_KEY_DIM];
} myx8_key_schedule_t;

// ----------------------------------------------------------------
#define MACRO_FEMUL
#ifdef MACRO_FEMUL
#define femul(a,b) \
 ((a) && (b) \
 ? myx8_exp_table[myx8_log_table[(u8)a] + myx8_log_table[(u8)b]] \
 : 0)

#else
static u8 femul(u8 a, u8 b)
{
	if (a && b)
		return myx8_exp_table[myx8_log_table[a] + myx8_log_table[b]];
	else
		return 0;
}
#endif

// ----------------------------------------------------------------
// Feistel forward:
// L := R
// R := L ^ f(R)
//
// Feistel reverse:
// L := R ^ f(L)
// R := L

static int myx8_encrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	myx8_key_schedule_t * pks = (myx8_key_schedule_t *)
		pciphinst->pkey_schedule;
	int r;
	int i;
	u8 * L;
	u8 * R;
	u8 Lt[KBE_MAX_BL/2];
	u8 Rt[KBE_MAX_BL/2];
	int bl  = pciphinst->bl;
	int blh = pciphinst->bl >> 1;

	for (i = 0; i < bl; i++)
		bytes_out[i] = bytes_in[i];
	L = (u8 *)&bytes_out[0];
	R = (u8 *)&bytes_out[blh];

	for (r = 0; r < NUM_ROUNDS; r += 2) {

		Lt[0] = R[0]; Rt[0] = L[0];
		Lt[1] = R[1]; Rt[1] = L[1];
		Lt[2] = R[2]; Rt[2] = L[2];
		Lt[3] = R[3]; Rt[3] = L[3];
		Lt[4] = R[4]; Rt[4] = L[4];
		Lt[5] = R[5]; Rt[5] = L[5];
		Lt[6] = R[6]; Rt[6] = L[6];
		Lt[7] = R[7]; Rt[7] = L[7];

		Rt[0]^=femul(pks->mat[0][0], myx8_S_box[R[0] + 0]);
		Rt[0]^=femul(pks->mat[0][1], myx8_S_box[R[1] + 1]);
		Rt[0]^=femul(pks->mat[0][2], myx8_S_box[R[2] + 2]);
		Rt[0]^=femul(pks->mat[0][3], myx8_S_box[R[3] + 3]);
		Rt[0]^=femul(pks->mat[0][4], myx8_S_box[R[4] + 4]);
		Rt[0]^=femul(pks->mat[0][5], myx8_S_box[R[5] + 5]);
		Rt[0]^=femul(pks->mat[0][6], myx8_S_box[R[6] + 6]);
		Rt[0]^=femul(pks->mat[0][7], myx8_S_box[R[7] + 7]);

		Rt[1]^=femul(pks->mat[1][0], myx8_S_box[R[0] + 0]);
		Rt[1]^=femul(pks->mat[1][1], myx8_S_box[R[1] + 1]);
		Rt[1]^=femul(pks->mat[1][2], myx8_S_box[R[2] + 2]);
		Rt[1]^=femul(pks->mat[1][3], myx8_S_box[R[3] + 3]);
		Rt[1]^=femul(pks->mat[1][4], myx8_S_box[R[4] + 4]);
		Rt[1]^=femul(pks->mat[1][5], myx8_S_box[R[5] + 5]);
		Rt[1]^=femul(pks->mat[1][6], myx8_S_box[R[6] + 6]);
		Rt[1]^=femul(pks->mat[1][7], myx8_S_box[R[7] + 7]);

		Rt[2]^=femul(pks->mat[2][0], myx8_S_box[R[0] + 0]);
		Rt[2]^=femul(pks->mat[2][1], myx8_S_box[R[1] + 1]);
		Rt[2]^=femul(pks->mat[2][2], myx8_S_box[R[2] + 2]);
		Rt[2]^=femul(pks->mat[2][3], myx8_S_box[R[3] + 3]);
		Rt[2]^=femul(pks->mat[2][4], myx8_S_box[R[4] + 4]);
		Rt[2]^=femul(pks->mat[2][5], myx8_S_box[R[5] + 5]);
		Rt[2]^=femul(pks->mat[2][6], myx8_S_box[R[6] + 6]);
		Rt[2]^=femul(pks->mat[2][7], myx8_S_box[R[7] + 7]);

		Rt[3]^=femul(pks->mat[3][0], myx8_S_box[R[0] + 0]);
		Rt[3]^=femul(pks->mat[3][1], myx8_S_box[R[1] + 1]);
		Rt[3]^=femul(pks->mat[3][2], myx8_S_box[R[2] + 2]);
		Rt[3]^=femul(pks->mat[3][3], myx8_S_box[R[3] + 3]);
		Rt[3]^=femul(pks->mat[3][4], myx8_S_box[R[4] + 4]);
		Rt[3]^=femul(pks->mat[3][5], myx8_S_box[R[5] + 5]);
		Rt[3]^=femul(pks->mat[3][6], myx8_S_box[R[6] + 6]);
		Rt[3]^=femul(pks->mat[3][7], myx8_S_box[R[7] + 7]);

		Rt[4]^=femul(pks->mat[4][0], myx8_S_box[R[0] + 0]);
		Rt[4]^=femul(pks->mat[4][1], myx8_S_box[R[1] + 1]);
		Rt[4]^=femul(pks->mat[4][2], myx8_S_box[R[2] + 2]);
		Rt[4]^=femul(pks->mat[4][3], myx8_S_box[R[3] + 3]);
		Rt[4]^=femul(pks->mat[4][4], myx8_S_box[R[4] + 4]);
		Rt[4]^=femul(pks->mat[4][5], myx8_S_box[R[5] + 5]);
		Rt[4]^=femul(pks->mat[4][6], myx8_S_box[R[6] + 6]);
		Rt[4]^=femul(pks->mat[4][7], myx8_S_box[R[7] + 7]);

		Rt[5]^=femul(pks->mat[5][0], myx8_S_box[R[0] + 0]);
		Rt[5]^=femul(pks->mat[5][1], myx8_S_box[R[1] + 1]);
		Rt[5]^=femul(pks->mat[5][2], myx8_S_box[R[2] + 2]);
		Rt[5]^=femul(pks->mat[5][3], myx8_S_box[R[3] + 3]);
		Rt[5]^=femul(pks->mat[5][4], myx8_S_box[R[4] + 4]);
		Rt[5]^=femul(pks->mat[5][5], myx8_S_box[R[5] + 5]);
		Rt[5]^=femul(pks->mat[5][6], myx8_S_box[R[6] + 6]);
		Rt[5]^=femul(pks->mat[5][7], myx8_S_box[R[7] + 7]);

		Rt[6]^=femul(pks->mat[6][0], myx8_S_box[R[0] + 0]);
		Rt[6]^=femul(pks->mat[6][1], myx8_S_box[R[1] + 1]);
		Rt[6]^=femul(pks->mat[6][2], myx8_S_box[R[2] + 2]);
		Rt[6]^=femul(pks->mat[6][3], myx8_S_box[R[3] + 3]);
		Rt[6]^=femul(pks->mat[6][4], myx8_S_box[R[4] + 4]);
		Rt[6]^=femul(pks->mat[6][5], myx8_S_box[R[5] + 5]);
		Rt[6]^=femul(pks->mat[6][6], myx8_S_box[R[6] + 6]);
		Rt[6]^=femul(pks->mat[6][7], myx8_S_box[R[7] + 7]);

		Rt[7]^=femul(pks->mat[7][0], myx8_S_box[R[0] + 0]);
		Rt[7]^=femul(pks->mat[7][1], myx8_S_box[R[1] + 1]);
		Rt[7]^=femul(pks->mat[7][2], myx8_S_box[R[2] + 2]);
		Rt[7]^=femul(pks->mat[7][3], myx8_S_box[R[3] + 3]);
		Rt[7]^=femul(pks->mat[7][4], myx8_S_box[R[4] + 4]);
		Rt[7]^=femul(pks->mat[7][5], myx8_S_box[R[5] + 5]);
		Rt[7]^=femul(pks->mat[7][6], myx8_S_box[R[6] + 6]);
		Rt[7]^=femul(pks->mat[7][7], myx8_S_box[R[7] + 7]);

#ifdef MYX8_DEBUG_ALG
		fprintf(stderr, "\n");
		hex_dump_region("MYX8 L", L, blh, blh);
		hex_dump_region("MYX8 R", R, blh, blh);
#endif // MYX8_DEBUG_ALG

		L[0] = Rt[0]; R[0] = Lt[0];
		L[1] = Rt[1]; R[1] = Lt[1];
		L[2] = Rt[2]; R[2] = Lt[2];
		L[3] = Rt[3]; R[3] = Lt[3];
		L[4] = Rt[4]; R[4] = Lt[4];
		L[5] = Rt[5]; R[5] = Lt[5];
		L[6] = Rt[6]; R[6] = Lt[6];
		L[7] = Rt[7]; R[7] = Lt[7];

		R[0]^= femul(pks->mat[0][0], myx8_S_box[Rt[0] + 0]);
		R[0]^= femul(pks->mat[0][1], myx8_S_box[Rt[1] + 1]);
		R[0]^= femul(pks->mat[0][2], myx8_S_box[Rt[2] + 2]);
		R[0]^= femul(pks->mat[0][3], myx8_S_box[Rt[3] + 3]);
		R[0]^= femul(pks->mat[0][4], myx8_S_box[Rt[4] + 4]);
		R[0]^= femul(pks->mat[0][5], myx8_S_box[Rt[5] + 5]);
		R[0]^= femul(pks->mat[0][6], myx8_S_box[Rt[6] + 6]);
		R[0]^= femul(pks->mat[0][7], myx8_S_box[Rt[7] + 7]);

		R[1]^= femul(pks->mat[1][0], myx8_S_box[Rt[0] + 0]);
		R[1]^= femul(pks->mat[1][1], myx8_S_box[Rt[1] + 1]);
		R[1]^= femul(pks->mat[1][2], myx8_S_box[Rt[2] + 2]);
		R[1]^= femul(pks->mat[1][3], myx8_S_box[Rt[3] + 3]);
		R[1]^= femul(pks->mat[1][4], myx8_S_box[Rt[4] + 4]);
		R[1]^= femul(pks->mat[1][5], myx8_S_box[Rt[5] + 5]);
		R[1]^= femul(pks->mat[1][6], myx8_S_box[Rt[6] + 6]);
		R[1]^= femul(pks->mat[1][7], myx8_S_box[Rt[7] + 7]);

		R[2]^= femul(pks->mat[2][0], myx8_S_box[Rt[0] + 0]);
		R[2]^= femul(pks->mat[2][1], myx8_S_box[Rt[1] + 1]);
		R[2]^= femul(pks->mat[2][2], myx8_S_box[Rt[2] + 2]);
		R[2]^= femul(pks->mat[2][3], myx8_S_box[Rt[3] + 3]);
		R[2]^= femul(pks->mat[2][4], myx8_S_box[Rt[4] + 4]);
		R[2]^= femul(pks->mat[2][5], myx8_S_box[Rt[5] + 5]);
		R[2]^= femul(pks->mat[2][6], myx8_S_box[Rt[6] + 6]);
		R[2]^= femul(pks->mat[2][7], myx8_S_box[Rt[7] + 7]);

		R[3]^= femul(pks->mat[3][0], myx8_S_box[Rt[0] + 0]);
		R[3]^= femul(pks->mat[3][1], myx8_S_box[Rt[1] + 1]);
		R[3]^= femul(pks->mat[3][2], myx8_S_box[Rt[2] + 2]);
		R[3]^= femul(pks->mat[3][3], myx8_S_box[Rt[3] + 3]);
		R[3]^= femul(pks->mat[3][4], myx8_S_box[Rt[4] + 4]);
		R[3]^= femul(pks->mat[3][5], myx8_S_box[Rt[5] + 5]);
		R[3]^= femul(pks->mat[3][6], myx8_S_box[Rt[6] + 6]);
		R[3]^= femul(pks->mat[3][7], myx8_S_box[Rt[7] + 7]);

		R[4]^= femul(pks->mat[4][0], myx8_S_box[Rt[0] + 0]);
		R[4]^= femul(pks->mat[4][1], myx8_S_box[Rt[1] + 1]);
		R[4]^= femul(pks->mat[4][2], myx8_S_box[Rt[2] + 2]);
		R[4]^= femul(pks->mat[4][3], myx8_S_box[Rt[3] + 3]);
		R[4]^= femul(pks->mat[4][4], myx8_S_box[Rt[4] + 4]);
		R[4]^= femul(pks->mat[4][5], myx8_S_box[Rt[5] + 5]);
		R[4]^= femul(pks->mat[4][6], myx8_S_box[Rt[6] + 6]);
		R[4]^= femul(pks->mat[4][7], myx8_S_box[Rt[7] + 7]);

		R[5]^= femul(pks->mat[5][0], myx8_S_box[Rt[0] + 0]);
		R[5]^= femul(pks->mat[5][1], myx8_S_box[Rt[1] + 1]);
		R[5]^= femul(pks->mat[5][2], myx8_S_box[Rt[2] + 2]);
		R[5]^= femul(pks->mat[5][3], myx8_S_box[Rt[3] + 3]);
		R[5]^= femul(pks->mat[5][4], myx8_S_box[Rt[4] + 4]);
		R[5]^= femul(pks->mat[5][5], myx8_S_box[Rt[5] + 5]);
		R[5]^= femul(pks->mat[5][6], myx8_S_box[Rt[6] + 6]);
		R[5]^= femul(pks->mat[5][7], myx8_S_box[Rt[7] + 7]);

		R[6]^= femul(pks->mat[6][0], myx8_S_box[Rt[0] + 0]);
		R[6]^= femul(pks->mat[6][1], myx8_S_box[Rt[1] + 1]);
		R[6]^= femul(pks->mat[6][2], myx8_S_box[Rt[2] + 2]);
		R[6]^= femul(pks->mat[6][3], myx8_S_box[Rt[3] + 3]);
		R[6]^= femul(pks->mat[6][4], myx8_S_box[Rt[4] + 4]);
		R[6]^= femul(pks->mat[6][5], myx8_S_box[Rt[5] + 5]);
		R[6]^= femul(pks->mat[6][6], myx8_S_box[Rt[6] + 6]);
		R[6]^= femul(pks->mat[6][7], myx8_S_box[Rt[7] + 7]);

		R[7]^= femul(pks->mat[7][0], myx8_S_box[Rt[0] + 0]);
		R[7]^= femul(pks->mat[7][1], myx8_S_box[Rt[1] + 1]);
		R[7]^= femul(pks->mat[7][2], myx8_S_box[Rt[2] + 2]);
		R[7]^= femul(pks->mat[7][3], myx8_S_box[Rt[3] + 3]);
		R[7]^= femul(pks->mat[7][4], myx8_S_box[Rt[4] + 4]);
		R[7]^= femul(pks->mat[7][5], myx8_S_box[Rt[5] + 5]);
		R[7]^= femul(pks->mat[7][6], myx8_S_box[Rt[6] + 6]);
		R[7]^= femul(pks->mat[7][7], myx8_S_box[Rt[7] + 7]);

#ifdef MYX8_DEBUG_ALG
		if (pciphinst->alg_debug) {
			fprintf(stderr, "\n");
			hex_dump_region("MYX8 L", L, blh, blh);
			hex_dump_region("MYX8 R", R, blh, blh);
		}
#endif // MYX8_DEBUG_ALG
	}

	return 1;
}

// ----------------------------------------------------------------
static int myx8_decrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	myx8_key_schedule_t * pks = (myx8_key_schedule_t *)
		pciphinst->pkey_schedule;
	int r;
	int i;
	u8 * L;
	u8 * R;
	u8 Lt[KBE_MAX_BL/2];
	u8 Rt[KBE_MAX_BL/2];
	int bl  = pciphinst->bl;
	int blh = pciphinst->bl >> 1;

	for (i = 0; i < bl; i++)
		bytes_out[i] = bytes_in[i];
	L = (u8 *)&bytes_out[0];
	R = (u8 *)&bytes_out[blh];

	for (r = 0; r < NUM_ROUNDS; r += 2) {

		Rt[0] = L[0]; Lt[0] = R[0];
		Rt[1] = L[1]; Lt[1] = R[1];
		Rt[2] = L[2]; Lt[2] = R[2];
		Rt[3] = L[3]; Lt[3] = R[3];
		Rt[4] = L[4]; Lt[4] = R[4];
		Rt[5] = L[5]; Lt[5] = R[5];
		Rt[6] = L[6]; Lt[6] = R[6];
		Rt[7] = L[7]; Lt[7] = R[7];

		Lt[0] ^= femul(pks->mat[0][0], myx8_S_box[L[0] + 0]);
		Lt[0] ^= femul(pks->mat[0][1], myx8_S_box[L[1] + 1]);
		Lt[0] ^= femul(pks->mat[0][2], myx8_S_box[L[2] + 2]);
		Lt[0] ^= femul(pks->mat[0][3], myx8_S_box[L[3] + 3]);
		Lt[0] ^= femul(pks->mat[0][4], myx8_S_box[L[4] + 4]);
		Lt[0] ^= femul(pks->mat[0][5], myx8_S_box[L[5] + 5]);
		Lt[0] ^= femul(pks->mat[0][6], myx8_S_box[L[6] + 6]);
		Lt[0] ^= femul(pks->mat[0][7], myx8_S_box[L[7] + 7]);

		Lt[1] ^= femul(pks->mat[1][0], myx8_S_box[L[0] + 0]);
		Lt[1] ^= femul(pks->mat[1][1], myx8_S_box[L[1] + 1]);
		Lt[1] ^= femul(pks->mat[1][2], myx8_S_box[L[2] + 2]);
		Lt[1] ^= femul(pks->mat[1][3], myx8_S_box[L[3] + 3]);
		Lt[1] ^= femul(pks->mat[1][4], myx8_S_box[L[4] + 4]);
		Lt[1] ^= femul(pks->mat[1][5], myx8_S_box[L[5] + 5]);
		Lt[1] ^= femul(pks->mat[1][6], myx8_S_box[L[6] + 6]);
		Lt[1] ^= femul(pks->mat[1][7], myx8_S_box[L[7] + 7]);

		Lt[2] ^= femul(pks->mat[2][0], myx8_S_box[L[0] + 0]);
		Lt[2] ^= femul(pks->mat[2][1], myx8_S_box[L[1] + 1]);
		Lt[2] ^= femul(pks->mat[2][2], myx8_S_box[L[2] + 2]);
		Lt[2] ^= femul(pks->mat[2][3], myx8_S_box[L[3] + 3]);
		Lt[2] ^= femul(pks->mat[2][4], myx8_S_box[L[4] + 4]);
		Lt[2] ^= femul(pks->mat[2][5], myx8_S_box[L[5] + 5]);
		Lt[2] ^= femul(pks->mat[2][6], myx8_S_box[L[6] + 6]);
		Lt[2] ^= femul(pks->mat[2][7], myx8_S_box[L[7] + 7]);

		Lt[3] ^= femul(pks->mat[3][0], myx8_S_box[L[0] + 0]);
		Lt[3] ^= femul(pks->mat[3][1], myx8_S_box[L[1] + 1]);
		Lt[3] ^= femul(pks->mat[3][2], myx8_S_box[L[2] + 2]);
		Lt[3] ^= femul(pks->mat[3][3], myx8_S_box[L[3] + 3]);
		Lt[3] ^= femul(pks->mat[3][4], myx8_S_box[L[4] + 4]);
		Lt[3] ^= femul(pks->mat[3][5], myx8_S_box[L[5] + 5]);
		Lt[3] ^= femul(pks->mat[3][6], myx8_S_box[L[6] + 6]);
		Lt[3] ^= femul(pks->mat[3][7], myx8_S_box[L[7] + 7]);

		Lt[4] ^= femul(pks->mat[4][0], myx8_S_box[L[0] + 0]);
		Lt[4] ^= femul(pks->mat[4][1], myx8_S_box[L[1] + 1]);
		Lt[4] ^= femul(pks->mat[4][2], myx8_S_box[L[2] + 2]);
		Lt[4] ^= femul(pks->mat[4][3], myx8_S_box[L[3] + 3]);
		Lt[4] ^= femul(pks->mat[4][4], myx8_S_box[L[4] + 4]);
		Lt[4] ^= femul(pks->mat[4][5], myx8_S_box[L[5] + 5]);
		Lt[4] ^= femul(pks->mat[4][6], myx8_S_box[L[6] + 6]);
		Lt[4] ^= femul(pks->mat[4][7], myx8_S_box[L[7] + 7]);

		Lt[5] ^= femul(pks->mat[5][0], myx8_S_box[L[0] + 0]);
		Lt[5] ^= femul(pks->mat[5][1], myx8_S_box[L[1] + 1]);
		Lt[5] ^= femul(pks->mat[5][2], myx8_S_box[L[2] + 2]);
		Lt[5] ^= femul(pks->mat[5][3], myx8_S_box[L[3] + 3]);
		Lt[5] ^= femul(pks->mat[5][4], myx8_S_box[L[4] + 4]);
		Lt[5] ^= femul(pks->mat[5][5], myx8_S_box[L[5] + 5]);
		Lt[5] ^= femul(pks->mat[5][6], myx8_S_box[L[6] + 6]);
		Lt[5] ^= femul(pks->mat[5][7], myx8_S_box[L[7] + 7]);

		Lt[6] ^= femul(pks->mat[6][0], myx8_S_box[L[0] + 0]);
		Lt[6] ^= femul(pks->mat[6][1], myx8_S_box[L[1] + 1]);
		Lt[6] ^= femul(pks->mat[6][2], myx8_S_box[L[2] + 2]);
		Lt[6] ^= femul(pks->mat[6][3], myx8_S_box[L[3] + 3]);
		Lt[6] ^= femul(pks->mat[6][4], myx8_S_box[L[4] + 4]);
		Lt[6] ^= femul(pks->mat[6][5], myx8_S_box[L[5] + 5]);
		Lt[6] ^= femul(pks->mat[6][6], myx8_S_box[L[6] + 6]);
		Lt[6] ^= femul(pks->mat[6][7], myx8_S_box[L[7] + 7]);

		Lt[7] ^= femul(pks->mat[7][0], myx8_S_box[L[0] + 0]);
		Lt[7] ^= femul(pks->mat[7][1], myx8_S_box[L[1] + 1]);
		Lt[7] ^= femul(pks->mat[7][2], myx8_S_box[L[2] + 2]);
		Lt[7] ^= femul(pks->mat[7][3], myx8_S_box[L[3] + 3]);
		Lt[7] ^= femul(pks->mat[7][4], myx8_S_box[L[4] + 4]);
		Lt[7] ^= femul(pks->mat[7][5], myx8_S_box[L[5] + 5]);
		Lt[7] ^= femul(pks->mat[7][6], myx8_S_box[L[6] + 6]);
		Lt[7] ^= femul(pks->mat[7][7], myx8_S_box[L[7] + 7]);

#ifdef MYX8_DEBUG_ALG
		fprintf(stderr, "\n");
		hex_dump_region("MYX8 L", L, blh, blh);
		hex_dump_region("MYX8 R", R, blh, blh);
#endif // MYX8_DEBUG_ALG

		R[0] = Lt[0]; L[0] = Rt[0];
		R[1] = Lt[1]; L[1] = Rt[1];
		R[2] = Lt[2]; L[2] = Rt[2];
		R[3] = Lt[3]; L[3] = Rt[3];
		R[4] = Lt[4]; L[4] = Rt[4];
		R[5] = Lt[5]; L[5] = Rt[5];
		R[6] = Lt[6]; L[6] = Rt[6];
		R[7] = Lt[7]; L[7] = Rt[7];

		L[0] ^= femul(pks->mat[0][0], myx8_S_box[Lt[0] + 0]);
		L[0] ^= femul(pks->mat[0][1], myx8_S_box[Lt[1] + 1]);
		L[0] ^= femul(pks->mat[0][2], myx8_S_box[Lt[2] + 2]);
		L[0] ^= femul(pks->mat[0][3], myx8_S_box[Lt[3] + 3]);
		L[0] ^= femul(pks->mat[0][4], myx8_S_box[Lt[4] + 4]);
		L[0] ^= femul(pks->mat[0][5], myx8_S_box[Lt[5] + 5]);
		L[0] ^= femul(pks->mat[0][6], myx8_S_box[Lt[6] + 6]);
		L[0] ^= femul(pks->mat[0][7], myx8_S_box[Lt[7] + 7]);

		L[1] ^= femul(pks->mat[1][0], myx8_S_box[Lt[0] + 0]);
		L[1] ^= femul(pks->mat[1][1], myx8_S_box[Lt[1] + 1]);
		L[1] ^= femul(pks->mat[1][2], myx8_S_box[Lt[2] + 2]);
		L[1] ^= femul(pks->mat[1][3], myx8_S_box[Lt[3] + 3]);
		L[1] ^= femul(pks->mat[1][4], myx8_S_box[Lt[4] + 4]);
		L[1] ^= femul(pks->mat[1][5], myx8_S_box[Lt[5] + 5]);
		L[1] ^= femul(pks->mat[1][6], myx8_S_box[Lt[6] + 6]);
		L[1] ^= femul(pks->mat[1][7], myx8_S_box[Lt[7] + 7]);

		L[2] ^= femul(pks->mat[2][0], myx8_S_box[Lt[0] + 0]);
		L[2] ^= femul(pks->mat[2][1], myx8_S_box[Lt[1] + 1]);
		L[2] ^= femul(pks->mat[2][2], myx8_S_box[Lt[2] + 2]);
		L[2] ^= femul(pks->mat[2][3], myx8_S_box[Lt[3] + 3]);
		L[2] ^= femul(pks->mat[2][4], myx8_S_box[Lt[4] + 4]);
		L[2] ^= femul(pks->mat[2][5], myx8_S_box[Lt[5] + 5]);
		L[2] ^= femul(pks->mat[2][6], myx8_S_box[Lt[6] + 6]);
		L[2] ^= femul(pks->mat[2][7], myx8_S_box[Lt[7] + 7]);

		L[3] ^= femul(pks->mat[3][0], myx8_S_box[Lt[0] + 0]);
		L[3] ^= femul(pks->mat[3][1], myx8_S_box[Lt[1] + 1]);
		L[3] ^= femul(pks->mat[3][2], myx8_S_box[Lt[2] + 2]);
		L[3] ^= femul(pks->mat[3][3], myx8_S_box[Lt[3] + 3]);
		L[3] ^= femul(pks->mat[3][4], myx8_S_box[Lt[4] + 4]);
		L[3] ^= femul(pks->mat[3][5], myx8_S_box[Lt[5] + 5]);
		L[3] ^= femul(pks->mat[3][6], myx8_S_box[Lt[6] + 6]);
		L[3] ^= femul(pks->mat[3][7], myx8_S_box[Lt[7] + 7]);

		L[4] ^= femul(pks->mat[4][0], myx8_S_box[Lt[0] + 0]);
		L[4] ^= femul(pks->mat[4][1], myx8_S_box[Lt[1] + 1]);
		L[4] ^= femul(pks->mat[4][2], myx8_S_box[Lt[2] + 2]);
		L[4] ^= femul(pks->mat[4][3], myx8_S_box[Lt[3] + 3]);
		L[4] ^= femul(pks->mat[4][4], myx8_S_box[Lt[4] + 4]);
		L[4] ^= femul(pks->mat[4][5], myx8_S_box[Lt[5] + 5]);
		L[4] ^= femul(pks->mat[4][6], myx8_S_box[Lt[6] + 6]);
		L[4] ^= femul(pks->mat[4][7], myx8_S_box[Lt[7] + 7]);

		L[5] ^= femul(pks->mat[5][0], myx8_S_box[Lt[0] + 0]);
		L[5] ^= femul(pks->mat[5][1], myx8_S_box[Lt[1] + 1]);
		L[5] ^= femul(pks->mat[5][2], myx8_S_box[Lt[2] + 2]);
		L[5] ^= femul(pks->mat[5][3], myx8_S_box[Lt[3] + 3]);
		L[5] ^= femul(pks->mat[5][4], myx8_S_box[Lt[4] + 4]);
		L[5] ^= femul(pks->mat[5][5], myx8_S_box[Lt[5] + 5]);
		L[5] ^= femul(pks->mat[5][6], myx8_S_box[Lt[6] + 6]);
		L[5] ^= femul(pks->mat[5][7], myx8_S_box[Lt[7] + 7]);

		L[6] ^= femul(pks->mat[6][0], myx8_S_box[Lt[0] + 0]);
		L[6] ^= femul(pks->mat[6][1], myx8_S_box[Lt[1] + 1]);
		L[6] ^= femul(pks->mat[6][2], myx8_S_box[Lt[2] + 2]);
		L[6] ^= femul(pks->mat[6][3], myx8_S_box[Lt[3] + 3]);
		L[6] ^= femul(pks->mat[6][4], myx8_S_box[Lt[4] + 4]);
		L[6] ^= femul(pks->mat[6][5], myx8_S_box[Lt[5] + 5]);
		L[6] ^= femul(pks->mat[6][6], myx8_S_box[Lt[6] + 6]);
		L[6] ^= femul(pks->mat[6][7], myx8_S_box[Lt[7] + 7]);

		L[7] ^= femul(pks->mat[7][0], myx8_S_box[Lt[0] + 0]);
		L[7] ^= femul(pks->mat[7][1], myx8_S_box[Lt[1] + 1]);
		L[7] ^= femul(pks->mat[7][2], myx8_S_box[Lt[2] + 2]);
		L[7] ^= femul(pks->mat[7][3], myx8_S_box[Lt[3] + 3]);
		L[7] ^= femul(pks->mat[7][4], myx8_S_box[Lt[4] + 4]);
		L[7] ^= femul(pks->mat[7][5], myx8_S_box[Lt[5] + 5]);
		L[7] ^= femul(pks->mat[7][6], myx8_S_box[Lt[6] + 6]);
		L[7] ^= femul(pks->mat[7][7], myx8_S_box[Lt[7] + 7]);

#ifdef MYX8_DEBUG_ALG
		fprintf(stderr, "\n");
		hex_dump_region("MYX8 L", L, blh, blh);
		hex_dump_region("MYX8 R", R, blh, blh);
#endif // MYX8_DEBUG_ALG
	}

	return 1;
}

// ----------------------------------------------------------------
static int myx8_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	myx8_key_schedule_t * pks;
	int   rv = 1;
	int   i, j, k;
	char * p;

	pks = (myx8_key_schedule_t *)malloc(sizeof(myx8_key_schedule_t));
	if (pks == 0)
		return -2;

	pciphinst->pkey_schedule = (void *)pks;

	if (pciphinst->have_user_key_mat) {
		memcpy(pks->mat, pciphinst->user_key_mat,
			pciphinst->kl);
	}
	else {
		p = key_material;
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 8; j++) {
				pks->mat[i][j] = *p;
				p++;
				if (!*p)
					p = key_material;
			}
		}
	}

	k = 0;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			pks->mat[i][j] ^= key_strengthener[k++];
		}
	}

	if (pciphinst->alg_debug) {
		if (rv == 1) {
			fprintf(stderr, "MYX8 key sched\n");
			for (i = 0; i < 8; i++) {
				fprintf(stderr, " ");
				for (j = 0; j < 8; j++) {
					fprintf(stderr, " %02x",
					((unsigned)pks->mat[i][j]) & 0xff);
				}
				fprintf(stderr, "\n");
			}
			fprintf(stderr, "\n");
		}
		else
			fprintf(stderr, "MYX8: key mat extraction failed.\n");
	}

	return rv;
}

// ----------------------------------------------------------------
static void myx8_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	myx8_key_schedule_t * pks = (myx8_key_schedule_t *)
		pciphinst->pkey_schedule;
	free(pks);
}

// ----------------------------------------------------------------
static int myx8_check_bl(
	int bl)
{
	switch (bl) {
	case  2:
	case  4:
	case  6:
	case  8:
	case 10:
	case 12:
	case 14:
	case 16:
		return 1;
	default:
		return -11;
	}
}

// ----------------------------------------------------------------
static int myx8_check_kl(
	int kl)
{
	switch (kl) {
	case  1:
	case  4:
	case  9:
	case 16:
	case 25:
	case 36:
	case 49:
	case 64:
		return 1;
	default:
		return -12;
	}
}

// ----------------------------------------------------------------
static char * myx8_explain_error(
	int    error_code)
{
	return "MYX8 does not yet feel the need to explain itself.";
}

// ----------------------------------------------------------------
block_cipher_t MYX8_cipher = {
	"MYX8",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	myx8_encrypt_block,
	myx8_decrypt_block,
	myx8_make_key,
	myx8_free_key,
	myx8_check_bl,
	myx8_check_kl,
	myx8_explain_error,
};
