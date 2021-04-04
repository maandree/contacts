/* See LICENSE file for copyright and license details. */
#ifdef MULTICALL_BINARY
# define LIBSIMPLY_CONFIG_MULTICALL_BINARY
#endif

#include <libcontacts.h>
#include <libsimple.h>
#include <libsimple-arg.h>


#ifndef BUFSIZ
# define BUFSIZ 4096
#endif


/* common-birthday.c */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((__pure__))
#endif
int get_age(struct libcontacts_birthday *bday, const struct tm *now);
void print_birthdate(struct libcontacts_birthday *bday, const struct tm *now);
void print_birthday(struct libcontacts_birthday *bday, const struct tm *now);
