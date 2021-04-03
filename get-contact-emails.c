/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c context] [-a address] [-CA] contact-id ...");


int
main(int argc, char *argv[])
{
	int display_ctx = 0, display_addr = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_email **emails, *email;
	const char *lookup_ctx = NULL, *lookup_addr = NULL;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'c':
		if (lookup_ctx)
			usage();
		lookup_ctx = ARG();
		break;
	case 'a':
		if (lookup_addr)
			usage();
		lookup_addr = ARG();
		break;
	case 'C':
		display_ctx = 1;
		break;
	case 'A':
		display_addr = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	if (lookup_ctx && !lookup_addr && !display_ctx && !display_addr)
		display_addr = 1;
	if (lookup_addr && !lookup_ctx && !display_ctx && !display_addr)
		display_ctx = 1;

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
			for (emails = contact.emails; (email = *emails); emails++) {
				if (lookup_ctx && (!email->context || strcmp(email->context, lookup_ctx)))
					continue;
				if (lookup_addr && (!email->address || strcmp(email->address, lookup_addr)))
					continue;
				if (lookup_ctx && lookup_addr && !display_ctx && !display_addr) {
					printf("%s\n", *argv);
					continue;
				}
				if (argc > 1)
					printf("%s: ", *argv);
				if (display_ctx && display_addr)
					printf("%s: %s\n", email->context, email->address);
				else if (display_addr)
					printf("%s\n", email->address);
				else
					printf("%s\n", email->context);
			}
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
