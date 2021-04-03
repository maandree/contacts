/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("group ...");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	struct libcontacts_contact **contacts;
	char **groups;
	int ret = 0;
	size_t i, j;

	NOFLAGS(!argc);

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((groups = contacts[i]->groups)) {
			for (; *groups; groups++) {
				for (j = 0; argv[j]; j++) {
					if (!strcmp(*groups, argv[j])) {
						printf("%s\n", contacts[i]->id);
						goto done;
					}
				}
			}
		}
	done:
		libcontacts_contact_destroy(contacts[i]);
		free(contacts[i]);
	}
	free(contacts);

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return ret;
}
