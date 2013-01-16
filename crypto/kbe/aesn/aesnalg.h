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

#ifndef AESNALG_H
#define AESNALG_H

#define MAXBC                           (256/32)
#define MAXKC                           (256/32)
#define MAXROUNDS                       14

typedef unsigned char           word8;
//typedef unsigned short          word16;
typedef unsigned long           word32;

int AESN_key_schedule(
	word32 * k,
	int   kl,
	int   bl,
	word32 rk[MAXROUNDS+1][MAXBC]);

int AESN_encrypt_block(
	word32  * a,
	word32  * b,
	int    kl,
	int    bl,
	word32  rk[MAXROUNDS+1][MAXBC]);

int AESN_decrypt_block(
	word32  * a,
	word32  * b,
	int    kl,
	int    bl,
	word32  rk[MAXROUNDS+1][MAXBC]);

#define PACK4(w, b0, b1, b2, b3) \
	((w) = (((word32)(b0) << 24) & 0xff000000) | \
	(((word32)(b1) << 16) & 0x00ff0000) | \
	(((word32)(b2) <<  8) & 0x0000ff00) | \
	(((word32)(b3) <<  0) & 0x000000ff))

#define UNPACK4(w,b0,b1,b2,b3) \
	{ \
	(b0) = (word8)(0xff & ((w) >> 24)); \
	(b1) = (word8)(0xff & ((w) >> 16)); \
	(b2) = (word8)(0xff & ((w) >>  8)); \
	(b3) = (word8)(0xff & ((w))); \
	}


#endif // AESNALG_H
