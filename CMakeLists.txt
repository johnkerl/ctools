# ================================================================
# cmake -B build && cmake --build build
# cmake --build build --target clean && rm -rf build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools")

# ----------------------------------------------------------------
# SINGLE-FILE MAINS

add_subdirectory(binv)
add_subdirectory(bytehisto)
add_subdirectory(cvt)
add_subdirectory(dos2unix)
add_subdirectory(hex)
add_subdirectory(jot)
add_subdirectory(millisleep)
add_subdirectory(xor)

# ================================================================
# TO DO:

# ----------------------------------------------------------------
# ISSUES

# Uses some non-standard (Solaris?) includes, or other portability issues
# add_executable(kdu   kdu/kdu.c)
# add_executable(kstat kstat/kstat.c)

# ----------------------------------------------------------------
# MAINS USING LIBS

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
# ./rand/urandom.c
# ./rand/urandom.h
# ./rand/psdes.c
# ./rand/psdes.h
#
# ./qrand/qrand.c
# ./qrand/qrand.h

# ----------------------------------------------------------------
# MULTI-FILE PROJECTS

# ./crypto/...
# ./csum/...
