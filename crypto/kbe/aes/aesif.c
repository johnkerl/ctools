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
#include "aesalg.h"
#include "blkcipht.h"
#include "util.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES 16
#define DEFAULT_KEY_LENGTH_IN_BYTES   16

// ----------------------------------------------------------------
typedef struct _AES_key_schedule_t {
	u8 rk[MAXROUNDS+1][4][MAXBC];
} AES_key_schedule_t;

static int valid_key_length(
	int               kl);

// ----------------------------------------------------------------
int AES_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	char   full_key_material[KBE_MAX_KL];
	AES_key_schedule_t * pks;
	u8 key_mat_block[4][MAXKC];
	int i;

	if (!valid_key_length(pciphinst->kl))
		return -4;

	pks = (AES_key_schedule_t *)malloc(sizeof(AES_key_schedule_t));
	if (pks == 0)
		return -2;
	pciphinst->pkey_schedule = pks;

	if (pciphinst->have_user_key_mat) {
		memcpy(full_key_material, pciphinst->user_key_mat,
			pciphinst->kl);
	}
	else {
		fill_region_from_string(full_key_material,
			pciphinst->kl, key_material);
	}

	for (i = 0; i < pciphinst->kl; i++)
		key_mat_block[i & 3][i >> 2] = (u8) full_key_material[i];

	if (pciphinst->alg_debug) {
		hex_dump_region("AES key material",
			full_key_material,
			pciphinst->kl,16);
	}

	AES_key_schedule(key_mat_block, pciphinst->kl,
		pciphinst->bl, pks->rk);

	return 1;
}

// ----------------------------------------------------------------
void AES_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	AES_key_schedule_t * pkey =
		(AES_key_schedule_t *)pciphinst->pkey_schedule;
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
static void bytes_to_block(
	int               bl,
	char            * input,
	u8             block[4][MAXBC])
{
	int j;
	int n = bl >> 2;
	int j2;

	for (j = 0; j < n; j++) {
		j2 = j << 2;
		block[0][j]  = 0xff & input[j2];
		block[1][j]  = 0xff & input[j2+1];
		block[2][j]  = 0xff & input[j2+2];
		block[3][j]  = 0xff & input[j2+3];
	}
}

// ----------------------------------------------------------------
static void block_to_bytes(
	int               bl,
	u8             block[4][MAXBC],
	char            * output)
{
	int j;
	int n = bl >> 2;
	int j2;

	for (j = 0; j < n; j++) {
		j2 = j << 2;
		output[j2  ] = (char) block[0][j];
		output[j2+1] = (char) block[1][j];
		output[j2+2] = (char) block[2][j];
		output[j2+3] = (char) block[3][j];

	}
}

// ----------------------------------------------------------------
int AES_encrypt_bytes(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	u8  block_in [4][MAXBC];
	u8  block_out[4][MAXBC];
	AES_key_schedule_t * pkey =
		(AES_key_schedule_t *)pciphinst->pkey_schedule;
	int    rv;

	bytes_to_block(pciphinst->bl, bytes_in, block_in);
	rv = AES_encrypt_block(block_in, block_out,
		pciphinst->kl,
		pciphinst->bl, pkey->rk);
	block_to_bytes(pciphinst->bl, block_out, bytes_out);

	return rv;
}

// ----------------------------------------------------------------
int AES_decrypt_bytes(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	u8  block_in [4][MAXBC];
	u8  block_out[4][MAXBC];
	AES_key_schedule_t * pkey =
		(AES_key_schedule_t *)pciphinst->pkey_schedule;
	int    rv;

	bytes_to_block(pciphinst->bl, bytes_in, block_in);
	rv = AES_decrypt_block(block_in, block_out,
		pciphinst->kl,
		pciphinst->bl, pkey->rk);
	block_to_bytes(pciphinst->bl, block_out, bytes_out);

	return rv;
}

// ----------------------------------------------------------------
static char * AES_explain_error(
	int code)
{
	return "not there yet";
}

// ----------------------------------------------------------------
static int AES_check_bl(
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
static int AES_check_kl(
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
block_cipher_t AES_cipher = {
	"AES",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	AES_encrypt_bytes,
	AES_decrypt_bytes,
	AES_make_key,
	AES_free_key,
	AES_check_bl,
	AES_check_kl,
	AES_explain_error,
};
