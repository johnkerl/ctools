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
add_subdirectory(csum)
add_subdirectory(cvt)
add_subdirectory(dos2unix)
add_subdirectory(hex)
add_subdirectory(jot)
add_subdirectory(millisleep)
add_subdirectory(qrand)
add_subdirectory(rand)
add_subdirectory(xor)

# Has 2023 compile issues with or without cmake:
# ./crypto/...
