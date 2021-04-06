/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-efmnou] [(-b service | -B service) [-g]]");


int
main(int argc, char *argv[])
{
	int names = 0, emergency = 0, check_global_blocks = 0;
	int include_men = 0, include_women = 0, include_orgs = 0, include_unspec = 0;
	const char *blocked_on = NULL, *unblocked_on = NULL, *service = NULL;
	struct passwd *user;
	struct libcontacts_contact **contacts, *contact;
	char **ids;
	size_t i, j;

	ARGBEGIN {
	case 'e':
		emergency = 1;
		break;
	case 'f':
		include_women = 1;
		break;
	case 'm':
		include_men = 1;
		break;
	case 'n':
		names = 1;
		break;
	case 'o':
		include_orgs = 1;
		break;
	case 'u':
		include_unspec = 1;
		break;
	case 'g':
		check_global_blocks = 1;
		break;
	case 'b':
		if (service)
			usage();
		service = blocked_on = ARG();
		break;
	case 'B':
		if (service)
			usage();
		service = unblocked_on = ARG();
		break;
	default:
		usage();
	} ARGEND;

	if (argc || (check_global_blocks && !service))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (!names && !emergency && !include_men && !include_women && !include_orgs && !include_unspec && !service) {
		if (libcontacts_list_contacts(&ids, user))
			eprintf("libcontacts_list_contacts:");
		for (i = 0; ids[i]; i++) {
			printf("%s\n", ids[i]);
			free(ids[i]);
		}
		free(ids);
	} else {
		if (!include_men && !include_women && !include_orgs && !include_unspec)
			include_men = include_women = include_orgs = include_unspec = 1;
		if (libcontacts_load_contacts(&contacts, user))
			eprintf("libcontacts_load_contacts:");
		for (i = 0; (contact = contacts[i]); i++) {
			if (emergency && !contact->in_case_of_emergency)
				goto next;
			if      (include_men    && contact->gender == LIBCONTACTS_MALE);
			else if (include_women  && contact->gender == LIBCONTACTS_FEMALE);
			else if (include_orgs   && contact->gender == LIBCONTACTS_NOT_A_PERSON);
			else if (include_unspec && contact->gender == LIBCONTACTS_UNSPECIFIED_GENDER);
			else
				goto next;
			if (service && !contact->blocks) {
				if (blocked_on)
					goto next;
			} else if (service) {
				for (j = 0; contact->blocks[j]; j++) {
					if (!contact->blocks[j]->service)
						continue;
					if (!strcmp(contact->blocks[j]->service, service))
						break;
					if (check_global_blocks && !strcmp(contact->blocks[j]->service, ".global"))
						break;
				}
				if (!blocked_on != !contact->blocks[j])
					goto next;
			}
			if (names && contact->name)
				printf("%s (%s)\n", contact->id, contact->name);
			else
				printf("%s\n", contact->id);
		next:
			libcontacts_contact_destroy(contact);
			free(contact);
		}
		free(contacts);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return 0;
}
