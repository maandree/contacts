/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("contact-id");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	struct libcontacts_contact contact;

	NOFLAGS(argc != 1);

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	if (contact.notes && *contact.notes) {
		printf("%s", contact.notes);
		if (fflush(stdout) || ferror(stdout) || fclose(stdout))
			eprintf("printf:");
	}

	libcontacts_contact_destroy(&contact);

	return 0;
}
