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
#include "myxtabs.h"
#include "myxdat.h"
#include "myxbox.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES  16 // Really, 8 + 8
#define DEFAULT_KEY_LENGTH_IN_BYTES    64 // Really, 8 * 8
#define MYX_MAX_KEY_DIM 16

#define NUM_ROUNDS 4

typedef unsigned char u8;

typedef struct _myx_key_schedule_t {
	u8 mat[MYX_MAX_KEY_DIM][MYX_MAX_KEY_DIM];
} myx_key_schedule_t;

// ----------------------------------------------------------------
static u8 femul(u8 a, u8 b)
{
	if (a && b)
		return myx_exp_table[myx_log_table[a] + myx_log_table[b]];
	else
		return 0;
}

// ----------------------------------------------------------------
// Feistel forward:
// L := R
// R := L ^ f(R)
//
// Feistel reverse:
// L := R ^ f(L)
// R := L
//
// See also Bruce Schneier's _Applied Cryptography_.

static int myx_encrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	myx_key_schedule_t * pks = (myx_key_schedule_t *)
		pciphinst->pkey_schedule;
	int r;
	int i;
	int j;
	u8 * L;
	u8 * R;
	u8 L_temp[KBE_MAX_BL/2];
	u8 R_temp[KBE_MAX_BL/2];
	int bl  = pciphinst->bl;
	int blh = pciphinst->bl >> 1;

	for (i = 0; i < bl; i++)
		bytes_out[i] = bytes_in[i];
	L = (u8 *)&bytes_out[0];
	R = (u8 *)&bytes_out[blh];

	for (r = 0; r < NUM_ROUNDS; ) {
		for (i = 0; i < blh; i++) {
			L_temp[i] = R[i];
			R_temp[i] = L[i];
			for (j = 0; j < blh; j++)
				R_temp[i] ^= femul(
					pks->mat[i][j],
					myx_S_box[(R[j] + j) & 0xff]);
		}
		r++;
#ifdef MYX_DEBUG_ALG
		fprintf(stderr, "\n");
		hex_dump_region("MYX L", L, blh, blh);
		hex_dump_region("MYX R", R, blh, blh);
#endif // MYX_DEBUG_ALG

		for (i = 0; i < blh; i++) {
			L[i] = R_temp[i];
			R[i] = L_temp[i];
			for (j = 0; j < blh; j++)
				R[i] ^= femul(
					pks->mat[i][j],
					myx_S_box[(R_temp[j] + j) & 0xff]);
		}
		r++;
#ifdef MYX_DEBUG_ALG
		if (pciphinst->alg_debug) {
			fprintf(stderr, "\n");
			hex_dump_region("MYX L", L, blh, blh);
			hex_dump_region("MYX R", R, blh, blh);
		}
#endif // MYX_DEBUG_ALG
	}

	return 1;
}

// ----------------------------------------------------------------
static int myx_decrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	myx_key_schedule_t * pks = (myx_key_schedule_t *)
		pciphinst->pkey_schedule;
	int r;
	int i;
	int j;
	u8 * L;
	u8 * R;
	u8 L_temp[KBE_MAX_BL/2];
	u8 R_temp[KBE_MAX_BL/2];
	int bl  = pciphinst->bl;
	int blh = pciphinst->bl >> 1;

	for (i = 0; i < bl; i++)
		bytes_out[i] = bytes_in[i];
	L = (u8 *)&bytes_out[0];
	R = (u8 *)&bytes_out[blh];

	for (r = 0; r < NUM_ROUNDS; ) {
		for (i = 0; i < blh; i++) {
			R_temp[i] = L[i];
			L_temp[i] = R[i];
			for (j = 0; j < blh; j++)
				L_temp[i] ^= femul(
					pks->mat[i][j],
					myx_S_box[(L[j] + j) & 0xff]);
		}
		r++;
#ifdef MYX_DEBUG_ALG
		fprintf(stderr, "\n");
		hex_dump_region("MYX L", L, blh, blh);
		hex_dump_region("MYX R", R, blh, blh);
#endif // MYX_DEBUG_ALG

		for (i = 0; i < blh; i++) {
			R[i] = L_temp[i];
			L[i] = R_temp[i];
			for (j = 0; j < blh; j++)
				L[i] ^= femul(
					pks->mat[i][j],
					myx_S_box[(L_temp[j] + j) & 0xff]);
		}
		r++;
#ifdef MYX_DEBUG_ALG
		fprintf(stderr, "\n");
		hex_dump_region("MYX L", L, blh, blh);
		hex_dump_region("MYX R", R, blh, blh);
#endif // MYX_DEBUG_ALG
	}

	return 1;
}

// ----------------------------------------------------------------
static int myx_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	myx_key_schedule_t * pks;
	int   rv = 1;
	int   i, j, k;
	char * p;

	pks = (myx_key_schedule_t *)malloc(sizeof(myx_key_schedule_t));
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
			fprintf(stderr, "MYX key sched\n");
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
			fprintf(stderr, "MYX: key mat extraction failed.\n");
	}

	return rv;
}

// ----------------------------------------------------------------
static void myx_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	myx_key_schedule_t * pks = (myx_key_schedule_t *)
		pciphinst->pkey_schedule;
	free(pks);
}

// ----------------------------------------------------------------
static int myx_check_bl(
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
static int myx_check_kl(
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
static char * myx_explain_error(
	int    error_code)
{
	return "MYX does not yet feel the need to explain itself.";
}

// ----------------------------------------------------------------
block_cipher_t MYX_cipher = {
	"MYX",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	myx_encrypt_block,
	myx_decrypt_block,
	myx_make_key,
	myx_free_key,
	myx_check_bl,
	myx_check_kl,
	myx_explain_error,
};
