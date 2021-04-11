/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-C old-context] [-F old-fingerprint] ([-c new-context] [-f new-fingerprint] | -u) contact-id");


int
main(int argc, char *argv[])
{
	int add = 1, edit = 0, remove = 0;
	char *id = NULL, *context = NULL;
	char *lookup_id = NULL, *lookup_context = NULL;
	char *old_id = NULL, *old_context = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_pgpkey **r, **w;
	size_t i;

	ARGBEGIN {
	case 'C':
		add = 0;
		if (lookup_context)
			usage();
		lookup_context = ARG();
		break;
	case 'c':
		edit = 0;
		if (context)
			usage();
		context = ARG();
		break;
	case 'F':
		add = 1;
		if (lookup_id)
			usage();
		lookup_id = ARG();
		break;
	case 'f':
		edit = 1;
		if (id)
			usage();
		id = ARG();
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
		eprintf("at least one of -cfu is required\n");
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
	if ((edit || remove) && contact.pgpkeys) {
		for (r = contact.pgpkeys; *r; r++) {
			if (lookup_context && strcmpnul((*r)->context, lookup_context))
				continue;
			if (lookup_id && strcmpnul((*r)->context, lookup_id))
				continue;
			break;
		}
		if (!edit) {
			libcontacts_pgpkey_destroy(*r);
			free(*r);
			for (w = r++; (*w++ = *r++););
		} else if (*r) {
			if (context) {
				old_context = contact.pgpkeys[i]->context;
				contact.pgpkeys[i]->context = context;
			}
			if (id) {
				old_id = contact.pgpkeys[i]->id;
				contact.pgpkeys[i]->id = id;
			}
		} else {
			libcontacts_contact_destroy(&contact);
			return 0;
		}
	} else if (!edit && !remove) {
		if (contact.pgpkeys)
			for (i = 0; contact.pgpkeys[i]; i++);
		contact.pgpkeys = erealloc(contact.pgpkeys, (i + 2) * sizeof(*contact.pgpkeys));
		contact.pgpkeys[i + 1] = NULL;
		contact.pgpkeys[i] = ecalloc(1, sizeof(**contact.pgpkeys));
		contact.pgpkeys[i]->context = context;
		contact.pgpkeys[i]->id = id;
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);

	contact.pgpkeys[i]->context = old_context;
	contact.pgpkeys[i]->id = old_id;
	libcontacts_contact_destroy(&contact);

	return 0;
}
