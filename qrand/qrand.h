// ================================================================
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

#ifndef QRAND_H
#define QRAND_H

// This is a "quick and dirty" 32-bit linear-congruential
// pseudo-random-number generator from _Numerical Recipes_.
//
// This macros is implemented as such, rather than as a function,
// to make it inline & omit the overhead of a function call.
#define STEP_RAND(x) ((x) = 1664525 * (x) + 1013904223)

#endif // QRAND_H
