/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a ask-at | -A ask-at] [-s service | -S service] [-t type | -T type] "
      "[-u unblock-at | -U unblock-at] [-y style | -Y style] contact-id");


int
main(int argc, char *argv[])
{
	int edit_srv = 0, edit_type = 0, edit_style = 0, edit_ask = 0, edit_ublk = 0;
	int edit = 0, explicit = 1;
	enum libcontacts_block_type shadow_block = LIBCONTACTS_BLOCK_IGNORE;
	time_t soft_unblock = 0, hard_unblock = 0;
	const char *srv = NULL, *type = NULL, *style = NULL, *ask = NULL, *ublk = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	char *p;
	size_t i;

	ARGBEGIN {
	case 'A':
		edit_ask = 1;
		edit = 1;
		/* fall through */
	case 'a':
		if (ask)
			usage();
		ask = ARG();
		if (!isdigit(*ask))
			usage();
		soft_unblock = (time_t)strtoumax(ask, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'S':
		edit_srv = 1;
		edit = 1;
		/* fall through */
	case 's':
		if (srv)
			usage();
		srv = ARG();
		break;
	case 'T':
		edit_type = 1;
		edit = 1;
		/* fall through */
	case 't':
		if (type)
			usage();
		type = ARG();
		break;
	case 'U':
		edit_ublk = 1;
		edit = 1;
		/* fall through */
	case 'u':
		if (ublk)
			usage();
		ublk = ARG();
		if (!isdigit(*ublk))
			usage();
		hard_unblock = (time_t)strtoumax(ublk, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'Y':
		edit_style = 1;
		edit = 1;
		/* fall through */
	case 'y':
		if (style)
			usage();
		style = ARG();
		break;
	default:
		usage();
	} ARGEND;

	if ((!srv || edit_srv) && (!type || edit_type) && (!style || edit_style) &&
	    (!ask || edit_ask) && (!ublk || edit_ublk))
		edit = 0;

	if (argc != 1 || !*argv[0] || strchr(argv[0], '/'))
		usage();

	if (type) {
		if (!strcmp(type, "explicit"))
			explicit = 1;
		else if (!strcmp(type, "shadow"))
			explicit = 0;
		else
			eprintf("value of -%c shall be either \"explicit\" or \"shadow\"\n", edit_type ? 'T' : 't');
	}

	if (style) {
		if (!strcmp(style, "silent"))
			shadow_block = LIBCONTACTS_SILENT;
		else if (!strcmp(style, "as-off"))
			shadow_block = LIBCONTACTS_BLOCK_OFF;
		else if (!strcmp(style, "as-busy"))
			shadow_block = LIBCONTACTS_BLOCK_BUSY;
		else if (!strcmp(style, "ignore"))
			shadow_block = LIBCONTACTS_BLOCK_IGNORE;
		else if (edit_style)
			eprintf("value of -Y shall be either \"silent\", \"as-off\", \"as-busy\", or \"ignore\"\n");
		else
			eprintf("value of -y shall be either \"silent\", \"as-off\", \"as-busy\", or \"ignore\"\n");
	}

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	if (edit && contact.blocks) {
		for (i = 0; contact.blocks[i]; i++) {
			if (srv && !edit_srv && strcmpnul(contact.blocks[i]->service, srv))
				continue;
			if (type && !edit_type && contact.blocks[i]->explicit != explicit)
				continue;
			if (style && !edit_style && contact.blocks[i]->shadow_block != shadow_block)
				continue;
			if (ask && !edit_ask && contact.blocks[i]->soft_unblock != soft_unblock)
				continue;
			if (ublk && !edit_ublk && contact.blocks[i]->hard_unblock != hard_unblock)
				continue;
			if (edit_srv) {
				free(contact.blocks[i]->service);
				contact.blocks[i]->service = estrdup(srv);
			}
			if (edit_type)
				contact.blocks[i]->explicit = explicit;
			if (edit_style)
				contact.blocks[i]->shadow_block = shadow_block;
			if (edit_ask)
				contact.blocks[i]->soft_unblock = soft_unblock;
			if (edit_ublk)
				contact.blocks[i]->hard_unblock = hard_unblock;
		}
	} else if (!edit) {
		i = 0;
		if (contact.blocks)
			for (; contact.blocks[i]; i++);
		contact.blocks = erealloc(contact.blocks, (i + 2) * sizeof(*contact.blocks));
		contact.blocks[i + 1] = NULL;
		contact.blocks[i] = ecalloc(1, sizeof(**contact.emails));
		contact.blocks[i]->service      = estrdup(srv ? srv : ".global");
		contact.blocks[i]->explicit     = explicit;
		contact.blocks[i]->shadow_block = shadow_block;
		contact.blocks[i]->soft_unblock = soft_unblock;
		contact.blocks[i]->hard_unblock = hard_unblock;
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);

	return 0;
}
