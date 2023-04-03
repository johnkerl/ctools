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
add_subdirectory(qrand)
add_subdirectory(rand)
add_subdirectory(xor)

# ================================================================
# TO DO:

# ----------------------------------------------------------------
# ISSUES

# Uses some non-standard (Solaris?) includes, or other portability issues
# add_executable(kdu   kdu/kdu.c)
# add_executable(kstat kstat/kstat.c)

# ----------------------------------------------------------------
# MULTI-FILE PROJECTS

# ./crypto/...
# ./csum/...
