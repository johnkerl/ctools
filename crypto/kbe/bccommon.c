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
// Common utils for block ciphers, independent of algorithm.

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#endif
#ifdef UNIX
#include <netinet/in.h> // For ntohl/htonl
#endif
#include <sys/time.h>
#include "util.h"
#include "blkciphs.h"

// ----------------------------------------------------------------
#define  CT_MAGIC_STRING   "KBE"
#define  PT_MAGIC_STRING   "JRK"

// ----------------------------------------------------------------
// xxx block size must be at least trailer size!
// "nn_" signifies network byte order.
typedef struct _trailer_t
{
	char      pt_magic_string[4];
	unsigned  nn_pt_CRC;
	int       nn_bytes_in_last_pt_block;
} trailer_t;

// ----------------------------------------------------------------
static int bc_encrypt(
	FILE      * fpin,
	FILE      * fpout,
	char      * fnamein,
	char      * fnameout,
	opts_t    * popts);

static int bc_decrypt(
	FILE      * fpin,
	FILE      * fpout,
	char      * fnamein,
	char      * fnameout,
	opts_t    * popts);

static int bc_check(
	FILE      * fpin,
	char      * fnamein,
	opts_t    * popts);

// ----------------------------------------------------------------
// Generate some pseudo-random data, for example, encrypting a time stamp.
// Output from a random-number generator would also do.

static int make_IV(
	char        * IV,
	opts_t      * popts)
{
	int enc_rc;
#ifdef UNIX
	struct timeval time_of_day;

	gettimeofday(&time_of_day, 0);
#endif
#ifdef WINDOWS
	SYSTEMTIME time_of_day;

	GetSystemTime(&time_of_day);
#endif
	fill_region_from_other(IV, popts->ciphinst.bl,
		(char *)&time_of_day, sizeof(time_of_day));

	enc_rc = popts->pcipher->pencryptor(IV, IV, &popts->ciphinst);
	if (enc_rc != 1) {
		fprintf(stderr, "%s:  %s while creating IV.\n",
			popts->pcipher->cipher_name,
			popts->pcipher->perror_explainer(enc_rc));
		return 0;
	}

	strncpy(IV, CT_MAGIC_STRING, popts->ciphinst.bl / 2);

	return 1;
}

// ----------------------------------------------------------------
int bc_handle_file_name(
	char      * input_file_name,
	opts_t    * popts)

{
	FILE * fpin  = 0;
	FILE * fpout = 0;
	char   temp_file_name[256];
	char   unlink_file_name[256];

	sprintf(temp_file_name,   "%s.__tmp__",    input_file_name);
	sprintf(unlink_file_name, "%s.__unlink__", input_file_name);

	fpin = fopen(input_file_name, "rb");
	if (fpin == 0) {
		fprintf(stderr, "Couldn't open \"%s\" for read.\n",
			input_file_name);
		return 0;
	}

	if (popts->operation == OP_CHECK) {
		int rv = bc_check(fpin, input_file_name, popts);
		fclose(fpin);
		return rv;
	}

	fpout = fopen(temp_file_name, "wb");
	if (fpout == 0) {
		fprintf(stderr, "Couldn't open \"%s\" for write.\n",
			temp_file_name);
		fclose(fpin);
		return 0;
	}

	if (!bc_handle_fp_pair(fpin, fpout, input_file_name,
	temp_file_name, popts)) {
		fclose(fpin);
		fclose(fpout);
		fprintf(stderr, "Error %s file %s; operation not completed.\n",
			(popts->operation == OP_ENCRYPT) ? "encrypting"
				: "decrypting", input_file_name);
		if (unlink(temp_file_name) < 0) {
			fprintf(stderr,
			"Couldn't remove intermediate file %s either!\n",
				temp_file_name);
		}
		return 0;
	}
	fclose(fpin);
	fclose(fpout);

	if (rename(input_file_name, unlink_file_name) < 0) {
		fprintf(stderr, "Couldn't move \"%s\" to \"%s\".\n",
			input_file_name, unlink_file_name);
		perror("Reason");
		return 0;
	}

	if (rename(temp_file_name, input_file_name) < 0) {
		fprintf(stderr, "Couldn't move \"%s\" to \"%s\".\n",
			temp_file_name, input_file_name);
		perror("Reason");
		return 0;
	}

	if (unlink(unlink_file_name) < 0) {
		fprintf(stderr, "Couldn't unlink \"%s\".\n",
			unlink_file_name);
		perror("Reason");
		return 0;
	}

	return 1;
}

// ----------------------------------------------------------------
int bc_handle_fp_pair(
	FILE      * fpin,
	FILE      * fpout,
	char      * fnamein,
	char      * fnameout,
	opts_t    * popts)
{
	int rv = 0;

	switch (popts->operation) {
	case OP_ENCRYPT:
		rv = bc_encrypt(fpin, fpout, fnamein, fnameout, popts);
		break;
	case OP_DECRYPT:
		rv = bc_decrypt(fpin, fpout, fnamein, fnameout, popts);
		break;
	}
	return rv;
}

// ----------------------------------------------------------------
static void progress_first(void)
{
	fprintf(stderr, "        ");
}

static void progress_note(int count)
{
	fprintf(stderr, "\b\b\b\b\b\b\b\b%08x", count);
}

static void progress_last(void)
{
	fprintf(stderr, "\b\b\b\b\b\b\b\b        \b\b\b\b\b\b\b\b");
}

// ----------------------------------------------------------------
// Input file:
// * plaintext block 0
// * plaintext block 1
// * plaintext block 2
// * plaintext last block
//
// Augmented plaintext:
// * plaintext block 0
// * plaintext block 1
// * plaintext block 2
// * plaintext last block+padpadpad
// * trailer
//
// Ciphertext:
// * IV
// * encrypted block 0
// * encrypted block 1
// * encrypted block 2
// * encrypted last block
// * encrypted trailer

// ----------------------------------------------------------------
static int bc_encrypt(
	FILE   * fpin,
	FILE   * fpout,
	char   * fnamein,
	char   * fnameout,
	opts_t * popts)
{
	int    bl = popts->ciphinst.bl;
	char   pt_block[KBE_MAX_BL];
	char   ct_buffs[2][KBE_MAX_BL];
	char * chain    = &ct_buffs[0][0];
	char * ct_block = &ct_buffs[1][0];
	char * temp;
	int    enc_rc;
	int    num_bytes_read;
	int    num_bytes_written;
	trailer_t trailer;
	unsigned count = 0;
	unsigned crc = 0;

	if (popts->ciphinst.have_user_IV)
		memcpy(chain, popts->ciphinst.user_IV, bl);
	else {
		if (!make_IV(chain, popts)) {
			fprintf(stderr, "Couldn't make IV.\n");
			return 0;
		}
	}

	num_bytes_written = fwrite(chain, sizeof(char), bl, fpout);
	if (num_bytes_written < bl) {
		fprintf(stderr, "Write failed.\n");
		return 0;
	}

	if (popts->app_debug)
		hex_dump_region("app IV", chain, bl, 16);

	strcpy(trailer.pt_magic_string, PT_MAGIC_STRING);
	if (popts->do_crc)
		accum_eth_CRC(0, 0, &crc, CSUM_PRE);

	if (popts->do_spin)
		progress_first();

	do {
		num_bytes_read = fread(pt_block, sizeof(char), bl, fpin);
		if (popts->do_crc)
			accum_eth_CRC(pt_block, num_bytes_read,
				&crc, CSUM_PERI);
		if (num_bytes_read < bl) {
			trailer.nn_bytes_in_last_pt_block =
				htonl(num_bytes_read);
			memset(&pt_block[num_bytes_read], 'p',
				bl - num_bytes_read);
		}

		if (popts->app_debug) {
			fprintf(stderr, "\n");
			hex_dump_region("app pt", pt_block, bl, 16);
		}

		if (popts->do_spin) {
			count += num_bytes_read;
			if ((count & 0x0000ffff) == 0)
				progress_note(count);
		}

		// C_i = E_K(P_i  ^ C_{i-1})
		// P_i = D_K(C_i) ^ C_{i-1}

		if (popts->ciphinst.do_cbc)
			XOR_blocks(ct_block, pt_block, chain, bl);
		else
			memcpy(ct_block, pt_block, bl);

		enc_rc = popts->pcipher->pencryptor(ct_block, ct_block,
			&popts->ciphinst);
		if (enc_rc != 1) {
			fprintf(stderr, "%s:  %s while encrypting.\n",
				popts->pcipher->cipher_name,
				popts->pcipher->perror_explainer(enc_rc));
		}

		if (popts->app_debug)
			hex_dump_region("app ct", ct_block, bl, 16);

		num_bytes_written = fwrite(ct_block, sizeof(char), bl, fpout);
		if (num_bytes_written < bl) {
			fprintf(stderr, "Write failed.\n");
			return 0;
		}

		temp = chain;
		chain = ct_block;
		ct_block = temp;
	} while (num_bytes_read == bl);

	if (popts->do_crc)
		accum_eth_CRC(0, 0, &crc, CSUM_POST);

	trailer.nn_pt_CRC = htonl(crc);
	memset(pt_block, 't', bl);
	memcpy(pt_block, (char *)&trailer, sizeof(trailer));

	if (popts->app_debug) {
		fprintf(stderr, "\n");
		hex_dump_region("app pt", pt_block, bl, 16);
	}

	if (popts->ciphinst.do_cbc)
		XOR_blocks(ct_block, pt_block, chain, bl);
	else
		memcpy(ct_block, pt_block, bl);

	enc_rc = popts->pcipher->pencryptor(ct_block, ct_block,
		&popts->ciphinst);

	if (popts->do_spin)
		progress_last();

	if (enc_rc != 1) {
		fprintf(stderr, "%s:  %s while encrypting.\n",
			popts->pcipher->cipher_name,
			popts->pcipher->perror_explainer(enc_rc));
	}

	if (popts->app_debug)
		hex_dump_region("app ct", ct_block, bl, 16);

	num_bytes_written = fwrite(ct_block, sizeof(char), bl, fpout);
	if (num_bytes_written < bl) {
		fprintf(stderr, "Write failed.\n");
		return 0;
	}

	return 1;
}

// ----------------------------------------------------------------
// Ciphertext:
// * IV
// * encrypted block 0
// * encrypted block 1
// * encrypted block 2
// * encrypted last block
// * encrypted trailer
//
// Augmented plaintext:
// * plaintext block 0
// * plaintext block 1
// * plaintext block 2
// * plaintext last block+padpadpad
// * trailer
//
// Output file:
// * plaintext block 0
// * plaintext block 1
// * plaintext block 2
// * plaintext last block

// ----------------------------------------------------------------
static int bc_decrypt(
	FILE   * fpin,
	FILE   * fpout,
	char   * fnamein,
	char   * fnameout,
	opts_t * popts)
{
	int    bl = popts->ciphinst.bl;
	char   ct_buffs[3][KBE_MAX_BL];
	char   pt_buffs[3][KBE_MAX_BL];

	char * ct_prev  = &ct_buffs[0][0];
	char * ct_curr  = &ct_buffs[1][0];
	char * ct_next  = &ct_buffs[2][0];

	char * pt_prev2 = &pt_buffs[0][0];
	char * pt_prev1 = &pt_buffs[1][0];
	char * pt_curr  = &pt_buffs[2][0];
	char * temp;

	int    dec_rc;
	int    num_bytes_read;
	int    num_bytes_written;
	int    num_blocks = 0;
	trailer_t trailer;
	unsigned new_CRC;
	int    count = 0;

	num_bytes_read = fread(ct_prev, sizeof(char), bl, fpin);
	if (num_bytes_read != bl) {
		fprintf(stderr, "File \"%s\" has short first block.\n",
			fnamein);
		return 0;
	}

	if (popts->app_debug)
		hex_dump_region("app IV", ct_prev, bl, 16);

	num_bytes_read = fread(ct_curr, sizeof(char), bl, fpin);
	if (num_bytes_read != bl) {
		fprintf(stderr, "File \"%s\" has short first block.\n",
			fnamein);
		return 0;
	}

	num_bytes_read = fread(ct_next, sizeof(char), bl, fpin);
	if (num_bytes_read != bl) {
		fprintf(stderr, "File \"%s\" has short first block.\n",
			fnamein);
		return 0;
	}

	if (!popts->ciphinst.have_user_IV) {
		if (strncmp(CT_MAGIC_STRING, ct_prev,
		popts->ciphinst.bl / 2) != 0) {
		fprintf(stderr, "File \"%s\" missing ct magic string.\n",
				fnamein);
			return 0;
		}
	}

	if (popts->do_crc)
		accum_eth_CRC(0, 0, &new_CRC, CSUM_PRE);

	if (popts->do_spin)
		progress_first();

	while (1) {

		if (popts->app_debug) {
			fprintf(stderr, "\n");
			hex_dump_region("app ct", ct_curr, bl, 16);
		}

		dec_rc = popts->pcipher->pdecryptor(ct_curr, pt_curr,
			&popts->ciphinst);
		if (dec_rc != 1) {
			fprintf(stderr, "%s:  %s while decrypting.\n",
				popts->pcipher->cipher_name,
				popts->pcipher->perror_explainer(dec_rc));
		}

		if (popts->ciphinst.do_cbc)
			XOR_blocks(pt_curr, pt_curr, ct_prev, bl);

		if (popts->app_debug)
			hex_dump_region("app pt", pt_curr, bl, 16);

		count += bl;
		if (popts->do_spin) {
			if ((count & 0x0000ffff) == 0)
				progress_note(count);
		}

		if (num_blocks++ >= 2) {
			if (popts->do_crc)
				accum_eth_CRC(pt_prev2, bl, &new_CRC,
					CSUM_PERI);
			num_bytes_written = fwrite(pt_prev2, sizeof(char),
				bl, fpout);
			if (num_bytes_written < bl) {
				fprintf(stderr, "Write failed.\n");
				return 0;
			}
		}

		temp     = ct_prev;
		ct_prev  = ct_curr;
		ct_curr  = ct_next;
		ct_next  = temp;

		num_bytes_read = fread(ct_next, sizeof(char), bl, fpin);
		if (num_bytes_read == 0)
			break;
		if (num_bytes_read != bl) {
			fprintf(stderr,
				"Size of file \"%s\" is not a mutiple of "
				"block length.\n",
				fnamein);
			return 0;
		}

		temp     = pt_prev2;
		pt_prev2 = pt_prev1;
		pt_prev1 = pt_curr;
		pt_curr  = temp;

	}

	if (popts->do_spin)
		progress_last();

	if (num_blocks++ >= 2) {
		if (popts->do_crc)
			accum_eth_CRC(pt_prev1, bl, &new_CRC, CSUM_PERI);
		num_bytes_written = fwrite(pt_prev1, sizeof(char), bl, fpout);
		if (num_bytes_written < bl) {
			fprintf(stderr, "Write failed.\n");
			return 0;
		}
	}

	// Need to find out from the trailer how much of the last
	// plaintext block to write.  Also, need to verify the checksum.

	if (popts->app_debug) {
		fprintf(stderr, "\n");
		hex_dump_region("app ct", ct_curr, bl, 16);
	}

	dec_rc = popts->pcipher->pdecryptor(ct_curr, pt_prev2,
		&popts->ciphinst);
	if (dec_rc != 1) {
		fprintf(stderr, "%s:  %s while decrypting.\n",
			popts->pcipher->cipher_name,
			popts->pcipher->perror_explainer(dec_rc));
	}

	if (popts->ciphinst.do_cbc)
		XOR_blocks(pt_prev2, pt_prev2, ct_prev, bl);

	if (popts->app_debug)
		hex_dump_region("app pt", pt_prev2, bl, 16);

	memcpy(&trailer, pt_prev2, sizeof(trailer));

	if (strncmp(PT_MAGIC_STRING, trailer.pt_magic_string,
	strlen(PT_MAGIC_STRING)) != 0) {
		fprintf(stderr, "Key appears to be incorrect.\n");
		return 0;
	}

	if ((ntohl(trailer.nn_bytes_in_last_pt_block) < 0)  ||
	(ntohl(trailer.nn_bytes_in_last_pt_block) > bl)) {
		fprintf(stderr, "File \"%s\" has absurd last block size %d,\n",
			fnamein, ntohl(trailer.nn_bytes_in_last_pt_block));
		fprintf(stderr, "out of range 0 to %d.\n",
			bl);
		return 0;
	}

	if (popts->do_crc)
		accum_eth_CRC(pt_curr, ntohl(trailer.nn_bytes_in_last_pt_block),
			&new_CRC, CSUM_PERI);
	if (popts->do_crc)
		accum_eth_CRC(0, 0, &new_CRC, CSUM_POST);

	if (popts->do_crc) {
		unsigned expect_CRC = ntohl(trailer.nn_pt_CRC);
		if (new_CRC != expect_CRC) {
			fprintf(stderr,
	"Checksum error on file \"%s\".  Expected 0x%08x; got 0x%08x.\n",
				fnamein, expect_CRC, new_CRC);
		return 0;
		}
	}

	num_bytes_written = fwrite(pt_curr, sizeof(char),
		ntohl(trailer.nn_bytes_in_last_pt_block), fpout);
	if (num_bytes_written != ntohl(trailer.nn_bytes_in_last_pt_block)) {
		fprintf(stderr, "Write failed.\n");
		return 0;
	}

	return 1;
}

// ----------------------------------------------------------------
static int bc_check(
	FILE   * fpin,
	char   * fnamein,
	opts_t * popts)
{
	char   block[KBE_MAX_BL];
	int    num_bytes_read;
	int    rv = 0;
	int    ct_magic_len = strlen(CT_MAGIC_STRING);
	char   temp[128];

	num_bytes_read = fread(block, sizeof(char),
		KBE_MAX_BL, fpin);
	if (num_bytes_read < ct_magic_len + 1)
		rv = 0;
	else if (strncmp(block, CT_MAGIC_STRING, ct_magic_len) != 0)
		rv = 0;
	else
		rv = 1;

	if (rv == 1)
		sprintf(temp, "%s file:\n", popts->pcipher->cipher_name);
	else
		sprintf(temp, "Non-%s file:\n", popts->pcipher->cipher_name);

	printf("%-16s %s\n", temp, fnamein);

	return rv;
}
