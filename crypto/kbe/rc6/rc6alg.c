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
#include <ctype.h>
#include "rc6alg.h"
#include "util.h"
#include "blkcipht.h"

#define P32 0xb7e15163
#define Q32 0x9e3779b9

// ----------------------------------------------------------------
void RC6_print_2(
	char * msg,
	u32    S1,
	u32    S2)
{
	u32  array[2];

	array[0] = S1;
	array[1] = S2;

	RC6_print_block(msg, array, 2);
}

// ----------------------------------------------------------------
void RC6_print_4(
	char * msg,
	u32    A,
	u32    B,
	u32    C,
	u32    D)
{
	u32  array[4];

	array[0] = A;
	array[1] = B;
	array[2] = C;
	array[3] = D;

	RC6_print_block(msg, array, 4);
}

// ----------------------------------------------------------------
void RC6_print_block(
	char * msg,
	u32  * a,
	int    BW)
{
	int i;
	u8 * pa = (u8 *)&a[0];

	fprintf(stderr, "%-6s: ", msg);
	for (i = 0; i < BW; i++)
		fprintf(stderr, "%08x ", (unsigned)a[i]);
	fprintf(stderr, "  ");
	for (i = 0; i < BW * 4; i++) {
		fprintf(stderr, "%c", isprint(pa[i]) ? pa[i] : '.');
	}

	fprintf(stderr, "\n");
}

//----------------------------------------------------------------
#ifdef DEBUG_RC6ALG
#define DEBUG_BLOCK(msg,blk,bc) RC6_print_block(msg,blk,bc)
#define DEBUG_2(msg,S1,S2) RC6_print_2(msg,S1,S2)
#define DEBUG_4(msg,A,B,C,D) RC6_print_4(msg,A,B,C,D)

#define DEBUG_TEXT(x) fprintf(stderr, "%s\n", x);
#define DEBUG_SEP fprintf(stderr, "\n")

#else
#define DEBUG_BLOCK(msg,blk,bc)
#define DEBUG_2(msg,S1,S2)
#define DEBUG_4(msg,A,B,C,D)
#define DEBUG_TEXT(x)
#define DEBUG_SEP
#endif

//----------------------------------------------------------------
void RC6_bytes_to_word(
	u8    b0,
	u8    b1,
	u8    b2,
	u8    b3,
	u32 * pw)
{
	*pw = (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

void RC6_bytes_to_block(
	int     num_bytes,
	char  * input,
	u32     block[MAXBW])
{
	int bi;
	int wi;

	for (bi = 0, wi = 0; bi < num_bytes; bi+=4, wi++) {
		RC6_bytes_to_word(
			input[bi],
			input[bi+1],
			input[bi+2],
			input[bi+3],
			&block[wi]);
	}
}

//----------------------------------------------------------------
void RC6_word_to_bytes(
	u32   w,
	u8  * pb0,
	u8  * pb1,
	u8  * pb2,
	u8  * pb3)
{
	*pb3 = w >> 24;
	*pb2 = w >> 16;
	*pb1 = w >>  8;
	*pb0 = w;
}

void RC6_block_to_bytes(
	int               num_bytes,
	u32            block[MAXBW],
	char            * output)
{
	int bi;
	int wi;

	for (bi = 0, wi = 0; bi < num_bytes; bi+=4, wi++) {
		RC6_word_to_bytes(
			block[wi],
			&output[bi],
			&output[bi+1],
			&output[bi+2],
			&output[bi+3]);
	}
}

//----------------------------------------------------------------
u32 rotl(u32 in, u32 shamt)
{
	shamt &= 31;
	if (shamt == 0)
		return in;
	return (in << shamt) | (in >> (32 - shamt));
}

u32 rotr(u32 in, u32 shamt)
{
	shamt &= 31;
	if (shamt == 0)
		return in;
	return (in >> shamt) | (in << (32 - shamt));
}

//----------------------------------------------------------------
int RC6_key_schedule (
	u32 L[MAXKW],
	int kl,
	int bl,
	u32 S[2 * NR + 4])
{
	u32 i, j, A, B, v, s;
	int c = kl >> 2;
	int maxw = 2 * NR + 4;

	S[0] = P32;
	for (i = 1; i < maxw; i++)
		S[i] = S[i-1] + Q32;

	A = B = i = j = 0;
	if (maxw > c)
		v = maxw;
	else
		v = c;
	v *= 3;

	for (s = 1; s <= v; s++) {
		A = S[i] = rotl((S[i] + A + B), 3);
		B = L[j] = rotl((L[j] + A + B), (A + B));
		i = (i + 1) % maxw;
		j = (j + 1) % c;
	}

	// xxx temp
	//for (i = 0; i < maxw; i++)
		//fprintf(stderr, "S[%2lu] = %08lx\n", i, S[i]);
	return 1;
}

// ----------------------------------------------------------------
int RC6_encrypt_block (
	u32 a[MAXBW],
	u32 b[MAXBW],
	int kl,
	u32 S[2 * NR + 4])
{
	int i;
	u32 A, B, C, D, u, t, sv;

	A = a[0];
	B = a[1];
	C = a[2];
	D = a[3];

	DEBUG_4("e in", A, B, C, D);

	B = B + S[0];
	D = D + S[1];

	DEBUG_2("e rk0", S[0], S[1]);
	DEBUG_4("e r0", A, B, C, D);

	for (i = 1; i <= NR; i++) {

		t = rotl(B * ((B << 1) + 1), 5);
		u = rotl(D * ((D << 1) + 1), 5);
		A = rotl((A ^ t), u) + S[i << 1];
		C = rotl((C ^ u), t) + S[(i << 1) + 1];
		sv = A;
		A = B;
		B = C;
		C = D;
		D = sv;

		DEBUG_2("e rk", S[i<<1], S[(i<<1)+1]);
		DEBUG_4("e ro", A, B, C, D);
	}

	A = A + S[ 2 * NR + 2];
	C = C + S[ 2 * NR + 3];
	DEBUG_2("e rkl", S[2 * NR + 2], S[2 * NR + 3]);
	DEBUG_4("e out", A, B, C, D);
	DEBUG_SEP;

	b[0] = A;
	b[1] = B;
	b[2] = C;
	b[3] = D;

	return 1;
}

// ----------------------------------------------------------------
int RC6_decrypt_block (
	u32 a[MAXBW],
	u32 b[MAXBW],
	int kl,
	u32 S[2 * NR + 4])
{
	int i;
	u32 A, B, C, D, u, t, sv;

	A = a[0];
	B = a[1];
	C = a[2];
	D = a[3];
	DEBUG_4("d in", A, B, C, D);

	C = C - S[2 * NR + 3];
	A = A - S[2 * NR + 2];
	DEBUG_2("d rkl", S[2 * NR + 2], S[2 * NR + 3]);
	DEBUG_4("d rl", A, B, C, D);

	for (i = NR; i >= 1; i--) {

		sv = D;
		D = C;
		C = B;
		B = A;
		A = sv;

		u = rotl(D * ((D << 1) + 1), 5);
		t = rotl(B * ((B << 1) + 1), 5);

		C = rotr((C - S[(i<<1)+1]), t) ^ u;
		A = rotr((A - S[(i<<1)  ]), u) ^ t;

		DEBUG_2("d rk", S[i<<1], S[(i<<1)+1]);
		DEBUG_4("d ro", A, B, C, D);
	}

	DEBUG_2("d rk0", S[0], S[1]);
	D = D - S[1];
	B = B - S[0];

	DEBUG_4("d out", A, B, C, D);
	DEBUG_SEP;

	b[0] = A;
	b[1] = B;
	b[2] = C;
	b[3] = D;

	return 1;
}
