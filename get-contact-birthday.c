/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-n] contact-id ...");


int
main(int argc, char *argv[])
{
	int next = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct tm *now;
	time_t tim;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'n':
		next = 1;
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

	tim = time(NULL);
	now = localtime(&tim);
	if (!now)
		eprintf("localtime:");

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			weprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
		} else {
			if (contact.birthday) {
				if (next) {
					print_birthday(contact.birthday, now);
				} else if (contact.birthday->month && contact.birthday->year) {
					if (argc > 1)
						printf("%s: ", *argv);
					print_birthdate(contact.birthday, now);
				}
			}
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
