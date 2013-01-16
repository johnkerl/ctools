build:
	make -f hex.mk
	make -f unhex.mk

mk:
	# https://github.com/johnkerl/scripts/blob/master/fundam/yamm
	yamm hex.mki
	yamm unhex.mki

install:
	make -f hex.mk install
	make -f unhex.mk install

clean:
	make -f hex.mk clean
	make -f unhex.mk clean
