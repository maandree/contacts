/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-g group | -n] contact-id ...");


int
main(int argc, char *argv[])
{
	int lookup_unassigned = 0;
	char *lookup_group = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	char **groups;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'g':
		if (lookup_group)
			usage();
		lookup_group = ARG();
		break;
	case 'n':
		lookup_unassigned = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			weprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
		}
		if (lookup_unassigned) {
			if (!contact.groups || !*contact.groups)
				printf("%s\n", *argv);
		} else if ((groups = contact.groups)) {
			for (; *groups; groups++) {
				if (lookup_group) {
					printf("%s\n", *argv);
				} else {
					if (argc > 1)
						printf("%s: ", *argv);
					printf("%s\n", *groups);
				}
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
