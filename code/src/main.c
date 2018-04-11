#include <stdio.h>
#include <locale.h>
#include <string.h>

#include "option.h"
#include "markov.h"
#include "hashtb.h"

//getword()
#include <wctype.h>
#include <wchar.h>

typedef char** DICT;

void fillmc(MARK *mc, DICT *dict, int quantity);
char *concat(char *s1, char *s2);
char *getword(void);
int isweos(wchar_t wc);
int iswign(wchar_t wc);
void debug(MARK p);

int main(int argc, char *argv[])
{
	int i, numword;

	MARK mword, mcword;
	DICT dict = NULL, dictcw = NULL;

	setlocale(LC_CTYPE, "");
	argsinit(argc, argv);

	mword = minit();
	mcword = minit();

	fillmc(&mword, &dict, 1);
	//fillmc(&mcword, &dictcw, 2);

	numword = 0;
	if (args.word != NULL)
		numword = getnum(args.word);

	if (numword == -1) {
		fprintf(stderr, "The word \"%s\" not found\n", args.word);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 10; ++i) {
		printf("%s ", dict[numword]);

		if ((numword = getel(mword, numword)) == -1)
			break;
	}

	putchar('\n');
	return 0;
}

void fillmc(MARK *mc, DICT *dict, int quantity)
{
	int i, size;
	int spos = 0;
	char *s;

	int prenw = -1, numword;
	int nwmax = 0;

	char *word, *preword = NULL;
	int dictlen = -1;

	for (i = 1; (word = getword()) != NULL; ++i, preword = word) {
		s = concat(preword, word);

		if (i % quantity != 0 || s == NULL)
			continue;

		if ((numword = getnum(s)) == -1) {
			if (dictlen < nwmax) {
				dictlen += 1000;
				size = (dictlen + 1) * sizeof(char**);
				*dict = xrealloc(*dict, size);
			}

			(*dict)[nwmax] = s;
			numword = nwmax++;
		}

		addentry(numword, s);
		mcount(*mc, prenw, numword);

		prenw = numword;
	}

	if (nwmax == 0) {
		fprintf(stderr, "File is not parsed\n");
		exit(EXIT_FAILURE);
	}

	xrealloc(*dict, dictlen * sizeof(char**));
	normalize(*mc);
}

char *concat(char *s1, char *s2)
{
	int len;
	char *retval;

	if (s1 == NULL || s2 == NULL)
		return NULL;

	len = strlen(s1) + strlen(s2) + 2;
	retval = xrealloc(s1, len * sizeof(char));

	strcat(retval, " ");
	strcat(retval, s2);

	return retval;
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
