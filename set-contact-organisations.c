/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-o | -t] contact-id organisation title | -u contact-id organisation [title] | -U contact-id [organisation] title");


int
main(int argc, char *argv[])
{
	int update_title = 0, update_organisation = 0;
	int remove_by_organisation = 0, remove_by_title = 0;
	int edit;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_organisation **r, **w;
	size_t i;

	ARGBEGIN {
	case 'o':
		update_title = 1;
		break;
	case 't':
		update_organisation = 1;
		break;
	case 'u':
		remove_by_organisation = 1;
		break;
	case 'U':
		remove_by_title = 1;
		break;
	default:
		usage();
	} ARGEND;

	edit = update_title + update_organisation + remove_by_organisation + remove_by_title;
	if (edit > 1 || argc < 3 - remove_by_organisation - remove_by_title || argc > 3)
		usage();

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	i = 0;
	if (contact.organisations) {
		if (!edit) {
			for (; contact.organisations[i]; i++);
		} else if (update_title) {
			for (; contact.organisations[i]; i++) {
				if (!strcmpnul(contact.organisations[i]->organisation, argv[1])) {
					free(contact.organisations[i]->title);
					contact.organisations[i]->title = estrdup(argv[2]);
					goto save;
				}
			}
		} else if (update_organisation) {
			for (; contact.organisations[i]; i++) {
				if (!strcmpnul(contact.organisations[i]->title, argv[2])) {
					free(contact.organisations[i]->organisation);
					contact.organisations[i]->organisation = estrdup(argv[1]);
					goto save;
				}
			}
		} else if (argc == 3) {
			for (; contact.organisations[i]; i++)
				if (!strcmpnul(contact.organisations[i]->organisation, argv[1]))
					if (!strcmpnul(contact.organisations[i]->title, argv[2]))
						break;
		} else if (remove_by_organisation) {
			for (; contact.organisations[i]; i++)
				if (!strcmpnul(contact.organisations[i]->organisation, argv[1]))
					break;
		} else {
			for (; contact.organisations[i]; i++)
				if (!strcmpnul(contact.organisations[i]->title, argv[1]))
					break;
		}
	}
	if (!edit || update_title || update_organisation) {
		contact.organisations = erealloc(contact.organisations, (i + 2) * sizeof(*contact.organisations));
		contact.organisations[i + 1] = NULL;
		contact.organisations[i] = ecalloc(1, sizeof(**contact.organisations));
		contact.organisations[i]->organisation = estrdup(argv[1]);
		contact.organisations[i]->title = estrdup(argv[2]);
	} else if (contact.organisations && contact.organisations[i]) {
		libcontacts_organisation_destroy(contact.organisations[i]);
		free(contact.organisations[i]);
		for (r = &1[w = &contact.organisations[i]]; *r;)
			*w++ = *r++;
		*w = NULL;
	}

save:
	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
