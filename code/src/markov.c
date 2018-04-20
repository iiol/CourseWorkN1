#include "markov.h"

unsigned int seed;

MARK minit(void)
{
	MARK p;

	p = xmalloc(sizeof (struct markglob));
	p->mp = xmalloc(sizeof (struct markov));

	p->maxi = 0;
	p->maxel = 0;
	memset(p->mp, 0, sizeof (struct markov));

	return p;
}

void mfree(MARK p)
{
	free(p->mp);
	free(p);
}

void mcount(MARK p, int ni, int nj)
{
	int size;
	struct markov *now;

	if (ni < 0 || nj < 0)
		return;

	p->maxel = p->maxel > ni ? p->maxel : ni;

	size = sizeof (struct markov);

	for (; p->maxi < ni;) {
		p->maxi = 2*p->maxi + 1;

		p->mp = xrealloc(p->mp, (p->maxi + 1) * size);
		memset(p->mp + (p->maxi + 1)/2, 0, ((p->maxi + 1)/2) * size);
	}

	size = sizeof (double);
	now = p->mp + ni;

	if (now->pj == NULL) {
		now->pj = xmalloc(size);
		memset(now->pj, 0, size);
	}

	if (now->maxj < nj) {
		now->pj = xrealloc(now->pj, size * (nj + 1));
		memset((now->pj + (now->maxj + 1)), 0, size * (nj - now->maxj));
		now->maxj = nj;
	}

	++(*(now->pj + nj));
}

void normalize(MARK p)
{
	int i, j;
	unsigned long size;
	int *amount;
	struct markov *now;

	if (p->maxi > p->maxel) {
		p->maxi = p->maxel - ((p->mp + p->maxel)->pj == NULL ? 1 : 0);
		size = (p->maxi + 1) * sizeof (struct markov);
		p->mp = xrealloc(p->mp, size);
	}

	size = (p->maxi + 1) * sizeof (int);
	amount = xmalloc(size);
	memset(amount, 0, size);

	for (i = 0; i <= p->maxi; ++i) {
		now = p->mp + i;
		if (now->pj != NULL)
			for (j = 0; j <= now->maxj; ++j)
				amount[i] += *(now->pj + j);
	}

	for (i = 0; i <= p->maxi; ++i) {
		now = p->mp + i;
		if (now->pj != NULL)
			for (j = 0; j <= (p->mp + i)->maxj; ++j)
				*(now->pj + j) /= amount[i];
	}
}

int getel(MARK p, int prewel)
{
	int j;
	double nrand;

	if (prewel < 0)
		exit(EXIT_FAILURE);

	if (prewel > p->maxi || (p->mp + prewel)->pj == NULL)
		return -1;

	if (seed == 0)
		seed = (int) time(NULL);

	for (nrand = 0; nrand == 0;)
		nrand = (double) (rand_r(&seed) % ((long) INT_MAX + 1)) / INT_MAX;

	for (j = 0; nrand > 0; ++j) {
		if (j > (p->mp + prewel)->maxj)
			j = 0;

		nrand -= *((p->mp + prewel)->pj + j);
	}

	return --j;
}
