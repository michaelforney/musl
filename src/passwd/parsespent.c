#include "pwf.h"

static long xatol(char **s)
{
	long x;
	if (**s == ':') return -1;
	for (x=0; **s-'0'<10U; ++*s) x=10*x+(**s-'0');
	return x;
}

int __parsespent(char *s, struct spwd *sp, int *namlen, int *pwdlen)
{
	sp->sp_namp = s;
	if (!(s = strchr(s, ':'))) return -1;

	if (namlen) *namlen = s - sp->sp_namp;
	else *s = 0;
	sp->sp_pwdp = ++s;
	if (!(s = strchr(s, ':'))) return -1;

	if (pwdlen) *pwdlen = s - sp->sp_pwdp;
	else *s = 0;
	s++; sp->sp_lstchg = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_min = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_max = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_warn = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_inact = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_expire = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_flag = xatol(&s);
	if (*s != '\n') return -1;
	return 0;
}
