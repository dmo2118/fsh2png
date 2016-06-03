# Bare POSIX doesn't provide anything like -Wall for cc(1).
# http://pubs.opengroup.org/onlinepubs/7908799/xcu/cc.html
CFLAGS=`pkg-config --cflags libpng` -O
LDLIBS=`pkg-config --libs libpng`
PREFIX=/usr/local

# Cygwin 'make' aliases to 'fsh2png' to 'fsh2png.exe'.

all: fsh2png

static: fsh2png-static

fsh2png: fsh2png.c
	$(CC) $(CFLAGS) fsh2png.c $(LDLIBS) -o $@

# OS X deprecated -s for ld(1). Go figure.
# OS X doesn't support -static, so we can't just do -static -lpng, like
# pkg-config suggests.
fsh2png-static: fsh2png.c
	$(CC) $(CFLAGS) fsh2png.c $(STATIC_LIB)/libpng.a $(STATIC_LIB)/libz.a -lm -o $@
	-strip $@

clean:
	-rm fsh2png fsh2png-static

install: fsh2png
	cp fsh2png $(PREFIX)/bin

uninstall:
	-rm $(PREFIX)/bin/fsh2png
