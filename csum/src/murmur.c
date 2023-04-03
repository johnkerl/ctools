// https://github.com/jmhodges/murmur_hash/blob/master/ext/murmur/murmur_hash.cpp

//=============================================================================
// MurmurHash2, by Austin Appleby
//
// Note - This code makes a few assumptions about how your machine behaves:
//
// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4
//
// And it has a few limitations:
//
// 1. It will not work incrementally. (<--- Fixed, John Kerl, 2013-09-16)
// 2. It will not produce the same results on little-endian and big-endian machines.
//=============================================================================

#include <stdio.h>
#include <stdint.h>
#include "csum.h"

void accum_murmur_hash_32(
	int8u  * buf,
	int32u   len,
	int32u * pcsum,
	int32u   op)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.
	const int32u const_m = 0x5bd1e995;
	const int    const_r = 24;
	const int32u const_seed = 0xdeadbeef;

	int32u csum;

	switch (op) {

	case CSUM_PRE:
		// Initialize the hash to a 'random' value
		*pcsum = const_seed ^ len;
		break;

	case CSUM_PERI:
		csum = *pcsum;

		// Mix 4 bytes at a time into the hash
		while (len >= 4) {
			unsigned int k = *(unsigned int *)buf;

			k *= const_m;
			k ^= k >> const_r;
			k *= const_m;

			csum *= const_m;
			csum ^= k;

			buf += 4;
			len -= 4;
		}

		// Handle the last few bytes of the input array
		switch(len) {
		case 3: csum ^= buf[2] << 16;
	    case 2: csum ^= buf[1] << 8;
	    case 1: csum ^= buf[0]; csum *= const_m;
	    }

		*pcsum = csum;
		break;

	case CSUM_POST:
		csum = *pcsum;
		// Do a few final mixes of the hash to ensure the last few
		// bytes are well-incorporated.
		*pcsum ^= *pcsum >> 13;
		*pcsum *= const_m;
		*pcsum ^= *pcsum >> 15;

		*pcsum = csum;
		break;
	}
}
