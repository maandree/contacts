/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a ask-at] [-s service] [-t type] [-u unblock-at] [-y style] contact-id ...");


int
main(int argc, char *argv[])
{
	int explicit = 0;
	enum libcontacts_block_type shadow_block = 0;
	time_t soft_unblock = 0, hard_unblock = 0;
	const char *srv = NULL, *type = NULL, *style = NULL, *ask = NULL, *ublk = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_block **r, **w;
	char *p;
	size_t i;
	int ret = 0;

	ARGBEGIN {
	case 'a':
		if (ask)
			usage();
		ask = ARG();
		if (!isdigit(*ask))
			usage();
		soft_unblock = (time_t)strtoumax(ask, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 's':
		if (srv)
			usage();
		srv = ARG();
		break;
	case 't':
		if (type)
			usage();
		type = ARG();
		break;
	case 'u':
		if (ublk)
			usage();
		ublk = ARG();
		if (!isdigit(*ublk))
			usage();
		hard_unblock = (time_t)strtoumax(ublk, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'y':
		if (style)
			usage();
		style = ARG();
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	if (type) {
		if (!strcmp(type, "explicit"))
			explicit = 1;
		else if (!strcmp(type, "shadow"))
			explicit = 0;
		else
			eprintf("value of -t shall be either \"explicit\" or \"shadow\"\n");
	}

	if (style) {
		if (!strcmp(style, "silent"))
			shadow_block = LIBCONTACTS_SILENT;
		else if (!strcmp(style, "as-off"))
			shadow_block = LIBCONTACTS_BLOCK_OFF;
		else if (!strcmp(style, "as-busy"))
			shadow_block = LIBCONTACTS_BLOCK_BUSY;
		else if (!strcmp(style, "ignore"))
			shadow_block = LIBCONTACTS_BLOCK_IGNORE;
		else
			eprintf("value of -y shall be either \"silent\", \"as-off\", \"as-busy\", or \"ignore\"\n");
	}

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
		if ((r = contact.blocks)) {
			for (w = r; *r; w++) {
				*w = *r++;
				if (srv && strcmpnul((*w)->service, srv))
					continue;
				if (type && (*w)->explicit != explicit)
					continue;
				if (style && (*w)->shadow_block != shadow_block)
					continue;
				if (ask && (*w)->soft_unblock != soft_unblock)
					continue;
				if (ublk && (*w)->hard_unblock != hard_unblock)
					continue;
				libcontacts_block_destroy(*w);
				free(*w--);
			}
			*w = NULL;
			if (libcontacts_save_contact(&contact, user))
				eprintf("libcontacts_save_contact %s:", *argv);
		}
		libcontacts_contact_destroy(&contact);
	}

	return ret;
}
