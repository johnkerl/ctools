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
// This is a simple little C program to compute checksums in various
// ways.  There are already Unix md5sum and /usr/bin/sum.  I wrote
// this in my NT days; moreover, it was good clean fun.
// ================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsutil.h"

#include "csum.h"
#include "ethcrc.h"
#include "crca.h"
#include "murmur.h"
#include "net.h"
#include "floppy.h"
#include "md5.h"
#include "murmur.h"
#include "nop.h"

#define STYLE_IP     1
#define STYLE_ETH    2
#define STYLE_CRCA   3
#define STYLE_MURMUR 4
#define STYLE_FLOPPY 5
#define STYLE_MD5    6
#define STYLE_NOP    7

static void usage(char * argv0);
static void do_one_argument(char * file_name, int style, int do_spin);

static int  spin_check(void);
static void spin_last (void);

//----------------------------------------------------------------------
int main(int argc, char **argv)
{
	int argi;
	int style = STYLE_ETH; // default
	int do_spin  = 0;
	int got_files = 0;

	for (argi = 1; argi < argc; argi++) {
		if (strcmp(argv[argi], "--help") == 0)
			usage(argv[0]);

		if ((argv[argi][0] == '-') && argv[argi][1]) {
			if (strcmp(&argv[argi][1], "ip") == 0)
				style = STYLE_IP;
			else if (strcmp(&argv[argi][1], "eth") == 0)
				style = STYLE_ETH;
			else if (strcmp(&argv[argi][1], "crca") == 0)
				style = STYLE_CRCA;
			else if (strcmp(&argv[argi][1], "murmur") == 0)
				style = STYLE_MURMUR;
			else if (strcmp(&argv[argi][1], "floppy") == 0)
				style = STYLE_FLOPPY;
			else if (strcmp(&argv[argi][1], "md5") == 0)
				style = STYLE_MD5;
			else if (strcmp(&argv[argi][1], "nop") == 0)
				style = STYLE_NOP;
			else if (strcmp(&argv[argi][1], "spin") == 0)
				do_spin = 1;
			else if (strcmp(&argv[argi][1], "nospin") == 0)
				do_spin = 0;
			else
				usage(argv[0]);
			continue;
		}

		got_files++;
		do_one_argument(argv[argi], style, do_spin);
	}

	if (!got_files)
		do_one_argument("-", style, do_spin);

	return 0;
}

// ----------------------------------------------------------------
static void do_one_argument(char * file_name, int style, int do_spin)
{
	FILE * fp;
	int8u  buf[FILE_BUF_LEN];
	int32u len;
	int32u file_size = 0;

	int32u s32 = 0;
	int16u s16 = 0;
	int8u  h8  = 0;
	int8u  l8  = 0;
	MD5_state_t MD5_state;
	int8u MD5_digest[16];

	if (is_a_directory(file_name))
		return;

	if (strcmp(file_name, "-") == 0) {
		fp = stdin;
	}
	else {
		fp = fopen(file_name, "rb");
		if (fp == 0) {
			fprintf(stderr,
				"Couldn't open file \"%s\" for read.  Skipping.\n",
				file_name);
			return;
		}
	}

	switch (style) {
	case STYLE_IP:
		accum_net_CRC(0, 0, &s32, &s16, CSUM_PRE);
		break;
	case STYLE_ETH:
		accum_eth_CRC(0, 0, &s32, CSUM_PRE);
		break;
	case STYLE_CRCA:
		accum_CRCA(0, 0, &s32, CSUM_PRE);
		break;
	case STYLE_MURMUR:
		accum_murmur_hash_32(0, 0, &s32, CSUM_PRE);
		break;
	case STYLE_FLOPPY:
		accum_floppy_CRC(0, 0, &h8, &l8, CSUM_PRE);
		break;
	case STYLE_MD5:
		accum_MD5_sum(0, 0, &MD5_state, MD5_digest, CSUM_PRE);
		break;
	case STYLE_NOP:
		accum_nop_sum(0, 0, &s32, CSUM_PRE);
		break;
	}


	len = fread(buf, sizeof(char), FILE_BUF_LEN, fp);
	while (len > 0) {
		file_size += len;


		switch (style) {
		case STYLE_IP:
			accum_net_CRC(buf, len, &s32, &s16, CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r0x%04hx  %8d  %s",
					(int16u)s32, file_size, file_name);
				fflush(stdout);
			}
			break;
		case STYLE_ETH:
			accum_eth_CRC(buf, len, &s32, CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r0x%08x  %8d  %s",
					s32, file_size, file_name);
				fflush(stdout);
			}
			break;
		case STYLE_CRCA:
			accum_CRCA(buf, len, &s32, CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r0x%08x  %8d  %s",
					s32, file_size, file_name);
				fflush(stdout);
			}
			break;
		case STYLE_MURMUR:
			accum_murmur_hash_32(buf, len, &s32, CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r0x%08x  %8d  %s",
					s32, file_size, file_name);
				fflush(stdout);
			}
			break;
		case STYLE_FLOPPY:
			accum_floppy_CRC(buf, len, &h8, &l8, CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r0x%02x:0x%02x  %8d  %s",
					(int32u)h8, (int32u)l8,
					file_size, file_name);
				fflush(stdout);
			}
			break;
		case STYLE_MD5:
			accum_MD5_sum(buf, len, &MD5_state, MD5_digest,
				CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r%08x-%08x-%08x-%08x  %8d  %s",
					MD5_state.hash[0],
					MD5_state.hash[1],
					MD5_state.hash[2],
					MD5_state.hash[3],
					file_size, file_name);
				fflush(stdout);
			}
			break;
		case STYLE_NOP:
			accum_nop_sum(buf, len, &s32, CSUM_PERI);
			if (do_spin && spin_check()) {
				printf("\r%08x  %8d  %s",
					s32, file_size, file_name);
				fflush(stdout);
			}
			break;
		}

		len = fread(buf, sizeof(char), FILE_BUF_LEN, fp);
	}

	if (do_spin)
		spin_last();

	switch (style) {
	case STYLE_IP:
		accum_net_CRC(buf, len, &s32, &s16, CSUM_POST);
		printf("0x%04hx  %8d  %s\n", s16, file_size, file_name);
		break;
	case STYLE_ETH:
		accum_eth_CRC(0, 0, &s32, CSUM_POST);
		printf("0x%08x  %8d  %s\n", s32, file_size, file_name);
		break;
	case STYLE_CRCA:
		accum_CRCA(0, 0, &s32, CSUM_POST);
		printf("0x%08x  %8d  %s\n", s32, file_size, file_name);
		break;
	case STYLE_MURMUR:
		accum_murmur_hash_32(0, 0, &s32, CSUM_POST);
		printf("0x%08x  %8d  %s\n", s32, file_size, file_name);
		break;
	case STYLE_FLOPPY:
		accum_floppy_CRC(0, 0, &h8, &l8, CSUM_POST);
		printf("0x%02x:0x%02x  %8d  %s\n",
			(int32u)h8, (int32u)l8, file_size, file_name);
		break;
	case STYLE_MD5:
		accum_MD5_sum(0, 0, &MD5_state, MD5_digest, CSUM_POST);
		printf(
			"%02x%02x%02x%02x-%02x%02x%02x%02x-"
			"%02x%02x%02x%02x-%02x%02x%02x%02x"
			"  %8d  %s\n",
			MD5_digest[ 0], MD5_digest[ 1],
			MD5_digest[ 2], MD5_digest[ 3],
			MD5_digest[ 4], MD5_digest[ 5],
			MD5_digest[ 6], MD5_digest[ 7],
			MD5_digest[ 8], MD5_digest[ 9],
			MD5_digest[10], MD5_digest[11],
			MD5_digest[12], MD5_digest[13],
			MD5_digest[14], MD5_digest[15],
			file_size, file_name);
		break;
	case STYLE_NOP:
		accum_nop_sum(0, 0, &s32, CSUM_POST);
		printf("%08x  %8d  %s\n",
			s32, file_size, file_name);
		break;

	}

	if (fp != stdin)
		fclose(fp);
}

//----------------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s [algorithm] {files ...}\n", argv0);
	fprintf(stderr,
		"Algorithm is one of -ip, -eth, -crca, -murmur, -floppy, or -md5.\n");
	fprintf(stderr,
		"If omitted, defaults to -eth.\n");
	exit(1);
}

// ----------------------------------------------------------------
static int spin_check(void)
{
	static int32u fast = 0;
	static int32u slow = 0;

	if (++fast > 400) {
		fast = 0;
		slow++;
		return 1;
	}
	else {
		return 0;
	}
}

// ----------------------------------------------------------------
static void spin_last(void)
{
	printf("\r                     \r");
	fflush(stdout);
}
