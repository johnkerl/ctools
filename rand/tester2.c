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

// ----------------------------------------------------------------
float fran32_print(
	int * pidum)
{
	unsigned irword, itemp, lword;
	static   int idums = 0;
	static unsigned jflone  = 0x3f800000;
	static unsigned jflmask = 0x007fffff;
	float fret;

	if (*pidum < 0) {
		idums = -(*pidum);
		*pidum = 1;
	}

	irword = *pidum;
	lword  = idums;
	printf("0x%08x 0x%08x ", lword, irword);
	psdes_hash_64(&lword, &irword);
	printf(" 0x%08x 0x%08x  ", lword, irword);

	++(*pidum);

	itemp = jflone | (jflmask & irword);
	fret = (*(float *)&itemp) - 1.0;
	printf("%9.6f\n", fret);
	return fret;
}

// ================================================================
void test_psdes_hash_64(
	void)
{
	unsigned i, j;
	unsigned a, b;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			a = i;
			b = j;
			psdes_hash_64(&a, &b);
			printf("0x%08x 0x%08x => 0x%08x 0x%08x\n",
				i, j, a, b);
		}
		printf("\n");
	}
}

// ----------------------------------------------------------------
void test_ref(
	void)
{
	int idum;

	idum =  -1; (void)fran32_print(&idum);
	idum =  99; (void)fran32_print(&idum);
	idum = -99; (void)fran32_print(&idum);
	idum =  99; (void)fran32_print(&idum);
}

// ----------------------------------------------------------------
void test_iran32(
	int     argc,
	char ** argv)
{
	int i;
	int N = 20;
	int s;
	unsigned r;

	if (argc > 1)
		N = atoi(argv[1]);

	s = -10;

	for (i = 0; i < N; i++) {
		r = iran32(&s);
		printf("0x%08x 0x%08x\n", s, r);
	}
}

#ifdef PSDES_TEST
// ----------------------------------------------------------------
int main(
	int     argc,
	char ** argv)
{
	// test_psdes_hash_64();
	// test_ref();
	// test_iran32(argc, argv);

	return 0;
}
#endif // PSDES_TEST
