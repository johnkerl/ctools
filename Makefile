# John Kerl
# 2012-01-04

build:
	cd binv       && make
	cd bytehisto  && make
	cd csum       && make
	cd cvt        && make
	cd dos2unix   && make
	cd hex        && make
	cd jot        && make
	cd qrand      && make
	cd rand       && make
	cd crypto/kbe && make

mk:
	cd binv        && make mk
	cd bytehisto   && make mk
	cd csum        && make mk
	cd cvt         && make mk
	cd dos2unix    && make mk
	cd hex         && make mk
	cd jot         && make mk
	cd qrand       && make mk
	cd rand        && make mk
	cd crypto/kbe  && make mk

install:
	cd binv        && make install
	cd bytehisto   && make install
	cd csum        && make install
	cd cvt         && make install
	cd dos2unix    && make install
	cd hex         && make install
	cd jot         && make install
	cd qrand       && make install
	cd rand        && make install
	cd crypto/kbe  && make install

clean:
	cd binv        && make clean
	cd bytehisto   && make clean
	cd csum        && make clean
	cd cvt         && make clean
	cd dos2unix    && make clean
	cd hex         && make clean
	cd jot         && make clean
	cd qrand       && make clean
	cd rand        && make clean
	cd crypto/kbe  && make clean
