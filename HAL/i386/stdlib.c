
#include "stdlib.h"

uint32_t round_up(uint32_t x, uint32_t bits)
{
	return (x-1&~0<<bits)+(1<<bits);
}

void *memcpy(void *dest, const void *src, size_t n)
{
	void *dest_copy = dest;
	unsigned char *dest_bytes = (unsigned char *)dest;
	const unsigned char *src_bytes = (const unsigned char *)src;
	while (n--) {
		*dest_bytes++ = *src_bytes++;
	}

	return dest_copy;
}

void *memset(void *s, int c, size_t n)
{
	char *r, *ss;
	r = ss = (char *)s;
	while (n--) {
		*ss++ = c;
	}
	return (void *)r;
}

long int strtol(const char *nptr, char **endptr, int base)
{

	return 0;
}

size_t itoa_n(char *buffer, size_t size, int value, unsigned base)
{
	char *rc;
	char *ptr;
	char *low;
	static const char chars[] =
		"zyxwvutsrqponmlkjihgfedcba987654321"
		"0123456789abcdefghijklmnopqrstuvwxyz";

	if (!size) {
		return 0;
	}

	if (base < 2 || base > 36) {
		*buffer = '\0';
		return 1;
	}
	rc = ptr = buffer;

	if (value < 0) {
		*ptr++ = '-';
	}

	low = ptr;
	do {
		*ptr++ = chars[35 + value % base];
		value /= base;
	} while ( value );

	*ptr-- = '\0';

	while (low < ptr) {
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}

	return 1;
}

size_t utoa_n(char *buffer, size_t size, unsigned value, unsigned base)
{
	char *rc;
	char *ptr;
	char *low;
	static const char chars[] =
		"zyxwvutsrqponmlkjihgfedcba987654321"
		"0123456789abcdefghijklmnopqrstuvwxyz";

	if (!size) {
		return 0;
	}

	if (base < 2 || base > 36) {
		*buffer = '\0';
		return 1;
	}
	rc = ptr = buffer;

	low = ptr;
	do {
		*ptr++ = chars[35 + value % base];
		value /= base;
	} while ( value );

	*ptr-- = '\0';

	while (low < ptr) {
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}

	return 1;
}



