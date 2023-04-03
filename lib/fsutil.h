// ================================================================
// Copyright (c) 2004 John Kerl.
// kerl at math dot arizona dot edu
//
// This code and information is provided as is without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.
//
// No restrictions are placed on copy or reuse of this code, as long
// as these paragraphs are included in the code.
// ================================================================

#ifndef FSUTIL_H
#define FSUTIL_H

void getdirname(
	char * in,
	char * out);

int is_a_directory(
	char * file_name);

int get_file_size(
	char * file_name,
	int  * pfile_size);

FILE * open_file_or_die(
	char * file_name,
	char * mode);

int read_file_to_region(
	char * file_name,
	int    file_size,
	char * region);

int write_region_to_file(
	char * region,
	int    file_size,
	char * file_name);

#endif // FSUTIL_H
