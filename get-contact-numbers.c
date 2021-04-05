/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a address-book-country-calling-code] [-l local-country-calling-code] "
      "[-c context] [-n number] [-F | -f] [-M | -m] [-CNT] contact-id ...");


int
main(int argc, char *argv[])
{
	int display_ctx = 0, display_num = 0, display_type = 0;
	int require_mobile = -1, require_fax = -1;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_number **numbers, *number;
	const char *lookup_ctx = NULL, *lookup_num = NULL;
	const char *cc_contacts = NULL, *cc_location = NULL;
	int ret = 0;
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
		if (lookup_ctx)
			usage();
		lookup_ctx = ARG();
		break;
	case 'n':
		if (lookup_num)
			usage();
		lookup_num = ARG();
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
	case 'C':
		display_ctx = 1;
		break;
	case 'N':
		display_num = 1;
		break;
	case 'T':
		display_type = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	if (!display_ctx && !display_num && !display_type) {
		display_ctx  = !lookup_ctx;
		display_num  = !lookup_num;
		display_type = (require_mobile < 0 || require_fax < 0);
	}

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
				if (lookup_num && !libcontacts_same_number(number->number, cc_contacts, lookup_num, cc_location))
					continue;
				if (argc > 1)
					printf("%s: ", *argv);
				if (display_type) {
					printf("%c%c", "-m"[number->is_mobile], "-f"[number->is_facsimile]);
					if (display_ctx || display_num)
						printf(" ");
				}
				if (display_ctx)
					printf("%s%s", number->context, display_num ? ": " : "");
				if (display_num)
					printf("%s", number->number);
				printf("\n");
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
