#ifndef __LONG_DOUBLE_64__

long double atan2l(long double y, long double x)
{
	__asm__ ("fpatan" : "=t"(x) : "0"(x), "u"(y) : "st(1)");
	return x;
}

#else

#include "../atan2l.c"

#endif
