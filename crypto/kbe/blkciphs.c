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
#include <string.h>
#include "blkcipht.h"

extern block_cipher_t AESF_cipher;
extern block_cipher_t AES_cipher;
extern block_cipher_t RC6_cipher;
extern block_cipher_t MYX_cipher;
extern block_cipher_t MYX8_cipher;
extern block_cipher_t MYX8T_cipher;
extern block_cipher_t sfei_cipher;
extern block_cipher_t twiddle_cipher;
extern block_cipher_t binv_cipher;
extern block_cipher_t null_cipher;

block_cipher_t * pblock_ciphers[] = {
	// First in list is default.
	&AES_cipher,
	&AESF_cipher,
	&RC6_cipher,
	&MYX_cipher,
	&MYX8_cipher,
	&MYX8T_cipher,
	&sfei_cipher,
	&twiddle_cipher,
	&binv_cipher,
	&null_cipher,
};

int num_block_ciphers = sizeof(pblock_ciphers) / sizeof(pblock_ciphers[0]);

// ----------------------------------------------------------------
void block_cipher_select_default(
	block_cipher_t ** ppcipher,
	block_cipher_instance_t * pciphinst)
{
	*ppcipher = pblock_ciphers[0];
}

// ----------------------------------------------------------------
int block_cipher_select(
	char            * cipher_name,
	block_cipher_t ** ppcipher,
	block_cipher_instance_t * pciphinst)
{
	int i;
	int got = 0;

	for (i = 0; i < num_block_ciphers; i++) {
		if (strcasecmp(
			cipher_name, pblock_ciphers[i]->cipher_name) == 0)
		{
			*ppcipher = pblock_ciphers[i];
			got = 1;
		}
	}

	return got;
}

// ----------------------------------------------------------------
void block_cipher_list(void)
{
	int i;

	fprintf(stderr, "Available ciphers:\n");
	for (i = 0; i < num_block_ciphers; i++) {
		fprintf(stderr, "  %s\n", pblock_ciphers[i]->cipher_name);
	}
	fprintf(stderr, "\n");
}
