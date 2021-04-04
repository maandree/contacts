/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("contact-id");


int
main(int argc, char *argv[])
{
	char buf[BUFSIZ];
	struct passwd *user;
	char *path;
	ssize_t r;
	size_t n, p;
	int fd;

	NOFLAGS(argc != 1);

	if (!*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	path = libcontacts_get_path(argv[0], user);
	if (!path)
		eprintf("libcontacts_get_path %s:", argv[0]);

	fd = open(path, O_RDONLY);
	if (fd < 0)
		eprintf("open %s O_RDONLY:", path);

	for (;;) {
		r = read(fd, buf, sizeof(buf));
		if (r <= 0) {
			if (!r)
				break;
			eprintf("read %s:", path);
		}
		n = (size_t)r;
		for (p = 0; p < n; p += (size_t)r) {
			r = write(STDOUT_FILENO, &buf[p], n - p);
			if (r <= 0)
				eprintf("write <stdout>:");
		}
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	close(fd);
	free(path);
	return 0;
}
