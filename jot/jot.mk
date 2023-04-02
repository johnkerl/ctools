# ================================================================
# Makefile for project jot
# Automatically generated from "jot.mki" at Fri Aug 20 19:46:02 2021

# yamm v1.0
# John Kerl
# 2002/05/04
# ================================================================


INCLUDE_DIRS =
LIB_DIRS =
DEFINES = -DUNIX
MISC_CFLAGS =
MISC_LFLAGS =
EXTRA_DEPS =
COMPILE_FLAGS = -c $(INCLUDE_DIRS) $(DEFINES) $(MISC_CFLAGS)
LINK_FLAGS =  $(LIB_DIRS) $(MISC_LFLAGS)

build: mk_obj_dir ./jot

mk_obj_dir:
	mkdir -p ./jot_objs

./jot_objs/jot.o:  jot.c
	gcc -O3 $(COMPILE_FLAGS)  jot.c -o ./jot_objs/jot.o

OBJS = \
	./jot_objs/jot.o

./jot: $(OBJS) $(EXTRA_DEPS)
	gcc $(OBJS) -o ./jot $(LINK_FLAGS)

install: build
	cp ./jot $(HOME)/bin

clean:
	-@rm -f $(OBJS)
	-@rm -f ./jot
