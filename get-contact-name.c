/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-F | -f | -l | n] contact-id ...");


int
main(int argc, char *argv[])
{
	size_t offset = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	int ret = 0;
	size_t i;
	char *name;

	ARGBEGIN {
	case 'F':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, full_name);
		break;
	case 'f':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, first_name);
		break;
	case 'l':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, last_name);
		break;
	case 'n':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, nickname);
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	if (!offset)
		offset = offsetof(struct libcontacts_contact, name);

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			weprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
		} else {
			name = *(char **)&offset[(char *)&contact];
			if (name) {
				if (argc > 1)
					printf("%s: ", *argv);
				printf("%s\n", name);
			}
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
