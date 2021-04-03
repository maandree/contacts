/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a | -c | -s | -u] contact-id context service address");


int
main(int argc, char *argv[])
{
	int update_address = 0, update_context = 0, update_service = 0;
	int remove = 0, edit;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_chat **r, **w;
	size_t i;

	ARGBEGIN {
	case 'a':
		update_address = 1;
		break;
	case 'c':
		update_context = 1;
		break;
	case 's':
		update_service = 1;
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	edit = update_address + update_context + update_service + remove;
	if (edit > 1 || argc != 4)
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
	if (contact.chats) {
		if (!edit) {
			for (; contact.chats[i]; i++);
		} else if (update_address) {
			for (; contact.chats[i]; i++) {
				if (!strcmpnul(contact.chats[i]->context, argv[1])) {
					if (!strcmpnul(contact.chats[i]->service, argv[2])) {
						free(contact.chats[i]->address);
						contact.chats[i]->address = estrdup(argv[3]);
						goto save;
					}
				}
			}
		} else if (update_context) {
			for (; contact.chats[i]; i++) {
				if (!strcmpnul(contact.chats[i]->service, argv[2])) {
					if (!strcmpnul(contact.chats[i]->address, argv[3])) {
						free(contact.chats[i]->context);
						contact.chats[i]->context = estrdup(argv[1]);
						goto save;
					}
				}
			}
		} else if (update_service) {
			for (; contact.chats[i]; i++) {
				if (!strcmpnul(contact.chats[i]->context, argv[1])) {
					if (!strcmpnul(contact.chats[i]->address, argv[3])) {
						free(contact.chats[i]->context);
						contact.chats[i]->service = estrdup(argv[2]);
						goto save;
					}
				}
			}
		} else {
			for (; contact.chats[i]; i++)
				if (!strcmpnul(contact.chats[i]->context, argv[1]))
					if (!strcmpnul(contact.chats[i]->service, argv[2]))
						if (!strcmpnul(contact.chats[i]->address, argv[3]))
							break;
		}
	}
	if (!edit || update_address || update_context || update_service) {
		contact.chats = erealloc(contact.chats, (i + 2) * sizeof(*contact.chats));
		contact.chats[i + 1] = NULL;
		contact.chats[i] = ecalloc(1, sizeof(**contact.chats));
		contact.chats[i]->context = estrdup(argv[1]);
		contact.chats[i]->service = estrdup(argv[2]);
		contact.chats[i]->address = estrdup(argv[3]);
	} else if (contact.chats && contact.chats[i]) {
		libcontacts_chat_destroy(contact.chats[i]);
		free(contact.chats[i]);
		for (r = &1[w = &contact.chats[i]]; *r;)
			*w++ = *r++;
		*w = NULL;
	}

save:
	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
