/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-t title] organisation");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_organisation **orgs, *org;
	char *title = NULL;
	size_t i;

	ARGBEGIN {
	case 't':
		if (title)
			usage();
		title = ARG();
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1)
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user, 0))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((orgs = contacts[i]->organisations)) {
			for (; (org = *orgs); orgs++) {
				if (strcmpnul(org->organisation, argv[0]))
					continue;
				if (title && strcmpnul(org->title, title))
					continue;
				if (title || !org->title)
					printf("%s\n", contacts[i]->id);
				else if (org->title)
					printf("%s (%s)\n", contacts[i]->id, org->title);
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
