/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a | -c] contact-id context address | -u contact-id context [address] | -U contact-id [context] address");


int
main(int argc, char *argv[])
{
	int update_address = 0, update_context = 0;
	int remove_by_context = 0, remove_by_address = 0;
	int edit;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_site **r, **w;
	size_t i;

	ARGBEGIN {
	case 'a':
		update_address = 1;
		break;
	case 'c':
		update_context = 1;
		break;
	case 'u':
		remove_by_context = 1;
		break;
	case 'U':
		remove_by_address = 1;
		break;
	default:
		usage();
	} ARGEND;

	edit = update_address + update_context + remove_by_context + remove_by_address;
	if (edit > 1 || argc < 3 - remove_by_context - remove_by_address || argc > 3)
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
	if (contact.sites) {
		if (!edit) {
			for (; contact.sites[i]; i++);
		} else if (update_address) {
			for (; contact.sites[i]; i++) {
				if (contact.sites[i]->context && !strcmp(contact.sites[i]->context, argv[1])) {
					free(contact.sites[i]->address);
					contact.sites[i]->address = estrdup(argv[2]);
					goto save;
				}
			}
		} else if (update_context) {
			for (; contact.sites[i]; i++) {
				if (contact.sites[i]->address && !strcmp(contact.sites[i]->address, argv[2])) {
					free(contact.sites[i]->context);
					contact.sites[i]->context = estrdup(argv[1]);
					goto save;
				}
			}
		} else if (argc == 3) {
			for (; contact.sites[i]; i++)
				if (contact.sites[i]->context && !strcmp(contact.sites[i]->context, argv[1]))
					if (contact.sites[i]->address && !strcmp(contact.sites[i]->address, argv[2]))
						break;
		} else if (remove_by_context) {
			for (; contact.sites[i]; i++)
				if (contact.sites[i]->context && !strcmp(contact.sites[i]->context, argv[1]))
					break;
		} else {
			for (; contact.sites[i]; i++)
				if (contact.sites[i]->address && !strcmp(contact.sites[i]->address, argv[1]))
					break;
		}
	}
	if (!edit || update_address || update_context) {
		contact.sites = erealloc(contact.sites, (i + 2) * sizeof(*contact.sites));
		contact.sites[i + 1] = NULL;
		contact.sites[i] = ecalloc(1, sizeof(**contact.sites));
		contact.sites[i]->context = estrdup(argv[1]);
		contact.sites[i]->address = estrdup(argv[2]);
	} else if (contact.sites && contact.sites[i]) {
		libcontacts_site_destroy(contact.sites[i]);
		free(contact.sites[i]);
		for (r = &1[w = &contact.sites[i]]; *r;)
			*w++ = *r++;
		*w = NULL;
	}

save:
	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
