#include "pwf.h"

struct spwd *sgetspent(const char *s)
{
	static char *buf;
	static size_t size;
	static struct spwd sp;
	int namlen, pwdlen;
	size_t reqsize;

	if (__parsespent((char *) s, &sp, &namlen, &pwdlen) < 0) return 0;
	if (size < namlen + pwdlen + 2) {
		buf = realloc(buf, namlen + pwdlen + 2);
		if (!buf) return 0;
		size = namlen + pwdlen + 2;
	}
	sp.sp_namp = memcpy(buf, sp.sp_namp, namlen);
	sp.sp_namp[namlen] = 0;
	sp.sp_pwdp = memcpy(buf + namlen + 1, sp.sp_pwdp, pwdlen);
	sp.sp_pwdp[pwdlen] = 0;
	return &sp;
}
