#ifndef __TRACKER_H
#define __TRACKER_H

#include "include.h"
#include "tag.h"

#define META_NAME_SIZE 16

typedef struct __image_meta
{
	int valid;
	char **paths;
	M_TAG *tags;
	int tc;
	char name[META_NAME_SIZE];
} I_M;

typedef struct __tracker
{
	char *profile;
	TAG root;
	I_M *meta;
	int mc;
	int autoC;
} TRACKER;

void TRACKER_init(TRACKER*);
void TRACKER_import(TRACKER*, const char *, const char *);
void TRACKER_reimport(TRACKER*, const char *);
void TRACKER_addTag(TRACKER*, const char *);
void TRACKER_tagImage(TRACKER*, int, const char *);
void TRACKER_favImage(TRACKER*, int);
void TRACKER_listTags(TRACKER*);
void TRACKER_listImages(TRACKER*);
void TRACKER_read(TRACKER*, const char *);
void TRACKER_flush(TRACKER*);
void TRACKER_dispose(TRACKER*);

#endif

