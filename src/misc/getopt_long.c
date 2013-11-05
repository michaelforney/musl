#define _GNU_SOURCE
#include <stddef.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

extern int __optpos, __optreset;

static int __getopt_long(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx, int longonly)
{
	if (!optind || __optreset) {
		__optreset = 0;
		__optpos = 0;
		optind = 1;
	}
	if (optind >= argc || !argv[optind] || argv[optind][0] != '-') return -1;
	if ((longonly && argv[optind][1]) ||
		(argv[optind][1] == '-' && argv[optind][2]))
	{
		int i, j;
		for (i=0; longopts[i].name; i++) {
			const char *name = longopts[i].name;
			char *opt = argv[optind]+1, *c;
			if (*opt == '-') opt++;
			for (c = opt; *name && *name == *c; name++, c++);
			if (*c && *c != '=') continue;
			if (*name) {
				if (name == longopts[i].name) continue;
				for (j=i+1; longopts[j].name && strncmp(opt, longopts[j].name, c - opt); j++);
				if (longopts[j].name) continue;
			}
			if (*c == '=') {
				if (!longopts[i].has_arg) continue;
				optarg = c+1;
			} else {
				if (longopts[i].has_arg == required_argument) {
					if (!(optarg = argv[++optind]))
						return ':';
				} else optarg = NULL;
			}
			optind++;
			if (idx) *idx = i;
			if (longopts[i].flag) {
				*longopts[i].flag = longopts[i].val;
				return 0;
			}
			return longopts[i].val;
		}
		if (argv[optind][1] == '-') {
			optind++;
			return '?';
		}
	}
	return getopt(argc, argv, optstring);
}

int getopt_long(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx)
{
	return __getopt_long(argc, argv, optstring, longopts, idx, 0);
}

int getopt_long_only(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx)
{
	return __getopt_long(argc, argv, optstring, longopts, idx, 1);
}
