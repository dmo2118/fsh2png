#!/bin/sh

rm teapot.png
./fsh2png teapot.fsh || exit
RESULT="`compare -metric AE teapot.png teapot_t.png /dev/null 2>&1`"
echo "$RESULT"
[ "$RESULT" = 0 ]
