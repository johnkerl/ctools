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
#include "psdes.h"

int main(void)
{
	unsigned in0, in1, out0, out1;

	printf("Default seed:\n");
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	printf("\n");

	printf("Specified seed:\n");
	sran32b(0xdeadbeef, 0xcafefeed);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	iran64(&out0, &out1); printf("%08x %08x\n", out0, out1);
	printf("\n");

	printf("Specified seed:\n");
	in0 = 0;
	in1 = 0;
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	printf("\n");

	printf("Specified seed:\n");
	in0 = 1;
	in1 = 0;
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	printf("\n");

	printf("Specified seed:\n");
	in0 = 0x00000002;
	in1 = 0xfffffffc;
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	iran64_r(&out0, &out1, &in0, &in1);
	printf("%08x %08x -> %08x %08x\n", in0, in1, out0, out1);
	printf("\n");

	return 0;
}
