fsh2png
=======

Converts Fish! 3.0 .FSH files to PNG.

Fish! is (c) Tom and Ed's Bogus Software. Shareware versions are still
available for [16-bit Windows](http://cd.textfiles.com/wingold/GAMES/F1120/)
and [Mac OS Classic](http://www.macintoshrepository.org/2752-fish-).

Requires [libpng](http://www.libpng.org/pub/png/libpng.html).

Synopsis
--------

	fsh2png fish1.fsh [fish2.fsh ...]

Each fish specified on the command line with be converted to a PNG file in the
same directory as the .FSH file.

Building on POSIX-compatible systems (Linux, OS X, Cygwin)
----------------------------------------------------------

	$ make

Building on Windows (MinGW-w64)
-------------------------------

	$ mingw32-make -f Makefile.mingw UNICODE=1

Building on Windows (Microsoft Visual Studio)
---------------------------------------------

From a Visual Studio developer command prompt:

	SET INCLUDE=%INCLUDE%;(path to png.h on your system)
	SET LIB=%LIB%;(path to libpng16.lib on your system)
	NMAKE /F Makefile.msc UNICODE=1

**Warning:** Both `fsh2png.exe` and `libpng16.dll` must use the same C runtime
library. (`MSVCRT.DLL` is a common choice.)

Building (static linking)
-------------------------

	$ make static STATIC_LIB=(path to libpng.a and libz.a)

`libpng.a` and `libz.a` may already be on your system:

<table>
	<tr><th>OS</th><th>Path</th><th>Package</th></tr>
	<tr>
		<td>Debian (x86-64)</td>
		<td>/usr/lib/x86_64-linux-gnu</td>
		<td><a href="https://packages.debian.org/stable/libpng-dev">libpng-dev</a></td>
	</tr>
	<tr>
		<td>OS X (MacPorts)</td>
		<td>/opt/local/lib</td>
		<td><a href="https://trac.macports.org/browser/trunk/dports/graphics/libpng/Portfile">libpng</a></td>
	</tr>
</table>

`STATIC_LIB=` is ignored on Windows.

Bugs
----

`fsh2png` will not check to make sure that the .FSH file contains an actual
fish; the file format doesn't appear to be documented.

License
-------

`fsh2png` is distributed under the terms of the
[ISC License](https://www.isc.org/downloads/software-support-policy/isc-license/);
see [`LICENSE.md`](LICENSE.md).
