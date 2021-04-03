/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-u] contact-id ... group");


int
main(int argc, char *argv[])
{
	int remove = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	char *group, **r, **w;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc < 2)
		usage();

	group = argv[--argc];
	argv[argc] = NULL;

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
		if (contact.groups) {
			for (i = 0; contact.groups[i]; i++)
				if (!strcmp(contact.groups[i], group))
					break;
			r = &contact.groups[i];
			if (remove && *r) {
				free(*r);
				for (w = r++; *r;)
					*w++ = *r++;
				*w = NULL;
				if (libcontacts_save_contact(&contact, user)) {
					weprintf("libcontacts_save_contact %s:", *argv);
					ret = 1;
				}
			} else if (!remove && !*r) {
				goto add_group;
			}
		} else if (!remove) {
			i = 0;
		add_group:
			contact.groups = erealloc(contact.groups, (i + 2) * sizeof(*contact.groups));
			contact.groups[i + 1] = NULL;
			contact.groups[i] = group;
			if (libcontacts_save_contact(&contact, user)) {
				weprintf("libcontacts_save_contact %s:", *argv);
				ret = 1;
			}
			contact.groups[i] = NULL;
		}
		libcontacts_contact_destroy(&contact);
	}

	return ret;
}
