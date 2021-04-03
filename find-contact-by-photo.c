/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("-L | photo");


int
main(int argc, char *argv[])
{
	int list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	char **photos, *photo;
	size_t i;

	ARGBEGIN {
	case 'L':
		list = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1 - list)
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((photos = contacts[i]->photos)) {
			for (; (photo = *photos); photos++) {
				if (list)
					printf("%s (%s)\n", contacts[i]->id, photo);
				else if (!strcmp(photo, argv[0]))
					printf("%s\n", contacts[i]->id);
			}
		}
		libcontacts_contact_destroy(contacts[i]);
		free(contacts[i]);
	}
	free(contacts);

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return 0;
}
