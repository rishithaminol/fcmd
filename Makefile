PREFIX=/usr
BINDIR=$(PREFIX)/bin
CFLAGS=-Wall
CFLAGS+=-g

all: fcmd

fcmd: fcmd.c

install: all
	cp fcmd $(BINDIR)

uninstall:
	rm -f $(BINDIR)/fcmd

clean:
	rm -rf fcmd
