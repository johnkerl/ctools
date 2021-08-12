# ----------------------------------------------------------------
COMPILE=gcc -c -O3 -Wall -Werror
LINK=gcc

# ----------------------------------------------------------------
all: d2h d2o h2d o2d d2b h2b b2h b2d

d2h: d2h.o
	$(LINK) d2h.o -o d2h

d2o: d2o.o
	$(LINK) d2o.o -o d2o

h2d: h2d.o
	$(LINK) h2d.o -o h2d

o2d: o2d.o
	$(LINK) o2d.o -o o2d

d2b: d2b.o binary_cvt.o
	$(LINK) d2b.o binary_cvt.o -o d2b

h2b: h2b.o binary_cvt.o
	$(LINK) h2b.o binary_cvt.o -o h2b

b2h: b2h.o binary_cvt.o
	$(LINK) b2h.o binary_cvt.o -o b2h

b2d: b2d.o binary_cvt.o
	$(LINK) b2d.o binary_cvt.o -o b2d

install: d2h d2o h2d o2d d2b h2b b2h b2d
	strip d2h d2o h2d o2d d2b h2b b2h b2d
	cp d2h d2o h2d o2d d2b h2b b2h b2d $(HOME)/bin

# ----------------------------------------------------------------
d2h.o: d2h.c
	$(COMPILE) d2h.c

h2d.o: h2d.c
	$(COMPILE) h2d.c

d2b.o: d2b.c binary_cvt.h
	$(COMPILE) d2b.c

h2b.o: h2b.c binary_cvt.h
	$(COMPILE) h2b.c

b2h.o: b2h.c binary_cvt.h
	$(COMPILE) b2h.c

b2d.o: b2d.c binary_cvt.h
	$(COMPILE) b2d.c

binary_cvt.o: binary_cvt.c binary_cvt.h
	$(COMPILE) binary_cvt.c

# ----------------------------------------------------------------
clean:
	-@rm *.o d2h d2o h2d o2d d2b h2b b2h b2d
