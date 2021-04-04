/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-c | -n] [-F | -f] [-M | -m] contact-id context number | -u contact-id context [number] | -U contact-id [context] number");


int
main(int argc, char *argv[])
{
	int set_facsimile = -1, set_mobile = -1;
	int update_number = 0, update_context = 0;
	int remove_by_context = 0, remove_by_number = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_number **r, **w;
	size_t i;

	ARGBEGIN {
	case 'c':
		update_context = 1;
		break;
	case 'n':
		update_number = 1;
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
		remove_by_context = 1;
		break;
	case 'U':
		remove_by_number = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (update_number + update_context + remove_by_context + remove_by_number > 0)
		usage();
	if (argc < 3 - remove_by_context - remove_by_number || argc > 3)
		usage();
	if ((set_facsimile >= 0 || set_mobile >= 0) && remove_by_context + remove_by_number)
		usage();

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	i = 0;
	if (contact.numbers) {
		if (update_number) {
			for (; contact.numbers[i]; i++) {
				if (!strcmpnul(contact.numbers[i]->context, argv[1])) {
					free(contact.numbers[i]->number);
					contact.numbers[i]->number = estrdup(argv[2]);
					goto save;
				}
			}
		} else if (update_context) {
			for (; contact.numbers[i]; i++) {
				if (!strcmpnul(contact.numbers[i]->number, argv[2])) {
					free(contact.numbers[i]->context);
					contact.numbers[i]->context = estrdup(argv[1]);
					goto save;
				}
			}
		} else if (!remove_by_context && !remove_by_number) {
			for (; contact.numbers[i]; i++) {
				if (!strcmpnul(contact.numbers[i]->context, argv[1]))
					if (!strcmpnul(contact.numbers[i]->number, argv[2]))
						goto save;
			}
		} else if (argc == 3) {
			for (; contact.numbers[i]; i++)
				if (!strcmpnul(contact.numbers[i]->context, argv[1]))
					if (!strcmpnul(contact.numbers[i]->number, argv[2]))
						break;
		} else if (remove_by_context) {
			for (; contact.numbers[i]; i++)
				if (!strcmpnul(contact.numbers[i]->context, argv[1]))
					break;
		} else {
			for (; contact.numbers[i]; i++)
				if (!strcmpnul(contact.numbers[i]->number, argv[1]))
					break;
		}
	}
	if (remove_by_context || remove_by_number) {
		if (contact.numbers && contact.numbers[i]) {
			libcontacts_number_destroy(contact.numbers[i]);
			free(contact.numbers[i]);
			for (r = &1[w = &contact.numbers[i]]; *r;)
				*w++ = *r++;
			*w = NULL;
		}
	} else {
		contact.numbers = erealloc(contact.numbers, (i + 2) * sizeof(*contact.numbers));
		contact.numbers[i + 1] = NULL;
		contact.numbers[i] = ecalloc(1, sizeof(**contact.numbers));
		contact.numbers[i]->context = estrdup(argv[1]);
		contact.numbers[i]->number = estrdup(argv[2]);
		contact.numbers[i]->is_mobile = set_mobile > 0;
		contact.numbers[i]->is_facsimile = set_facsimile > 0;
	}

save:
	if (set_mobile >= 0)
		contact.numbers[i]->is_mobile = set_mobile;
	if (set_facsimile >= 0)
		contact.numbers[i]->is_facsimile = set_facsimile;
	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
