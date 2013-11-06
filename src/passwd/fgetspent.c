#include "pwf.h"

struct spwd *fgetspent(FILE *f)
{
	static char *line;
	static struct spwd sp;
	static size_t size;
	struct spwd *res = 0;
	if (getline(&line, &size, f) >= 0 && __parsespent(line, &sp, 0, 0) >= 0) res = &sp;
	return res;
}
