#ifndef __LONG_DOUBLE_64__

long double sqrtl(long double x)
{
	__asm__ ("fsqrt" : "=t"(x) : "0"(x));
	return x;
}

#else

#include "../sqrtl.c"

#endif
