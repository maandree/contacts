/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-C old-context] [-c new-context] [-N old-number] [-n new-number] [-F | -f] [-M | -m] [-u] contact-id");


int
main(int argc, char *argv[])
{
	int set_facsimile = -1, set_mobile = -1, add = 1, edit = 0, remove = 0;
	const char *context, *number, *lookup_context, *lookup_number;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_number **r, **w;
	size_t i;

	ARGBEGIN {
	case 'C':
		add = 0;
		if (lookup_context)
			usage();
		lookup_context = ARG();
		break;
	case 'c':
		edit = 1;
		if (context)
			usage();
		context = ARG();
		break;
	case 'N':
		add = 0;
		if (lookup_number)
			usage();
		lookup_number = ARG();
		break;
	case 'n':
		edit = 1;
		if (number)
			usage();
		number = ARG();
		break;
	case 'F':
		if (set_facsimile >= 0)
			return 0;
		set_facsimile = 0;
		break;
	case 'f':
		if (set_facsimile >= 0)
			return 0;
		set_facsimile = 1;
		break;
	case 'M':
		if (set_mobile >= 0)
			return 0;
		set_mobile = 0;
		break;
	case 'm':
		if (set_mobile >= 0)
			return 0;
		set_mobile = 1;
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (remove == edit) {
		if (edit)
			eprintf("-u cannot be combined with -cn\n");
		eprintf("at least one of -cnu is required\n");
	}

	if (add)
		edit = 0;

	if (argc != 0 || !*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	i = 0;
	if ((edit || remove) && contact.numbers) {
		for (r = contact.numbers; *r; r++) {
			if (lookup_context && strcmpnul((*r)->context, lookup_context))
				continue;
			if (lookup_number && strcmpnul((*r)->context, lookup_number))
				continue;
			break;
		}
		if (!edit) {
			libcontacts_number_destroy(*r);
			free(*r);
			for (w = r++; (*w++ = *r++););
		} else if (*r) {
			if (context) {
				free(contact.numbers[i]->context);
				contact.numbers[i]->context = estrdup(context);
			}
			if (number) {
				free(contact.numbers[i]->number);
				contact.numbers[i]->number = estrdup(number);
			}
			if (set_mobile >= 0)
				contact.numbers[i]->is_mobile = set_mobile;
			if (set_facsimile >= 0)
				contact.numbers[i]->is_facsimile = set_facsimile;
		} else {
			libcontacts_contact_destroy(&contact);
			return 0;
		}
	} else if (!edit && !remove) {
		if (contact.numbers)
			for (i = 0; contact.numbers[i]; i++);
		contact.numbers = erealloc(contact.numbers, (i + 2) * sizeof(*contact.numbers));
		contact.numbers[i + 1] = NULL;
		contact.numbers[i] = ecalloc(1, sizeof(**contact.numbers));
		contact.numbers[i]->context = estrdup(argv[1]);
		contact.numbers[i]->number = estrdup(argv[2]);
		contact.numbers[i]->is_mobile = set_mobile > 0;
		contact.numbers[i]->is_facsimile = set_facsimile > 0;
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
