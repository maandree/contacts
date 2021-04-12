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


#define X_SET_USAGE_UPPER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"UPPERS" old-"DISPLAY"] "

#define X_SET_USAGE_LOWER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"LOWERS" new-"DISPLAY"] "

#define X_SET_USAGE_BOOL(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"UPPERS" | -"LOWERS"] "

#define X_SET_PARAMS(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	char *VAR = NULL, *lookup_##VAR = NULL, *old_##VAR = NULL;

#define X_SET_BOOL_PARAMS(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	int set_##VAR = -1;

#define X_SET_ARGPARSE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
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

#define X_SET_BOOL_ARGPARSE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
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

#define X_SET_LOWER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	LOWERS

#define X_SET_LOOKUP(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (lookup_##VAR && strcmpnul(contact.CATEGORY[i]->VAR, lookup_##VAR))\
		continue;

#define X_SET_BOOL_LOOKUP(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (remove && set_##VAR >= 0 && contact.CATEGORY[i]->is_##VAR != set_##VAR)\
		continue;

#define X_SET_CHANGE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (VAR) {\
		old_##VAR = contact.CATEGORY[i]->VAR;\
		contact.CATEGORY[i]->VAR = VAR;\
	}

#define X_SET_BOOL_SET(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (set_##VAR >= 0)\
		contact.CATEGORY[i]->is_##VAR = set_##VAR;

#define X_SET_ADD(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	contact.CATEGORY[i]->VAR = VAR;

#define X_SET_RESTORE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	contact.CATEGORY[i]->VAR = old_##VAR;

#define IMPLEMENT_SET_ON_LIST(CAT)\
	USAGE(LIST_PARAMS(X_SET_USAGE_UPPER)LIST_BOOL_PARAMS(X_SET_USAGE_BOOL)"("LIST_PARAMS(X_SET_USAGE_LOWER)"| -u) contact-id");\
	\
	int\
	main(int argc, char *argv[])\
	{\
		int add = 1, edit = 0, remove = 0;\
		LIST_PARAMS(X_SET_PARAMS)\
		LIST_BOOL_PARAMS(X_SET_BOOL_PARAMS)\
		struct passwd *user;\
		struct libcontacts_contact contact;\
		struct libcontacts_##CAT **r = NULL, **w;\
		size_t i;\
		\
		ARGBEGIN {\
		LIST_PARAMS(X_SET_ARGPARSE)\
		LIST_BOOL_PARAMS(X_SET_BOOL_ARGPARSE)\
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
			eprintf("at least one of -"LIST_PARAMS(X_SET_LOWER)"u is required\n");\
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
				LIST_PARAMS(X_SET_LOOKUP)\
				LIST_BOOL_PARAMS(X_SET_BOOL_LOOKUP)\
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
				LIST_PARAMS(X_SET_CHANGE)\
				LIST_BOOL_PARAMS(X_SET_BOOL_SET)\
			}\
		} else if (!edit && !remove) {\
			if (contact.CATEGORY)\
				for (; contact.CATEGORY[i]; i++);\
			contact.CATEGORY = erealloc(contact.CATEGORY, (i + 2) * sizeof(*contact.CATEGORY));\
			contact.CATEGORY[i + 1] = NULL;\
			contact.CATEGORY[i] = ecalloc(1, sizeof(**contact.CATEGORY));\
			LIST_PARAMS(X_SET_ADD)\
			LIST_BOOL_PARAMS(X_SET_BOOL_SET)\
		}\
		\
		if (libcontacts_save_contact(&contact, user))\
			eprintf("libcontacts_save_contact %s:", argv[0]);\
		\
		if (!r) {\
			LIST_PARAMS(X_SET_RESTORE)\
		}\
		\
		libcontacts_contact_destroy(&contact);\
		return 0;\
	}


#define X_GET_USAGE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"LOWERS" "DISPLAY"] "

#define X_GET_UPPER(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	UPPERS

#define X_GET_PARAMS(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	int display_##VAR = 0;\
	const char *lookup_##VAR = NULL;

#define X_GET_ARGPARSE(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	case LOWERC:\
		if (lookup_##VAR)\
			usage();\
		lookup_##VAR = ARG();\
		break;\
	case UPPERC:\
		fields += !display_##VAR;\
		display_##VAR = 1;\
		break;

#define X_GET_AUTO_DISPLAY(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	fields += display_##VAR = !lookup_##VAR;

#define X_GET_LOOKUP(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (lookup_##VAR && strcmpnul(elem->VAR, lookup_##VAR))\
		continue;

#define X_GET_FULL_LOOKUP(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	lookup_##VAR && 

#define X_GET_DISPLAY(UPPERC, UPPERS, LOWERC, LOWERS, VAR, DISPLAY)\
	if (display_##VAR) {\
		fields_ -= 1;\
		printf("%s%s", elem->VAR, fields_ ? ": " : "\n");\
	}

#define IMPLEMENT_GET_ON_LIST(CAT)\
	USAGE(LIST_PARAMS(X_GET_USAGE)"[-"LIST_PARAMS(X_GET_UPPER)"] contact-id ..."); \
	\
	int\
	main(int argc, char *argv[])\
	{\
		LIST_PARAMS(X_GET_PARAMS)\
		struct passwd *user;\
		struct libcontacts_contact contact;\
		struct libcontacts_##CAT **elems, *elem;\
		int ret = 0, fields = 0, fields_;\
		size_t i; \
		\
		ARGBEGIN {\
		LIST_PARAMS(X_GET_ARGPARSE)\
		default:\
			usage();\
		} ARGEND;\
		\
		if (!argc)\
			usage();\
		\
		if (!fields) {\
			LIST_PARAMS(X_GET_AUTO_DISPLAY)\
		}\
		\
		for (i = 0; argv[i]; i++)\
			if (!*argv[i] || strchr(argv[i], '/'))\
				usage();\
		\
		errno = 0;\
		user = getpwuid(getuid());\
		if (!user)\
			eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");\
		\
		for (; *argv; argv++) {\
			if (libcontacts_load_contact(*argv, &contact, user)) {\
				weprintf("libcontacts_load_contact %s: %s\n", *argv,\
				         errno ? strerror(errno) : "contact file is malformatted");\
				ret = 1;\
				continue;\
			}\
			if ((elems = contact.CATEGORY)) {\
				for (; (elem = *elems); elems++) {\
					LIST_PARAMS(X_GET_LOOKUP)\
					if (LIST_PARAMS(X_GET_FULL_LOOKUP) !fields) {\
						printf("%s\n", *argv);\
						continue;\
					}\
					if (argc > 1)\
						printf("%s: ", *argv);\
					fields_ = fields;\
					LIST_PARAMS(X_GET_DISPLAY)\
				}\
			}\
			libcontacts_contact_destroy(&contact);\
		}\
		\
		if (fflush(stdout) || ferror(stdout) || fclose(stdout))\
			eprintf("printf:");\
		return ret;\
	}


#define X_FIND_USAGE(LOWERC, LOWERS, VAR, DISPLAY)\
	"[-"LOWERS" "DISPLAY"] "

#define X_FIND_PARAMS(LOWERC, LOWERS, VAR, DISPLAY)\
	const char *VAR = NULL;

#define X_FIND_ARGPARSE(LOWERC, LOWERS, VAR, DISPLAY)\
	case LOWERC:\
		if (VAR)\
			usage();\
		VAR = ARG();\
		break;

#define X_FIND_LOOKUP(LOWERC, LOWERS, VAR, DISPLAY)\
	if (VAR && strcmpnul(elem->VAR, VAR))\
		continue;

#ifdef SUBCATEGORY
# define PRINT_PREFIX_LIST(DATUM)\
	if (elem->SUBCATEGORY && !SUBCATEGORY)\
		printf("%s (%s: %s)\n", contacts[i]->id, elem->SUBCATEGORY, elem->DATUM);\
	else
# define PRINT_PREFIX_LOOKUP(DATUM)\
	if (elem->SUBCATEGORY && !SUBCATEGORY)\
		printf("%s (%s)\n", contacts[i]->id, elem->SUBCATEGORY);\
	else
#else
# define PRINT_PREFIX_LIST(DATUM)
# define PRINT_PREFIX_LOOKUP(DATUM)
#endif

#define IMPLEMENT_FIND_ON_LIST(CAT, DATUM, DISPLAY)\
	USAGE(LIST_PARAMS(X_FIND_USAGE)"(-L | "DISPLAY")");\
	\
	int\
	main(int argc, char *argv[])\
	{\
		int list = 0;\
		LIST_PARAMS(X_FIND_PARAMS)\
		struct passwd *user;\
		struct libcontacts_contact **contacts;\
		struct libcontacts_##CAT **elems, *elem;\
		size_t i;\
		\
		ARGBEGIN {\
		LIST_PARAMS(X_FIND_ARGPARSE)\
		case 'L':\
			list = 1;\
			break;\
		default:\
			usage();\
		} ARGEND;\
		\
		if (argc != 1 - list)\
			usage();\
		\
		errno = 0;\
		user = getpwuid(getuid());\
		if (!user)\
			eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");\
		\
		if (libcontacts_load_contacts(&contacts, user, 1))\
			eprintf("libcontacts_load_contacts:");\
		for (i = 0; contacts[i]; i++) {\
			if ((elems = contacts[i]->CATEGORY)) {\
				for (; (elem = *elems); elems++) {\
					if (!elem->DATUM)\
						continue;\
					LIST_PARAMS(X_FIND_LOOKUP)\
					if (list) {\
						PRINT_PREFIX_LIST(DATUM)\
						printf("%s (%s)\n", contacts[i]->id, elem->DATUM);\
					} else if (!strcmp(elem->DATUM, argv[0])) {\
						PRINT_PREFIX_LOOKUP(DATUM)\
						printf("%s\n", contacts[i]->id);\
					}\
				}\
			}\
			libcontacts_contact_destroy(contacts[i]);\
			free(contacts[i]);\
		}\
		free(contacts);\
		\
		if (fflush(stdout) || ferror(stdout) || fclose(stdout))\
			eprintf("printf:");\
		\
		return 0;\
	}
