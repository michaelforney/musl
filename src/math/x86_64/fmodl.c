#ifndef __LONG_DOUBLE_64__

long double fmodl(long double x, long double y)
{
	unsigned short sw;
	do {
		__asm__ (
			"fprem\n"
			"fnstsw %1\n"
			: "=t"(x), "=a"(sw) : "0"(x), "u"(y)
		);
	} while (sw & 0x0400);
	return x;
}

#else

#include "../fmodl.c"

#endif
