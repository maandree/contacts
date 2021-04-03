/* See LICENSE file for copyright and license details. */
#include <libcontacts.h>
#include <libsimple.h>
#include <libsimple-arg.h>


#ifdef MULTICALL_BINARY
# undef NUSAGE
# define NUSAGE(STATUS, SYNOPSIS)\
	static _LIBSIMPLE_NORETURN void\
	usage(void)\
	{\
		const char *syn = SYNOPSIS ? SYNOPSIS : "";\
		fprintf(stderr, "usage: %s%s%s\n", argv0, *syn ? " " : "", syn);\
		exit(STATUS);\
	}\
	extern char *argv0
#endif
