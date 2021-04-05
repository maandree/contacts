/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-f] contact-id ...");


int
main(int argc, char *argv[])
{
	int output_flag = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'f':
		output_flag = 1;
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
			if (argc > 1)
				printf("%s: ", *argv);
			if (contact.gender == LIBCONTACTS_NOT_A_PERSON)
				printf("%s\n", output_flag ? "-o" : "none");
			else if (contact.gender == LIBCONTACTS_MALE)
				printf("%s\n", output_flag ? "-m" : "male");
			else if (contact.gender == LIBCONTACTS_FEMALE)
				printf("%s\n", output_flag ? "-f" : "female");
			else
				printf("%s\n", output_flag ? "-u" : "unset");
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
