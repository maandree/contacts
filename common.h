/* See LICENSE file for copyright and license details. */
#ifdef MULTICALL_BINARY
# define LIBSIMPLY_CONFIG_MULTICALL_BINARY
#endif

#include <libcontacts.h>
#include <libsimple.h>
#include <libsimple-arg.h>

#include <math.h>


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

/* common-address.c */
int parse_coord(const char *s, double *lat, double *lat_min, double *lat_max, double *lon, double *lon_min, double *lon_max);



#ifndef LIST_BOOL_PARAMS
# define LIST_BOOL_PARAMS(X)
#endif

#define X_USAGE_UPPER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"UPPERS" old-"DISPLAY"] "

#define X_USAGE_LOWER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"LOWERS" new-"DISPLAY"] "

#define X_USAGE_BOOL(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"UPPERS" | -"LOWERS"] "

#define X_PARAMS(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	char *VAR = NULL, *lookup_##VAR = NULL, *old_##VAR = NULL;

#define X_BOOL_PARAMS(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	int set_##VAR = -1;

#define X_ARGPARSE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	case UPPERC:\
		add = 0;\
		if (lookup_##VAR)\
			usage();\
		lookup_##VAR = ARG();\
		break;\
	case LOWERC:\
		edit = 1;\
		if (VAR)\
			usage();\
		VAR = ARG();\
		break;

#define X_BOOL_ARGPARSE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	case UPPERC:\
		if (set_##VAR >= 0)\
			usage();\
		set_##VAR = 0;\
		break;\
	case LOWERC:\
		if (set_##VAR >= 0)\
			usage();\
		set_##VAR = 1;\
		break;

#define X_LOWER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	LOWERS

#define X_LOOKUP(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (lookup_##VAR && strcmpnul(contact.CATEGORY[i]->VAR, lookup_##VAR))\
		continue;

#define X_BOOL_LOOKUP(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (remove && set_##VAR >= 0 && contact.CATEGORY[i]->is_##VAR != set_##VAR)\
		continue;

#define X_CHANGE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (VAR) {\
		old_##VAR = contact.CATEGORY[i]->VAR;\
		contact.CATEGORY[i]->VAR = VAR;\
	}

#define X_BOOL_SET(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (set_##VAR >= 0)\
		contact.CATEGORY[i]->is_##VAR = set_##VAR;

#define X_ADD(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	contact.CATEGORY[i]->VAR = VAR;

#define X_RESTORE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	contact.CATEGORY[i]->VAR = old_##VAR;

#define IMPLEMENT_SET_ON_LIST(CAT)\
	USAGE(LIST_PARAMS(X_USAGE_UPPER)LIST_BOOL_PARAMS(X_USAGE_BOOL)"("LIST_PARAMS(X_USAGE_LOWER)"| -u) contact-id");\
	\
	int\
	main(int argc, char *argv[])\
	{\
		int add = 1, edit = 0, remove = 0;\
		LIST_PARAMS(X_PARAMS)\
		LIST_BOOL_PARAMS(X_BOOL_PARAMS)\
		struct passwd *user;\
		struct libcontacts_contact contact;\
		struct libcontacts_##CAT **r = NULL, **w;\
		size_t i;\
		\
		ARGBEGIN {\
		LIST_PARAMS(X_ARGPARSE)\
		LIST_BOOL_PARAMS(X_BOOL_ARGPARSE)\
		case 'u':\
			remove = 1;\
			break;\
		default:\
			usage();\
		} ARGEND;\
		\
		if (argc != 1 || !*argv[0] || strchr(argv[0], '/'))\
			usage();\
		\
		if (remove == edit) {\
			if (edit)\
				usage();\
			eprintf("at least one of -"LIST_PARAMS(X_LOWER)"u is required\n");\
		}\
		\
		if (add)\
			edit = 0;\
		\
		errno = 0;\
		user = getpwuid(getuid());\
		if (!user)\
			eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");\
		\
		if (libcontacts_load_contact(argv[0], &contact, user)) {\
			eprintf("libcontacts_load_contact %s: %s\n", argv[0],\
			        errno ? strerror(errno) : "contact file is malformatted");\
		}\
		\
		i = 0;\
		if ((edit || remove) && contact.CATEGORY) {\
			for (; contact.CATEGORY[i]; i++) {\
				LIST_PARAMS(X_LOOKUP)\
				LIST_BOOL_PARAMS(X_BOOL_LOOKUP)\
				break;\
			}\
			if (!contact.CATEGORY[i]) {\
				libcontacts_contact_destroy(&contact);\
				return 0;\
			} else if (!edit) {\
				r = &contact.CATEGORY[i];\
				libcontacts_##CAT##_destroy(*r);\
				free(*r);\
				for (w = r++; (*w++ = *r++););\
			} else {\
				LIST_PARAMS(X_CHANGE)\
				LIST_BOOL_PARAMS(X_BOOL_SET)\
			}\
		} else if (!edit && !remove) {\
			if (contact.CATEGORY)\
				for (; contact.CATEGORY[i]; i++);\
			contact.CATEGORY = erealloc(contact.CATEGORY, (i + 2) * sizeof(*contact.CATEGORY));\
			contact.CATEGORY[i + 1] = NULL;\
			contact.CATEGORY[i] = ecalloc(1, sizeof(**contact.CATEGORY));\
			LIST_PARAMS(X_ADD)\
			LIST_BOOL_PARAMS(X_BOOL_SET)\
		}\
		\
		if (libcontacts_save_contact(&contact, user))\
			eprintf("libcontacts_save_contact %s:", argv[0]);\
		\
		if (!r) {\
			LIST_PARAMS(X_RESTORE)\
		}\
		\
		libcontacts_contact_destroy(&contact);\
		return 0;\
	}
