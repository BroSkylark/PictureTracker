#ifndef __EVALUATE_H
#define __EVALUATE_H

#include "include.h"
#include "tag.h"

#define E_NONE   0
#define E_INS    1
#define E_STR    2
#define E_INT    3
#define E_BOOL   4
#define _E_I_AND 5
#define _E_I_OR  6
#define _E_I_XOR 7
#define _E_I_NOT 8

#define META_NAME_SIZE 16

typedef struct __image_meta
{
	int valid;
	char **paths;
	M_TAG *tags;
	int tc, favs;
	char name[META_NAME_SIZE];
} I_M;

typedef struct __evaluation_node
{
	int type;
	void *data;
} E_N;

typedef struct __int_stack
{
	int *v;
	int c;
} I_S;

typedef struct __evaluation_tree
{
	struct __evaluation_node *nodes;
	int c;
} E_TREE;

void E_TREE_init(E_TREE*);
E_TREE E_TREE_copy(E_TREE*);
void E_TREE_print(E_TREE*);
void E_TREE_evaluateInput(E_TREE*, const char *, TAG *);
void E_TREE_evaluateTags(E_TREE*, TAG *);
void E_TREE_evaluateMeta(E_TREE*, I_M *);
int E_TREE_evaluateFinal(E_TREE*);
int E_TREE_evaluate(E_TREE*, const char *, TAG *, I_M *);
void E_TREE_dispose(E_TREE*);

#endif

