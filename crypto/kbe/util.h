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

void * my_malloc(
	int num_bytes);

void hex_dump_region(
	char * msg,
	char * region,
	int    num_bytes,
	int    max_bytes_per_line);

int bc_read_block_from_file(
	char * file_name,
	char * block,
	int    num_bytes);

int round_up(
	int x,
	int n);

void fill_region_from_string(
	char * region,
	int    region_length_in_bytes,
	char * string);

void fill_region_from_other(
	char * dest,
	int    dest_length_in_bytes,
	char * src,
	int    src_length_in_bytes);

void XOR_blocks(
	char * dst,
	char * src1,
	char * src2,
	int    num_bytes);

void dump_block(
	char * desc,
	char * block,
	int    num_bytes);

void accum_eth_CRC(
	char     * buf,
	unsigned   len,
	unsigned * pcsum,
	unsigned   op);

#define CSUM_PRE  1
#define CSUM_PERI 2
#define CSUM_POST 3

#endif // UTIL_H
