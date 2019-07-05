long double sqrtl(long double x)
{
	__asm__ ("fsqrt" : "=t"(x) : "0"(x));
	return x;
}
