/* See LICENSE file for copyright and license details. */
#include "common.h"

/* Date components are in different options to avoid date format confusion */
USAGE("([-y year | -Y] [-m month | -M] [-d day | -D] [-b | -B] | -u) contact-id ...");


static int
getintarg(const char *arg)
{
	int ret = 0;
	for (; isdigit(*arg); arg++) {
		if (ret > (INT_MAX - (*arg & 15)) / 10)
			usage();
		ret = ret * 10 + (*arg & 15);
	}
	if (*arg)
		usage();
	return ret;
}

int
main(int argc, char *argv[])
{
	static int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int year = 0, month = 0, day = 0, before_on_common = -1, remove = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	int ret = 0;
	size_t i;

	ARGBEGIN {
	case 'y':
		if (year)
			usage();
		year = getintarg(ARG());
		break;
	case 'Y':
		if (year)
			usage();
		year = -1;
		break;
	case 'm':
		if (month)
			usage();
		month = getintarg(ARG());
		break;
	case 'M':
		if (month)
			usage();
		month = -1;
		break;
	case 'd':
		if (day)
			usage();
		day = getintarg(ARG());
		break;
	case 'D':
		if (day)
			usage();
		day = -1;
		break;
	case 'b':
		if (before_on_common == 0)
			usage();
		before_on_common = 1;
		break;
	case 'B':
		if (before_on_common == 1)
			usage();
		before_on_common = 0;
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if ((year | month | day | (before_on_common + 1)) && remove)
		usage();
	if (year % 4 == 0 && (year % 100 || year % 400 == 0)) /* 0 is a leap year */
		days_in_month[1] += 1;
	if ((month && month > 12) || day > days_in_month[month - 1])
		usage();
	if (!argc)
		usage();

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
		} else {
			if (remove) {
				if (!contact.birthday)
					goto next;
				libcontacts_birthday_destroy(contact.birthday);
				free(contact.birthday);
				contact.birthday = NULL;
			} else {
				if (!contact.birthday)
					contact.birthday = ecalloc(1, sizeof(*contact.birthday));
				if (year)
					contact.birthday->year = year < 0 ? 0 : (unsigned int)year;
				if (month)
					contact.birthday->month = month < 0 ? 0 : (unsigned char)month;
				if (day)
					contact.birthday->day = day < 0 ? 0 : (unsigned char)day;
				if (before_on_common >= 0)
					contact.birthday->before_on_common = (unsigned char)before_on_common;
			}
			if (libcontacts_save_contact(&contact, user)) {
				weprintf("libcontacts_save_contact %s:", *argv);
				ret = 1;
			}
		next:
			libcontacts_contact_destroy(&contact);
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}
