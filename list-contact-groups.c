/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("-L | contact-id ...");


struct group_node {
	char *name;
	struct group_node *next;
	struct group_node *prev;
};

static struct group_node head;
static struct group_node tail;

static void
list_groups(const struct libcontacts_contact *contact)
{
	char **groups, *group;
	struct group_node *node, *new;
	int cmp;

	if ((groups = contact->groups)) {
		for (; (group = *groups); groups++) {
			for (node = head.next; node->next; node = node->next) {
				cmp = strcmp(group, node->name);
				if (cmp < 0)
					continue;
				if (cmp == 0)
					goto next_group;
				break;
			}
			printf("%s\n", group);
			new = ecalloc(1, sizeof(*new));
			(new->prev = node->prev)->next = new;
			(new->next = node)->prev = new;
			new->name = estrdup(group);
		next_group:;
		}
	}
}

int
main(int argc, char *argv[])
{
	int list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts, contact;
	struct group_node *node, *next_node;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'L':
		list = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (list ? argc : !argc)
		usage();

	head.prev = NULL;
	head.next = &tail;
	tail.prev = &head;
	tail.next = NULL;

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (list) {
		if (libcontacts_load_contacts(&contacts, user, 0))
			eprintf("libcontacts_load_contacts:");
		for (i = 0; contacts[i]; i++) {
			list_groups(contacts[i]);
			libcontacts_contact_destroy(contacts[i]);
			free(contacts[i]);
		}
		free(contacts);
	} else {
		for (; *argv; argv++) {
			if (libcontacts_load_contact(*argv, &contact, user)) {
				weprintf("libcontacts_load_contact %s: %s\n", *argv,
				         errno ? strerror(errno) : "contact file is malformatted");
				ret = 1;
			} else {
				list_groups(&contact);
				libcontacts_contact_destroy(&contact);
                        }
                }
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	for (node = head.next; node->next; node = next_node) {
		next_node = node->next;
		free(node->name);
		free(node);
	}

	return ret;
}
