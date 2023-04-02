# ================================================================
# cmake -B build && cmake --build build
# ================================================================

cmake_minimum_required(VERSION 3.21)

project("ctools/cvt")

add_executable(b2d b2d.c binary_cvt.c)
add_executable(b2h b2h.c binary_cvt.c)
add_executable(d2b d2b.c binary_cvt.c)
add_executable(d2h d2h.c)
add_executable(d2o d2o.c)
add_executable(h2b h2b.c binary_cvt.c)
add_executable(h2d h2d.c)
add_executable(o2d o2d.c)
