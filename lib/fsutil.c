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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "fsutil.h"

// ----------------------------------------------------------------
void getdirname(
	char * in,
	char * out)
{
	int len = strlen(in);
	char * p;
	int found_it = 0;

	strcpy(out, in);

	for (p = &out[len - 1]; p > out; p--) {
		if (*p == '/') {
			p[1] = 0;
			found_it = 1;
			break;
		}
	}
	if (!found_it)
		out[0] = 0;
}

// ----------------------------------------------------------------
int is_a_directory(
	char * file_name)
{
	struct stat stat_buf;

	if (stat(file_name, &stat_buf) != 0)
		return 0;

	if (S_ISDIR(stat_buf.st_mode & S_IFMT))
		return 1;
	return 0;
}

// ----------------------------------------------------------------
int get_file_size(
	char * file_name,
	int  * pfile_size)
{
	struct stat stat_buf;

	if (stat(file_name, &stat_buf) != 0)
		return 0;

	*pfile_size =  stat_buf.st_size;
	return 1;
}

// ----------------------------------------------------------------
FILE * open_file_or_die(
	char * file_name,
	char * mode)
{
	FILE * fp = fopen(file_name, mode);
	if (fp == 0) {
		fprintf(stderr, "Couldn't open file \"%s\" for mode \"%s\".\n",
			file_name, mode);
		exit(1);
	}
	return fp;
}

// ----------------------------------------------------------------
int read_file_to_region(
	char * file_name,
	int    file_size,
	char * region)
{
	FILE * fp;
	int bytes_read;
	int rc = 0;

	fp = open_file_or_die(file_name, "rb");
	bytes_read = fread(region, sizeof(char), file_size, fp);
	if (bytes_read != file_size) {
		fprintf(stderr, "Could not read %d bytes from file \"%s\"; got %d.\n",
			file_size, file_name, bytes_read);
		rc = 0;
	}
	else {
		rc = 1;
	}

	fclose(fp);
	return rc;
}

// ----------------------------------------------------------------
int write_region_to_file(
	char * region,
	int    file_size,
	char * file_name)
{
	FILE * fp;
	int    bytes_written;
	int    rc = 0;

	fp = open_file_or_die(file_name, "wb");
	bytes_written = fwrite(region, sizeof(char), file_size, fp);
	if (bytes_written != file_size) {
		fprintf(stderr,
			"Could not write %d bytes to file \"%s\"; wrote %d.  File is now damaged.\n",
			file_size, file_name, bytes_written);
		rc = 0;
	}
	else {
		rc = 1;
	}

	fclose(fp);
	return rc;
}
