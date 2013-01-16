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
#include <string.h>
#include "aesalg.h"
#include "util.h"

static int debug = 0;

#define  LINE_BUF_SIZE 256

#define  RESET      0x00
#define  KPC_FLAGS  (HAVE_KEY | HAVE_PT | HAVE_CT)
#define  HAVE_KEY   0x01
#define  HAVE_IV    0x02
#define  HAVE_PT    0x04
#define  HAVE_CT    0x08
#define  ENCRYPTING 0x10
#define  DECRYPTING 0x20

#define  ST_0    RESET
#define  ST_K    HAVE_KEY
#define  ST_P    HAVE_PT
#define  ST_C    HAVE_CT
#define  ST_KP  (HAVE_KEY | HAVE_PT)
#define  ST_KC  (HAVE_KEY | HAVE_CT)
#define  ST_PC  (HAVE_PT  | HAVE_CT)
#define  ST_KPC (HAVE_KEY | HAVE_PT | HAVE_CT)

static char hex_digits_uc[] = "0123456789ABCDEF";

// ----------------------------------------------------------------
static int nybble_to_binary(
	char   ascii_nybble_in,
	char * byte_out)
{
	if ((ascii_nybble_in >= '0') && (ascii_nybble_in <= '9'))
		*byte_out = ascii_nybble_in - '0';
	else if ((ascii_nybble_in >= 'a') && (ascii_nybble_in <= 'f'))
		*byte_out = ascii_nybble_in - 'a' + 0xa;
	else if ((ascii_nybble_in >= 'A') && (ascii_nybble_in <= 'F'))
		*byte_out = ascii_nybble_in - 'A' + 0xa;
	else
		return 0;
	return 1;
}

// ----------------------------------------------------------------
static int ascii_to_binary(
	char * ascii_in,
	char * binary_out,
	int    num_nybbles)
{
	int    i;
	char * pa = ascii_in;
	char * pb = binary_out;
	char   left, right;

	for (i = 0; i < num_nybbles; i+= 2, pa+=2, pb++) {
		if (!nybble_to_binary(pa[0], &left)
		|| !nybble_to_binary(pa[1], &right)) {
			fprintf(stderr, "Parse error byte %d:\n", i);
			fprintf(stderr, "<<%s>>\n", ascii_in);
			return 0;
		}
		*pb = (left << 4) | right;
	}

	return 1;
}

// ----------------------------------------------------------------
static void bytes_to_block(
	int               block_length_in_bytes,
	char            * input,
	word8             block[4][MAXBC])
{
	int j;
	int n = block_length_in_bytes >> 2;
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
	int               block_length_in_bytes,
	word8             block[4][MAXBC],
	char            * output)
{
	int j;
	int n = block_length_in_bytes >> 2;
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
static void write_compare(
	char * desc,
	char * block,
	int    block_length_in_bytes)
{
	int i;

	fprintf(stderr, "<<%s=", desc);
	for (i = 0; i < block_length_in_bytes; i++)
		fprintf(stderr, "%c%c",
			hex_digits_uc[(block[i] >> 4) & 0x0f],
			hex_digits_uc[(block[i] >> 0) & 0x0f]);
	fprintf(stderr, ">>\n");
}

// ----------------------------------------------------------------
static void xor_blocks(
	char * a,
	char * b,
	char * x,
	int    n)
{
	int i;

	for (i = 0; i < n; i++)
		x[i] = a[i] ^ b[i];
}

// ----------------------------------------------------------------
int run_test(
	int    kl_bits,
	int    bl_bits,
	char * key,
	char * iv,
	char * exp_pt,
	char * exp_ct,
	char * act_pt,
	char * act_ct,
	int  * pstate)
{
	word8 key_mat[4][MAXKC];
	word8 rk[MAXROUNDS+1][4][MAXKC];
	word8 crypt_in [4][MAXBC];
	word8 crypt_out[4][MAXBC];
	char xor[64];
	int ok =  0;

	bytes_to_block(kl_bits / 8, key, key_mat);
	if (AES_key_schedule(key_mat, kl_bits/8, bl_bits / 8, rk) < 0) {
		fprintf(stderr,
		"Failed AES key sched kl=%d bl=%d.\n",
			kl_bits, bl_bits);
		exit(1);
	}

	if (*pstate & ENCRYPTING) {
		bytes_to_block(bl_bits / 8, exp_pt, crypt_in);
		if (debug >= 2) {
			write_compare("  ky", key,    kl_bits / 8);
			write_compare("  in", exp_pt, bl_bits / 8);
		}
		if (AES_encrypt_block(crypt_in, crypt_out,
			kl_bits / 8, bl_bits / 8, rk) < 0)
		{
			fprintf(stderr,
			"Failed AES encrypt kl=%d bl=%d.\n",
				kl_bits, bl_bits);
		}
		block_to_bytes(bl_bits / 8, crypt_out, act_ct);
		if (debug) {
			write_compare("  ou", act_ct, bl_bits / 8);
			write_compare("CA", act_ct, bl_bits / 8);
			write_compare("CT", exp_ct, bl_bits / 8);
			xor_blocks(act_ct, exp_ct, xor, bl_bits/8);
			write_compare("XR", xor, bl_bits / 8);
		}
		if (memcmp(act_ct, exp_ct, bl_bits/8) != 0) {
			if (debug)
				fprintf(stderr, "Fail.\n");
			ok = 0;
		}
		else {
			ok = 1;
		}
		*pstate &= ~HAVE_CT;
		*pstate &= ~ENCRYPTING;
	}
	else if (*pstate & DECRYPTING) {
		bytes_to_block(bl_bits / 8, exp_ct, crypt_in);
		if (debug >= 2) {
			write_compare("  ky", key,    kl_bits / 8);
			write_compare("  in", exp_ct, bl_bits / 8);
		}
		if (AES_decrypt_block(crypt_in, crypt_out,
			kl_bits / 8, bl_bits / 8, rk) < 0)
		{
			fprintf(stderr,
			"Failed AES encrypt kl=%d bl=%d.\n",
				kl_bits, bl_bits);
		}
		block_to_bytes(bl_bits / 8, crypt_out, act_pt);
		if (debug) {
			write_compare("  ou", act_pt, bl_bits / 8);
			write_compare("PA", act_pt, bl_bits / 8);
			write_compare("PT", exp_pt, bl_bits / 8);
			xor_blocks(act_pt, exp_pt, xor, bl_bits/8);
			write_compare("XR", xor, bl_bits / 8);
		}
		if (memcmp(act_pt, exp_pt, bl_bits/8) != 0) {
			if (debug)
				fprintf(stderr, "Fail.\n");
			ok = 0;
		}
		else {
			ok = 1;
		}
		*pstate &= ~HAVE_PT;
		*pstate &= ~DECRYPTING;
	}
	else {
		fprintf(stderr,
			"Coding error file %s line %d\n",
			__FILE__, __LINE__);
	}

	return ok;
}

// ----------------------------------------------------------------
static void foo(void)
{
	int  state = RESET;

	char line[LINE_BUF_SIZE];
	char key[64];
	char iv[64];
	char exp_pt[64];
	char exp_ct[64];
	char act_pt[64];
	char act_ct[64];

	int  kl_bits = 128;
	int  bl_bits = 128;
	int  ascii_len;
	int  nfail = 0;
	int  npass = 0;

	memset(key,     0xaa, sizeof(key));
	memset(iv,      0xbb, sizeof(iv));
	memset(exp_pt,  0xcc, sizeof(exp_pt));
	memset(exp_ct,  0xdd, sizeof(exp_ct));
	memset(act_pt,  0xee, sizeof(act_pt));
	memset(act_ct,  0xff, sizeof(act_ct));

	while (fgets(line, LINE_BUF_SIZE, stdin) != NULL) {
		ascii_len = strlen(line);
		if (line[ascii_len - 1] == '\n')
			line[ascii_len - 1] = 0;

		if (line[0] == '#')
			continue;

		if (strncmp(line, "KEY=", strlen("KEY=")) == 0) {
			ascii_len = strlen(&line[4]);
			if (!ascii_to_binary(&line[4], key, ascii_len))
				exit(1); // xxx
			kl_bits = ascii_len * 4;
			state |= HAVE_KEY;

			if (debug)
				fprintf(stderr, "<<%s>>\n", line);
		}

		else if (strncmp(line, "IV=", strlen("IV=")) == 0) {
			ascii_len = strlen(&line[3]);
			if (!ascii_to_binary(&line[3], iv, ascii_len))
				exit(1); // xxx
			bl_bits = ascii_len * 4;
			state |= HAVE_IV;

			if (debug)
				fprintf(stderr, "<<%s>>\n", line);
		}

		else if (strncmp(line, "PT=", strlen("PT=")) == 0) {
			ascii_len = strlen(&line[3]);
			if (!ascii_to_binary(&line[3], exp_pt, ascii_len))
				exit(1); // xxx
			bl_bits = ascii_len * 4;
			state |= HAVE_PT;

			if (debug)
				fprintf(stderr, "<<%s>>\n", line);
		}

		else if (strncmp(line, "CT=", strlen("CT=")) == 0) {
			ascii_len = strlen(&line[3]);
			
			if (!ascii_to_binary(&line[3], exp_ct, ascii_len))
				exit(1); // xxx
			bl_bits = ascii_len * 4;
			state |= HAVE_CT;

			if (debug)
				fprintf(stderr, "<<%s>>\n", line);
		}
		else {
			continue;
		}

		if (debug >= 2) {
			fprintf(stderr, "\n");
		}

		if (debug >= 2) {
			fprintf(stderr, "state %02x (", state);
			if (state == RESET)
				fprintf(stderr, "0");
			if (state & HAVE_KEY)
				fprintf(stderr, "K");
			if (state & HAVE_PT)
				fprintf(stderr, "P");
			if (state & HAVE_CT)
				fprintf(stderr, "C");
			if (state & ENCRYPTING)
				fprintf(stderr, "/E");
			if (state & DECRYPTING)
				 fprintf(stderr, "/D");
			 fprintf(stderr, ")\n");
		 }

		switch(state & KPC_FLAGS) {

		case  ST_0:
		case  ST_K:
		case  ST_P:
		case  ST_C:
			break;

		case  ST_KP:
			state |= ENCRYPTING;
			break;

		case  ST_KC:
			state |= DECRYPTING;
			break;

		case  ST_PC:
			fprintf(stderr, "Error: can't encrypt without key.\n");
			break;

		case  ST_KPC:
			if (run_test(kl_bits, bl_bits, key, iv,
			exp_pt, exp_ct, act_pt, act_ct, &state))
				npass++;
			else
				nfail++;

			break;

		}
	}
	fprintf(stderr, "# pass %5d   # fail %5d\n", npass, nfail);
}

// ----------------------------------------------------------------
int main(int argc, char ** argv)
{
	debug = argc - 1;
	foo();
	return 0;
}
