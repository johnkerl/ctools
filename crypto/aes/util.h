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

#ifndef UTIL_H
#define UTIL_H

#include "aesapi.h"

void * my_malloc(
	int num_bytes);

void hex_dump_region(
	char * msg,
	BYTE * region,
	int    num_bytes);

int round_up(
	int x,
	int n);

void fill_region_from_string(
	BYTE * region,
	int    region_length_in_bytes,
	char * string);

void fill_region_from_other(
	BYTE * dest,
	int    dest_length_in_bytes,
	BYTE * src,
	int    src_length_in_bytes);

unsigned get_ethernet_CRC(
	void *         pfile_contents,
	int            file_size);

#endif // UTIL_H
