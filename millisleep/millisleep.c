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
#include <sys/time.h>

static void usage(char *prog);

/*--------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int milliseconds;
	struct timeval wait_time;

	if (argc != 2)
		usage(argv[0]);
	if (sscanf(argv[1], "%d", &milliseconds) != 1)
		usage(argv[0]);
	wait_time.tv_sec  = milliseconds / 1000;
	wait_time.tv_usec = (milliseconds % 1000) * 1000;
	select(0, 0, 0, 0, &wait_time);
	exit(0);
}

/*--------------------------------------------------------------------*/
static void usage(char *prog)
{
	fprintf(stderr, "Usage: %s {sleep time in milliseconds}.\n", prog);
	exit(1);
}
