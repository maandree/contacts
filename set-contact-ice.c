/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-u] contact-id ...");


int
main(int argc, char *argv[])
{
	int set_ice = 1, ret = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t i;

	ARGBEGIN {
	case 'u':
		set_ice = 0;
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
		} else {
			contact.in_case_of_emergency = set_ice;
			if (libcontacts_save_contact(&contact, user)) {
				weprintf("libcontacts_save_contact %s:", *argv);
				ret = 1;
			}
			libcontacts_contact_destroy(&contact);
		}
	}

	return ret;
}
