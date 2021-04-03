/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c context] [-f fingerprint] [-CF] contact-id ...");


int
main(int argc, char *argv[])
{
	int display_ctx = 0, display_id = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_pgpkey **keys, *key;
	const char *lookup_ctx = NULL, *lookup_id = NULL;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'c':
		if (lookup_ctx)
			usage();
		lookup_ctx = ARG();
		break;
	case 'f':
		if (lookup_id)
			usage();
		lookup_id = ARG();
		break;
	case 'C':
		display_ctx = 1;
		break;
	case 'F':
		display_id = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	if (lookup_ctx && !lookup_id && !display_ctx && !display_id)
		display_id = 1;
	if (lookup_id && !lookup_ctx && !display_ctx && !display_id)
		display_ctx = 1;

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			weprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
			continue;
		}
		if ((keys = contact.pgpkeys)) {
			for (; (key = *keys); keys++) {
				if (lookup_ctx && strcmpnul(key->context, lookup_ctx))
					continue;
				if (lookup_id && strcmpnul(key->id, lookup_id))
					continue;
				if (lookup_ctx && lookup_id && !display_ctx && !display_id) {
					printf("%s\n", *argv);
					continue;
				}
				if (argc > 1)
					printf("%s: ", *argv);
				if (display_ctx && display_id)
					printf("%s: %s\n", key->context, key->id);
				else if (display_id)
					printf("%s\n", key->id);
				else
					printf("%s\n", key->context);
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
