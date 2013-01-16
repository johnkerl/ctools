# ================================================================
# Makefile for project csum
# Automatically generated from "csum.mki" at Thu Aug 12 10:26:22 2004

# yamm v1.0
# John Kerl
# 2002/05/04
# ================================================================


INCLUDE_DIRS =
LIB_DIRS =
DEFINES = -DUNIX
MISC_CFLAGS =
MISC_LFLAGS =
COMPILE_FLAGS = -c $(INCLUDE_DIRS) $(DEFINES) $(MISC_CFLAGS)
LINK_FLAGS =  $(LIB_DIRS) $(MISC_LFLAGS)

build: mk_obj_dir ./csum

mk_obj_dir:
	@-mkdir -p ./csum_objs

./csum_objs/csum.o:  crca.h csum.c csum.h ethcrc.h floppy.h fsutil.h md5.h net.h nop.h
	gcc -O3 $(COMPILE_FLAGS)  csum.c -o ./csum_objs/csum.o

./csum_objs/ethcrc.o:  csum.h ethcrc.c ethcrc.h
	gcc -O3 $(COMPILE_FLAGS)  ethcrc.c -o ./csum_objs/ethcrc.o

./csum_objs/crca.o:  crca.c crca.h csum.h
	gcc -O3 $(COMPILE_FLAGS)  crca.c -o ./csum_objs/crca.o

./csum_objs/net.o:  csum.h net.c net.h netorder.h
	gcc -O3 $(COMPILE_FLAGS)  net.c -o ./csum_objs/net.o

./csum_objs/floppy.o:  csum.h floppy.c floppy.h
	gcc -O3 $(COMPILE_FLAGS)  floppy.c -o ./csum_objs/floppy.o

./csum_objs/md5.o:  csum.h md5.c md5.h
	gcc -O3 $(COMPILE_FLAGS)  md5.c -o ./csum_objs/md5.o

./csum_objs/nop.o:  csum.h nop.c nop.h
	gcc -O3 $(COMPILE_FLAGS)  nop.c -o ./csum_objs/nop.o

./csum_objs/fsutil.o:  ../fsutil/fsutil.c fsutil.h
	gcc -O3 $(COMPILE_FLAGS)  ../fsutil/fsutil.c -o ./csum_objs/fsutil.o

OBJS = \
	./csum_objs/csum.o \
	./csum_objs/ethcrc.o \
	./csum_objs/crca.o \
	./csum_objs/net.o \
	./csum_objs/floppy.o \
	./csum_objs/md5.o \
	./csum_objs/nop.o \
	./csum_objs/fsutil.o

./csum: $(OBJS)
	gcc $(OBJS) -o ./csum $(LINK_FLAGS)

install: build
	cp ./csum $(HOME)/bin

clean:
	-@rm -f $(OBJS)
	-@rm -f ./csum
