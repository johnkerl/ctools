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

#ifndef AESAPI_H
#define AESAPI_H

// ----------------------------------------------------------------
#include <stdio.h>
#include "aesalg.h"

// ----------------------------------------------------------------
#define     DIR_ENCRYPT     0
#define     DIR_DECRYPT     1
#define     MODE_ECB        1
#define     MODE_CBC        2
#define     TRUE            1
#define     FALSE           0

//  Error codes
#define     BAD_KEY_MAT        -2  //  Key material not of correct length
#define     BAD_KEY_INSTANCE   -3  //  Key passed is not valid
#define     BAD_CIPHER_MODE    -4  //  Params passed to AES_encrypt/decrypt invalid
#define     BAD_CIPHER_STATE   -5  //  Cipher in wrong state (e.g., not initialized)

// ----------------------------------------------------------------
typedef    unsigned char    BYTE;

typedef struct _key_instance{
      int   key_length_in_bits;
      word8 key_sched[MAXROUNDS+1][4][MAXBC];
} key_instance;


// ----------------------------------------------------------------
int AES_make_key(
	key_instance    * key,
	int               key_length_in_bits,
	int               block_length_in_bits,
	char            * key_material);

int AES_encrypt(
	int               mode,
	int               block_length_in_bits,
	key_instance    * key,
	BYTE            * IV,
	BYTE            * input,
	int               input_length_in_bits,
	BYTE            * output);

int AES_decrypt(
	int               mode,
	int               block_length_in_bits,
	key_instance    * key,
	BYTE            * IV,
	BYTE            * input,
	int               input_length_in_bits,
	BYTE            * output);

// ----------------------------------------------------------------
#endif // AESAPI_H
