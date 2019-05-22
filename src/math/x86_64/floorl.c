#ifndef __LONG_DOUBLE_64__

__asm__(
	".global floorl\n"
	".type floorl,@function\n"
	"floorl:\n"
	"	fldt 8(%rsp)\n"
	"1:	mov $0x7,%al\n"
	"1:	fstcw 8(%rsp)\n"
	"	mov 9(%rsp),%ah\n"
	"	mov %al,9(%rsp)\n"
	"	fldcw 8(%rsp)\n"
	"	frndint\n"
	"	mov %ah,9(%rsp)\n"
	"	fldcw 8(%rsp)\n"
	"	ret\n"
	"\n"
	".global ceill\n"
	".type ceill,@function\n"
	"ceill:\n"
	"	fldt 8(%rsp)\n"
	"	mov $0xb,%al\n"
	"	jmp 1b\n"
	"\n"
	".global truncl\n"
	".type truncl,@function\n"
	"truncl:\n"
	"	fldt 8(%rsp)\n"
	"	mov $0xf,%al\n"
	"	jmp 1b\n"
);

#else

#include "../floorl.c"

#endif
