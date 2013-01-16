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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// ----------------------------------------------------------------
static int rfd = -1;
void open_rand(void)
{
	char * dev = "/dev/urandom";
	rfd = open(dev, O_RDONLY);
	if (rfd == -1) {
		fprintf(stderr, "Couldn't open %s.\n", dev);
		perror("Reason");
		exit(1);
	}
}

unsigned char next_rand(void)
{
	char r;
	if (read(rfd, &r, 1) < 0) {
		fprintf(stderr, "Couldn't read /dev/random.\n");
		perror("Reason");
		exit(1);
	}
	return r;
}

void close_rand(void)
{
	(void) close(rfd);
}


// ----------------------------------------------------------------
void dump_perm(
	unsigned char box[256],
	unsigned char ibox[256])
{
	int i, j;
	int num_fixed = 0;

	for (i = 0; i < 256; i++) {
		if (i == box[i])
			num_fixed++;
		fprintf(stderr, "%c%02x:%02x%c",
			(i == box[i]) ? '#' : ' ',
			(unsigned)i & 0xff,
			(unsigned)box[i] & 0xff,
			(i & 7) == 7 ? '\n' : ' ');
	}
	fprintf(stderr, "# fixed points %d\n", num_fixed);

	for (i = 0; i < 256; i++) {
		fprintf(stderr, "%02x:%02x%c",
			(unsigned)ibox[box[i]] & 0xff,
			(unsigned)box[ibox[i]] & 0xff,
			(i & 7) == 7 ? '\n' : ' ');
	}
	fprintf(stderr, "# fixed points %d\n", num_fixed);

	printf("\n");
	printf("static char myx8t_S_box[] = {\n");
	for (i = 0; i < 256; i++) {
		printf("%s0x%02x,%c",
			(i & 7) == 0 ? "\t" : "",
			box[i],
			(i & 7) == 7 ? '\n' : ' ');
	}
	printf("};\n");
	printf("\n");

	printf("\n");
	printf("static char myx8t_inv_S_box[] = {\n");
	for (i = 0; i < 256; i++) {
		printf("%s0x%02x,%c",
			(i & 7) == 0 ? "\t" : "",
			ibox[i],
			(i & 7) == 7 ? '\n' : ' ');
	}
	printf("};\n");
	printf("\n");

}

// ----------------------------------------------------------------
void make_perm(
	unsigned char box[256],
	unsigned char ibox[256])
{
	int i, j;
	unsigned char t;

	open_rand();
	for (i = 0; i < 256; i++)
		box[i] = i;

	for (i = 0; i < 256; i++) {
		j = next_rand();

		t = box[i];
		box[i] = box[j];
		box[j] = t;
	}
	close_rand();

	for (i = 0; i < 256; i++) {
		ibox[box[i]] = i;
	}
}

// ----------------------------------------------------------------
int main(void)
{
	unsigned char box[256];
	unsigned char ibox[256];
	make_perm(box, ibox);
	dump_perm(box, ibox);
	return 0;
}
