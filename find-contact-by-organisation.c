/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-o organisation] [-t title] [-OT]");


int
main(int argc, char *argv[])
{
	int display_organisation = 0, display_title = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_organisation **orgs, *org;
	char *organisation = NULL, *title = NULL;
	size_t i;

	ARGBEGIN {
	case 'o':
		if (organisation)
			usage();
		organisation = ARG();
		break;
	case 't':
		if (title)
			usage();
		title = ARG();
		break;
	case 'O':
		display_organisation = 1;
		break;
	case 'T':
		display_title = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc)
		usage();

	if (!organisation && !title && !display_organisation && !display_title)
		display_organisation = 1;

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((orgs = contacts[i]->organisations)) {
			for (; (org = *orgs); orgs++) {
				if (organisation && strcmpnul(org->organisation, organisation))
					continue;
				if (title && strcmpnul(org->title, title))
					continue;
				if (display_organisation && display_title) {
					printf("%s (%s: %s)\n", contacts[i]->id, org->organisation, org->title);
				} else if (display_organisation) {
					printf("%s (%s)\n", contacts[i]->id, org->organisation);
				} else if (display_title) {
					printf("%s (%s)\n", contacts[i]->id, org->title);
				} else {
					printf("%s\n", contacts[i]->id);
					break;
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
