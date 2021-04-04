/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-n] (-L | contact-id ...)");


static struct tm *now;

static int
compare_by_birthdate(const void *apv, const void *bpv)
{
	struct libcontacts_contact *const *ap = apv, *const *bp = bpv;
	const struct libcontacts_contact *a = *ap, *b = *bp;
	if (!a->birthday != !b->birthday)
		return !a->birthday ? -1 : +1;
	if (!a->birthday)
		return 0;
	if (a->birthday->year != b->birthday->year)
		return a->birthday->year < b->birthday->year ? -1 : +1;
	if (a->birthday->month != b->birthday->month)
		return a->birthday->month < b->birthday->month ? -1 : +1;
	if (a->birthday->day != b->birthday->day)
		return a->birthday->day < b->birthday->day ? -1 : +1;
	if (a->birthday->before_on_common != b->birthday->before_on_common)
		return a->birthday->before_on_common ? -1 : +1;
	return 0;
}

static int
compare_by_birthday(const void *apv, const void *bpv)
{
	struct libcontacts_contact *const *ap = apv, *const *bp = bpv;
	const struct libcontacts_contact *a = *ap, *b = *bp;
	int ac, bc;
	if (!a->birthday != !b->birthday)
		return !a->birthday ? -1 : +1;
	if (!a->birthday)
		return 0;
	if (!a->birthday->month != !b->birthday->month)
		return !a->birthday->month ? -1 : +1;
	if (!a->birthday->month)
		return 0;
	if (!a->birthday->day != !b->birthday->day)
		return !a->birthday->day ? -1 : +1;
	if (!a->birthday->day)
		return 0;
	ac = (12 + (a->birthday->month - 1 - now->tm_mon) % 12) % 12;
	bc = (12 + (b->birthday->month - 1 - now->tm_mon) % 12) % 12;
	if (ac != bc)
		return ac - bc;
	ac = (31 + (a->birthday->day - now->tm_mday) % 31) % 31;
	bc = (31 + (b->birthday->day - now->tm_mday) % 31) % 31;
	if (ac != bc)
		return ac - bc;
	if (a->birthday->before_on_common != b->birthday->before_on_common)
		return a->birthday->before_on_common ? -1 : +1;
	return 0;
}

int
main(int argc, char *argv[])
{
	int next = 0, list = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	time_t tim;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'n':
		next = 1;
		break;
	case 'L':
		list = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (list ? argc : !argc)
		usage();

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	tim = time(NULL);
	now = localtime(&tim);
	if (!now)
		eprintf("localtime:");

	if (list) {
		if (libcontacts_load_contacts(&contacts, user))
			eprintf("libcontacts_load_contacts:");
		for (i = 0; contacts[i]; i++);
	} else {
		contacts = ecalloc((size_t)argc + 1, sizeof(*contacts));
		for (i = 0; *argv; argv++) {
			contacts[i] = emalloc(sizeof(**contacts));
			if (libcontacts_load_contact(*argv, contacts[i], user)) {
				weprintf("libcontacts_load_contact %s: %s\n", *argv,
				         errno ? strerror(errno) : "contact file is malformatted");
				ret = 1;
				free(contacts[i]);
			} else {
				i++;
			}
		}
		contacts[i] = NULL;
	}

	if (next)
		qsort(contacts, i, sizeof(*contacts), compare_by_birthday);
	else
		qsort(contacts, i, sizeof(*contacts), compare_by_birthdate);

	for (i = 0; contacts[i]; i++) {
		if (contacts[i]->birthday) {
			if (next) {
				if (argc != 1)
					printf("%s: ", contacts[i]->id);
				print_birthday(contacts[i]->birthday, now);
			} else if (contacts[i]->birthday->month && contacts[i]->birthday->year) {
				if (argc != 1)
					printf("%s: ", contacts[i]->id);
				print_birthdate(contacts[i]->birthday, now);
			}
		}
		libcontacts_contact_destroy(contacts[i]);
		free(contacts[i]);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	free(contacts);
	return ret;
}
