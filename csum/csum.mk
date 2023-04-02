# ================================================================
# Makefile for project csum
# Automatically generated from "csum.mki" at Sun Apr  2 12:12:00 2023

# yamm v1.0
# John Kerl
# 2002/05/04
# ================================================================


INCLUDE_DIRS = -I./include
LIB_DIRS =
DEFINES = -DUNIX
MISC_CFLAGS =
MISC_LFLAGS =
EXTRA_DEPS =
COMPILE_FLAGS = -c $(INCLUDE_DIRS) $(DEFINES) $(MISC_CFLAGS)
LINK_FLAGS =  $(LIB_DIRS) $(MISC_LFLAGS)

build: mk_obj_dir ./csum

mk_obj_dir:
	mkdir -p ./csum_objs

./csum_objs/csum.o:  ./include/crca.h ./include/csum.h ./include/ethcrc.h ./include/floppy.h ./include/fsutil.h ./include/md5.h ./include/murmur.h ./include/net.h ./include/nop.h src/csum.c
	gcc -O3 $(COMPILE_FLAGS)  src/csum.c -o ./csum_objs/csum.o

./csum_objs/ethcrc.o:  ./include/csum.h ./include/ethcrc.h src/ethcrc.c
	gcc -O3 $(COMPILE_FLAGS)  src/ethcrc.c -o ./csum_objs/ethcrc.o

./csum_objs/crca.o:  ./include/crca.h ./include/csum.h src/crca.c
	gcc -O3 $(COMPILE_FLAGS)  src/crca.c -o ./csum_objs/crca.o

./csum_objs/net.o:  ./include/csum.h ./include/net.h ./include/netorder.h src/net.c
	gcc -O3 $(COMPILE_FLAGS)  src/net.c -o ./csum_objs/net.o

./csum_objs/floppy.o:  ./include/csum.h ./include/floppy.h src/floppy.c
	gcc -O3 $(COMPILE_FLAGS)  src/floppy.c -o ./csum_objs/floppy.o

./csum_objs/md5.o:  ./include/csum.h ./include/md5.h src/md5.c
	gcc -O3 $(COMPILE_FLAGS)  src/md5.c -o ./csum_objs/md5.o

./csum_objs/murmur.o:  ./include/csum.h src/murmur.c
	gcc -O3 $(COMPILE_FLAGS)  src/murmur.c -o ./csum_objs/murmur.o

./csum_objs/nop.o:  ./include/csum.h ./include/nop.h src/nop.c
	gcc -O3 $(COMPILE_FLAGS)  src/nop.c -o ./csum_objs/nop.o

./csum_objs/fsutil.o:  ../fsutil/fsutil.c ./include/fsutil.h
	gcc -O3 $(COMPILE_FLAGS)  ../fsutil/fsutil.c -o ./csum_objs/fsutil.o

OBJS = \
	./csum_objs/csum.o \
	./csum_objs/ethcrc.o \
	./csum_objs/crca.o \
	./csum_objs/net.o \
	./csum_objs/floppy.o \
	./csum_objs/md5.o \
	./csum_objs/murmur.o \
	./csum_objs/nop.o \
	./csum_objs/fsutil.o

./csum: $(OBJS) $(EXTRA_DEPS)
	gcc $(OBJS) -o ./csum $(LINK_FLAGS)

install: build
	cp ./csum $(HOME)/bin

clean:
	-@rm -f $(OBJS)
	-@rm -f ./csum
