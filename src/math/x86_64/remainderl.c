#ifndef __LONG_DOUBLE_64__

long double remainderl(long double x, long double y)
{
	unsigned short sw;
	do {
		__asm__ (
			"fprem1\n"
			"fnstsw %1\n"
			: "=t"(x), "=a"(sw) : "0"(x), "u"(y)
		);
	} while (sw & 0x0400);
	return x;
}

#else

#include "../remainderl.c"

#endif
