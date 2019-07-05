long double atanl(long double x)
{
	__asm__ (
		"fld1\n"
		"fpatan\n" : "=t"(x) : "0"(x)
	);
	return x;
}
