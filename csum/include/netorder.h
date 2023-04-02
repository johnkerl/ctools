// ================================================================
// Copyright (c) 2001 John Kerl.
//
// This code and information is provided as is without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.
//
// No restrictions are placed on copy or reuse of this code, as long
// as these paragraphs are included in the code.
// ================================================================

#ifndef NETORDER_H
#define NETORDER_H

// ================================================================
// NOTE:
//
// Change only the following lines in this file to suit your processor: Comment
// out the #define for big-endian systems, or uncomment it for little-endian
// systems.  Also, make sure that NE_16 and NE_32 are really 16-bit and 32-bit
// unsigned integers, respectively, for your system.

#define PROCESSOR_IS_LITTLE_ENDIAN
typedef unsigned short  NE_16;
typedef unsigned long   NE_32;

// ================================================================
// "Network" byte ordering is the same as big-endianness.
// "Host"    byte ordering is the same as the processor endianness.
//
// So, for big-endian processors, converting back and forth between network and
// host ordering is a no-op.  For little-endian processors, the conversion
// simply amounts to reversing bytes.
//
// Macros provided by this file:
//
// * ntohs -- Network-to-host ordering conversion, for shorts (16-bit integers).
// * htons -- Host-to-network ordering conversion, for shorts (16-bit integers).
// * ntohl -- Network-to-host ordering conversion, for longs  (32-bit integers).
// * htonl -- Host-to-network ordering conversion, for longs  (32-bit integers).

// ----------------------------------------------------------------
#ifdef PROCESSOR_IS_LITTLE_ENDIAN

#define htons(x) ntohs(x)

#define ntohs(x) \
	((((NE_16)(x) & 0x00ff) << 8) | \
	 (((NE_16)(x) & 0xff00) >> 8))

#define htonl(x) ntohl(x)

#define ntohl(x) \
	((((NE_32)(x) & 0x000000ff) << 24) | \
	 (((NE_32)(x) & 0x0000ff00) << 8)  | \
	 (((NE_32)(x) & 0x00ff0000) >> 8)  | \
	 (((NE_32)(x) & 0xff000000) >> 24))

// ----------------------------------------------------------------
#else // Processor is big-endian.
#define htons(x) (x)
#define ntohs(x) (x)
#define htonl(x) (x)
#define ntohl(x) (x)
#endif

// ----------------------------------------------------------------
#endif // NETORDER_H
