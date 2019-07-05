long double logl(long double x)
{
	long double ln2;
	__asm__ ("fldln2" : "=t"(ln2));
	__asm__ ("fyl2x" : "=t"(x) : "0"(x), "u"(ln2) : "st(1)");
	return x;
}
