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

#ifndef AESFALG_H
#define AESFALG_H

#define MAXBC                   (256/32)
#define MAXKC                   (256/32)
#define MAXROUNDS               14

typedef unsigned char           u8;
typedef unsigned long           u32;

int AESF_key_schedule(
	u32 k[MAXKC],
	int    kl,
	int    bl,
	u32 rk[MAXROUNDS+1][MAXKC]);

int AESF_encrypt_block(
	u32 a[MAXBC],
	u32 b[MAXBC],
	int    kl,
	int    bl,
	u32 rk[MAXROUNDS+1][MAXKC]);

int AESF_decrypt_block(
	u32 a[MAXBC],
	u32 b[MAXBC],
	int    kl,
	int    bl,
	u32 rk[MAXROUNDS+1][MAXKC]);


void AESF_bytes_to_word(
	u8    b0,
	u8    b1,
	u8    b2,
	u8    b3,
	u32 * pw);

void AESF_bytes_to_block(
	int               num_bytes,
	char            * input,
	u32            block[MAXBC]);

void AESF_word_to_bytes(
	u32   w,
	u8  * pb0,
	u8  * pb1,
	u8  * pb2,
	u8  * pb3);

void AESF_block_to_bytes(
	int               num_bytes,
	u32            block[MAXBC],
	char            * output);

void AESF_print_block(
	char * msg,
	u32 a[MAXBC],
	int    BC);

#endif // AESFALG_H
