/* See LICENSE file for copyright and license details. */
#include "common.h"


static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int
get_age(struct libcontacts_birthday *bday, const struct tm *now)
{
	int age = now->tm_year + 1900 - (int)bday->year;
	if (now->tm_mon + 1 < bday->month)
		age -= 1;
	else if (now->tm_mon + 1 == bday->month)
		age -= (now->tm_mday < bday->day);
	return age;
}

void
print_birthdate(struct libcontacts_birthday *bday, const struct tm *now)
{
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

void
print_birthday(struct libcontacts_birthday *bday, const struct tm *now)
{
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
