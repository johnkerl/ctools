# ================================================================
# Makefile for project kbe
# Automatically generated from "kbe.mki" at Sun Apr  2 23:30:53 2023

# yamm v1.0
# John Kerl
# 2002/05/04
# ================================================================


INCLUDE_DIRS = -I. -I./aes -I./aesf -I./myx -I./myx8 -I./myx8t
LIB_DIRS =
DEFINES = -DUNIX
MISC_CFLAGS =
MISC_LFLAGS =
EXTRA_DEPS =
COMPILE_FLAGS = -c $(INCLUDE_DIRS) $(DEFINES) $(MISC_CFLAGS)
LINK_FLAGS =  $(LIB_DIRS) $(MISC_LFLAGS)

build: mk_obj_dir ./kbe

mk_obj_dir:
	mkdir -p ./kbe_objs

./kbe_objs/main.o:  ./blkciphs.h ./blkcipht.h ./util.h main.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  main.c -o ./kbe_objs/main.o

./kbe_objs/util.o:  ./util.h util.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  util.c -o ./kbe_objs/util.o

./kbe_objs/bccommon.o:  ./blkciphs.h ./blkcipht.h ./util.h bccommon.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  bccommon.c -o ./kbe_objs/bccommon.o

./kbe_objs/blkciphs.o:  ./blkcipht.h blkciphs.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  blkciphs.c -o ./kbe_objs/blkciphs.o

./kbe_objs/aesfif.o:  ./aesf/aesfalg.h ./blkcipht.h ./util.h aesf/aesfif.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  aesf/aesfif.c -o ./kbe_objs/aesfif.o

./kbe_objs/aesfalg.o:  ./aesf/aesf_tabs.h ./aesf/aesfalg.h ./blkcipht.h ./util.h aesf/aesfalg.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  aesf/aesfalg.c -o ./kbe_objs/aesfalg.o

./kbe_objs/aesif.o:  ./aes/aesalg.h ./blkcipht.h ./util.h aes/aesif.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  aes/aesif.c -o ./kbe_objs/aesif.o

./kbe_objs/aesalg.o:  ./aes/aesalg.h ./aes/boxes-ref.dat ./blkcipht.h ./util.h aes/aesalg.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  aes/aesalg.c -o ./kbe_objs/aesalg.o

./kbe_objs/rc6if.o:  ./blkcipht.h ./util.h rc6/rc6if.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  rc6/rc6if.c -o ./kbe_objs/rc6if.o

./kbe_objs/rc6alg.o:  ./blkcipht.h ./util.h rc6/rc6alg.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  rc6/rc6alg.c -o ./kbe_objs/rc6alg.o

./kbe_objs/myx.o:  ./blkcipht.h ./myx/myxbox.h ./myx/myxdat.h ./myx/myxtabs.h ./util.h myx/myx.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  myx/myx.c -o ./kbe_objs/myx.o

./kbe_objs/myx8.o:  ./blkcipht.h ./myx8/myx8box.h ./myx8/myx8dat.h ./myx8/myx8tabs.h ./util.h myx8/myx8.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  myx8/myx8.c -o ./kbe_objs/myx8.o

./kbe_objs/myx8t.o:  ./blkcipht.h ./myx8t/myx8tbox.h ./myx8t/myx8tdat.h ./myx8t/myx8ttabs.h ./util.h myx8t/myx8t.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  myx8t/myx8t.c -o ./kbe_objs/myx8t.o

./kbe_objs/null.o:  ./blkcipht.h triv/null.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  triv/null.c -o ./kbe_objs/null.o

./kbe_objs/binv.o:  ./blkcipht.h triv/binv.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  triv/binv.c -o ./kbe_objs/binv.o

./kbe_objs/twiddle.o:  ./blkcipht.h ./util.h triv/twiddle.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  triv/twiddle.c -o ./kbe_objs/twiddle.o

./kbe_objs/sfei.o:  ./blkcipht.h ./util.h sfei/sfei.c
	gcc -Wall -Werror -O3 $(COMPILE_FLAGS)  sfei/sfei.c -o ./kbe_objs/sfei.o

OBJS = \
	./kbe_objs/main.o \
	./kbe_objs/util.o \
	./kbe_objs/bccommon.o \
	./kbe_objs/blkciphs.o \
	./kbe_objs/aesfif.o \
	./kbe_objs/aesfalg.o \
	./kbe_objs/aesif.o \
	./kbe_objs/aesalg.o \
	./kbe_objs/rc6if.o \
	./kbe_objs/rc6alg.o \
	./kbe_objs/myx.o \
	./kbe_objs/myx8.o \
	./kbe_objs/myx8t.o \
	./kbe_objs/null.o \
	./kbe_objs/binv.o \
	./kbe_objs/twiddle.o \
	./kbe_objs/sfei.o

./kbe: $(OBJS) $(EXTRA_DEPS)
	gcc $(OBJS) -o ./kbe $(LINK_FLAGS)

install: build
	cp ./kbe $(HOME)/bin

clean:
	-@rm -f $(OBJS)
	-@rm -f ./kbe
