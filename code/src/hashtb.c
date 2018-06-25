#include "hashtb.h"

#define MAX_HASH 10000

struct entry {
	int n;
	char *s;
	struct entry *next;
} *hashtb[MAX_HASH];

unsigned int gethash(const char *s);
struct entry *ealloc(int n, const char *s);

void addentry(int n, const char *s)
{
	unsigned int hash;
	struct entry *hashp;

	hash = gethash(s);
	hashp = hashtb[hash];

	if (hashp == NULL)
		hashp = ealloc(n, s);

	for(;; hashp = hashp->next) {
		if (strcmp(hashp->s, s) == 0) {
			hashp->n = n;
			break;
		}
		if (hashp->next == NULL)
			hashp->next = ealloc(n, s);
	}

	hashtb[hash] = hashp;
}

int getnum(const char *s)
{
	unsigned int hash;

	hash = gethash(s);

	if (hashtb[hash] == NULL)
		return -1;

	return hashtb[hash]->n;
}

//djb2 hash
unsigned int gethash(const char *s)
{
	unsigned int i;
	unsigned int hash = 5381;

	for (i = 0; s[i] != '\0'; ++i)
		hash += (hash << 5) + s[i];

	return hash % MAX_HASH;
}

struct entry *ealloc(int n, const char *s)
{
	struct entry *tmp;

	tmp = xmalloc(sizeof (struct entry));
	tmp->s = xmalloc(strlen(s) + 1);

	tmp->n = n;
	strcpy(tmp->s, s);
	tmp->next = NULL;

	return tmp;
}
