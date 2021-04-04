/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[contact-id]");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	struct libcontacts_contact contact;
	char *path;
	int fd;

	NOFLAGS(argc > 1);

	if (argc && (!*argv[0] || strchr(argv[0], '/')))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (argc) {
		path = libcontacts_get_path(argv[0], user);
		if (!path)
			eprintf("libcontacts_get_path %s:", argv[0]);
		fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0666);
		if (fd < 0)
			eprintf("open %s O_WRONLY|O_CREAT|O_EXCL 0666:", path);
		if (close(fd))
			eprintf("close %s:", path);
		free(path);
	} else {
		memset(&contact, 0, sizeof(contact));
		if (libcontacts_save_contact(&contact, user))
			eprintf("libcontacts_save_contact:");
		printf("%s\n", contact.id);
		if (fflush(stdout) || ferror(stdout) || fclose(stdout))
			eprintf("printf:");
		free(contact.id);
	}

	return 0;
}
