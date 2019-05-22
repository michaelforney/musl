#ifndef __LONG_DOUBLE_64__

long double log2l(long double x)
{
	long double y = 1;
	__asm__ ("fyl2x" : "=t"(x) : "0"(x), "u"(y) : "st(1)");
	return x;
}

#else

#include "../log2l.c"

#endif
