/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c context] [-s service] (-L | address)");


int
main(int argc, char *argv[])
{
	int list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_chat **chats, *chat;
	char *context = NULL, *service;
	size_t i;

	ARGBEGIN {
	case 'c':
		if (context)
			usage();
		context = ARG();
		break;
	case 's':
		if (service)
			usage();
		service = ARG();
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

	if (libcontacts_load_contacts(&contacts, user))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((chats = contacts[i]->chats)) {
			for (; (chat = *chats); chats++) {
				if (!chat->address)
					continue;
				if (context && strcmpnul(chat->context, context))
					continue;
				if (service && strcmpnul(chat->service, service))
					continue;
				if (list) {
					if (chat->service && !service)
						printf("%s (%s: %s)\n", contacts[i]->id, chat->service, chat->address);
					else
						printf("%s (%s)\n", contacts[i]->id, chat->address);
				} else if (!strcmp(chat->address, argv[0])) {
					if (chat->service && !service)
						printf("%s (%s)\n", contacts[i]->id, chat->service);
					else
						printf("%s\n", contacts[i]->id);
				}
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
