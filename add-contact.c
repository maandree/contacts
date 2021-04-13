/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[contact-id]");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	struct libcontacts_contact contact;
	const char *id = NULL;
	char *path;
	int fd;
	size_t i;

	NOFLAGS(argc > 1);

	if (argc) {
		id = argv[0];
		if (!id[0])
			usage();
		for (i = 0; id[i]; i++) {
			if (id[i] == '/')
				eprintf("contact-id cannot contain /\n");
			else if (!islower(id[i]) && !isdigit(id[i]) && (i && id[i] != '.') && id[i] != '-')
				if (id[i] != '~' || id[i + 1])
					weprintf("it is recommended to only have [a-z0-9-] in contact-id\n");
		}
		if (id[i - 1] == '~')
			eprintf("contact-id cannot end with ~\n");
		if (id[0] == '.' && strcmp(id, ".me") && strcmp(id, ".nobody"))
			weprintf("it is recommended to only have [a-z0-9-] in contact-id\n");
	}

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");
	if (!user->pw_dir || !*user->pw_dir)
		eprintf("user does not have a home directory\n");
	if (access(user->pw_dir, R_OK | W_OK | X_OK))
		eprintf("access %s R_OK|W_OK|X_OK:", user->pw_dir);

	if (id) {
		path = libcontacts_get_path(id, user);
		if (!path)
			eprintf("libcontacts_get_path %s:", id);
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
