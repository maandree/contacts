/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c context] [-F | -f] [-M | -m] contact-id ...");


int
main(int argc, char *argv[])
{
	int require_mobile = -1, require_fax = -1;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_number **numbers, *number;
	const char *lookup_ctx = NULL;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'c':
		if (lookup_ctx)
			usage();
		lookup_ctx = ARG();
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
			continue;
		}
		if ((numbers = contact.numbers)) {
			for (; (number = *numbers); numbers++) {
				if (!number->number)
					continue;
				if (require_mobile >= 0 && number->is_mobile != require_mobile)
					continue;
				if (require_fax >= 0 && number->is_facsimile != require_fax)
					continue;
				if (lookup_ctx && strcmpnul(number->context, lookup_ctx))
					continue;
				if (argc > 1)
					printf("%s: ", *argv);
				printf("%c%c", "-m"[number->is_mobile], "-f"[number->is_facsimile]);
				if (!lookup_ctx)
					printf(" %s (%s)\n", number->number, number->context);
				else
					printf(" %s\n", number->number);
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
