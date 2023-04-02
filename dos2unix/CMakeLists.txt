# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools/dos2unix")

add_executable(dos2unix dos2unix.c)
add_executable(unix2dos unix2dos.c)
