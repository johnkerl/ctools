build:
	make -f dos2unix.mk
	make -f unix2dos.mk

mk:
	# https://github.com/johnkerl/scripts/blob/master/fundam/yamm
	yamm dos2unix.mki
	yamm unix2dos.mki

install:
	make -f dos2unix.mk install
	make -f unix2dos.mk install

clean:
	make -f dos2unix.mk clean
	make -f unix2dos.mk clean
