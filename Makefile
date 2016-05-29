CFLAGS=-O
LDLIBS=-lpng

# Cygwin 'make' aliases to 'fsh2png' to 'fsh2png.exe'.

all: fsh2png

fsh2png: fsh2png.c

clean:
	-rm fsh2png

PREFIX=/usr/local

install: fsh2png
	cp fsh2png $(PREFIX)/bin

uninstall:
	-rm $(PREFIX)/bin/fsh2png
