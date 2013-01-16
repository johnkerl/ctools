build:
	make -f psdrandgen.mk
	make -f gasdev.mk

mk:
	# https://github.com/johnkerl/scripts/blob/master/fundam/yamm
	yamm psdrandgen.mki
	yamm gasdev.mki

install:
	make -f psdrandgen.mk install
	make -f gasdev.mk install

clean:
	make -f psdrandgen.mk clean
	make -f gasdev.mk clean
