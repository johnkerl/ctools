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

// ----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "fsutil.h"
#include "aesapi.h"

#include "util.h"

// ----------------------------------------------------------------
#define  BLOCK_SIZE_IN_BYTES 32
#define  KEY_SIZE_IN_BYTES   32

#define  MAGIC_NUMBER 0xa0e0b9df

#define  CT_HEADER    "AES"

// ----------------------------------------------------------------
static int my_AES_encrypt_file(
	key_instance    * pkey_instance,
	char            * input_file_name,
	char            * output_file_name,
	int               plaintext_file_size);

static int my_AES_decrypt_file(
	key_instance    * pkey_instance,
	char            * input_file_name,
	char            * output_file_name,
	int               ciphertext_file_size);

static void make_IV(
	BYTE            * IV,
	int               length_in_bytes,
	key_instance    * pkey_instance);

static int parse_command_line(
	int     argc,
	char ** argv,
	char ** pkey_text,
	int   * pforward,
	int   * pargind,
	int   * poutcat);

static void usage(
	char * argv0);

// ----------------------------------------------------------------
typedef struct _header_t
{
	int magic_number;
	int bytes_in_last_plaintext_block;
	unsigned CRC;
} header_t;

// ================================================================
int main(int argc, char **argv)
{
	int    forward;
	char * input_file_name;
	char * output_file_name;
	char * key_text;
	int    argind;
	int    outcat;
	char   key_material[KEY_SIZE_IN_BYTES];
	int    argi;
	int    file_size_success;
	int    file_size;
	key_instance AES_key_instance;
	int    make_key_rc;
	int    rv = 0;

	if (!parse_command_line(argc, argv, &key_text, &forward, &argind,
			&outcat))
		usage(argv[0]);

	fill_region_from_string(key_material, KEY_SIZE_IN_BYTES, key_text);
	make_key_rc = AES_make_key(&AES_key_instance, KEY_SIZE_IN_BYTES * 8,
		BLOCK_SIZE_IN_BYTES * 8, key_material);

	if (make_key_rc != TRUE) {
		fprintf(stderr, "AES_make_key() failure:  code %d.\n",
			make_key_rc);
		exit(1);
	}

	for (argi = argind; argi < argc; argi++) {

		input_file_name = argv[argi];
		if (outcat)
			output_file_name = "-";
		else
			output_file_name = input_file_name;

		file_size_success = get_file_size(input_file_name, &file_size);
		if (!file_size_success) {
			fprintf(stderr, "Couldn't open file \"%s\".\n",
				input_file_name);
			rv = 1;
			continue;
		}

		if (forward) {
			if (!my_AES_encrypt_file(&AES_key_instance,
				input_file_name, output_file_name, file_size))
				rv = 1;
		}
		else {
			if (!my_AES_decrypt_file(&AES_key_instance,
				input_file_name, output_file_name, file_size))
				rv = 1;
		}

	}

	fprintf(stderr, "Exit status %d.\n", rv);

	return rv;
}

// ----------------------------------------------------------------
static int parse_command_line(
	int     argc,
	char ** argv,
	char ** pkey_text,
	int   * pforward,
	int   * pargind,
	int   * poutcat)
{
	char * dirkey;

	*pargind = 2;
	*poutcat = 0;

	if (argc >= 2) {
		if (strcmp(argv[1], "cat") == 0) {
			*poutcat = 1;
			*pargind = 3;
		}
	}

	// 0   1      2    c = 3; ai = 2
	// aes +hello foo

	// 0   1   2      3    c = 4; ai = 3
	// aes cat +hello foo

	if ((argc - *pargind) < 1)
		return 0;

	dirkey = argv[*pargind - 1];
	*pkey_text = &dirkey[1];
	if (dirkey[0] == '+')
		*pforward = 1;
	else if (dirkey[0] == '-')
		*pforward = 0;
	else
		return 0;

	return 1;
}

// ----------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s {+key} {file name(s) ...\n", argv0);
	fprintf(stderr, "Or:    %s {-key} {file name(s) ...\n", argv0);
	exit(1);
}

// ----------------------------------------------------------------
// Input file:
// +--------------------------------+
// | plaintext block 0              |
// +--------------------------------+
// | plaintext block 1              |
// +--------------------------------+
// | plaintext block 2              |
// +----------------------+---------+
// | plaintext last block |
// +----------------------+
//
// Augmented plaintext in memory:
// +--------------------------------+
// | header                         |
// +--------------------------------+
// | plaintext block 0              |
// +--------------------------------+
// | plaintext block 1              |
// +--------------------------------+
// | plaintext block 2              |
// +--------------------------------+
// | plaintext last block+padpadpad |
// +--------------------------------+
//
// Ciphertext in memory (size is identical):
// +--------------------------------+
// | IV                             |
// +--------------------------------+
// | encrypted header               |
// +--------------------------------+
// | encrypted block 0              |
// +--------------------------------+
// | encrypted block 1              |
// +--------------------------------+
// | encrypted block 2              |
// +--------------------------------+
// | encrypted last block           |
// +--------------------------------+


static int my_AES_encrypt_file(
	key_instance   * pkey_instance,
	char           * input_file_name,
	char           * output_file_name,
	int              plaintext_file_size)
{
	int    augmented_plaintext_file_size = 0;
	int    ciphertext_file_size = 0;
	header_t header;
	char * input_buffer;
	char * output_buffer;
	char * pinblock0;  // Contains header
	char * pinblock1;  // Contains start of plaintext
	char * poutblock0; // Contains IV
	char * poutblock1; // Contains start of ciphertext
	int    encrypt_rc;
	BYTE   IV[BLOCK_SIZE_IN_BYTES];

// Round file size up to block size, then add twice block size for
// CBC IV and header.

	augmented_plaintext_file_size = round_up(plaintext_file_size,
		BLOCK_SIZE_IN_BYTES);
	augmented_plaintext_file_size += BLOCK_SIZE_IN_BYTES;
	ciphertext_file_size = augmented_plaintext_file_size +
		BLOCK_SIZE_IN_BYTES;

	input_buffer   = my_malloc(ciphertext_file_size);
	output_buffer  = my_malloc(ciphertext_file_size);

	memset(input_buffer,  'p', ciphertext_file_size);
	memset(output_buffer, 'c', ciphertext_file_size);

	pinblock0  = &input_buffer [0];
	pinblock1  = &input_buffer [BLOCK_SIZE_IN_BYTES];
	poutblock0 = &output_buffer[0];
	poutblock1 = &output_buffer[BLOCK_SIZE_IN_BYTES];

// Read file contents into blocks 1 and up.
	fprintf(stderr, "%s:\n", input_file_name);
	fprintf(stderr, "  Reading ... ", input_file_name); fflush(stderr);
	read_file_to_region(input_file_name, plaintext_file_size, pinblock1);
	fprintf(stderr, "  done.\n");

// Put file size mod block size in block 0, followed by checksum of plaintext.
	header.magic_number = MAGIC_NUMBER;
	header.bytes_in_last_plaintext_block =
		plaintext_file_size % BLOCK_SIZE_IN_BYTES;
	header.CRC = get_ethernet_CRC(pinblock1, plaintext_file_size);

	memset(pinblock0, 0, BLOCK_SIZE_IN_BYTES);
	memcpy(pinblock0, &header, sizeof(header));

// Put IV into block 0 of ciphertext region.

	make_IV(IV, BLOCK_SIZE_IN_BYTES, pkey_instance);
	memcpy(poutblock0, IV, BLOCK_SIZE_IN_BYTES);

// Encrypt in memory.
	fprintf(stderr, "  Encrypting ... "); fflush(stderr);
	encrypt_rc = AES_encrypt(MODE_CBC, BLOCK_SIZE_IN_BYTES * 8,
		pkey_instance, IV, pinblock0,
		augmented_plaintext_file_size * 8, poutblock1);
	if (encrypt_rc < 0) {
		fprintf(stderr, "\nAES_encrypt() failure:  code %d.\n",
			encrypt_rc);
		free(input_buffer);
		free(output_buffer);
		return 0;
	}
	fprintf(stderr, "  done.\n");

// Write ciphertext, overwriting original file.
	fprintf(stderr, "  Writing ... "); fflush(stderr);
	write_region_to_file(output_buffer, ciphertext_file_size,
		output_file_name);
	fprintf(stderr, "  done.\n");

	free(input_buffer);
	free(output_buffer);

	return 1;
}

// ----------------------------------------------------------------
static int my_AES_decrypt_file(
	key_instance   * pkey_instance,
	char           * input_file_name,
	char           * output_file_name,
	int              ciphertext_file_size)
{
	int    plaintext_file_size = 0;
	header_t header;
	unsigned new_CRC;
	char * input_buffer;
	char * output_buffer;
	char * pinblock0;  // Contains IV
	char * pinblock1;  // Contains start of ciphertext
	char * poutblock0; // Contains header
	char * poutblock1; // Contains start of plaintext
	int    decrypt_rc;
	BYTE   IV[BLOCK_SIZE_IN_BYTES];

// Allocate memory.
	input_buffer   = my_malloc(ciphertext_file_size);
	output_buffer  = my_malloc(ciphertext_file_size);

	memset(input_buffer,  'C', ciphertext_file_size);
	memset(output_buffer, 'P', ciphertext_file_size);

	pinblock0  = &input_buffer [0];
	pinblock1  = &input_buffer [BLOCK_SIZE_IN_BYTES];
	poutblock0 = &output_buffer[0];
	poutblock1 = &output_buffer[BLOCK_SIZE_IN_BYTES];

// Read ciphertext into memory.
	fprintf(stderr, "%s:\n", input_file_name);
	fprintf(stderr, "  Reading ... "); fflush(stderr);
	read_file_to_region(input_file_name, ciphertext_file_size,
		input_buffer);
	fprintf(stderr, "  done.\n");

	if (strncmp(CT_HEADER, input_buffer, strlen(CT_HEADER)) != 0) {
		fprintf(stderr,
"File \"%s\" appears not to have been encrypted with this program.\n",
			input_file_name);
		free(input_buffer);
		free(output_buffer);
		return 0;
	}

// Retrieve original IV from first block of file.
	memcpy(IV, pinblock0, BLOCK_SIZE_IN_BYTES);

// Decrypt in memory.
	fprintf(stderr, "  Decrypting ... "); fflush(stderr);
	decrypt_rc = AES_decrypt(MODE_CBC, BLOCK_SIZE_IN_BYTES * 8,
		pkey_instance, IV, pinblock1,
		(ciphertext_file_size - BLOCK_SIZE_IN_BYTES) * 8, poutblock0);
	if (decrypt_rc < 0) {
		fprintf(stderr, "\nAES_decrypt() failure:  code %d.\n",
			decrypt_rc);
		free(input_buffer);
		free(output_buffer);
		return 0;
	}
	fprintf(stderr, "  done.\n");

// Find number of bytes in last block and plaintext checksum from block 0 of
// decrypted data.  Don't overwrite file unless checksums match.
	memcpy(&header, poutblock0, sizeof(header));

	if (header.magic_number != MAGIC_NUMBER) {
		fprintf(stderr, "Key appears to be incorrect.\n");
		free(input_buffer);
		free(output_buffer);
		return 0;
	}

	if ((header.bytes_in_last_plaintext_block < 0)
	|| (header.bytes_in_last_plaintext_block >= BLOCK_SIZE_IN_BYTES)) {
		fprintf(stderr, "Header damaged in file \"%s\".\n",
			input_file_name);
		free(input_buffer);
		free(output_buffer);
		return 0;
	}

	plaintext_file_size = ciphertext_file_size - 2 * BLOCK_SIZE_IN_BYTES;
	if (header.bytes_in_last_plaintext_block != 0)
		plaintext_file_size -=
			(BLOCK_SIZE_IN_BYTES - header.bytes_in_last_plaintext_block);

	new_CRC = get_ethernet_CRC(poutblock1, plaintext_file_size);
	if (new_CRC != header.CRC) {
		fprintf(stderr,
	"Checksum error on file \"%s\".  Expected 0x%08x; got 0x%08x.\n",
			input_file_name, header.CRC, new_CRC);
		fprintf(stderr, "Leaving file as it was.\n");
		free(input_buffer);
		free(output_buffer);
		return 0;
	}

// Write plaintext, overwriting original file.

	fprintf(stderr, "  Writing ... "); fflush(stderr);
	write_region_to_file(poutblock1, plaintext_file_size, output_file_name);
	fprintf(stderr, "  done.\n");

	free(input_buffer);
	free(output_buffer);

	return 1;
}

// ----------------------------------------------------------------
// Generate some pseudo-random data, for example, encrypting a time stamp.
// Output from a random-number generator would also do.

static void make_IV(
	BYTE            * IV,
	int               length_in_bytes,
	key_instance    * pkey_instance)
{
	struct timeval time_of_day;

	gettimeofday(&time_of_day, 0);
	fill_region_from_other(IV, length_in_bytes,
		(BYTE *)&time_of_day, sizeof(time_of_day));

	AES_encrypt(MODE_ECB, length_in_bytes * 8, pkey_instance, IV,
		IV, length_in_bytes * 8, IV);

	strcpy(IV, CT_HEADER);
}
