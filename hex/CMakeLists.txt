# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools/hex")

add_executable(hex hex.c)
add_executable(unhex unhex.c)
