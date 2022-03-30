#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	(void) argc;
	fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

	errno = ENOENT;
	perror(argv[0]);

	exit(0);
}
