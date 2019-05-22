#ifndef __LONG_DOUBLE_64__

__asm__ (
	".global log1pl\n"
	".type log1pl,@function\n"
	"log1pl:\n"
	"	mov 14(%rsp),%eax\n"
	"	fldln2\n"
	"	and $0x7fffffff,%eax\n"
	"	fldt 8(%rsp)\n"
	"	cmp $0x3ffd9400,%eax\n"
	"	ja 1f\n"
	"	fyl2xp1\n"
	"	ret\n"
	"1:	fld1\n"
	"	faddp\n"
	"	fyl2x\n"
	"	ret\n"
);

#else

#include "../log1pl.c"

#endif
