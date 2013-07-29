#ifndef UTX_INCLUDE_HAL_CONSOLE_H
#define UTX_INCLUDE_HAL_CONSOLE_H

#include "types.h"
#include "stdlib.h"

void console_mode_set(unsigned width, unsigned height, unsigned graphics);
void console_clear(void);
void console_cursor_set(unsigned x, unsigned y);
void console_cursor_get(unsigned *x, unsigned *y);
void console_attribute_set(unsigned attr);
void console_attribute_get(unsigned *attr);
void console_scroll(void);

void console_write(const char *string);
void console_write_hex(uint32_t value, unsigned nibbles);
void console_write_address(const void *address);
void console_write_char(char c);
void console_write_unsigned(unsigned v, unsigned base);

int console_printf(const char *format, ...);

#endif

