#ifndef __TRACKER_H
#define __TRACKER_H

#include "include.h"
#include "tag.h"
#include "evaluate.h"

typedef struct __tracker
{
	char *profile;
	TAG root;
	I_M *meta;
	int mc;
} TRACKER;

void TRACKER_init(TRACKER*);
void TRACKER_search(TRACKER*, const char *);
void TRACKER_import(TRACKER*, const char *, const char *);
void TRACKER_reimport(TRACKER*, const char *);
void TRACKER_addTag(TRACKER*, const char *, int);
void TRACKER_deleteTag(TRACKER*, const char *, int);
void TRACKER_tagImage(TRACKER*, int, const char *);
void TRACKER_untagImage(TRACKER*, int, const char *);
void TRACKER_favImage(TRACKER*, int);
void TRACKER_listTags(TRACKER*);
void TRACKER_listImage(TRACKER*, int);
void TRACKER_listImages(TRACKER*);
void TRACKER_read(TRACKER*, const char *);
void TRACKER_flush(TRACKER*);
void TRACKER_dispose(TRACKER*);

#endif

