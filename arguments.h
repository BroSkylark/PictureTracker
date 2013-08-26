#ifndef __ARGUMENTS_H
#define __ARGUMENTS_H

#include "include.h"

typedef struct __arguments
{
	char **args;
	void **vals;
	int c;
} ARG;

void ARG_init(ARG*);
void ARG_parse(ARG*, int, const char * const *);
const char *ARG_getArg(ARG*, const char *);
void ARG_dispose(ARG*);

#endif

