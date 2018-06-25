#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <wctype.h>
#include <wchar.h>
#include <getopt.h>

#include "markov.h"
#include "hashtb.h"
#include "xstdlib.h"

#define DEF_EOS ".;!?"
#define DEF_IGN "'\"[]{}(),:"

enum
{
	EM = 0x0100,
	EA,
	IM,
	IA,
};

struct arguments
{
	wchar_t *eos;
	wchar_t *ign;
	char *word;
} args;

typedef struct dictionary {
	char **word;
	int pos;
	int len;
} DICT;

void argsinit(int argc, char **argv);
char *getword(void);
int isweos(wchar_t wc);
int iswign(wchar_t wc);

int main(int argc, char **argv)
{
	int i, size;
	int prenw = -1, numword;
	MARK mword;
	char *word;
	DICT dict = {NULL, 0, 0};

	setlocale(LC_CTYPE, "");

	mword = minit();
	argsinit(argc, argv);

	for (i = 1; (word = getword()) != NULL; ++i) {
		if ((numword = getnum(word)) == -1) {
			if (dict.len == dict.pos) {
				dict.len += 1000;
				size = (dict.len + 1) * sizeof(char**);
				dict.word = xrealloc(dict.word, size);
			}

			numword = dict.pos;
			dict.word[dict.pos] = word;
			++dict.pos;
		}

		addentry(numword, word);
		mcount(mword, prenw, numword);

		prenw = numword;
	}

	if (dict.pos <= 1) {
		fprintf(stderr, "File is not parsed\n");
		exit(EXIT_FAILURE);
	}

	xrealloc(dict.word, dict.len * sizeof(char**));
	normalize(mword);

	numword = 0;

	if (args.word != NULL && (numword = getnum(args.word)) == -1) {
		fprintf(stderr, "The word \"%s\" not found\n", args.word);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 10; ++i) {
		printf("%s ", dict.word[numword]);

		if ((numword = getel(mword, numword)) == -1)
			break;
	}
	putchar('\n');

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

void printhelp(void)
{
	printf("Usage: markov [OPTION]...\n\n");

	printf("OPTIONS:\n");
	printf("--eosA EOS\t\tadd End-Of-Sentence characters\n");
	printf("--eosM EOS\t\tmodify End-Of-Sentence characters\n");
	printf("-h, --help\t\tgive this help list and exit\n");
	printf("--ignA IGN\t\tadd IGNored characters\n");
	printf("--ignM IGN\t\tmodify IGNored characters\n");
	printf("\n");

	printf("DEFAULT:\n");
	printf("EOS: %s\n", DEF_EOS);
	printf("IGN: %s\n", DEF_IGN);
}

void argsinit(int argc, char **argv)
{
	int size;
	int neos;
	int nign;

	int opt;
	char sopt[] = "hw:";
	const struct option lopt[] = {
		{"help", no_argument,       NULL, 'h'},
		{"eosM", required_argument, NULL, EM },
		{"eosA", required_argument, NULL, EA },
		{"ignM", required_argument, NULL, IM },
		{"ignA", required_argument, NULL, IA },
		{NULL,   0,                 NULL, 0  },
	};

	size = sizeof (wchar_t);
	args.eos = xmalloc((strlen(DEF_EOS) + 1) * size);
	args.ign = xmalloc((strlen(DEF_IGN) + 1) * size);

	neos = mbstowcs(args.eos, DEF_EOS, sizeof (DEF_EOS));
	nign = mbstowcs(args.ign, DEF_IGN, sizeof (DEF_IGN));

	for (opterr = 0; (opt = getopt_long(argc, argv, sopt, lopt, NULL)) != -1;) {
		switch (opt) {
		case 'h':
			printhelp();
			exit(EXIT_SUCCESS);

		case 'w':
			args.word = optarg;

			break;

		case EM:
			neos = 0;
		case EA:
			args.eos = realloc(args.eos, (neos + 1025) * size);
			neos += mbstowcs(args.eos + neos, optarg, 1024);
			args.eos = realloc(args.eos, (neos + 1) * size);

			break;

		case IM:
			nign = 0;
		case IA:
			args.ign = realloc(args.ign, (nign + 1025) * size);
			nign += mbstowcs(args.ign + nign, optarg, 1024);
			args.ign = realloc(args.ign, (nign + 1) * size);

			break;

		default:
			fprintf(stderr, "markov: Unknown argument:\n");
			exit(EXIT_FAILURE);
		}
	}
}
