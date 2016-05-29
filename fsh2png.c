#ifdef _MSC_VER
#	define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <png.h> /* Requires libpng. */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h> /* C99 standard library, but the code is C89. */
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#	include <tchar.h>
typedef ptrdiff_t ssize_t;
#else

typedef char _TCHAR;
#	define _T(s)        s
#	define _tmain       main

#	define _fputts      fputs
#	define _ftprintf    fprintf
#	define _tfopen      fopen
#	define _tperror     perror

#	define _tcscpy      strcpy
#	define _tcserror    strerror
#	define _tcslen      strlen

#endif

#define arraysize(a) (sizeof(a) / sizeof(*(a)))
#define arrayend(a) ((a) + arraysize(a))

/* Fish! 2.0 used 32x32, at least on Mac OS. */
#define FISH_SIZE 64

struct _fsh_frame
{
	uint8_t color[FISH_SIZE * FISH_SIZE / (8 / 4)];
	uint8_t alpha[FISH_SIZE * FISH_SIZE / (8 / 1)];
};

struct _fsh_file
{
	uint8_t header[0x84]; /* Contents are a mystery. */
	uint8_t palette[16][4];
	struct _fsh_frame frames[2];
};

int _enomem(_TCHAR *progname)
{
	/* MinGW says: _wcserror requires __MSVCRT_VERSION__ >= 0x0700 */
	_ftprintf(stderr, _T("%s: %s"), progname, _tcserror(ENOMEM));
	return EXIT_FAILURE;
}

#if defined __x86_64__ || defined __i386__ || defined _M_I86 || defined _M_IX86 || defined _M_X64

#	define IS_BIG_ENDIAN() (0)

#else

/* This always works, but #defining IS_BIG_ENDIAN() like above probably helps with optimization. */

/*
LSBFirst = 0
MSBFirst = 1
*/

union _endian
{
	uint16_t u16;
	uint8_t u8;
};

static const union _endian _endian_data = {0x100};

#	define IS_BIG_ENDIAN() (_endian_data.u8)

#endif

int _tmain(int argc, _TCHAR **argv)
{
	_TCHAR *progname = argv[0];
	if(argc <= 1)
	{
		_ftprintf(stderr, _T("Usage: %s fish1.fsh [ fish2.fsh ...]\n"), progname);
		return EXIT_SUCCESS;
	}

	for(;;)
	{
		_TCHAR *fsh_name;
		struct _fsh_file fsh;

		png_structp png;
		png_infop png_info;
		FILE *stream_png;
		png_byte png_image[FISH_SIZE * FISH_SIZE * arraysize(fsh.frames)];
		png_byte *png_rows[FISH_SIZE];

		/* Use of 'continue' means this must go here. */
		++argv;
		if(!*argv)
			break;

		fsh_name = *argv;

		{
			FILE *fsh_stream = _tfopen(fsh_name, _T("rb"));
			ssize_t fsh_read;

			if(!fsh_stream)
			{
				_tperror(fsh_name);
				continue;
			}

			fsh_read = fread(&fsh, sizeof(fsh), 1, fsh_stream);

			if(fsh_read != 1)
			{
				if(fsh_read < 0)
					_tperror(fsh_name);
				else
					_ftprintf(stderr, _T("%s: Too small"), fsh_name);
				fclose(fsh_stream);
				continue;
			}

			fclose(fsh_stream);
		}

		png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!png)
			return _enomem(progname);

		png_info = png_create_info_struct(png);
		if(!png_info)
		{
			png_destroy_write_struct(&png, NULL);
			return _enomem(progname);
		}

		{
			size_t name_len = _tcslen(fsh_name);

			const _TCHAR *name_out;
			_TCHAR *name_buf = NULL;
			if(name_len >= 4 &&
				fsh_name[name_len - 4] == '.' &&
				tolower(fsh_name[name_len - 3]) == 'f' &&
				tolower(fsh_name[name_len - 2]) == 's' &&
				tolower(fsh_name[name_len - 1]) == 'h')
			{
				fsh_name[name_len - 3] = 'p';
				fsh_name[name_len - 2] = 'n';
				fsh_name[name_len - 1] = 'g';
				name_out = fsh_name;
			}
			else
			{
				name_buf = malloc((name_len + 5) * sizeof(_TCHAR));
				memcpy(name_buf, fsh_name, sizeof(_TCHAR) * name_len);
				_tcscpy(name_buf + name_len, _T(".png"));
				name_out = name_buf;
			}

			stream_png = _tfopen(name_out, _T("wb"));
			if(!stream_png)
			{
				_tperror(name_out);
				png_destroy_write_struct(&png, &png_info);
				continue;
			}

			free(name_buf);
		}

		png_init_io(png, stream_png);

		png_set_IHDR(
			png,
			png_info,
			FISH_SIZE * arraysize(fsh.frames),
			FISH_SIZE,
			8,
			PNG_COLOR_TYPE_PALETTE,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

		{
			/* PNG transparency must come at the beginning of the palette. */
			png_color palette[17];
			unsigned i;

			palette[0].red = 0;
			palette[0].green = 0;
			palette[0].blue = 0;

			for(i = 0; i != 16; ++i)
			{
				palette[i + 1].red = fsh.palette[i][2];
				palette[i + 1].green = fsh.palette[i][1];
				palette[i + 1].blue = fsh.palette[i][0];
			}

			png_set_PLTE(png, png_info, palette, arraysize(palette));
		}

		{
			png_byte png_alpha = 0;
			png_set_tRNS(png, png_info, &png_alpha, 1, NULL);
		}

		{
			/* Upside-down. */
			unsigned y;
			png_byte *row = png_image;
			for(y = 0; y != FISH_SIZE; ++y)
			{
				png_rows[y] = row;
				row += FISH_SIZE * arraysize(fsh.frames);
			}
		}

		memset(png_image, 0, sizeof(png_image));

		{
			size_t frame, x, y;
			for(frame = 0; frame != arraysize(fsh.frames); ++frame)
			{
				size_t dest_x = frame * FISH_SIZE;
				png_byte *row_src = fsh.frames[frame].color;
				png_byte *row_alpha = fsh.frames[frame].alpha;

				for(y = 0; y != FISH_SIZE; ++y)
				{
#if 0
					/* Simple! */
					png_byte *row_dest = png_rows[y ^ (FISH_SIZE - 1)] + dest_x;

					for(x = 0; x != FISH_SIZE; ++x)
						row_dest[x] = row_alpha[x / 8] & (1 << ((x & 7) ^ 7)) ?
							(png_byte)((row_src[x / 2] >> ((x & 1 ^ 1) * 4) & 0xf) + 1) :
							0;
#else
					/* Fast! And pointlessly complicated! */
					uint64_t *row_dest = (uint64_t *)(png_rows[y ^ (FISH_SIZE - 1)] + dest_x);
					const uint32_t *row_src32 = (const uint32_t *)(row_src);

					for(x = 0; x != FISH_SIZE / 8; ++x)
					{
						/* No love for 32-bit systems here. */
						uint64_t alpha1, alpha7, color;

						color = row_src32[x];
						color = (color & 0x0000ffff0000ffffull) | ((color & 0xffff0000ffff0000ull) << 16);
						color = (color & 0x00ff00ff00ff00ffull) | ((color & 0xff00ff00ff00ff00ull) <<  8);

						if(IS_BIG_ENDIAN())
						{
							color = (color & 0x0f0f0f0f0f0f0f0full) | ((color & 0xf0f0f0f0f0f0f0f0ull) <<  4);

							alpha1 = row_alpha[x];
							alpha1 |= (alpha1 & 0x00000000000000f0ull) << (32 - 4);
							alpha1 |= (alpha1 & 0x0000000c0000000cull) << (16 - 2);
							alpha1 |= (alpha1 & 0x0002000200020002ull) << ( 8 - 1);
							alpha1 &= 0x0101010101010101ull;
							alpha7 = alpha1 << 7;
						}
						else
						{
							color = (color & 0xf0f0f0f0f0f0f0f0ull) | ((color & 0x0f0f0f0f0f0f0f0full) << (8 + 4));
							color >>= 4;

							alpha7 = row_alpha[x];
							alpha7 |= (alpha7 & 0x000000000000000full) << (32 + 4);
							alpha7 |= (alpha7 & 0x0000003000000030ull) << (16 + 2);
							alpha7 |= (alpha7 & 0x0040004000400040ull) << ( 8 + 1);
							alpha7 &= 0x8080808080808080ull;
							alpha1 = alpha7 >> 7;
						}

						row_dest[x] = (color + 0x0101010101010101ull) & (alpha7 - alpha1);
					}
#endif

					row_src += FISH_SIZE / 2;
					row_alpha += FISH_SIZE / 8;
				}
			}
		}

		png_set_rows(png, png_info, png_rows);
		png_write_png(png, png_info, 0, NULL);

		png_destroy_write_struct(&png, &png_info);
	}

	return EXIT_SUCCESS;
}
