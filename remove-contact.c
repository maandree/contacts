/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("contact-id");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	char *path;

	NOFLAGS(argc != 1);

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	path = libcontacts_get_path(argv[0], user);
	if (!path)
		eprintf("libcontacts_get_path %s:", argv[0]);

	if (unlink(path))
		eprintf("unlink %s:", path);

	free(path);
	return 0;
}
