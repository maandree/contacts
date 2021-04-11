/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-u] contact-id"); /* TODO add -a (append) */


int
main(int argc, char *argv[])
{
	int unset = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t size = 0, len = 0;
	ssize_t r;

	ARGBEGIN {
	case 'u':
		unset = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1)
		usage();

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	free(contact.notes);
	contact.notes = NULL;
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
		if (size)
			contact.notes[len] = '\0';
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);

	libcontacts_contact_destroy(&contact);

	return 0;
}
