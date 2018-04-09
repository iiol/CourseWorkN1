#ifndef _OPTION_H
#define _OPTION_H

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <wchar.h>

#include "xstdlib.h"

struct arguments
{
	char *progname;
	wchar_t *eos;
	wchar_t *ign;
	char *word;
} args;

void argsinit(int argc, char **argv);
void printhelp(void);

#endif
