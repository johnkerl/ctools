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
#include "rc6alg.h"
#include "blkcipht.h"
#include "util.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES 16
#define DEFAULT_KEY_LENGTH_IN_BYTES   16

// ----------------------------------------------------------------
typedef struct _RC6_key_schedule_t {
	u32 L[2 * NR + 4];
} RC6_key_schedule_t;

static int valid_key_length(
	int kl);

//static int valid_block_length(
//	int bl);

// ----------------------------------------------------------------
int RC6_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	char full_key_material[KBE_MAX_KL];
	RC6_key_schedule_t * pks;
	u32 key_mat_block[MAXKW];

	if (!valid_key_length(pciphinst->kl))
		return -4;

	pks = (RC6_key_schedule_t *)malloc(sizeof(RC6_key_schedule_t));
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

	RC6_bytes_to_block(pciphinst->kl, full_key_material, key_mat_block);

	if (pciphinst->alg_debug) {
		hex_dump_region("RC6 key material",
			full_key_material,
			pciphinst->kl,16);
	}

	RC6_key_schedule(key_mat_block, pciphinst->kl,
		pciphinst->bl, pks->L);

	return 1;
}

// ----------------------------------------------------------------
void RC6_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	RC6_key_schedule_t * pkey =
		(RC6_key_schedule_t *)pciphinst->pkey_schedule;
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
int RC6_encrypt_bytes(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	u32  block_in [MAXBW];
	u32  block_out[MAXBW];
	RC6_key_schedule_t * pkey =
		(RC6_key_schedule_t *)pciphinst->pkey_schedule;
	int    rv;

	RC6_bytes_to_block(pciphinst->bl, bytes_in, block_in);
	rv = RC6_encrypt_block(block_in, block_out,
		pciphinst->kl, pkey->L);
	RC6_block_to_bytes(pciphinst->bl, block_out, bytes_out);

	return rv;
}

// ----------------------------------------------------------------
int RC6_decrypt_bytes(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	u32  block_in [MAXBW];
	u32  block_out[MAXBW];
	RC6_key_schedule_t * pkey =
		(RC6_key_schedule_t *)pciphinst->pkey_schedule;
	int    rv;

	RC6_bytes_to_block(pciphinst->bl, bytes_in, block_in);
	rv = RC6_decrypt_block(block_in, block_out,
		pciphinst->kl, pkey->L);
	RC6_block_to_bytes(pciphinst->bl, block_out, bytes_out);

	return rv;
}

// ----------------------------------------------------------------
static char * RC6_explain_error(
	int code)
{
	return "not there yet";
}

// ----------------------------------------------------------------
static int RC6_check_bl(
	int bl)
{
	switch (bl) {
	case 16:
		return 1;
	default:
		return -11;
	}
}

// ----------------------------------------------------------------
static int RC6_check_kl(
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
block_cipher_t RC6_cipher = {
	"RC6",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	RC6_encrypt_bytes,
	RC6_decrypt_bytes,
	RC6_make_key,
	RC6_free_key,
	RC6_check_bl,
	RC6_check_kl,
	RC6_explain_error,
};
