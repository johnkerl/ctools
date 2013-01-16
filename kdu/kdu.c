// ================================================================
// Copyright (c) 1995 John Kerl.
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

/*
A workalike for du, which avoids following symlinks etc.  The intent is
to work around shortcomings in the SunOS du command.
John Kerl
1995
*/

/*
TO DO:
1.	? If -d,  sum dirents' usages & subtract from dir total:  This is
	non-subdir file usage for that dir.  Fake a du_contents[] entry
	for it & sort it.
2.	? Allow user to specify reference file as option.
3.	? When arg is symlink/mount, follow it ... e.g. kdu /sys should give
	the same output as kdu /usr/kvm/sys?  Or should it?
4.	? Print out host:/mount/point in df-style format.
*/

/*--------------------------------------------------------------------*/
/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <dirent.h>

#define SORT_INCR  1
#define SORT_NONE  0
#define SORT_DECR -1

/*--------------------------------------------------------------------*/
/* External data */
extern int optind;
extern int errno;

/*--------------------------------------------------------------------*/
/* Private functions */
static void usage(char *prog);
static void parse_command_line(int argc, char **argv, int *first_non_opt_arg_p);
static void do_one_argument(char *filename);

static struct du_info_t *build_tree (char *filename, fsid_t fs, unsigned int level,
	unsigned int *deepest_level_p);
static void sort_tree(struct du_info_t *du_tree);
static int du_entry_compare_decr(struct du_info_t **du_entry_1, struct du_info_t **du_entry_2);
static int du_entry_compare_incr(struct du_info_t **du_entry_1, struct du_info_t **du_entry_2);
static void print_tree(struct du_info_t *du_tree, unsigned int level,
	unsigned int deepest_level);
static void free_tree(struct du_info_t *du_tree);

static int is_file_on_this_fs(char *filename, fsid_t fs);
static unsigned int file_size(char *filename);
static int is_a_directory(char *filename);

static  char *my_malloc(unsigned int nbytes);

/*--------------------------------------------------------------------*/
#define YES 1
#define NO  0
#define PROGRAM_NAME "kdu"

/*--------------------------------------------------------------------*/
/* Private data types & data structures */

/* This is a tree structure to store disk-usage information.  It
	contains a file name and recursive disk usage.  If the file is
	a directory, du_contents is an array of directory entries.
	du_nentries is the length of the du_contents array.  If the file
	is not a directory, du_contents is NULL and du_nentries is 0.
*/
struct du_info_t {
	char *du_filename;
	unsigned int du_nblocks;
	unsigned int du_nentries;
	struct du_info_t **du_contents;
};

/* These globals store information from the command line.
	They could be locals and passed as arguments; however,
	I've made them globals since they are set once at startup
	and are constant thereafter.
*/

/* Default for -a/-d/-f/-s options: */
static int want_to_print_directories = YES;
static int want_to_print_non_directories = NO;

/* Default for -e option: */
static int want_to_print_errors = NO;

/* Default for -l option: */
static int want_to_print_all_levels = YES;
static unsigned int max_print_level = 0;
	/* Ignored unless want_to_print_all_levels == NO. */

/* Default for -u option: */
static int sort_op = SORT_DECR;

/* Default for -n option: */
static int want_to_print_indentation_prefixes = YES;

/*--------------------------------------------------------------------*/
static void usage(char *prog)
{
	fprintf(stderr,
		"Usage:  %s [ -adfseun ] [ -l {depth} ] [filenames ...]\n", prog);
	exit(1);
}

/*--------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int first_non_opt_arg;
	int argi;

	/* Scan through all the "-" options on the command line; set the
		appropriate globals.
	*/
	parse_command_line(argc, argv, &first_non_opt_arg);

	if (first_non_opt_arg == argc)
		/* If there is no non-option argument, "." is the default. */
		do_one_argument(".");
	else for (argi = first_non_opt_arg; argi < argc; argi++) {
		/* Otherwise, calculate recursive disk usage for each argument. */
		do_one_argument(argv[argi]);

		/* Put a blank line between entries, but not after the last one. */
		if (argi < argc - 1)
			printf("\n");
	}

	return 0;
}

/*--------------------------------------------------------------------*/
static void parse_command_line(int argc, char **argv, int *first_non_opt_arg_p)
{
	int argi;
	char opt;

	while ( (opt = getopt( argc, argv, "adfsel:unr" )) != -1 ) {
		switch(opt) {
			case 'a':
				want_to_print_directories = YES;
				want_to_print_non_directories = YES;
				break;
			case 'd':
				want_to_print_directories = YES;
				want_to_print_non_directories = NO;
				break;
			case 'f':
				want_to_print_directories = NO;
				want_to_print_non_directories = YES;
				break;
			case 's':
				want_to_print_directories = NO;
				want_to_print_non_directories = NO;
				break;
			case 'e':
				want_to_print_errors = YES;
				break;
			case 'l':
				want_to_print_all_levels = NO;
				if (sscanf(argv[optind-1], "%d", &max_print_level) != 1)
					usage(argv[0]);
				if (max_print_level < 1)
					usage(argv[0]);
				argi++;
				break;
			case 'u':
				sort_op = SORT_NONE;
				break;
			case 'r':
				sort_op = SORT_INCR;
				break;
			case 'n':
				want_to_print_indentation_prefixes = NO;
				break;
			default:
				usage(argv[0]);
				break;
		}
	}

	*first_non_opt_arg_p = optind;
}

/*--------------------------------------------------------------------*/
static void do_one_argument(char *filename)
{
	unsigned int deepest_level = 0;
	struct statfs device_statfs_buf;
	struct du_info_t *du_tree;

	/* Get filesystem that arg is on. */
	if (statfs(filename, &device_statfs_buf) != 0) {
		fprintf(stderr, "%s:  Couldn't statfs \"%s\":  %s.\n",
			PROGRAM_NAME, filename, sys_errlist[errno]);
		/* Go on to the next argument (if any). */
		return;
	}

	du_tree = build_tree(filename, device_statfs_buf.f_fsid, 1, &deepest_level);
	if (sort_op != SORT_NONE)
		sort_tree(du_tree);
	print_tree(du_tree, 1, deepest_level);
	free_tree(du_tree);
}

/*====================================================================*/
static struct du_info_t *build_tree (char *filename, fsid_t fs, unsigned int level,
unsigned int *deepest_level_p)
{
	struct du_info_t *this_du;
	DIR *_dir;
	struct dirent *dir_ptr;
	char directory_entry[1024];
	unsigned int dirent_i;

	if (level > *deepest_level_p)
		*deepest_level_p = level;


	this_du = (struct du_info_t *)my_malloc(sizeof(struct du_info_t));

	this_du->du_filename = strdup(filename);
	/* Count the size of the file itself -- whether a directory or not,
		and whether a symlink or not.  E.g. find the space taken by the
		link itself, not by what the link points to. */
	this_du->du_nblocks  = file_size(filename);
	/* If this turns out to be a directory of interest, these last two
		attributes will change. */
	this_du->du_nentries = 0;
	this_du->du_contents = NULL;

	/* If a directory of interest, then recursively find the size of its contents. */
	if (is_a_directory(filename)) {
		if (is_file_on_this_fs(filename, fs)) {
			_dir = opendir(filename);
			if (_dir == NULL) {
				/* Perhaps it exists but we just don't have permission to go there -- not fatal. */
				if (want_to_print_errors)
					fprintf(stderr, "%s:  Couldn't open directory \"%s\":  %s.\n",
						PROGRAM_NAME, filename, sys_errlist[errno]);
			}
			else {

				/* Read through the directory once to total up a count of filenames. */
				for (dir_ptr = readdir(_dir); dir_ptr != NULL; dir_ptr = readdir(_dir))
					if (strcmp(dir_ptr->d_name, ".") != 0 && strcmp(dir_ptr->d_name, "..") != 0)
						this_du->du_nentries++;

				/* Then, malloc an array of that size, and copy filenames into each element. */
				this_du->du_contents = (struct du_info_t **)my_malloc(
					this_du->du_nentries * sizeof(struct du_info_t *));

				/* Now that we have an array of du_info_t structures, recursively find the
					disk utilization for each directory entry.  Also attribute that
					entry's disk usage to this directory's disk usage. */
				rewinddir(_dir);
				dirent_i = 0;
				for (dir_ptr = readdir(_dir); dir_ptr != NULL; dir_ptr = readdir(_dir)) {
					if (strcmp(dir_ptr->d_name, ".") != 0 && strcmp(dir_ptr->d_name, "..") != 0) {
						sprintf(directory_entry, "%s/%s", filename, dir_ptr->d_name);
						this_du->du_contents[dirent_i] = build_tree(directory_entry, fs, level+1,
							deepest_level_p);
						this_du->du_nblocks += this_du->du_contents[dirent_i]->du_nblocks;
						dirent_i++;
					}
				}
				closedir(_dir);

			}
		}
		else
			if (want_to_print_errors)
				printf("directory %s not on this filesystem.\n", filename);
	}

	return this_du;
}


/*--------------------------------------------------------------------*/
static void sort_tree(struct du_info_t *du_tree)
{
	unsigned int entry_i;

	/* Leaf node -- nothing to sort. */
	if (du_tree->du_nentries == 0)
		return;

	/* Sort this directory's entries by disk usage. */
	if (sort_op == SORT_DECR)
		qsort((char *)du_tree->du_contents, du_tree->du_nentries,
			sizeof(struct du_info_t *), du_entry_compare_decr);
	else if (sort_op == SORT_INCR)
		qsort((char *)du_tree->du_contents, du_tree->du_nentries,
			sizeof(struct du_info_t *), du_entry_compare_incr);

	/* Now, sort each entry. */
	for (entry_i = 0; entry_i < du_tree->du_nentries; entry_i++)
		sort_tree(du_tree->du_contents[entry_i]);
}

/*--------------------------------------------------------------------*/
/* Sort in numerically decreasing order. */
static int du_entry_compare_decr(
	struct du_info_t **du_entry_1,
	struct du_info_t **du_entry_2)
{
	struct du_info_t *d1 = *du_entry_1;
	struct du_info_t *d2 = *du_entry_2;
	return ((int)d2->du_nblocks - (int)d1->du_nblocks);
}

/*--------------------------------------------------------------------*/
/* Sort in numerically increasing order. */
static int du_entry_compare_incr(
	struct du_info_t **du_entry_1,
	struct du_info_t **du_entry_2)
{
	struct du_info_t *d1 = *du_entry_1;
	struct du_info_t *d2 = *du_entry_2;
	return ((int)d1->du_nblocks - (int)d2->du_nblocks);
}

/*--------------------------------------------------------------------*/
static void print_tree(struct du_info_t *du_tree, unsigned int level,
	unsigned int deepest_level)
{
	unsigned int entry_i;
	unsigned int print_i;
	int will_we_print_this = NO;

	/* Always print the top-level disk usage, regardless of -a/-f/-d/-s. */
	if (level == 1)
		will_we_print_this = YES;
	/* Does the user (as communicated by through command line) want to see this entry? */
	else if (level <= max_print_level || want_to_print_all_levels) {
		if (du_tree->du_nentries > 0) {
			/* A directory */
			if (want_to_print_directories)
				will_we_print_this = YES;
		}
		else {
			/* A non-directory */
			if (want_to_print_non_directories)
				will_we_print_this = YES;
		}
	}

	/* Print this directory's disk usage. */
	if (will_we_print_this) {

		if (want_to_print_indentation_prefixes == YES) {
			for (print_i = 1; print_i < level; print_i++)
				printf("_  ");
		}
		printf("%-8u ", du_tree->du_nblocks);

		if (want_to_print_indentation_prefixes == YES) {
			for (print_i = level; print_i < deepest_level; print_i++)
				printf("   ");
		}
		else {
			printf("   ");
		}
		printf("%s%s\n", du_tree->du_filename,
			(du_tree->du_nentries > 0) ? "/" : "");
	}

	/* Now, print each entry's disk usage. */
	for (entry_i = 0; entry_i < du_tree->du_nentries; entry_i++)
		print_tree(du_tree->du_contents[entry_i], level+1, deepest_level);
}

/*--------------------------------------------------------------------*/
/* Free leaves first, then the root. */
static void free_tree(struct du_info_t *du_tree)
{
	unsigned int entry_i;

	if (du_tree->du_nentries > 0) {
		for (entry_i = 0; entry_i < du_tree->du_nentries; entry_i++)
			free_tree(du_tree->du_contents[entry_i]);
		free(du_tree->du_contents);
	}
	free(du_tree->du_filename);
	free(du_tree);
}

/*====================================================================*/
static int is_file_on_this_fs(char *filename, fsid_t fs)
{
	struct statfs statfs_buf;

	if (statfs(filename, &statfs_buf) != 0) {
		if (want_to_print_errors)
			fprintf(stderr, "%s:  Couldn't statfs \"%s\":  %s.\n",
				PROGRAM_NAME, filename, sys_errlist[errno]);
		return 0;
	}
	return ((statfs_buf.f_fsid.__val[0] == fs.__val[0]) &&
		(statfs_buf.f_fsid.__val[1] == fs.__val[1]));
}

/*--------------------------------------------------------------------*/
/* Finds the size of a file itself -- e.g. for a directory,
	returns the size of the directory table, but does *not*
	traverse the directory.
*/

static unsigned int file_size(char *filename)
{
	struct stat stat_buf;

	if (lstat(filename, &stat_buf) != 0) {
		if (want_to_print_errors)
			fprintf(stderr, "%s:  Couldn't stat \"%s\":  %s.\n",
				PROGRAM_NAME, filename, sys_errlist[errno]);
		return 0;
	}

	return stat_buf.st_blocks / 2;
}

/*--------------------------------------------------------------------*/
static int is_a_directory(char *filename)
{
	struct stat stat_buf;
	mode_t mode;

	if (lstat(filename, &stat_buf) != 0) {
		if (want_to_print_errors)
			fprintf(stderr, "%s:  Couldn't stat \"%s\":  %s.\n",
				PROGRAM_NAME, filename, sys_errlist[errno]);
		return 0;
	}
	mode = stat_buf.st_mode & S_IFMT;

	return S_ISDIR(mode);
}


/*====================================================================*/
static char *my_malloc(unsigned int nbytes)
{
	char *rv;

	rv = (char *)malloc(nbytes);
	if (rv == NULL) {
		fprintf(stderr, "malloc(%u) failed; aborting.\n", nbytes);
		exit(1);
	}
	return rv;
}
