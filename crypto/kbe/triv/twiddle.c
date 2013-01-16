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
#include "util.h"
#include "blkcipht.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES 32
#define DEFAULT_KEY_LENGTH_IN_BYTES    0 // Fixed-length key

// ----------------------------------------------------------------
static int twiddle_encrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	int single_xor = *(int *)pciphinst->pkey_schedule;
	int i;

	for (i = 0; i < pciphinst->bl; i++)
		*bytes_out++ = *bytes_in++ ^ single_xor;
	return 1;
}

// ----------------------------------------------------------------
static int twiddle_decrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	int single_xor = *(int *)pciphinst->pkey_schedule;
	int i;

	for (i = 0; i < pciphinst->bl; i++)
		*bytes_out++ = *bytes_in++ ^ single_xor;
	return 1;
}

// ----------------------------------------------------------------
static int twiddle_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	int * pxor;
	int   rv;

	pxor = (int *)malloc(sizeof(int));
	if (pxor == 0)
		return -2;

	*pxor = 0;
	pciphinst->pkey_schedule = (void *)pxor;

	if (sscanf(key_material, "%x", pxor) != 1)
		rv = -1;
	else
		rv = 1;

	if (pciphinst->alg_debug) {
		if (rv == 1)
			fprintf(stderr, "Twiddle: key mat \"%s\" -> 0x%02x\n",
				key_material, *pxor);
		else
			fprintf(stderr, "Twiddle: key mat extraction failed.\n");
	}

	return rv;
}

// ----------------------------------------------------------------
static void twiddle_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	int * pxor = (int *)pciphinst->pkey_schedule;
	free(pxor);
}

// ----------------------------------------------------------------
static int twiddle_check_bl(
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
static int twiddle_check_kl(
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
static char * twiddle_explain_error(
	int    error_code)
{
	return "Twiddle does not yet feel the need to explain itself";
}

// ----------------------------------------------------------------
block_cipher_t twiddle_cipher = {
	"TWID",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	twiddle_encrypt_block,
	twiddle_decrypt_block,
	twiddle_make_key,
	twiddle_free_key,
	twiddle_check_bl,
	twiddle_check_kl,
	twiddle_explain_error,
};
