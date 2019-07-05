__asm__ (
	".global expm1l\n"
	".type expm1l,@function\n"
	"expm1l:\n"
	"	fldt 8(%rsp)\n"
	"	fldl2e\n"
	"	fmulp\n"
	"	movl $0xc2820000,-4(%rsp)\n"
	"	flds -4(%rsp)\n"
	"	fucomip %st(1),%st\n"
	"	fld1\n"
	"	jb 1f\n"
	"		# x*log2e <= -65, return -1 without underflow\n"
	"	fstp %st(1)\n"
	"	fchs\n"
	"	ret\n"
	"1:	fld %st(1)\n"
	"	fabs\n"
	"	fucomip %st(1),%st\n"
	"	fstp %st(0)\n"
	"	ja 1f\n"
	"	f2xm1\n"
	"	ret\n"
	"1:	push %rax\n"
	"	call 1f\n"
	"	pop %rax\n"
	"	fld1\n"
	"	fsubrp\n"
	"	ret\n"
	"\n"
	".global exp2l\n"
	".type exp2l,@function\n"
	"exp2l:\n"
	"	fldt 8(%rsp)\n"
	"1:	fld %st(0)\n"
	"	sub $16,%rsp\n"
	"	fstpt (%rsp)\n"
	"	mov 8(%rsp),%ax\n"
	"	and $0x7fff,%ax\n"
	"	cmp $0x3fff+13,%ax\n"
	"	jb 4f             # |x| < 8192\n"
	"	cmp $0x3fff+15,%ax\n"
	"	jae 3f            # |x| >= 32768\n"
	"	fsts (%rsp)\n"
	"	cmpl $0xc67ff800,(%rsp)\n"
	"	jb 2f             # x > -16382\n"
	"	movl $0x5f000000,(%rsp)\n"
	"	flds (%rsp)       # 0x1p63\n"
	"	fld %st(1)\n"
	"	fsub %st(1)\n"
	"	faddp\n"
	"	fucomip %st(1),%st\n"
	"	je 2f             # x - 0x1p63 + 0x1p63 == x\n"
	"	movl $1,(%rsp)\n"
	"	flds (%rsp)       # 0x1p-149\n"
	"	fdiv %st(1)\n"
	"	fstps (%rsp)      # raise underflow\n"
	"2:	fld1\n"
	"	fld %st(1)\n"
	"	frndint\n"
	"	fxch %st(2)\n"
	"	fsub %st(2)       # st(0)=x-rint(x), st(1)=1, st(2)=rint(x)\n"
	"	f2xm1\n"
	"	faddp             # 2^(x-rint(x))\n"
	"1:	fscale\n"
	"	fstp %st(1)\n"
	"	add $16,%rsp\n"
	"	ret\n"
	"3:	xor %eax,%eax\n"
	"4:	cmp $0x3fff-64,%ax\n"
	"	fld1\n"
	"	jb 1b             # |x| < 0x1p-64\n"
	"	fstpt (%rsp)\n"
	"	fistl 8(%rsp)\n"
	"	fildl 8(%rsp)\n"
	"	fsubrp %st(1)\n"
	"	addl $0x3fff,8(%rsp)\n"
	"	f2xm1\n"
	"	fld1\n"
	"	faddp             # 2^(x-rint(x))\n"
	"	fldt (%rsp)       # 2^rint(x)\n"
	"	fmulp\n"
	"	add $16,%rsp\n"
	"	ret\n"
);
