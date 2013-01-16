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
#include "aesapi.h"

#include "util.h"

// ----------------------------------------------------------------
#define XOR_INPUT_YES 1
#define XOR_INPUT_NO  0

static void stream_to_block(
	int               block_length_in_bits,
	BYTE            * input,
	word8             block[4][MAXBC],
	int               do_xor_input);

static void block_to_stream(
	int               block_length_in_bits,
	word8             block[4][MAXBC],
	BYTE            * output);

static int valid_key_length(
	int               key_length_in_bits);

static int valid_block_length(
	int               block_length_in_bits);

// ----------------------------------------------------------------
int AES_make_key(
	key_instance * key,
	int            key_length_in_bits,
	int            block_length_in_bits,
	char         * key_material)
{
	word8 k[4][MAXKC];
	int i, j;

	if (key == NULL)
		return BAD_KEY_INSTANCE;

	if (valid_key_length(key_length_in_bits))
		key->key_length_in_bits = key_length_in_bits;
	else
		return BAD_KEY_MAT;

	if (key_material == NULL)
		return BAD_KEY_MAT;


	// Initialize key schedule
	for (i = 0; i < key->key_length_in_bits >> 3; i++)
		k[i & 3][i >> 2] = (word8) key_material[i];

	AES_key_schedule(k, key->key_length_in_bits,
		block_length_in_bits, key->key_sched);

	return TRUE;
}

// ----------------------------------------------------------------
int AES_encrypt(
	int               mode,
	int               block_length_in_bits,
	key_instance    * key,
	BYTE            * IV,
	BYTE            * input,
	int               input_length_in_bits,
	BYTE            * output)
{
	int i, num_blocks;
	word8 block[4][MAXBC];
	int progress_interval;
	int bl5 = block_length_in_bits >> 5;
	int bl3 = block_length_in_bits >> 3;

	// Check parameter consistency
	if (key == NULL)
		return BAD_KEY_MAT;

	if (!valid_key_length(key->key_length_in_bits))
		return BAD_KEY_MAT;

	if (mode != MODE_ECB && mode != MODE_CBC)
		return BAD_CIPHER_MODE;

	if (!valid_block_length(block_length_in_bits))
		return BAD_CIPHER_STATE;


	num_blocks = input_length_in_bits/block_length_in_bits;

	switch (mode) {
	case MODE_ECB:

		for (i = 0; i < num_blocks; i++) {
			stream_to_block(block_length_in_bits, input, block, XOR_INPUT_NO);
			AES_encrypt_block(block, key->key_length_in_bits,
				block_length_in_bits, key->key_sched);
			block_to_stream(block_length_in_bits, block, output);
			input  += bl3;
			output += bl3;
		}
		break;

	case MODE_CBC:

		stream_to_block(block_length_in_bits, IV, block, XOR_INPUT_NO);

		if (num_blocks < 10)
			progress_interval = 1;
		else if (num_blocks > 1000)
			progress_interval = num_blocks / 20;
		else
			progress_interval = num_blocks / 10;

		fprintf(stderr, "\n");
		for (i = 0; i < num_blocks; i++) {

			if (((i % progress_interval) == 0) || (i+1 == num_blocks)) {
				fprintf(stderr, "\r    %3d%%",
					(100 * (i+1)) / num_blocks);
				fflush(stdout);
			}

			stream_to_block(block_length_in_bits, input, block, XOR_INPUT_YES);
			AES_encrypt_block(block, key->key_length_in_bits,
				block_length_in_bits, key->key_sched);
			block_to_stream(block_length_in_bits, block, output);

			input  += bl3;
			output += bl3;
		}
		fprintf(stderr, "\n");
		break;

	default:
		return BAD_CIPHER_MODE;
	}

	return num_blocks * block_length_in_bits;
}

// ----------------------------------------------------------------
int AES_decrypt(
	int               mode,
	int               block_length_in_bits,
	key_instance    * key,
	BYTE            * IV,
	BYTE            * input,
	int               input_length_in_bits,
	BYTE            * output)
{
	int i, j, num_blocks;
	word8 block[4][MAXBC];
	int progress_interval;
	int bl5 = block_length_in_bits >> 5;
	int bl3 = block_length_in_bits >> 3;

	// Check parameter consistency
	if (key == NULL)
		return BAD_KEY_MAT;

	if (!valid_key_length(key->key_length_in_bits))
		return BAD_KEY_MAT;

	if (mode != MODE_ECB && mode != MODE_CBC)
		return BAD_CIPHER_MODE;

	if (!valid_block_length(block_length_in_bits))
		return BAD_CIPHER_STATE;


	num_blocks = input_length_in_bits/block_length_in_bits;

	switch (mode) {
	case MODE_ECB:
		for (i = 0; i < num_blocks; i++) {
			stream_to_block(block_length_in_bits, input, block, XOR_INPUT_NO);
			AES_decrypt_block(block, key->key_length_in_bits,
				block_length_in_bits, key->key_sched);
			block_to_stream(block_length_in_bits, block, output);
			input  += bl3;
			output += bl3;
		}
		break;

	case MODE_CBC:

		if (num_blocks < 10)
			progress_interval = 1;
		else if (num_blocks > 1000)
			progress_interval = num_blocks / 20;
		else
			progress_interval = num_blocks / 10;

		// First block
		stream_to_block(block_length_in_bits, input, block, XOR_INPUT_NO);
		AES_decrypt_block(block, key->key_length_in_bits,
			block_length_in_bits, key->key_sched);

		input  += bl3;

		// XOR the IV and parse rectangular array into output ciphertext bytes
		for (j = 0; j < bl5; j++) {
			int idx = j << 2;
			output[idx  ] = (BYTE) (block[0][j] ^ IV[  idx]);
			output[idx+1] = (BYTE) (block[1][j] ^ IV[1+idx]);
			output[idx+2] = (BYTE) (block[2][j] ^ IV[2+idx]);
			output[idx+3] = (BYTE) (block[3][j] ^ IV[3+idx]);
		}

		output += bl3;

		// next blocks
		fprintf(stderr, "\n");
		for (i = 1; i < num_blocks; i++) {

			if (((i % progress_interval) == 0) || (i+1 == num_blocks)) {
				fprintf(stderr, "\r    %3d%%", (100 * (i+1)) / num_blocks);
				fflush(stdout);
			}

			stream_to_block(block_length_in_bits, input, block, XOR_INPUT_NO);
			AES_decrypt_block(block, key->key_length_in_bits,
				block_length_in_bits, key->key_sched);

			// XOR previous ciphertext block and parse rectangular array
			// into output ciphertext bytes
			for (j = 0; j < bl5; j++) {
				int idxo = j << 2;
				int idxi = idxo - bl3;

				output[idxo+0] = (BYTE) (block[0][j] ^ input[idxi+0]);
				output[idxo+1] = (BYTE) (block[1][j] ^ input[idxi+1]);
				output[idxo+2] = (BYTE) (block[2][j] ^ input[idxi+2]);
				output[idxo+3] = (BYTE) (block[3][j] ^ input[idxi+3]);

			}
			input  += bl3;
			output += bl3;
		}
		fprintf(stderr, "\n");
		break;

	default: return BAD_CIPHER_MODE;
	}

	return num_blocks * block_length_in_bits;
}

// ----------------------------------------------------------------
// Parse input stream into rectangular array

static void stream_to_block(
	int               block_length_in_bits,
	BYTE            * input,
	word8             block[4][MAXBC],
	int               do_xor_input)
{
	int j;
	int n = block_length_in_bits >> 5;
	int j2;

	if (do_xor_input) {
		for (j = 0; j < n; j++) {
			j2 = j << 2;
			block[0][j] ^= 0xff & input[j2];
			block[1][j] ^= 0xff & input[j2+1];
			block[2][j] ^= 0xff & input[j2+2];
			block[3][j] ^= 0xff & input[j2+3];
		}
	}

	else {
		for (j = 0; j < n; j++) {
			j2 = j << 2;
			block[0][j]  = 0xff & input[j2];
			block[1][j]  = 0xff & input[j2+1];
			block[2][j]  = 0xff & input[j2+2];
			block[3][j]  = 0xff & input[j2+3];
		}
	}

}

// ----------------------------------------------------------------
// Parse rectangular array into output stream

static void block_to_stream(
	int               block_length_in_bits,
	word8             block[4][MAXBC],
	BYTE            * output)
{
	int j;
	int n = block_length_in_bits >> 5;
	int j2;

	for (j = 0; j < n; j++) {
		j2 = j << 2;
		output[j2  ] = (BYTE) block[0][j];
		output[j2+1] = (BYTE) block[1][j];
		output[j2+2] = (BYTE) block[2][j];
		output[j2+3] = (BYTE) block[3][j];

	}
}

// ----------------------------------------------------------------
static int valid_key_length(
	int key_length_in_bits)
{
	if (key_length_in_bits == 128)
		return 1;
	else if (key_length_in_bits == 192)
		return 1;
	else if (key_length_in_bits == 256)
		return 1;
	else
		return 0;
}

// ----------------------------------------------------------------
static int valid_block_length(
	int block_length_in_bits)
{
	if (block_length_in_bits == 128)
		return 1;
	else if (block_length_in_bits == 192)
		return 1;
	else if (block_length_in_bits == 256)
		return 1;
	else
		return 0;
}
