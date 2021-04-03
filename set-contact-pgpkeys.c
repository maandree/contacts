/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c | -f] contact-id context fingerprint | -u contact-id context [fingerprint] | -U contact-id [context] fingerprint");


int
main(int argc, char *argv[])
{
	int update_id = 0, update_context = 0;
	int remove_by_context = 0, remove_by_id = 0;
	int edit;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_pgpkey **r, **w;
	size_t i;

	ARGBEGIN {
	case 'c':
		update_context = 1;
		break;
	case 'f':
		update_id = 1;
		break;
	case 'u':
		remove_by_context = 1;
		break;
	case 'U':
		remove_by_id = 1;
		break;
	default:
		usage();
	} ARGEND;

	edit = update_id + update_context + remove_by_context + remove_by_id;
	if (edit > 1 || argc < 3 - remove_by_context - remove_by_id || argc > 3)
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
	if (contact.pgpkeys) {
		if (!edit) {
			for (; contact.pgpkeys[i]; i++);
		} else if (update_id) {
			for (; contact.pgpkeys[i]; i++) {
				if (contact.pgpkeys[i]->context && !strcmp(contact.pgpkeys[i]->context, argv[1])) {
					free(contact.pgpkeys[i]->id);
					contact.pgpkeys[i]->id = estrdup(argv[2]);
					goto save;
				}
			}
		} else if (update_context) {
			for (; contact.pgpkeys[i]; i++) {
				if (contact.pgpkeys[i]->id && !strcmp(contact.pgpkeys[i]->id, argv[2])) {
					free(contact.pgpkeys[i]->context);
					contact.pgpkeys[i]->context = estrdup(argv[1]);
					goto save;
				}
			}
		} else if (argc == 3) {
			for (; contact.pgpkeys[i]; i++)
				if (contact.pgpkeys[i]->context && !strcmp(contact.pgpkeys[i]->context, argv[1]))
					if (contact.pgpkeys[i]->id && !strcmp(contact.pgpkeys[i]->id, argv[2]))
						break;
		} else if (remove_by_context) {
			for (; contact.pgpkeys[i]; i++)
				if (contact.pgpkeys[i]->context && !strcmp(contact.pgpkeys[i]->context, argv[1]))
					break;
		} else {
			for (; contact.pgpkeys[i]; i++)
				if (contact.pgpkeys[i]->id && !strcmp(contact.pgpkeys[i]->id, argv[1]))
					break;
		}
	}
	if (!edit || update_id || update_context) {
		contact.pgpkeys = erealloc(contact.pgpkeys, (i + 2) * sizeof(*contact.pgpkeys));
		contact.pgpkeys[i + 1] = NULL;
		contact.pgpkeys[i] = ecalloc(1, sizeof(**contact.pgpkeys));
		contact.pgpkeys[i]->context = estrdup(argv[1]);
		contact.pgpkeys[i]->id = estrdup(argv[2]);
	} else if (contact.pgpkeys && contact.pgpkeys[i]) {
		libcontacts_pgpkey_destroy(contact.pgpkeys[i]);
		free(contact.pgpkeys[i]);
		for (r = &1[w = &contact.pgpkeys[i]]; *r;)
			*w++ = *r++;
		*w = NULL;
	}

save:
	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
