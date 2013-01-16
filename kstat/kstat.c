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

/*
STAT.C:
Calls stat() on the command-line arguments, printing out all the
available information in the stat struct that comes back from stat().
Returns 0 to the caller on success (one or more stats were done and all
were successful), or 1 on failure (no stats were done, or an
unsuccessful stat was done).

John R. Kerl
10/28/95
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#define SUCCESS 0
#define FAILURE 1

int main(argc, argv)
	int argc;
	char **argv;
{
	int i,  /* Looper through command-line args */
		return_value;
	struct stat stat_buf;
	struct passwd *my_pw;
	struct group *my_gr;

	if (argc <= 1) {
		fprintf(stderr, "Usage: %s {filenames}\n", argv[0]);
		fprintf(stderr,
				"  Prints out information from stat() on each argument.\n");
		return FAILURE;
	}

	return_value=SUCCESS;
	/* Now that we know we have at least one arg, we'll set this to failure
		if any stat fails.
	*/

	for (i=1; i < argc; i++) {
		if ((stat(argv[i], &stat_buf)) != 0) {
			return_value=FAILURE; /* but we'll keep on statting the other args */
			fprintf(stderr, "%s: stat() failed for %s; errno=%d.\n",
				argv[0], argv[i], errno);
			continue;
		}

		fprintf(stdout, "Stats for %s:\n", argv[i]);

		/* Now we'll just crunch through each attribute of the stat struct. */

		fprintf(stdout, "  Device file resides on:       0x%04x\n",
				/* dev_t */  (unsigned)stat_buf.st_dev);
		fprintf(stdout, "  Serial (inode) number:        %lu\n",
				/* ino_t */  stat_buf.st_ino);

		/* Now, the file type and mode (setuid & setgid bits,
			sticky bit, and permissions) are all in one unsigned
			short integer (stat_buf.st_mode).  We'll need to
			decode these by masking off the relevant bits.
		*/
		{	/* Start a nameless block for the local variables we'll need to do that.  */
			char *file_type; /* description that we'll print out */
			char permissions[3][4]; /* 3 strings: rwx for owner/group/other */
			mode_t m; /* We'll mask off first the type and then the permissions part of the mode */

			m = stat_buf.st_mode & S_IFMT;
			if      (S_ISBLK (m)) file_type="block special";
			else if (S_ISCHR (m)) file_type="character special";
			else if (S_ISDIR (m)) file_type="directory";
			else if (S_ISFIFO(m)) file_type="FIFO";
			else if (S_ISLNK (m)) file_type="symbolic link";
			else if (S_ISREG (m)) file_type="regular";
			else if (S_ISSOCK(m)) file_type="socket";
			else                  file_type="unknown mode!";

			m = stat_buf.st_mode;
			strcpy (permissions[0], "---");
			strcpy (permissions[1], "---");
			strcpy (permissions[2], "---");
			if (m&S_IRUSR) permissions[0][0] = 'r';
			if (m&S_IWUSR) permissions[0][1] = 'w';
			if (m&S_IXUSR) permissions[0][2] = 'x';
			if (m&S_IRGRP) permissions[1][0] = 'r';
			if (m&S_IWGRP) permissions[1][1] = 'w';
			if (m&S_IXGRP) permissions[1][2] = 'x';
			if (m&S_IROTH) permissions[2][0] = 'r';
			if (m&S_IWOTH) permissions[2][1] = 'w';
			if (m&S_IXOTH) permissions[2][2] = 'x';
			/* S_IRUSR, etc. are just defines in sys/stat.h for 0000400, etc. */

			fprintf(stdout, "  File mode: 0%o, which means ...\n",
					/* mode_t */ stat_buf.st_mode);
			fprintf(stdout, "    Type: 0%o (%s)\n",
					(stat_buf.st_mode & S_IFMT) >> 12, file_type);
					/* The rightmost 12 bits are used for permissions */
			fprintf(stdout,
			"    Permissions: 0%o (setuid = %s; setgid = %s; sticky bit = %s;\n",
					m & (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)
					/* 07777 */,
					m & S_ISUID ? "yes" : "no",
					m & S_ISGID ? "yes" : "no",
					m & S_ISVTX ? "yes" : "no");
			fprintf(stdout, "      owner,group,other = %s,%s,%s)\n",
					permissions[0], permissions[1], permissions[2]);
		} /* end nameless block for file mode */

		fprintf(stdout, "  Number of hard links to the file: %d\n",
				/* nlink_t */ stat_buf.st_nlink);
		my_pw = getpwuid(stat_buf.st_uid);
		fprintf(stdout, "  User ID of owner:             %u (%s)\n",
				/* uid_t */  stat_buf.st_uid, my_pw?my_pw->pw_name:"???");
		my_gr = getgrgid(stat_buf.st_gid);
		fprintf(stdout, "  Group ID of owner:            %u (%s)\n",
				/* gid_t */  stat_buf.st_gid, my_gr?my_gr->gr_name:"???");

		if ((S_ISBLK (stat_buf.st_mode & S_IFMT)) ||(S_ISCHR (stat_buf.st_mode & S_IFMT)))
				/* If the file is a block or character "special" device,
					then we want to get the major & minor device numbers
					out of stat_buf.st_rdev (which is of type dev_t).
				*/
		{
			dev_t bit_width  = (sizeof(dev_t)*4);
					/* Multiply by 8 for bits/byte; then divide by 2
						since there are two halves (major & minor).
						That gives us the number of bits for each mode.
						We need to play with sizeof, etc. because dev_t
						is (for example) a short on SunOS 4, but a long
						on AIX 3.  So we can't depend on a particular size.
					*/
			dev_t major_mask = ((1 << bit_width) - 1) << bit_width;
			dev_t minor_mask =  (1 << bit_width) - 1;
			/* These look like 0xff00 & 0x00ff, or 0xffff0000 & 0x0000ffff, etc.*/

			/* printf("0x%x 0x%x\n", major_mask, minor_mask); */
			fprintf(stdout, "  Major device number:          %lld\n",
					(stat_buf.st_rdev & major_mask) >> bit_width);
			fprintf(stdout, "  Minor device number:          %lld\n",
					/* dev_t */  stat_buf.st_rdev & minor_mask);
		}
		else { /* not a special file */
			fprintf(stdout, "  Major device number:          n/a\n");
			fprintf(stdout, "  Minor device number:          n/a\n");
		}

		fprintf(stdout, "  Total size of file, in bytes: %ld\n",
				/* off_t */  stat_buf.st_size);
		fprintf(stdout, "  File last access time:        (%ld) %s",
				/* time_t */ stat_buf.st_atime, ctime(&stat_buf.st_atime));
		fprintf(stdout, "  File last modify time:        (%ld) %s",
				/* time_t */ stat_buf.st_mtime, ctime(&stat_buf.st_mtime));
		fprintf(stdout, "  File last status change time: (%ld) %s",
				/* time_t */ stat_buf.st_ctime, ctime(&stat_buf.st_ctime));
		fprintf(stdout, "  Preferred blocksize for file system I/O: %ld\n",
				/* long */   stat_buf.st_blksize);
		fprintf(stdout, "  Actual number of blocks allocated: %ld\n",
				/* long */   stat_buf.st_blocks);
		fprintf(stdout, "\n");

	} /* end for-argc loop */

	return return_value;
} /* end main() */

/* On my Sparc,
	dev_t   = short,
	ino_t   = u_long,
	mode_t  = u_short;
	nlink_t = short;
	uid_t   = u_short;
	gid_t   = u_short;
	off_t   = long;
	time_t  = long;
*/

/*
 *   st_atime    Time when file data was last accessed.  This can
 *               also  be  set explicitly by utimes(2).  st_atime
 *               is not updated for directories  searched  during
 *               pathname resolution.
 *
 *   st_mtime    Time when file data was last modified.  This can
 *               also  be set explicitly by utimes(2).  It is not
 *               set by changes of owner, group, link  count,  or
 *               mode.
 *
 *   st_ctime    Time when file status was last  changed.  It  is
 *               set  both  both by writing and changing the file
 *               status information, such as  changes  of  owner,
 *               group, link count, or mode.
 */

/*
 * ERRORS
 *   stat() and lstat() will fail if one or more of the following
 *   are true:
 *
 *   EACCES              Search permission is denied for  a  com-
 *                       ponent of the path prefix of path.
 *
 *   EFAULT              buf  or  path  points  to   an   invalid
 *                       address.
 *
 *   EIO                 An I/O error occurred while reading from
 *                       or writing to the file system.
 *
 *   ELOOP               Too many symbolic links were encountered
 *                       in translating path.
 *
 *   ENAMETOOLONG        The length of the path argument  exceeds
 *                       {PATH_MAX}.n
 *
 *                       A  pathname  component  is  longer  than
 *                       {NAME_MAX} while {_POSIX_NO_TRUNC} is in
 *                       effect (see pathconf(2V)).
 *
 *   ENOENT              The file referred to by  path  does  not
 *                       exist.
 *
 *   ENOTDIR             A component of the path prefix  of  path
 *                       is not a directory.
 *
 *   fstat() will fail if one or more of the following are true:
 *
 *   EBADF               fd is not a valid open file descriptor.
 *
 *   EFAULT              buf points to an invalid address.
 *
 *   EIO                 An I/O error occurred while reading from
 *                       or writing to the file system.
 *
 * SYSTEM V ERRORS
 *   In addition to the above, the following may also occur:
 *
 *   ENOENT              path points to an empty string.
 */
