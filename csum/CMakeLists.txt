# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools/csum")

include_directories(AFTER ./include)

add_executable(csum 
  ./src/csum.c
  ../lib/fsutil.c
  ./src/ethcrc.c
  ./src/floppy.c
  ./src/net.c
  ./src/md5.c
  ./src/murmur.c
  ./src/crca.c
  ./src/nop.c
)
