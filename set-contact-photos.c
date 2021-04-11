/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-r | -u] contact-id ... photo");


static char *
get_target(const char *path, size_t *lenp)
{
	char *target = NULL;
	size_t size = 0;
	ssize_t n = 0;

        do {
                if ((size_t)n == size)
                        target = erealloc(target, size += 512);
                n = readlink(path, target, size);
                if (n < 0) {
			if (errno == EINVAL) {
				free(target);
				return NULL;
			}
			eprintf("readlink %s:", path);
		}
        } while ((size_t)n >= size);

	*lenp = (size_t)n;
	target[n] = '\0';

	return target;

}

static char *
get_absolute_path(const char *path, const char *cwd)
{
	char *ret, *r, *w, *target, *new;
	size_t len1, len2, len3, targetlen, retlennul, looplimit = 64;

	if (path[0] == '/') {
		retlennul = strlen(path) + 1;
		ret = emalloc(retlennul);
		memcpy(ret, path, retlennul);
	} else {
		len1 = strlen(cwd);
		len2 = strlen(path);
		retlennul = len1 + len2 + 2;
		ret = emalloc(retlennul);
		memcpy(ret, cwd, len1);
		ret[len1++] = '/';
		memcpy(&ret[len1], path, ++len2);
	}

again:
	for (w = r = ret; *r;) {
		if (r[0] == '/' && (r[1] == '/' || !r[1])) {
			r += 1;
		} else if (r[0] == '/' && r[1] == '.' && (r[2] == '/' || !r[2])) {
			r += 2;
		} else if (r[0] == '/' && r[1] == '.' && r[2] == '.' && (r[3] == '/' || !r[3])) {
			*w = '\0';
			target = get_target(ret, &targetlen);
			if (!target) {
				while (w[-1] != '/')
					w--;
				r += 3;
			} else if (!target[0]) {
				eprintf("%s: encountered symlink with empty target\n", path);
			} else if (target[0] == '/') {
				if (!looplimit--) {
					errno = ELOOP;
					eprintf("%s:", path);
				}
				len1 = targetlen;
				len2 = retlennul - (size_t)(r - ret);
				retlennul = len1 + len2;
				new = emalloc(retlennul);
				memcpy(new, target, len1);
				memcpy(&new[len1], r, len2);
				free(ret);
				ret = new;
				free(target);
				goto again;
			} else {
				if (!looplimit--) {
					errno = ELOOP;
					eprintf("%s:", path);
				}
				while (w[-1] != '/')
					w--;
				len1 = (size_t)(w - ret);
				len2 = targetlen;
				len3 = retlennul - (size_t)(r - ret);
				retlennul = len1 + len2 + len3;
				new = emalloc(retlennul);
				memcpy(new, ret, len1);
				memcpy(&new[len1], target, len2);
				memcpy(&new[len1 + len2], r, len3);
				free(ret);
				ret = new;
				r = w = &ret[len1];
				free(target);
			}
		} else {
			*w++ = *r++;
		}
	}

	*w = '\0';
	if (!*ret) {
		ret[0] = '/';
		ret[1] = '\0';
	}
	return ret;
}

static const char *
get_cwd(char **free_this)
{
        char *cwd = NULL;
        size_t size = 64 / 2;
        int saved_errno = errno;
        const char *pwd;
	struct stat cst, pst;

        *free_this = NULL;
	for (;;) {
                cwd = erealloc(*free_this, size *= 2);
                *free_this = cwd;
		if (getcwd(cwd, size))
                        break;
                if (errno != ERANGE)
			eprintf("getcwd %zu:", size);
        }

        errno = saved_errno;
        if (!(pwd = getenv("PWD")) || *pwd != '/' || stat(pwd, &pst) || stat(cwd, &cst))
                return cwd;

	if (pst.st_dev == cst.st_dev && pst.st_ino == cst.st_ino) {
		free(*free_this);
		*free_this = NULL;
		return pwd;
	}
        return cwd;
}

int
main(int argc, char *argv[])
{
	int remove = 0, as_is = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	char *photo, **r, **w, *newpath = NULL, *home, *cwd_free;
	const char *cwd;
	int ret = 0;
	size_t i, len;

	ARGBEGIN {
	case 'r':
		as_is = 1;
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc < 2 || (as_is & remove))
		usage();

	as_is |= remove;
	photo = argv[--argc];
	argv[argc] = NULL;

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (!as_is && user->pw_dir && user->pw_dir[0] == '/' && photo[0] != '/') {
		cwd = get_cwd(&cwd_free);
		if (cwd[0] != '/')
			abort();
		newpath = get_absolute_path(photo, cwd);
		home = get_absolute_path(user->pw_dir, cwd);
		free(cwd_free);
		len = strlen(home);
		if (!strncmp(newpath, home, len) && newpath[len] == '/')
			photo = &newpath[len + 1];
		else
			photo = newpath;
		free(home);
	}

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			weprintf("libcontacts_load_contact %s: %s\n", *argv,
			         errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
			continue;
		}
		if (contact.photos) {
			for (i = 0; contact.photos[i]; i++)
				if (!strcmp(contact.photos[i], photo))
					break;
			r = &contact.photos[i];
			if (!remove && !*r)
				goto add_photo;
			if (remove && *r) {
				free(*r);
				for (w = r++; (*w++ = *r++););
				if (libcontacts_save_contact(&contact, user)) {
					weprintf("libcontacts_save_contact %s:", *argv);
					ret = 1;
				}
			}
		} else if (!remove) {
			i = 0;
		add_photo:
			contact.photos = erealloc(contact.photos, (i + 2) * sizeof(*contact.photos));
			contact.photos[i + 1] = NULL;
			contact.photos[i] = photo;
			if (libcontacts_save_contact(&contact, user)) {
				weprintf("libcontacts_save_contact %s:", *argv);
				ret = 1;
			}
			contact.photos[i] = NULL;
		}
		libcontacts_contact_destroy(&contact);
	}

	free(newpath);
	return ret;
}
