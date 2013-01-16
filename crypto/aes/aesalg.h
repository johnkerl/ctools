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

#ifndef AESALG_H
#define AESALG_H

#define MAXBC				(256/32)
#define MAXKC				(256/32)
#define MAXROUNDS			14

typedef unsigned char		word8;
typedef unsigned short		word16;
typedef unsigned long		word32;

int AES_key_schedule(
	word8 k[4][MAXKC],
	int   key_length_in_bits,
	int   block_length_in_bits,
	word8 rk[MAXROUNDS+1][4][MAXBC]);

int AES_encrypt_block(
	word8 a[4][MAXBC],
	int   key_length_in_bits,
	int   block_length_in_bits,
	word8 rk[MAXROUNDS+1][4][MAXBC]);

int AES_decrypt_block(
	word8 a[4][MAXBC],
	int   key_length_in_bits,
	int   block_length_in_bits,
	word8 rk[MAXROUNDS+1][4][MAXBC]);

#endif // AESALG_H
