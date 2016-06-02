# Bare POSIX doesn't provide anything like -Wall for cc(1).
# http://pubs.opengroup.org/onlinepubs/7908799/xcu/cc.html
CFLAGS=`pkg-config --cflags libpng` -O
LDLIBS=`pkg-config --libs libpng`
PREFIX=/usr/local

TAG=`git tag --contains HEAD`
NAME=fsh2png-$(TAG)-`uname -s`

# Cygwin 'make' aliases to 'fsh2png' to 'fsh2png.exe'.

all: fsh2png

static: fsh2png-static

fsh2png: fsh2png.c
	$(CC) $(CFLAGS) fsh2png.c $(LDLIBS) -o $@

# OS X deprecated -s for ld(1). Go figure.
fsh2png-static: fsh2png.c
	$(CC) $(CFLAGS) fsh2png.c -static `pkg-config --libs --static libpng` -o $@
	-strip $@

release: fsh2png-static
	if [ -z "$(TAG)" ]; then echo "HEAD has no git tag."; exit 1; fi
	mkdir $(NAME)
	ln fsh2png-static $(NAME)/fsh2png
	ln README.md LICENSE.md $(NAME)
	tar cvzf $(NAME).tar.gz $(NAME)
	rm -r $(NAME)

clean:
	-rm fsh2png fsh2png-static

install: fsh2png
	cp fsh2png $(PREFIX)/bin

uninstall:
	-rm $(PREFIX)/bin/fsh2png
