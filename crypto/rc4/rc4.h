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

#ifndef RC4_H
#define RC4_H

typedef struct _RC4_stream_t
{
	unsigned char si;
	unsigned char sj;
	unsigned char S[256];
} RC4_stream_t;

void RC4_stream_init(
	RC4_stream_t  * pstream,
	unsigned char * key,
	unsigned        key_length);

unsigned char RC4_stream_step(
	RC4_stream_t  * pstream);

#endif // RC4_H
