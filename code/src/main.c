#include <stdio.h>
#include <locale.h>
#include <string.h>

#include "option.h"
#include "markov.h"
#include "hashtb.h"

//getword()
#include <wctype.h>
#include <wchar.h>

char *getword(void);
int isweos(wchar_t wc);
int iswign(wchar_t wc);
void debug(MARK p);

int main(int argc, char *argv[])
{
	int n = 0;
	int prenw = -1, numword;

	MARK mword;//, mcouplew;
	char *word, **dict = NULL;//, *couplew, **dictcw = NULL;
	int dictlen = -1;//, cwdictlen = -1;

	setlocale(LC_CTYPE, "");
	argsinit(argc, argv);

	mword = minit();

	for (; (word = getword()) != NULL; prenw = numword) {
		numword = getnum(word);
		if (numword == -1) {
			if (dictlen < n) {
				dictlen += 1;
				dict = xrealloc(dict, (dictlen + 1) * sizeof(char**));
			}

			dict[n] = word;
			numword = n++;
		}

		addentry(numword, word);
		mcount(mword, prenw, numword);
		printf("%d -> %d\n", prenw, numword);
	}
	putchar('\n');

	normalize(mword);
	debug(mword);

	return 0;
}

char *getword(void)
{
	int i;
	wchar_t wc;
	char *word;
	int lenword = 128;

	word = xmalloc(lenword);

	for (; (wc = fgetwc(stdin)) != WEOF && iswspace(wc);)
		;

	if (isweos(wc)) {
		i = wctomb(word, wc);
		word[i] = '\0';
		xrealloc(word, lenword - i - 1);

		return word;
	}

	ungetwc(wc, stdin);

	for (i = 0; ((wc = fgetwc(stdin)) != WEOF);) {
		if (lenword < i + MB_CUR_MAX + 1) {
			lenword *= 2;
			xrealloc(word, lenword);
		}

		if (iswign(wc))
			continue;
		else if (iswalpha(wc) || wc == '-') {
			wc = towlower(wc);
			i += wctomb(&(word[i]), wc);
		}
		else if (wc == '\n' && word[i - 1] == '-')
			--i;
		else if ((iswspace(wc) || isweos(wc)) && i > 0) {
			ungetwc(wc, stdin);

			break;
		} else {
			//endword();
			i = 0;
		}
	}

	xrealloc(word, i + 1);
	word[i] = '\0';

	if (i == 0)
		return NULL;
	else
		return word;
}

int isweos(wchar_t wc)
{
	int i;

	for (i = 0; args.eos[i] != '\0'; ++i) {
		if (wc == args.eos[i])
			return ++i;
	}

	return 0;
}

int iswign(wchar_t wc)
{
	int i;

	for (i = 0; args.ign[i] != '\0'; ++i) {
		if (wc == args.ign[i])
			return ++i;
	}

	return 0;
}

void debug(MARK p)
{
	struct markov *now;
	int i, j;

	for (i = 0; i <= p->maxi; ++i) {
		printf("%d)", i);
		now = p->mp + i;
		if (now->pj != NULL) {
			for (j = 0; j <= now->maxj; ++j)
				printf(" %.2f,", *(now->pj + j));
			printf("\b \b");
		}
		printf("\n");
	}
#if 0
	putchar('\n');
	for (i = 0;;) {
		printf("%d ", i);

		if ((i = getel(p, i)) < 0)
			break;
	}
	putchar('\n');
#endif
}
