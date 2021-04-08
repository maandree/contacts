/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c context] [-a] service");


int
main(int argc, char *argv[])
{
	int display_address = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_chat **chats, *chat;
	char *context = NULL;
	size_t i;

	ARGBEGIN {
	case 'a':
		display_address = 1;
		break;
	case 'c':
		if (context)
			usage();
		context = ARG();
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1)
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user, 0))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((chats = contacts[i]->chats)) {
			for (; (chat = *chats); chats++) {
				if (display_address && !chat->address)
					continue;
				if (strcmpnul(chat->service, argv[0]))
					continue;
				if (context && strcmpnul(chat->context, context))
					continue;
				if (context || !chat->context) {
					if (display_address)
						printf("%s (%s)\n", contacts[i]->id, chat->address);
					else
						printf("%s\n", contacts[i]->id);
				} else if (chat->context) {
					if (display_address)
						printf("%s (%s: %s)\n", contacts[i]->id, chat->context, chat->address);
					else
						printf("%s (%s)\n", contacts[i]->id, chat->context);
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
