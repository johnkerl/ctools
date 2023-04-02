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

#include "net.h"
#ifdef UNIX
#include <netinet/in.h>
#else
#include <netorder.h>
#endif

// ----------------------------------------------------------------
// This routine calculates the complemented 16-bit one's-complement sum
// required to for network CRCs.  If an odd number of bytes is received, the
// routine adds a 0 byte to the end of the data before doing the CRC and
// includes that in the calculation.

// There is no need to swap for network ordering (big-endian vs. little-endian)
// during calculation because of the end-around carry used in one's-complement
// addition.

// (IP checksum calculation from RFC 1071)

void accum_net_CRC(
	int8u  * bbuf,
	int32u   blen,
	int32u * ptemp,
	int16u * pcsum,
	int32u   op)
{
	int16u * sbuf = (int16u *)bbuf;
	int32s   byte_down_counter = blen;
	int32u   carry_out;

	switch (op) {
	case CSUM_PRE:
		*pcsum = 0;
		*ptemp =0;
		break;

	case CSUM_PERI:
		// Calculate running sum over region, storing the running sum into a
		// 32-bit int.
		while (byte_down_counter > 1) {
			*ptemp += *sbuf;
			sbuf++;
			byte_down_counter -= 2;
		}

		// Add odd, last byte (if any).
		if (byte_down_counter > 0) {
			int8u  last_byte = *((int8u *) sbuf);
			int16u last_byte_padded = (int16u) last_byte;
			*ptemp += htons(last_byte_padded);
		}

	case CSUM_POST:
		// Now, we have to add back in all the carry outs from the lower
		// 16 bits because this is one's-complement arithmetic.
		while (*ptemp & 0xffff0000) {

			// Extract top 16 bits of 32-bit sum.
			carry_out = *ptemp >> 16;

			// Clear top 16 bits of 32-bit sum.
			*ptemp &= 0x0000ffff;

			// Add top 16 bits into bottom 16 bits.  This sum might carry
			// over into the 17th bit, so we'll loop and do it again,
			// until it doesn't carry.
			*ptemp += carry_out;
		}

		// That's our one's-complement sum!  Put it into a 16-bit word.
		*pcsum = (int16u) *ptemp;

		// Take the one's complement of the one's complement sum.  There is no
		// need to flip for network byte order because we did all the sums
		// backwards already.
		*pcsum = ~ *pcsum;
		break;
	}
}
