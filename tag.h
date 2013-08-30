#ifndef __TAG_H
#define __TAG_H

#include "include.h"

#define TAG_NAME_SIZE 64

typedef struct __meta_tag
{
	int *path;
	int c;
} M_TAG;

typedef struct __tag
{
	char name[TAG_NAME_SIZE];
	int favs;
	struct __tag *tags;
	int tc;
} TAG;

void TAG_init(TAG*);
int TAG_findTag(TAG*, TAG);
void TAG_read(TAG*, FILE *);
void TAG_write(TAG*, FILE *);
void TAG_addTag(TAG*, const char *, int);
int TAG_removeTag(TAG*, const char *, int);
void TAG_listTags(TAG*, int);
void TAG_dispose(TAG*);

int M_TAG_compIntArr(M_TAG *, M_TAG *);
M_TAG *TAG_evaluateAbs(TAG*, const char *, M_TAG *);
M_TAG *TAG_evaluatePart(TAG*, const char *, M_TAG *, int *);
char **TAG_expand(TAG*, const char *, int *);

#endif

