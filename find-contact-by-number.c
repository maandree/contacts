/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a address-book-country-calling-code] [-l local-country-calling-code] [-c context] [-F | -f] [-M | -m] [-t] (-L | number)");


int
main(int argc, char *argv[])
{
	int list = 0, require_mobile = -1, require_fax = -1, display_type = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_number **numbers, *number;
	const char *context = NULL, *cc_contacts = NULL, *cc_location = NULL;
	size_t i;

	ARGBEGIN {
	case 'a':
		if (cc_contacts)
			usage();
		cc_contacts = ARG();
		break;
	case 'l':
		if (cc_location)
			usage();
		cc_location = ARG();
		break;
	case 'c':
		if (context)
			usage();
		context = ARG();
		break;
	case 'F':
		if (require_fax >= 0)
			usage();
		require_fax = 0;
		break;
	case 'f':
		if (require_fax >= 0)
			usage();
		require_fax = 1;
		break;
	case 'M':
		if (require_mobile >= 0)
			usage();
		require_mobile = 0;
		break;
	case 'm':
		if (require_mobile >= 0)
			usage();
		require_mobile = 1;
		break;
	case 'L':
		list = 1;
		break;
	case 't':
		display_type = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1 - list)
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user, 1))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((numbers = contacts[i]->numbers)) {
			for (; (number = *numbers); numbers++) {
				if (!number->number)
					continue;
				if (require_mobile >= 0 && number->is_mobile != require_mobile)
					continue;
				if (require_fax >= 0 && number->is_facsimile != require_fax)
					continue;
				if (context && strcmpnul(number->context, context))
					continue;
				if (list) {
					if (display_type)
						printf("%c%c ", "-m"[number->is_mobile], "-f"[number->is_facsimile]);
					printf("%s (%s)\n", contacts[i]->id, number->number);
				} else if (libcontacts_same_number(number->number, cc_contacts, argv[0], cc_location)) {
					if (display_type)
						printf("%c%c ", "-m"[number->is_mobile], "-f"[number->is_facsimile]);
					printf("%s\n", contacts[i]->id);
				}
			}
		}
		libcontacts_contact_destroy(contacts[i]);
		free(contacts[i]);
	}
	free(contacts);

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return 0;
}
