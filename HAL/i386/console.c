
#include "console.h"

#include <stdarg.h>

/* grey foreground, black background, no flashing, no intensity */
static unsigned cursor_attribute = 7;
static unsigned cursor_x = 0, cursor_y = 0;
static unsigned console_width = 80;
static unsigned console_height = 25;
static unsigned console_pitch = 160;
static unsigned char *console_base = (unsigned char *)0xb8000;

void console_clear(void)
{
	volatile char *s = console_base;
	unsigned int n = console_width * console_height;

	while (n--)
	{
		*s++ = 0;
		*s++ = cursor_attribute;
	}

	console_cursor_set(0, 0);
}

void console_mode_set(unsigned width, unsigned height, unsigned graphics)
{
	if (vga_mode_set(width, height, graphics)) {
		console_width = width;
		console_height = height;
		console_pitch = width*2;
		console_cursor_set(0, 0);
	}
}

void console_cursor_get(unsigned *x, unsigned *y)
{
	*x = cursor_x;
	*y = cursor_y;
}

void console_cursor_set(unsigned x, unsigned y)
{
	unsigned pos = x + y*console_width;
	cursor_x = x;
	cursor_y = y;

	if (pos >= console_width * console_height) {
		return;
	}
	out16(0x3d4, (pos & 0xff00) | 0x0e);
	out16(0x3d4, ((pos << 8) & 0xff00) | 0x0f);
}

void console_attribute_set(unsigned attribute)
{
	cursor_attribute = attribute;
}

void console_attribute_get(unsigned *attribute)
{
	*attribute = cursor_attribute;
}

void console_write_hex(uint32_t value, unsigned nibbles)
{
	char string[2];

	string[1] = 0;
	value <<= (8-nibbles)*4;

	while (nibbles--) {
		value = (value << 4) | (value >> 28);
		string[0] = "0123456789ABCDEF"[value & 0xf];
		console_write(string);
	}
}

void console_write_address(const void *address)
{
	console_write_hex(*(uint32_t *)&address, 8);
}

int console_printf(const char *format, ...)
{
	char c;
	const char *f = format;
	enum { CHAR, FORMAT_START } state;
	enum { NORMAL, LONG, LONG_LONG } modifier;
	enum { FILL, WIDTH } fill_width_state;
	char fill = '\0';
	unsigned width = 0;
	char buffer[16];
	va_list args;
	va_start(args, format);

	state = CHAR;

	while (c = *f++)
	{
		switch (state)
		{
			case CHAR :
				switch (c)
				{
					case '%' :
						state = FORMAT_START;
						modifier = NORMAL;
						continue;
						break;
					default :
						buffer[0] = c;
						buffer[1] = '\0';
						console_write(buffer);
				}
				break;
			case FORMAT_START :
				switch (c)
				{
					case '%' :
						buffer[0] = '%';
						buffer[1] = '\0';
						console_write(buffer);
						state = CHAR;
						break;
					case 'l' :
						switch (modifier)
						{
							case NORMAL : modifier = LONG; break;
							case LONG : modifier = LONG_LONG; break;
						}
						break;
					case 'x' :
					{
						switch (modifier)
						{
							case NORMAL :
							{
								unsigned value = va_arg(args, unsigned);
								console_write_hex(value, width);
								state = CHAR;
								fill_width_state = FILL;
							}
							break;
							case LONG_LONG :
							{
								unsigned long long value = va_arg(args, unsigned long long);
								console_write_hex(value >> 32, 8);
								console_write_hex(value & 0xffffffffULL, 8);
								state = CHAR;
								fill_width_state = FILL;
							}
							break;
						}
						break;
					}
					case 'u' :
					{
						switch (modifier)
						{
							case NORMAL :
							{
								unsigned value = va_arg(args, unsigned);
								console_write_unsigned(value, 10);
								state = CHAR;
								fill_width_state = FILL;
							}
							break;
							case LONG_LONG :
							{
								unsigned long long value = va_arg(args, unsigned long long);
								console_write_hex(value >> 32, 8);
								console_write_hex(value & 0xffffffffULL, 8);
								state = CHAR;
								fill_width_state = FILL;
							}
							break;
						}
						break;
					}
					case 'p' :
					{
						const void *address = va_arg(args, const void *);
						console_write_address(address);
						state = CHAR;
						fill_width_state = FILL;
						break;
					}
					case 's' :
					{
						const char *string = va_arg(args, const char *);
						console_write(string);
						state = CHAR;
						fill_width_state = FILL;
						break;
					}
					case '0' : case '1' : case '2' : case '3' : case '4' :
					case '5' : case '6' : case '7' : case '8' : case '9' :
					{
						if (fill_width_state == FILL) {
							if (c == '0') {
								fill = '0';
							} else {
							}
							fill_width_state = WIDTH;
						} else if (fill_width_state == WIDTH) {
							width = c - '0';
						} else {
						}
					}
				}
				break;
		}
	}

	va_arg(args, int);
	va_end(args);

	return 0;
}

void console_scroll(void)
{
	unsigned y = 0;
	for (y=0; y<console_height; y++) {
		memcpy(console_base + y*console_width*2
			,console_base + (y+1)*console_width*2
			,console_width*2
		);
	}
	memset(console_base+(console_height-1)*console_width*2, 0, console_width*2);
	cursor_y--;
}

void console_write(const char *s)
{
	char c;
	while (c = *s) {
		console_write_char(c);
		s++;
	}
}



void console_write_unsigned(unsigned v, unsigned base)
{
	unsigned d = 0, r = 0;
	char buffer[64] = { 0 };
	char *pb = buffer;
	char c = '\0';

	do {
		d = v / base;
		r = v - (d * base);
		v = d;
		if (r < 10) {
			c = r + '0';
		} else {
			c = (r - 10) + 'A';
		}
		*pb++ = c;
	} while (v);

	while (pb > buffer) {
		--pb;
		console_write_char(*pb);
	}
}

void console_write_char(char c)
{
	unsigned char *char_ptr, *attr_ptr;

	if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
		if (cursor_y >= console_height) {
			console_scroll();
		}
	} else {
		char_ptr = console_base + (cursor_x*2) + (cursor_y*console_width*2);
		attr_ptr = char_ptr + 1;
		*char_ptr = c;
		*attr_ptr = cursor_attribute;

		cursor_x++;
		if (cursor_x >= console_width) {
			cursor_x = 0;
			cursor_y++;
		}
		if (cursor_y >= console_height) {
			console_scroll();
		}
	}

	console_cursor_set(cursor_x, cursor_y);
}


