/* see ../i386/acos.s */

long double acosl(long double x)
{
	__asm__ (
		"fld1\n"
		"fsub %%st(0), %%st(1)\n"
		"fadd %%st(2)\n"
		"fmulp\n"
		"fsqrt\n"
		"fabs\n"
		"fxch\n"
		"fpatan\n"
		: "=t"(x) : "0"(x), "u"(x) : "st(1)"
	);
	return x;
}
