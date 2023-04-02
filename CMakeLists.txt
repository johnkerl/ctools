# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools")

# ----------------------------------------------------------------
# SINGLE-FILE MAINS

add_executable(binv       binv/binv.c)
add_executable(bytehisto  bytehisto/bytehisto.c)
add_executable(cxor       xor/cxor.c)
add_executable(fxor       xor/fxor.c)
add_executable(hex        hex/hex.c)
add_executable(jot        jot/jot.c)
add_executable(microsleep millisleep/microsleep.c)
add_executable(millisleep millisleep/millisleep.c)
add_executable(unhex      hex/unhex.c)

# ================================================================
# TO DO:

# ----------------------------------------------------------------
# ISSUES

# Uses some non-standard (Solaris?) includes, or other portability issues
# add_executable(kdu   kdu/kdu.c)
# add_executable(kstat kstat/kstat.c)

# ----------------------------------------------------------------
# MAINS USING LIBS

# ./cvt/b2d.c
# ./cvt/b2h.c
# ./cvt/d2b.c
# ./cvt/d2h.c
# ./cvt/d2o.c
# ./cvt/h2b.c
# ./cvt/h2d.c
# ./cvt/o2d.c
# ./dos2unix/dos2unix.c
# ./dos2unix/unix2dos.c
# ./rand/gasdevmain.c
# ./rand/psdrandgen.c
# ./rand/randq1.c
# ./rand/showhash.c
# ./rand/tester.c
# ./rand/tester2.c

# ----------------------------------------------------------------
# LIB

# ./fsutil/fsutil.c
# ./fsutil/fsutil.h
#
# ./rand/gasdev.c
# ./rand/gasdev.h
#
# ./cvt/binary_cvt.c
# ./cvt/binary_cvt.h
#
# ./rand/urandom.c
# ./rand/urandom.h
#
# ./rand/psdes.c
# ./rand/psdes.h
#
# ./qrand/qrand.c
# ./qrand/qrand.h

# ----------------------------------------------------------------
# MULTI-FILE PROJECTS

# ./crypto/...
# ./csum/...
