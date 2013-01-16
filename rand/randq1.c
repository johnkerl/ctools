// ================================================================
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

// From _Numerical Recipes_.  This is a "quick and dirty" random-number
// generator.  Note that the least significant bit has period two, the
// next-least significant bit has period four, the next-least significant
// bit has period eight, etc.

// Minimum output is 0x00000000.
// Maximum output is 0xffffffff.

// Usage:
//
// To seed:
//   unsigned state = ... whatever seed value ... ;
//
// To step:
//   unsigned output = randq1(&state);

unsigned randq1(
	unsigned *pstate)
{
	if (*pstate == 0)
		*pstate = 1;
	*pstate = 1664525 * (*pstate) + 1013904223;
	return *pstate;
}
