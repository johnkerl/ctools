#COMPILE=gcc -c -O2 -I../../lib -pg
#LINK=gcc -pg

COMPILE=gcc -c -O3 -I../../lib
LINK=gcc

aes: main.o util.o fsutil.o aesapi.o aesalg.o
	$(LINK) main.o util.o fsutil.o aesapi.o aesalg.o -o aes

aescheck: aescheck.c
	gcc aescheck.c -o aescheck

install: aes aescheck
	cp aes aescheck $(HOME)/bin

main.o: main.c aesapi.h aesalg.h util.h
	$(COMPILE) main.c

util.o: util.c util.h
	$(COMPILE) util.c

fsutil.o: ../../lib/fsutil.c aesapi.h aesalg.h
	$(COMPILE) ../../lib/fsutil.c

aesapi.o: aesapi.c aesapi.h aesalg.h util.h
	$(COMPILE) aesapi.c

aesalg.o: aesalg.c aesalg.h boxes-ref.dat
	$(COMPILE) aesalg.c

#$(COMPILE) -finline-functions aesalg.c

# ----------------------------------------------------------------
backup:
	-mkdir -p backups/`date +%Y-%m-%d`
	cp *.h *.c *.dat Makefile -p backups/`date +%Y-%m-%d`/

# ----------------------------------------------------------------
clean:
	-@rm -f *.o aes aescheck
