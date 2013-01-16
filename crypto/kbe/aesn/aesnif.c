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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "aesnalg.h"
#include "blkcipht.h"
#include "util.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES 32
#define DEFAULT_KEY_LENGTH_IN_BYTES   32

static void bytes_to_block(
	int               bl,
	char            * input,
	word32            block[MAXBC]);

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

// ----------------------------------------------------------------
typedef struct _AESN_key_schedule_t {
	word32 rk[MAXROUNDS+1][MAXKC];
} AESN_key_schedule_t;

static int valid_key_length(
	int               kl);

// static int valid_block_length(
// 	int               bl);

// ----------------------------------------------------------------
int AESN_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	char   full_key_material[KBE_MAX_KL];
	AESN_key_schedule_t * pks;
	word32 key_mat_block[MAXKC];

	if (!valid_key_length(pciphinst->kl))
		return -4;

	pks = (AESN_key_schedule_t *)malloc(sizeof(AESN_key_schedule_t));
	if (pks == 0)
		return -2;
	pciphinst->pkey_schedule = pks;

	memset(pks, 0, sizeof(*pks));

	if (pciphinst->have_user_key_mat) {
		memcpy(full_key_material, pciphinst->user_key_mat,
			pciphinst->kl);
	}
	else {
		fill_region_from_string(full_key_material,
			pciphinst->kl, key_material);
	}

	bytes_to_block(pciphinst->kl, full_key_material, key_mat_block);

	AESN_key_schedule(key_mat_block, pciphinst->kl,
		pciphinst->bl, pks->rk);
	
	if (pciphinst->alg_debug) {
		hex_dump_region("AESN key material",
			full_key_material,
			pciphinst->kl,16);
		hex_dump_region("AESN key schedule",
			(char *)pks, sizeof(*pks), 16);
	}

	return 1;
}

// ----------------------------------------------------------------
void AESN_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	AESN_key_schedule_t * pkey =
		(AESN_key_schedule_t *)pciphinst->pkey_schedule;
	free(pkey);
}

// ----------------------------------------------------------------
static int valid_key_length(
	int kl)
{
	if (kl == 16)
		return 1;
	else if (kl == 24)
		return 1;
	else if (kl == 32)
		return 1;
	else
		return 0;
}

// ----------------------------------------------------------------
// static int valid_block_length(
// 	int bl)
// {
// 	if (bl == 16)
// 		return 1;
// 	else if (bl == 24)
// 		return 1;
// 	else if (bl == 32)
// 		return 1;
// 	else
// 		return 0;
// }

// ----------------------------------------------------------------
static void bytes_to_block(
	int               bl,
	char            * input,
	word32            block[MAXBC])
{
	int j;
	int n = bl >> 2;
	int j2;

	for (j = 0; j < n; j++) {
		j2 = j << 2;
		pack4(&block[j], input[j2], input[j2+1],
			input[j2+2], input[j2+3]);
	}
}

// ----------------------------------------------------------------
static void block_to_bytes(
	int               bl,
	word32            block[MAXBC],
	char            * output)
{
	int j;
	int n = bl >> 2;
	int j2;

	for (j = 0; j < n; j++) {
		j2 = j << 2;
		unpack4(block[j], &output[j2], &output[j2+1],
			&output[j2+2], &output[j2+3]);

	}
}

// ----------------------------------------------------------------
int AESN_encrypt_bytes(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	word32  block_in [MAXBC];
	word32  block_out[MAXBC];
	AESN_key_schedule_t * pkey =
		(AESN_key_schedule_t *)pciphinst->pkey_schedule;
	int    rv;

	bytes_to_block(pciphinst->bl, bytes_in, block_in);
	rv = AESN_encrypt_block(block_in, block_out,
		pciphinst->kl, pciphinst->bl, pkey->rk);
	block_to_bytes(pciphinst->bl, block_out, bytes_out);

	return rv;
}

// ----------------------------------------------------------------
int AESN_decrypt_bytes(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	word32 block_in [MAXBC];
	word32 block_out[MAXBC];
	AESN_key_schedule_t * pkey =
		(AESN_key_schedule_t *)pciphinst->pkey_schedule;
	int    rv;

	bytes_to_block(pciphinst->bl, bytes_in, block_in);
	rv = AESN_decrypt_block(block_in, block_out,
		pciphinst->kl, pciphinst->bl, pkey->rk);
	block_to_bytes(pciphinst->bl, block_out, bytes_out);

	return rv;
}

// ----------------------------------------------------------------
static int AESN_check_bl(
	int bl)
{
	switch (bl) {
	case 16:
	case 24:
	case 32:
		return 1;
	default:
		return -11;
	}
}

// ----------------------------------------------------------------
static int AESN_check_kl(
	int kl)
{
	switch (kl) {
	case 16:
	case 24:
	case 32:
		return 1;
	default:
		return -12;
	}
}

// ----------------------------------------------------------------
static char * AESN_explain_error(
	int code)
{
	return "not there yet";
}

// ----------------------------------------------------------------
block_cipher_t AESN_cipher = {
	"AESN",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	AESN_encrypt_bytes,
	AESN_decrypt_bytes,
	AESN_make_key,
	AESN_free_key,
	AESN_check_bl,
	AESN_check_kl,
	AESN_explain_error,
};
