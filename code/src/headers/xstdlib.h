#ifndef _XSTDLIB_H
#define _XSTDLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void *xmalloc(size_t size);
void *xrealloc(void *p, size_t size);

#endif
