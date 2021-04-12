/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a] contact-id ...");


int
main(int argc, char *argv[])
{
	int absolute_path = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	const char *slash = "";
	char **photos, **r, **w;
	size_t i;

	ARGBEGIN {
	case 'a':
		absolute_path = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(*argv, &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");
	photos = contact.photos;
	contact.photos = NULL;
	libcontacts_contact_destroy(&contact);
	if (!photos || !*photos) {
		free(photos);
		return 0;
	}

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			eprintf("libcontacts_load_contact %s: %s\n", *argv,
			        errno ? strerror(errno) : "contact file is malformatted");
		}
		if (!contact.photos) {
			libcontacts_contact_destroy(&contact);
			for (i = 0; photos[i]; i++)
				free(photos[i]);
			free(photos);
			return 0;
		}
		for (w = r = photos; *r; r++) {
			for (i = 0; contact.photos[i]; i++)
				if (!strcmp(contact.photos[i], *r))
					break;
			if (!contact.photos[i])
				free(*r);
			else
				*w++ = *r;
		}
		*w = NULL;
		libcontacts_contact_destroy(&contact);
		if (!*photos) {
			free(photos);
			return 0;
		}
	}

	if (absolute_path && strchr(user->pw_dir, '\0')[-1] != '/')
		slash = "/";
	for (i = 0; photos[i]; i++) {
		if (photos[i][0] != '/' && absolute_path)
			printf("%s%s%s\n", user->pw_dir, slash, photos[i]);
		else if (photos[i][0])
			printf("%s\n", photos[i]);
		free(photos[i]);
	}
	free(photos);

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return 0;
}
