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

#ifndef NET_H
#define NET_H

#include <stdio.h>
#include "csum.h"

void accum_net_CRC(
	int8u  * buf,
	int32u   len,
	int32u * ptemp,
	int16u * pcsum,
	int32u   op);

#endif // NET_H
