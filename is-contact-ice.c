/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("contact-id ...");


int
main(int argc, char *argv[])
{
	struct passwd *user;
	struct libcontacts_contact contact;
	int ret = 0;
	size_t i;

	NOFLAGS(!argc);

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
		} else {
			if (argc > 1)
				printf("%s: ", *argv);
			printf("%s\n", contact.in_case_of_emergency ? "yes" : "no");
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
