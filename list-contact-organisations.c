/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-t title] (-L | contact-id ...)");


struct org_node {
	char *name;
	struct org_node *next;
	struct org_node *prev;
};

static struct org_node head;
static struct org_node tail;

static void
list_orgs(const struct libcontacts_contact *contact, const char *title)
{
	struct libcontacts_organisation **orgs, *org;
	struct org_node *node, *new;
	int cmp;

	if ((orgs = contact->organisations)) {
		for (; (org = *orgs); orgs++) {
			if (!org->organisation)
				continue;
			if (title && strcmpnul(org->title, title))
				continue;
			for (node = head.next; node->next; node = node->next) {
				cmp = strcmp(org->organisation, node->name);
				if (cmp < 0)
					continue;
				if (cmp == 0)
					goto next_org;
				break;
			}
			printf("%s\n", org->organisation);
			new = ecalloc(1, sizeof(*new));
			(new->prev = node->prev)->next = new;
			(new->next = node)->prev = new;
			new->name = org->organisation;
			org->organisation = NULL;
		next_org:;
		}
	}
}

int
main(int argc, char *argv[])
{
	int list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts, contact;
	struct org_node *node, *next_node;
	int ret = 0;
	char *title = NULL;
	size_t i;

	ARGBEGIN {
	case 't':
		if (title)
			usage();
		title = ARG();
		break;
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
		if (libcontacts_load_contacts(&contacts, user))
			eprintf("libcontacts_load_contacts:");
		for (i = 0; contacts[i]; i++) {
			list_orgs(contacts[i], title);
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
				list_orgs(&contact, title);
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
