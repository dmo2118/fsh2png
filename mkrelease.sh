#!/bin/sh

MAKE=make
LN=ln
E=

# uname -o is somewhat less likely to not include a version number. (i.e. Cygwin, MSYS)
# Linux may show up as "GNU/Linux", and the "GNU" is important here: static/release binaries are still linked dynamically with
# glibc.
UNAME=`(uname -o 2>/dev/null || uname -s) | sed 's:[ /]:_:g'`

if [ "$UNAME" = "Darwin" ]
then
	UNAME=OSX
	if [ -z "$STATIC_LIB" ]
	then
		# For MacPorts.
		export STATIC_LIB=/opt/local/lib
	fi
elif [ "$UNAME" = "Msys" ]
then
	MAKE="mingw32-make -f Makefile.mingw UNICODE=1"
	LN=cp
	UNAME=Win32
	E=.exe
fi

if [ -z "$STATIC_LIB" -a "$UNAME" != "Win32" ]
then
	echo STATIC_LIB undefined.
	exit 1
fi

TAG=`git tag --contains HEAD`

if [ -z "$TAG" ]
then
	echo "warning: HEAD has no git tag."
	TAG=`git show --format=%H -s | dd bs=8 count=1`
fi

NAME="fsh2png-$TAG-$UNAME"

$MAKE clean static || exit 1
mkdir $NAME || exit 1
$LN fsh2png-static$E "$NAME/fsh2png$E" || exit 1
$LN README.md LICENSE.md "$NAME" || exit 1
if [ $UNAME = "Win32" ]
then
	(cd "$NAME" && unix2dos README.md LICENSE.md) || exit 1
	rm "$NAME.zip"
	zip -rm "$NAME.zip" "$NAME" || exit 1
else
	tar czf "$NAME.tar.gz" "$NAME" || exit 1
	rm -r "$NAME"
fi

echo NAME=$NAME
