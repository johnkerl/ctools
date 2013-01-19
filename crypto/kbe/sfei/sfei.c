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

// SFEI is a simple Feistel cipher (non-hard -- I am not a cryptanalyst) which
// I wrote for fun, after I learned of the elegant simplicity of Feistel
// networks (in particular, symmetry between encrypt and decrypt operations) in
// Bruce Schneier's _Applied Cryptography_.

// See also http://en.wikipedia.org/wiki/Feistel_cipher

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "blkcipht.h"

#define NR  4
#define HBL 4
#define BL  (HBL * 2)

#define DEFAULT_BLOCK_LENGTH_IN_BYTES BL
#define DEFAULT_KEY_LENGTH_IN_BYTES   (NR * HBL)

// ----------------------------------------------------------------
static void dump(
	char * desc,
	char * p,
	int    n)
{
	int i;

	fprintf(stderr, "%-4s: ", desc);
	for (i = 0; i < n; i++)
		fprintf(stderr, " %02x", ((unsigned) p[i]) & 0xff);
	fprintf(stderr, "\n");
}

// ----------------------------------------------------------------
static void dump3(
	char * desca,
	char * descb,
	char * descc,
	char * pa,
	char * pb,
	char * pc,
	int    na,
	int    nb,
	int    nc)
{
	int i;

	fprintf(stderr, "%-4s: ", desca);
	for (i = 0; i < na; i++)
		fprintf(stderr, " %02x", ((unsigned) pa[i]) & 0xff);
	fprintf(stderr, "    %-4s: ", descb);
	for (i = 0; i < nb; i++)
		fprintf(stderr, " %02x", ((unsigned) pb[i]) & 0xff);
	fprintf(stderr, "    %-4s: ", descc);
	for (i = 0; i < nc; i++)
		fprintf(stderr, " %02x", ((unsigned) pc[i]) & 0xff);
	fprintf(stderr, "\n");
}

// ----------------------------------------------------------------
static void f(
	char * fXK,
	char * X,
	char * rk)
{
	int i;
	for (i = 0; i < HBL; i++)
		fXK[i] = (X[i] << 1) ^ rk[i];
}

// ----------------------------------------------------------------
static int sfei_encrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	char * ks = (char *)pciphinst->pkey_schedule;
	char * rk = &ks[0];
	int r;
	int i;

	if (pciphinst->alg_debug)
		dump("blin", bytes_in, BL);

	for (i = 0; i < BL; i++)
		bytes_out[i] = bytes_in[i];

	for (r = 0; r < NR; r++) {
		char * L = &bytes_out[0];
		char * R = &bytes_out[HBL];
		char   Lt[HBL];
		char   Rt[HBL];
		char   fXK[HBL];

		f(fXK, R, rk);
		for (i = 0; i < HBL; i++) {
			Lt[i] = R[i];
			Rt[i] = L[i] ^ fXK[i];
		}

		for (i = 0; i < HBL; i++) {
			L[i] = Lt[i];
			R[i] = Rt[i];
		}
		if (pciphinst->alg_debug)
			dump3("rnd", "rk", "fXK",
				bytes_out, rk, fXK, BL, HBL, HBL);
		rk += HBL;
	}
	if (pciphinst->alg_debug)
		fprintf(stderr, "\n");

	return 1;
}

// ----------------------------------------------------------------
static int sfei_decrypt_block(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst)
{
	char * ks = (char *)pciphinst->pkey_schedule;
	char * rk = &ks[(NR - 1) * HBL];
	int r;
	int i;

	if (pciphinst->alg_debug)
		dump("blin", bytes_in, BL);

	for (i = 0; i < BL; i++)
		bytes_out[i] = bytes_in[i];

	for (r = 0; r < NR; r++) {
		char * L = &bytes_out[0];
		char * R = &bytes_out[HBL];
		char   Lt[HBL];
		char   Rt[HBL];
		char   fXK[HBL];

		f(fXK, L, rk);
		for (i = 0; i < HBL; i++) {
			Lt[i] = R[i] ^ fXK[i];
			Rt[i] = L[i];
		}

		for (i = 0; i < HBL; i++) {
			L[i] = Lt[i];
			R[i] = Rt[i];
		}
		if (pciphinst->alg_debug)
			dump3("rnd", "rk", "fXK",
				bytes_out, rk, fXK, BL, HBL, HBL);
		rk -= HBL;
	}
	if (pciphinst->alg_debug)
		fprintf(stderr, "\n");

	return 1;
}

// ----------------------------------------------------------------
static int sfei_make_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	char * ks;
	int    rv = 1;
	int    len;

	ks = (char *)malloc(sizeof(char) * NR * HBL);
	if (ks == 0)
		return -2;

	pciphinst->pkey_schedule = (void *)ks;

	len = strlen(key_material);
	fill_region_from_other(ks, NR * HBL, key_material, len);

	if (pciphinst->alg_debug)
		hex_dump_region("SFEI KS", ks, NR * HBL, HBL);

	return rv;
}

// ----------------------------------------------------------------
static void sfei_free_key(
	char  * key_material,
	block_cipher_instance_t * pciphinst)
{
	char * ks = (char *)pciphinst->pkey_schedule;
	free(ks);
}

// ----------------------------------------------------------------
static int sfei_check_bl(
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
static int sfei_check_kl(
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
static char * sfei_explain_error(
	int    error_code)
{
	return "SFEI does not yet feel the need to explain itself";
}

// ----------------------------------------------------------------
block_cipher_t sfei_cipher = {
	"SFEI",
	DEFAULT_BLOCK_LENGTH_IN_BYTES,
	DEFAULT_KEY_LENGTH_IN_BYTES,
	sfei_encrypt_block,
	sfei_decrypt_block,
	sfei_make_key,
	sfei_free_key,
	sfei_check_bl,
	sfei_check_kl,
	sfei_explain_error,
};
