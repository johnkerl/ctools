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

#ifndef BLKCIPHS_H
#define BLKCIPHS_H

#include "blkcipht.h"

// ----------------------------------------------------------------
#define  OP_UNDEF   0
#define  OP_ENCRYPT 1
#define  OP_DECRYPT 2
#define  OP_CHECK   3

// ----------------------------------------------------------------
typedef struct _opts_t
{
	block_cipher_t * pcipher;
	block_cipher_instance_t ciphinst;
	char    * key_text;
	int       operation;
	int       first_file_argi;
	int       app_debug;
	int       do_crc;
	int       do_spin;
} opts_t;

// ----------------------------------------------------------------
extern block_cipher_t * pblock_ciphers[];
extern  int num_block_ciphers;

// ----------------------------------------------------------------
void block_cipher_select_default(
	block_cipher_t ** ppcipher,
	block_cipher_instance_t * pciphinst);

int block_cipher_select(
	char            * cipher_name,
	block_cipher_t ** ppcipher,
	block_cipher_instance_t * pciphinst);

void block_cipher_list(
	void);

int bc_handle_file_name(
	char      * file_name,
	opts_t    * popts);

int bc_handle_fp_pair(
	FILE      * fpin,
	FILE      * fpout,
	char      * fnamein,
	char      * fnameout,
	opts_t    * popts);

#endif // BLKCIPHS_H
