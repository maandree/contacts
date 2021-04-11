/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-A old-ask-at] [-S old-service] [-T old-type] [-U old-unblock-at] [-Y old-style] "
      "[-a new-ask-at] [-s new-service] [-t new-type] [-u new-unblock-at] [-y new-style] contact-id");


int
main(int argc, char *argv[])
{
	int add = 1, edit = 0, explicit = 1, lookup_explicit = 1;
	enum libcontacts_block_type shadow_block = LIBCONTACTS_BLOCK_IGNORE;
	enum libcontacts_block_type lookup_shadow_block = LIBCONTACTS_BLOCK_IGNORE;
	time_t soft_unblock = 0, hard_unblock = 0;
	time_t lookup_soft_unblock = 0, lookup_hard_unblock = 0;
	const char *srv = NULL, *type = NULL, *style = NULL, *ask = NULL, *ublk = NULL;
	const char *lookup_srv = NULL, *lookup_type = NULL, *lookup_style = NULL;
	const char *lookup_ask = NULL, *lookup_ublk = NULL;
	struct passwd *user;
	struct libcontacts_contact contact;
	char *p;
	size_t i;

	ARGBEGIN {
	case 'A':
		add = 0;
		if (lookup_ask)
			usage();
		lookup_ask = ARG();
		if (!isdigit(*lookup_ask))
			usage();
		lookup_soft_unblock = (time_t)strtoumax(lookup_ask, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'a':
		edit = 1;
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
		add = 0;
		if (lookup_srv)
			usage();
		lookup_srv = ARG();
		break;
	case 's':
		edit = 1;
		if (srv)
			usage();
		srv = ARG();
		break;
	case 'T':
		add = 0;
		if (lookup_type)
			usage();
		lookup_type = ARG();
		break;
	case 't':
		edit = 1;
		if (type)
			usage();
		type = ARG();
		break;
	case 'U':
		add = 0;
		if (lookup_ublk)
			usage();
		lookup_ublk = ARG();
		if (!isdigit(*lookup_ublk))
			usage();
		lookup_hard_unblock = (time_t)strtoumax(lookup_ublk, &p, 10);
		if (errno || *p)
			usage();
		break;
	case 'u':
		edit = 1;
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
		add = 0;
		if (lookup_style)
			usage();
		lookup_style = ARG();
		break;
	case 'y':
		edit = 1;
		if (style)
			usage();
		style = ARG();
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1 || !*argv[0] || strchr(argv[0], '/'))
		usage();

	if (lookup_type) {
		if (!strcmp(lookup_type, "explicit"))
			lookup_explicit = 1;
		else if (!strcmp(lookup_type, "shadow"))
			lookup_explicit = 0;
		else
			eprintf("value of -T shall be either \"explicit\" or \"shadow\"\n");
	}

	if (type) {
		if (!strcmp(type, "explicit"))
			explicit = 1;
		else if (!strcmp(type, "shadow"))
			explicit = 0;
		else
			eprintf("value of -t shall be either \"explicit\" or \"shadow\"\n");
	}

	if (lookup_style) {
		if (!strcmp(lookup_style, "silent"))
			lookup_shadow_block = LIBCONTACTS_SILENT;
		else if (!strcmp(lookup_style, "as-off"))
			lookup_shadow_block = LIBCONTACTS_BLOCK_OFF;
		else if (!strcmp(lookup_style, "as-busy"))
			lookup_shadow_block = LIBCONTACTS_BLOCK_BUSY;
		else if (!strcmp(lookup_style, "ignore"))
			lookup_shadow_block = LIBCONTACTS_BLOCK_IGNORE;
		else
			eprintf("value of -Y shall be either \"silent\", \"as-off\", \"as-busy\", or \"ignore\"\n");
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
		else
			eprintf("value of -y shall be either \"silent\", \"as-off\", \"as-busy\", or \"ignore\"\n");
	}

	if (add)
		edit = 0;

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user)) {
		eprintf("libcontacts_load_contact %s: %s\n", argv[0],
		        errno ? strerror(errno) : "contact file is malformatted");
	}

	i = 0;
	if (edit && contact.blocks) {
		for (; contact.blocks[i]; i++) {
			if (lookup_srv && strcmpnul(contact.blocks[i]->service, lookup_srv))
				continue;
			if (lookup_type && contact.blocks[i]->explicit != lookup_explicit)
				continue;
			if (lookup_style && contact.blocks[i]->shadow_block != lookup_shadow_block)
				continue;
			if (lookup_ask && contact.blocks[i]->soft_unblock != lookup_soft_unblock)
				continue;
			if (lookup_ublk && contact.blocks[i]->hard_unblock != lookup_hard_unblock)
				continue;
			if (srv) {
				free(contact.blocks[i]->service);
				contact.blocks[i]->service = estrdup(srv);
			}
			if (type)
				contact.blocks[i]->explicit = explicit;
			if (style)
				contact.blocks[i]->shadow_block = shadow_block;
			if (ask)
				contact.blocks[i]->soft_unblock = soft_unblock;
			if (ublk)
				contact.blocks[i]->hard_unblock = hard_unblock;
		}
	} else if (!edit) {
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
