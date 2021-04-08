/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c context] (-L | fingerprint)");


int
main(int argc, char *argv[])
{
	int list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_pgpkey **pgpkeys, *pgpkey;
	char *context = NULL;
	size_t i;

	ARGBEGIN {
	case 'c':
		if (context)
			usage();
		context = ARG();
		break;
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

	if (libcontacts_load_contacts(&contacts, user, 1))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((pgpkeys = contacts[i]->pgpkeys)) {
			for (; (pgpkey = *pgpkeys); pgpkeys++) {
				if (!pgpkey->id)
					continue;
				if (context && strcmpnul(pgpkey->context, context))
					continue;
				if (list)
					printf("%s (%s)\n", contacts[i]->id, pgpkey->id);
				else if (!strcmp(pgpkey->id, argv[0]))
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
