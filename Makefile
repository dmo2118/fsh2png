# Bare POSIX doesn't provide anything like -Wall for cc(1).
# http://pubs.opengroup.org/onlinepubs/7908799/xcu/cc.html
CFLAGS=-O
LDLIBS=-lpng
PREFIX=/usr/local

# Cygwin 'make' aliases to 'fsh2png' to 'fsh2png.exe'.

all: fsh2png

static: fsh2png-static

fsh2png: fsh2png.c

# OS X deprecated -s for ld(1). Go figure.
fsh2png-static: fsh2png.c
	$(CC) $(CFLAGS) $^ $(STATIC_LIB)/libpng.a $(STATIC_LIB)/libz.a -o fsh2png-static
	-strip fsh2png-static

clean:
	-rm fsh2png fsh2png-static

install: fsh2png
	cp fsh2png $(PREFIX)/bin

uninstall:
	-rm $(PREFIX)/bin/fsh2png
