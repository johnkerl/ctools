# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools/xor")

add_executable(cxor cxor.c)
add_executable(fxor fxor.c)
