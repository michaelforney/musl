long double rintl(long double x)
{
	__asm__ ("frndint" : "=t"(x) : "0"(x));
	return x;
}
