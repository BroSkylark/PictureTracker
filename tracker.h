#ifndef __TRACKER_H
#define __TRACKER_H

#include "include.h"

struct meta_tag
{
	int id;
	int lvl;
};

typedef struct __image_meta
{
	int valid;
	struct meta_tag *tags;
	int tc;
	char name[16];
} I_M;

typedef struct __tag
{
	int id;
	char name[64];
} TAG;

typedef struct __tracker
{
	char *profile;
	TAG *tags;
	I_M *meta;
	int tc, mc;
} TRACKER;

void TRACKER_init(TRACKER*);
void TRACKER_import(TRACKER*, const char *);
int TRACKER_read(TRACKER*, const char *);
int TRACKER_flush(TRACKER*);
void TRACKER_dispose(TRACKER*);

#endif

