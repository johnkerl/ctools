# About
bytehisto.c is a simple histogram printer for byte values in a file.
I use it every once in a while, e.g. to help find non-printables.

John Kerl
2004

# Example

```
$ bytehisto bytehisto.c
----------------------------------------------------------------
bytehisto.c:
00: 0    20: 346   @: 0     `: 0    80: 0    a0: 0    c0: 0    e0: 0     
01: 0     !: 2     A: 0     a: 93   81: 0    a1: 0    c1: 0    e1: 0     
02: 0     ": 28    B: 0     b: 16   82: 0    a2: 0    c2: 0    e2: 0     
03: 0     #: 5     C: 2     c: 50   83: 0    a3: 0    c3: 0    e3: 0     
04: 0     $: 0     D: 0     d: 100  84: 0    a4: 0    c4: 0    e4: 0     
05: 0     %: 10    E: 1     e: 132  85: 0    a5: 0    c5: 0    e5: 0     
06: 0     &: 6     F: 1     f: 71   86: 0    a6: 0    c6: 0    e6: 0     
07: 0     ': 5     G: 0     g: 31   87: 0    a7: 0    c7: 0    e7: 0     
08: 0     (: 55    H: 0     h: 29   88: 0    a8: 0    c8: 0    e8: 0     
09: 147   ): 55    I: 1     i: 167  89: 0    a9: 0    c9: 0    e9: 0     
0a: 150   *: 24    J: 1     j: 6    8a: 0    aa: 0    ca: 0    ea: 0     
0b: 0     +: 21    K: 1     k: 2    8b: 0    ab: 0    cb: 0    eb: 0     
0c: 0     ,: 29    L: 1     l: 51   8c: 0    ac: 0    cc: 0    ec: 0     
0d: 0     -: 388   M: 3     m: 40   8d: 0    ad: 0    cd: 0    ed: 0     
0e: 0     .: 16    N: 7     n: 145  8e: 0    ae: 0    ce: 0    ee: 0     
0f: 0     /: 37    O: 0     o: 81   8f: 0    af: 0    cf: 0    ef: 0     
10: 0     0: 26    P: 0     p: 47   90: 0    b0: 0    d0: 0    f0: 0     
11: 0     1: 3     Q: 0     q: 1    91: 0    b1: 0    d1: 0    f1: 0     
12: 0     2: 11    R: 0     r: 87   92: 0    b2: 0    d2: 0    f2: 0     
13: 0     3: 2     S: 1     s: 85   93: 0    b3: 0    d3: 0    f3: 0     
14: 0     4: 3     T: 1     t: 106  94: 0    b4: 0    d4: 0    f4: 0     
15: 0     5: 6     U: 1     u: 57   95: 0    b5: 0    d5: 0    f5: 0     
16: 0     6: 6     V: 0     v: 27   96: 0    b6: 0    d6: 0    f6: 0     
17: 0     7: 3     W: 0     w: 4    97: 0    b7: 0    d7: 0    f7: 0     
18: 0     8: 1     X: 0     x: 10   98: 0    b8: 0    d8: 0    f8: 0     
19: 0     9: 0     Y: 0     y: 7    99: 0    b9: 0    d9: 0    f9: 0     
1a: 0     :: 7     Z: 0     z: 5    9a: 0    ba: 0    da: 0    fa: 0     
1b: 0     ;: 72    [: 16    {: 15   9b: 0    bb: 0    db: 0    fb: 0     
1c: 0     <: 15    \: 11    |: 0    9c: 0    bc: 0    dc: 0    fc: 0     
1d: 0     =: 163   ]: 16    }: 15   9d: 0    bd: 0    dd: 0    fd: 0     
1e: 0     >: 7     ^: 0     ~: 0    9e: 0    be: 0    de: 0    fe: 0     
1f: 0     ?: 0     _: 20   7f: 0    9f: 0    bf: 0    df: 0    ff: 0     
Min: 0       Max: 388   
Mean: 12.5508  Stddev: 42.2454
```

# Build instructions

* Compiler-only:
  * `gcc bytehisto.c -o bytehisto`
  * Executable is `./bytehisto`
* Using [yamm](https://github.com/johnkerl/scripts/blob/main/fundam/yamm):
  * `make mk`
  * `make`
  * Executable is `./bytehisto`
* Using CMake:
  * `cmake -B build`
  * `cmake --build build`
  * Executable is `./build/bytehisto`
