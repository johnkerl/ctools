#ifndef MURMUR_H
#define MURMUR_H

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

void accum_murmur_hash_32(
	int8u  * buf,
	int32u   len,
	int32u * pcsum,
	int32u   op);

#endif // MURMUR_H
