#include "option.h"

#define DEF_EOS ".;!?"
#define DEF_IGN "'\"[]{}(),:"

enum
{
	EM = 0x0100,
	EA,
	IM,
	IA,
};

void argsinit(int argc, char **argv)
{
	int size;
	int neos;
	int nign;

	int opt;
	char sopt[] = "h";
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

	args.progname = argv[0];
	neos = mbstowcs(args.eos, DEF_EOS, sizeof (DEF_EOS));
	nign = mbstowcs(args.ign, DEF_IGN, sizeof (DEF_IGN));

	for (opterr = 0; (opt = getopt_long(argc, argv, sopt, lopt, NULL)) != -1;) {
		switch (opt) {
		case 'h':
			printhelp();
			exit(EXIT_SUCCESS);

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
			fprintf(stderr, "%s: Unknown argument:\n", args.progname);
			exit(EXIT_FAILURE);
		}
	}
}

void printhelp(void)
{
	printf("Usage: %s [OPTION]...\n\n", args.progname);

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
