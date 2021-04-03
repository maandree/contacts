/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-F | -f | -l | -n] (-L | name)");


int
main(int argc, char *argv[])
{
	int list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	char *name;
	size_t i, offset = 0;

	ARGBEGIN {
	case 'F':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, full_name);
		break;
	case 'f':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, first_name);
		break;
	case 'l':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, last_name);
		break;
	case 'n':
		if (offset)
			usage();
		offset = offsetof(struct libcontacts_contact, nickname);
		break;
	case 'L':
		list = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1 - list)
		usage();

	if (!offset)
		offset = offsetof(struct libcontacts_contact, name);

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		name = *(char **)&offset[(char *)contacts[i]];
		if (list) {
			if (name)
				printf("%s (%s)\n", contacts[i]->id, name);
		} else if (!strcmpnul(name, argv[0])) {
			printf("%s\n", contacts[i]->id);
		}
		libcontacts_contact_destroy(contacts[i]);
		free(contacts[i]);
	}
	free(contacts);

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return 0;
}
