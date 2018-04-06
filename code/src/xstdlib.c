#include "xstdlib.h"

void *xmalloc(size_t size)
{
	void *vp;

	errno = 0;
	vp = malloc(size);

	if (vp == NULL) {
		perror(args.progname);

		exit(EXIT_FAILURE);
	}

	return vp;
}

void *xrealloc(void *p, size_t size)
{
	void *vp;

	errno = 0;
	vp = realloc(p, size);

	if (vp == NULL) {
		perror(args.progname);

		exit(EXIT_FAILURE);
	}

	return vp;
}
