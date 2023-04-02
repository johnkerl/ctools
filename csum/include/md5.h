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

#ifndef MD5_H
#define MD5_H

#include <stdio.h>
#include "csum.h"

typedef struct _MD5_state_t {
	int8u  buffer[64];
	int32u bit_count[2];
	int32u hash[4];
} MD5_state_t;

void accum_MD5_sum(
	int8u       * buf,         // Input
	int32u        byte_count,
	MD5_state_t * pstate,      // Workspace
	int8u         digest[16],  // Output
	int32u        op);

#endif // MD5_H
