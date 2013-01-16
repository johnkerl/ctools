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

#ifndef NOP_H
#define NOP_H

#include <stdio.h>
#include "csum.h"

void accum_nop_sum(
	int8u  * buf,
	int32u   len,
	int32u * pcsum,
	int32u   op);

#endif // NOP_H
