/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-A old-address] [-C old-context] ([-a new-adress] [-c new-context] | -u) contact-id");


int
main(int argc, char *argv[])
{
	int add = 1, edit = 0, remove = 0;
	char *address = NULL, *context = NULL;
	char *lookup_address = NULL, *lookup_context = NULL;
	char *old_address = NULL, *old_context = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_site **r, **w;
	size_t i;

	ARGBEGIN {
	case 'A':
		add = 0;
		if (lookup_address)
			usage();
		lookup_address = ARG();
		break;
	case 'a':
		edit = 1;
		if (address)
			usage();
		address = ARG();
		break;
	case 'C':
		add = 0;
		if (lookup_context)
			usage();
		lookup_context = ARG();
		break;
	case 'c':
		edit = 1;
		if (context)
			usage();
		context = ARG();
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
		eprintf("at least one of -acu is required\n");
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
	if ((edit || remove) && contact.sites) {
		for (r = contact.sites; *r; r++) {
			if (lookup_context && strcmpnul((*r)->context, lookup_context))
				continue;
			if (lookup_address && strcmpnul((*r)->context, lookup_address))
				continue;
			break;
		}
		if (!edit) {
			libcontacts_site_destroy(*r);
			free(*r);
			for (w = r++; (*w++ = *r++););
		} else if (*r) {
			if (context) {
				old_context = contact.sites[i]->context;
				contact.sites[i]->context = context;
			}
			if (address) {
				old_address = contact.sites[i]->address;
				contact.sites[i]->address = address;
			}
		} else {
			libcontacts_contact_destroy(&contact);
			return 0;
		}
	} else if (!edit && !remove) {
		if (contact.sites)
			for (i = 0; contact.sites[i]; i++);
		contact.sites = erealloc(contact.sites, (i + 2) * sizeof(*contact.sites));
		contact.sites[i + 1] = NULL;
		contact.sites[i] = ecalloc(1, sizeof(**contact.sites));
		contact.sites[i]->context = context;
		contact.sites[i]->address = address;
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);

	contact.sites[i]->context = old_context;
	contact.sites[i]->address = old_address;
	libcontacts_contact_destroy(&contact);

	return 0;
}
