# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools/rand")

include_directories(AFTER ../lib)

add_executable(gasdev    gasdevmain.c ../lib/gasdev.c ../lib/psdes.c)
add_executable(psrandgen psdrandgen.c ../lib/psdes.c)
add_executable(showhash  showhash.c   ../lib/psdes.c)
