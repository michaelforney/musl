#ifndef __LONG_DOUBLE_64__

long double rintl(long double x)
{
	__asm__ ("frndint" : "=t"(x) : "0"(x));
	return x;
}

#else

#include "../rintl.c"

#endif
