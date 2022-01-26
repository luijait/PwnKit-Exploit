CFLAGS=-Wall
TRUE=$(shell which true)

.PHONY: all
all: exploit

.PHONY: clean
clean:
	rm -rf tmp/ exploit GCONV_PATH=.
