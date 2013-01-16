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

#include "blkcipht.h"

#define DEFAULT_BLOCK_LENGTH_IN_BYTES 32
#define DEFAULT_KEY_LENGTH_IN_BYTES    0 // No key

// ----------------------------------------------------------------
static int binv_encrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	int i;

	for (i = 0; i < pciphinst->bl; i++)
		*bytes_out++ = ~*bytes_in++;

	return 1;
}

// ----------------------------------------------------------------
static int binv_decrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	int i;

	for (i = 0; i < pciphinst->bl; i++)
		*bytes_out++ = ~*bytes_in++;

	return 1;
}

// ----------------------------------------------------------------
static int binv_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	return 1;
}

// ----------------------------------------------------------------
static void binv_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
}

// ----------------------------------------------------------------
static int binv_check_bl(
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
static int binv_check_kl(
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
static char * binv_explain_error(
	int    error_code)
{
	return "BINV cipher has nothing to explain.";
}

// ----------------------------------------------------------------
block_cipher_t binv_cipher = {
	"BINV",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	binv_encrypt_block,
	binv_decrypt_block,
	binv_make_key,
	binv_free_key,
	binv_check_bl,
	binv_check_kl,
	binv_explain_error,
};
