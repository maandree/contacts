/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-F | -f | -l | -n] (-u contact-id | contact-id name)");


int
main(int argc, char *argv[])
{
	int unset = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t offset = 0;
	char **namep;

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
	case 'u':
		unset = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 2 - unset)
		usage();

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	if (!offset)
		offset = offsetof(struct libcontacts_contact, name);

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(*argv, &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");

	namep = (char **)&offset[(char *)&contact];
	if (!unset || *namep) {
		free(*namep);
		*namep = unset ? NULL : estrdup(argv[1]);

		if (libcontacts_save_contact(&contact, user))
			eprintf("libcontacts_save_contact %s:", *argv);
	}

	libcontacts_contact_destroy(&contact);
	return 0;
}
