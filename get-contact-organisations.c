/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-o organisation] [-t title] [-OT] contact-id ...");


int
main(int argc, char *argv[])
{
	int display_org = 0, display_title = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_organisation **orgs, *org;
	const char *lookup_org = NULL, *lookup_title = NULL;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'o':
		if (lookup_org)
			usage();
		lookup_org = ARG();
		break;
	case 't':
		if (lookup_title)
			usage();
		lookup_title = ARG();
		break;
	case 'O':
		display_org = 1;
		break;
	case 'T':
		display_title = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	if (lookup_org && !lookup_title && !display_org && !display_title)
		display_title = 1;
	if (lookup_title && !lookup_org && !display_org && !display_title)
		display_org = 1;

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
			for (orgs = contact.organisations; (org = *orgs); orgs++) {
				if (lookup_org && strcmpnul(org->organisation, lookup_org))
					continue;
				if (lookup_title && strcmpnul(org->title, lookup_title))
					continue;
				if (lookup_org && lookup_title && !display_org && !display_title) {
					printf("%s\n", *argv);
					continue;
				}
				if (argc > 1)
					printf("%s: ", *argv);
				if (display_org && display_title)
					printf("%s: %s\n", org->organisation, org->title);
				else if (display_title)
					printf("%s\n", org->title);
				else
					printf("%s\n", org->organisation);
			}
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
