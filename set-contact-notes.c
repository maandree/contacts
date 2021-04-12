/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a | -u] contact-id");


int
main(int argc, char *argv[])
{
	int append = 0, unset = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t size = 0, len = 0;
	ssize_t r;

	ARGBEGIN {
	case 'a':
		append = 1;
		break;
	case 'u':
		unset = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1 || (append && unset))
		usage();

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	if (contact.notes) {
		if (append) {
			len = strlen(contact.notes);
			size = len + 1;
		} else {
			free(contact.notes);
			contact.notes = NULL;
		}
	}
	if (!unset) {
		for (;;) {
			if (len == size)
				contact.notes = erealloc(contact.notes, size += 512);
			r = read(STDIN_FILENO, &contact.notes[len], size - len);
			if (r <= 0) {
				if (!r)
					break;
				eprintf("read <stdin>:");
			}
			len += (size_t)r;
		}
		if (size) {
			if (len == size)
				contact.notes = erealloc(contact.notes, size += 1);
			contact.notes[len] = '\0';
		}
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);

	libcontacts_contact_destroy(&contact);

	return 0;
}
