#include <wchar.h>

wchar_t *wmempcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	while (n--) *dest++ = *src++;
	return dest;
}
