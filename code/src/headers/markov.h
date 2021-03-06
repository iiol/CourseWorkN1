#ifndef _MARKOV_H
#define _MARKOV_H

#include <string.h>
#include <time.h>
#include <limits.h>

#include "xstdlib.h"

struct matrix {
	int maxj;
	double *pj;
};

typedef struct markov {
	int maxi;
	int maxel;
	struct matrix *mp;
} *MARK;

MARK minit(void);
void mfree(MARK p);
void mcount(MARK p, int ni, int nj);
void normalize(MARK p);
int getel(MARK p, int prewel);

#endif
