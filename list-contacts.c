/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-efmnou]");


int
main(int argc, char *argv[])
{
	int names = 0, emergency = 0;
	int include_men = 0, include_women = 0, include_orgs = 0, include_unspec = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	char **ids;
	size_t i;

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
	default:
		usage();
	} ARGEND;

	if (argc)
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (!names && !emergency && !include_men && !include_women && !include_orgs && !include_unspec) {
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
		for (i = 0; contacts[i]; i++) {
			if (emergency && !contacts[i]->in_case_of_emergency)
				continue;
			if      (include_men    && contacts[i]->gender == LIBCONTACTS_MALE);
			else if (include_women  && contacts[i]->gender == LIBCONTACTS_FEMALE);
			else if (include_orgs   && contacts[i]->gender == LIBCONTACTS_NOT_A_PERSON);
			else if (include_unspec && contacts[i]->gender == LIBCONTACTS_UNSPECIFIED_GENDER);
			else
				continue;
			if (names && contacts[i]->name)
				printf("%s (%s)\n", contacts[i]->id, contacts[i]->name);
			else
				printf("%s\n", contacts[i]->id);
			libcontacts_contact_destroy(contacts[i]);
			free(contacts[i]);
		}
		free(contacts);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return 0;
}
