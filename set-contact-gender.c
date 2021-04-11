/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("(-f | -m | -o | -u) contact-id ...");


int
main(int argc, char *argv[])
{
	enum libcontacts_gender gender = LIBCONTACTS_UNSPECIFIED_GENDER;
	int gender_specified = 0, ret = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t i;

	ARGBEGIN {
	case 'f':
		if (gender_specified)
			usage();
		gender_specified = 1;
		gender = LIBCONTACTS_FEMALE;
		break;
	case 'm':
		if (gender_specified)
			usage();
		gender_specified = 1;
		gender = LIBCONTACTS_MALE;
		break;
	case 'o':
		if (gender_specified)
			usage();
		gender_specified = 1;
		gender = LIBCONTACTS_NOT_A_PERSON;
		break;
	case 'u':
		if (gender_specified)
			usage();
		gender_specified = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!gender_specified || !argc)
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
			weprintf("libcontacts_load_contact %s: %s\n", *argv,
			         errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
			continue;
		}
		if (contact.gender != gender) {
			contact.gender = gender;
			if (libcontacts_save_contact(&contact, user)) {
				weprintf("libcontacts_save_contact %s:", *argv);
				ret = 1;
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	return ret;
}
