// ================================================================
// Copyright (c) 2004 John Kerl.
// kerl.john.r@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
// ================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "urandom.h"

#define BUFSZ 2048

// ----------------------------------------------------------------
int get_urandom(void)
{
	static int fd = -1;
	static int buf[BUFSZ];
	static int bufpos = 0;
	static char * dev_name = "/dev/urandom";
	int bytes_wanted;
	int bytes_read;
	int rv;
	if (fd == -1) {
		fd = open(dev_name, O_RDONLY);
		if (fd < 0) {
			perror("open");
			fprintf(stderr, "Couldn't open %s.\n", dev_name);
			exit(1);
		}
	}

	if (bufpos == 0) {
		bytes_wanted = BUFSZ * sizeof(int);
		bytes_read   = read(fd, buf, BUFSZ * sizeof(int));
		if (bytes_read != bytes_wanted) {
			perror("read");
			fprintf(stderr, "Couldn't read %s.\n", dev_name);
			fprintf(stderr, "Expected %d byte(s); got %d.\n",
				bytes_wanted, bytes_read);
			exit(1);
		}
	}
	rv = buf[bufpos];
	bufpos++;
	if (bufpos >= BUFSZ)
		bufpos = 0;
	return rv;
}

// ----------------------------------------------------------------
float get_urandomu(void)
{
	unsigned u = get_urandom();
	double numer = (double)u;
	double denom = 4294967296.0;
	return (float)(numer/denom);
}
