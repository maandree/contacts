/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a ask-at] [-s service] [-t type] [-u unblock-at] [-y style] [-ASTUY] contact-id ...");


int
main(int argc, char *argv[])
{
	int display_srv = 0, display_type = 0, display_style = 0;
	int display_ask = 0, display_ublk = 0, explicit = 0;
	enum libcontacts_block_type style = 0;
	time_t soft_unblock = 0, hard_unblock = 0;
	const char *lookup_srv = NULL, *lookup_type = NULL, *lookup_style = NULL;
	const char *lookup_ask = NULL, *lookup_ublk = NULL;
	char *p;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_block **blocks, *block;
	int ret = 0;
	size_t i;

	errno = 0;

	ARGBEGIN {
	case 's':
		if (lookup_srv)
			usage();
		lookup_srv = ARG();
		break;
	case 't':
		if (lookup_type)
			usage();
		lookup_type = ARG();
		break;
	case 'y':
		if (lookup_style)
			usage();
		lookup_style = ARG();
		break;
	case 'a':
		if (lookup_ask)
			usage();
		lookup_ask = ARG();
		if (!isdigit(*lookup_ask))
			usage();
		soft_unblock = (time_t)strtoumax(lookup_ask, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'u':
		if (lookup_ublk)
			usage();
		lookup_ublk = ARG();
		if (!isdigit(*lookup_ublk))
			usage();
		hard_unblock = (time_t)strtoumax(lookup_ublk, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'S':
		display_srv = 1;
		break;
	case 'T':
		display_type = 1;
		break;
	case 'Y':
		display_style = 1;
		break;
	case 'A':
		display_ask = 1;
		break;
	case 'U':
		display_ublk = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	if (lookup_type) {
		if (!strcmp(lookup_type, "explicit"))
			explicit = 1;
		else if (!strcmp(lookup_type, "shadow"))
			explicit = 0;
		else
			eprintf("value of -t shall be either \"explicit\" or \"shadow\"\n");
	}

	if (lookup_style) {
		if (!strcmp(lookup_style, "silent"))
			style = LIBCONTACTS_SILENT;
		else if (!strcmp(lookup_style, "as-off"))
			style = LIBCONTACTS_BLOCK_OFF;
		else if (!strcmp(lookup_style, "as-busy"))
			style = LIBCONTACTS_BLOCK_BUSY;
		else if (!strcmp(lookup_style, "ignore"))
			style = LIBCONTACTS_BLOCK_IGNORE;
		else
			eprintf("value of -y shall be either \"silent\", \"as-off\", \"as-busy\", or \"ignore\"\n");
	}

	if (!display_srv && !display_type && !display_style && !display_ask && !display_ublk) {
		display_srv   = !lookup_srv;
		display_type  = !lookup_type;
		display_style = !lookup_style;
		display_ask   = !lookup_ask;
		display_ublk  = !lookup_ublk;
		if (!display_srv && !display_type && !display_style && !display_ask && !display_ublk)
			display_srv = display_type = display_style = display_ask = display_ublk = 1;
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
		if ((blocks = contact.blocks)) {
			for (; (block = *blocks); blocks++) {
				if (lookup_srv && strcmpnul(block->service, lookup_srv))
					continue;
				if (lookup_type && block->explicit != explicit)
					continue;
				if (lookup_style && block->shadow_block != style)
					continue;
				if (lookup_ask && block->soft_unblock != soft_unblock)
					continue;
				if (lookup_ublk && block->hard_unblock != hard_unblock)
					continue;
				if (argc > 1)
					printf("%s: ", *argv);
				if (display_srv) {
					printf("%s%s", block->service, display_type + display_style +
					               display_ask + display_ublk ? "; " : "\n");
				}
				if (display_type) {
					printf("%s%s", block->explicit ? "explicit" : "shadow",
					               display_style + display_ask + display_ublk ? "; " : "\n");
				}
				if (display_style) {
					if (block->shadow_block == LIBCONTACTS_SILENT)
						printf("silent");
					else if (block->shadow_block == LIBCONTACTS_BLOCK_OFF)
						printf("as-off");
					else if (block->shadow_block == LIBCONTACTS_BLOCK_BUSY)
						printf("as-busy");
					else if (block->shadow_block == LIBCONTACTS_BLOCK_IGNORE)
						printf("ignore");
					else
						printf("???");
					printf("%s", display_ask + display_ublk ? "; " : "\n");
				}
				if (display_ask)
					printf("%ju%s", (uintmax_t)block->soft_unblock, display_ublk ? "; " : "\n");
				if (display_ublk)
					printf("%ju\n", (uintmax_t)block->hard_unblock);
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
