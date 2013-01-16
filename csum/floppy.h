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

#ifndef FLOPPY_H
#define FLOPPY_H

#include <stdio.h>
#include "csum.h"

// From Amiga floppy-disk controller in Linux distribution.
void accum_floppy_CRC(
	int8u  * buf,
	int32u   len,
	int8u  * pcsumh,
	int8u  * pcsuml,
	int32u   op);

#endif // FLOPPY_H
