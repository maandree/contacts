/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-O old-organisation] [-T old-title] ([-o new-adress] [-t new-title] | -u) contact-id");


int
main(int argc, char *argv[])
{
	int add = 1, edit = 0, remove = 0;
	char *organisation = NULL, *title = NULL;
	char *lookup_organisation = NULL, *lookup_title = NULL;
	char *old_organisation = NULL, *old_title = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_organisation **r, **w;
	size_t i;

	ARGBEGIN {
	case 'O':
		add = 0;
		if (lookup_organisation)
			usage();
		lookup_organisation = ARG();
		break;
	case 'o':
		edit = 1;
		if (organisation)
			usage();
		organisation = ARG();
		break;
	case 'T':
		add = 0;
		if (lookup_title)
			usage();
		lookup_title = ARG();
		break;
	case 't':
		edit = 1;
		if (title)
			usage();
		title = ARG();
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (remove == edit) {
		if (edit)
			usage();
		eprintf("at least one of -otu is required\n");
	}

	if (add)
		edit = 0;

	if (argc != 1 || !*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	i = 0;
	if ((edit || remove) && contact.organisations) {
		for (r = contact.organisations; *r; r++) {
			if (lookup_title && strcmpnul((*r)->title, lookup_title))
				continue;
			if (lookup_organisation && strcmpnul((*r)->title, lookup_organisation))
				continue;
			break;
		}
		if (!edit) {
			libcontacts_organisation_destroy(*r);
			free(*r);
			for (w = r++; (*w++ = *r++););
		} else if (*r) {
			if (title) {
				old_title = contact.organisations[i]->title;
				contact.organisations[i]->title = title;
			}
			if (organisation) {
				old_organisation = contact.organisations[i]->organisation;
				contact.organisations[i]->organisation = organisation;
			}
		} else {
			libcontacts_contact_destroy(&contact);
			return 0;
		}
	} else if (!edit && !remove) {
		if (contact.organisations)
			for (i = 0; contact.organisations[i]; i++);
		contact.organisations = erealloc(contact.organisations, (i + 2) * sizeof(*contact.organisations));
		contact.organisations[i + 1] = NULL;
		contact.organisations[i] = ecalloc(1, sizeof(**contact.organisations));
		contact.organisations[i]->title = title;
		contact.organisations[i]->organisation = organisation;
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);

	contact.organisations[i]->title = old_title;
	contact.organisations[i]->organisation = old_organisation;
	libcontacts_contact_destroy(&contact);

	return 0;
}
