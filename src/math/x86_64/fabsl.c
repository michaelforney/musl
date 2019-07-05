long double fabsl(long double x)
{
	__asm__ ("fabs" : "=t"(x) : "0"(x));
	return x;
}
