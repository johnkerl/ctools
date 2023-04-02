# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("millisleep")

add_executable(millisleep millisleep.c)
add_executable(microsleep microsleep.c)
