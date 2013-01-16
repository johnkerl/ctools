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

#ifndef RC6ALG_H
#define RC6ALG_H

#define MAXBW  4
#define MAXKW  8
#define NR    20

typedef unsigned char u8;
typedef unsigned long u32;

int RC6_key_schedule(
	u32    k[MAXKW],
	int    kl,
	int    bl,
	u32    L[2 * NR + 4]);

int RC6_encrypt_block(
	u32    a[MAXBW],
	u32    b[MAXBW],
	int    kl,
	u32    L[2 * NR + 4]);

int RC6_decrypt_block(
	u32    a[MAXBW],
	u32    b[MAXBW],
	int    kl,
	u32    L[2 * NR + 4]);


void RC6_bytes_to_word(
	u8     b0,
	u8     b1,
	u8     b2,
	u8     b3,
	u32  * pw);

void RC6_bytes_to_block(
	int    num_bytes,
	char * input,
	u32    block[MAXBW]);

void RC6_word_to_bytes(
	u32    w,
	u8   * pb0,
	u8   * pb1,
	u8   * pb2,
	u8   * pb3);

void RC6_block_to_bytes(
	int    num_bytes,
	u32    block[MAXBW],
	char * output);

void RC6_print_block(
	char * msg,
	u32    a[MAXBW],
	int    BW);

#endif // RC6ALG_H
