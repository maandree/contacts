/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a address] [-c context] [-s service] [-ACS] contact-id ...");


int
main(int argc, char *argv[])
{
	int display_ctx = 0, display_addr = 0, display_srv = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_chat **chats, *chat;
	const char *lookup_ctx = NULL, *lookup_addr = NULL, *lookup_srv = NULL;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'a':
		if (lookup_addr)
			usage();
		lookup_addr = ARG();
		break;
	case 'c':
		if (lookup_ctx)
			usage();
		lookup_ctx = ARG();
		break;
	case 's':
		if (lookup_srv)
			usage();
		lookup_srv = ARG();
		break;
	case 'A':
		display_addr = 1;
		break;
	case 'C':
		display_ctx = 1;
		break;
	case 'S':
		display_srv = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();

	if (!display_ctx && !display_addr && !display_srv) {
		display_ctx  = !lookup_ctx;
		display_addr = !lookup_addr;
		display_srv  = !lookup_srv;
	}

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
		if ((chats = contact.chats)) {
			for (; (chat = *chats); chats++) {
				if (lookup_ctx && strcmpnul(chat->context, lookup_ctx))
					continue;
				if (lookup_addr && strcmpnul(chat->address, lookup_addr))
					continue;
				if (lookup_srv && strcmpnul(chat->service, lookup_srv))
					continue;
				if (!display_ctx && !display_addr && !display_srv) {
					printf("%s\n", *argv);
					continue;
				}
				if (display_ctx && !chat->context)
					continue;
				if (display_srv && !chat->service)
					continue;
				if (display_addr && !chat->address)
					continue;
				if (argc > 1)
					printf("%s: ", *argv);
				if (display_ctx)
					printf("%s%s", chat->context, (display_srv || display_addr) ? ": " : "\n");
				if (display_srv)
					printf("%s%s", chat->service, display_addr ? ": " : "\n");
				if (display_addr)
					printf("%s\n", chat->address);
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
