/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-n] (-L | contact-id ...)");


static int
get_age(struct libcontacts_birthday *bday, const struct tm *now)
{
	int age = now->tm_year + 1900 - (int)bday->year;
	if (now->tm_mon + 1 < bday->month)
		age -= 1;
	else if (now->tm_mon + 1 == bday->month)
		age -= (now->tm_mday < bday->day);
	return age;
}

static void
print_birthdate(struct libcontacts_birthday *bday, const struct tm *now)
{
	static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int age, days = 0, y, m, d;
	if (bday->year)
		printf("%04u-", bday->year);
	else
		printf("??""??""-");
	if (bday->month) {
		printf("(%02u)%.3s-", (unsigned)bday->month,
		       &"JanFebMarAprMayJunJulAugSepOctNovDec"[3 * ((bday->month - 1) % 12)]);
	} else {
		printf("(??"")??""?-");
	}
	if (bday->day)
		printf("%02u", (unsigned)bday->day);
	else
		printf("??");
	if ((bday->month - 1) % 12 == 1 && bday->day == 29) {
		bday->day -= bday->before_on_common;
		printf(" (%s on common years)", bday->before_on_common ? "(02)Feb-28" : "(03)Mar-01");
	}
	if (bday->year && bday->month && bday->day) {
		age = get_age(bday, now);
		y = (int)bday->year - 1900;
		m = (int)bday->month - 1;
		d = (int)bday->day;
		while (m != now->tm_mon || d > now->tm_mday) {
			days += days_in_month[m] - (d - 1);
			days += (m == 1 && y % 4 == 0 && (y % 100 || y % 400 == 0));
			d = 1;
			if (++m == 12) {
				y += 1;
				m = 0;
			}
		}
		days += now->tm_mday - d;
		printf(", %i %s and %i %s old", age, age == 1 ? "year" : "years", days, days == 1 ? "day" : "days");
	}
	printf("\n");
}

static void
print_birthday(struct libcontacts_birthday *bday, const struct tm *now)
{
	static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int next_year = 0, leap_year = 0, year, age, days = 0, y, m, d;
	struct tm when;
	if (now->tm_mon + 1 > bday->month)
		next_year = 1;
	else if (now->tm_mon + 1 == bday->month)
		next_year = (now->tm_mday > bday->day);
	if (bday->year) {
		printf("%04i-", now->tm_year + next_year + 1900);
	} else {
		printf("??""??""-");
	}
	year = now->tm_year + next_year + 1900;
	if (year % 4 == 0 && (year % 100 || year % 400 == 0))
		leap_year = 1;
	if (((bday->month - 1) % 12 == 1 && bday->day == 29) && leap_year) {
		if (bday->before_on_common) {
			bday->day -= 1;
		} else {
			bday->day = 1;
			bday->month += 1;
		}
	}
	printf("(%02u)%.3s-%02u (", (unsigned)bday->month,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[3 * ((bday->month - 1) % 12)], (unsigned)bday->day);
	when.tm_year = now->tm_year + next_year;
	when.tm_mon = (bday->month - 1) % 12;
	when.tm_mday = bday->day;
	if (bday->year) {
		age = get_age(bday, &when);
		printf("%i %s ", age, age == 1 ? "year" : "years");
	}
	if (when.tm_mon == now->tm_mon && when.tm_mday == now->tm_mday) {
		printf("today)\n");
	} else {
		y = now->tm_year;
		m = now->tm_mon;
		d = now->tm_mday;
		while (m != when.tm_mon || d > when.tm_mday) {
			days += days_in_month[m] - (d - 1);
			days += (m == 1 && y % 4 == 0 && (y % 100 || y % 400 == 0));
			d = 1;
			if (++m == 12) {
				y += 1;
				m = 0;
			}
		}
		days += when.tm_mday - d;
		printf("in %i %s)\n", days, days == 1 ? "day" : "days");
	}
}

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

static struct tm *now;

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
