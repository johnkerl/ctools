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

#ifndef ETHCRC_H
#define ETHCRC_H

#include <stdio.h>
#include "csum.h"

// Ethernet 32-bit CRC.
void accum_eth_CRC(
	int8u  * buf,
	int32u   len,
	int32u * pcsum,
	int32u   op);

#endif // ETHCRC_H
