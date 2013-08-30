#include "evaluate.h"

const int E_I_AND = 5;
const int E_I_OR  = 6;
const int E_I_XOR = 7;
const int E_I_NOT = 8;

void E_TREE_push(E_TREE*, const int, const void *);
void E_TREE_evaluateFull(E_TREE*, char **, TAG *);
void E_TREE_evaluateTerm(E_TREE*, char **, TAG *);

void I_S_init(I_S*);
void I_S_push(I_S*, int);
int I_S_pop(I_S*);
int I_S_size(I_S*);
void I_S_dispose(I_S*);

E_N E_N_copy(E_N*);
void E_N_dispose(E_N*);

int inline isA(char c) { return c != '\0' && c != '&' && c != '|' && c != '^' && c != '!' && c != '~' && c != ')'; }


void E_TREE_init(E_TREE *this)
{
	this->c     = 0;
	this->nodes = NULL;
}

void E_TREE_print(E_TREE *this)
{
	printf("> Printing E_TREE:\n");
	
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		E_N *node = &this->nodes[i];
		
		switch(node->type)
		{
			case E_INS:
				printf(">> Instruction: ");
				switch(*((int *) node->data))
				{
					case _E_I_AND: printf("'AND'\n"); break;
					case _E_I_OR:  printf("'OR'\n");  break;
					case _E_I_XOR: printf("'XOR'\n"); break;
					case _E_I_NOT: printf("'NOT'\n"); break;
				}
				break;
			case E_BOOL:
				printf(">> Bool: %s\n", *((int *) node->data) == 0 ? "false" : "true");
				break;
			case E_STR:
				printf(">> String: '%s'\n", (char *) node->data);
				break;
			case E_INT:
				printf(">> IntArr: %d steps.\n", ((M_TAG *) node->data)->c);
				break;
		}
	}
	
	printf("\n");
}

E_TREE E_TREE_copy(E_TREE *this)
{
	E_TREE ret;
	E_TREE_init(&ret);
	
	if(this->c > 0)
	{
		ret.c = this->c;
		
		ret.nodes = malloc(ret.c * sizeof(E_N));
		
		int i;
		for(i = 0 ; i < ret.c ; i++)
		{
			ret.nodes[i] = E_N_copy(&this->nodes[i]);
		}
	}
	
	return ret;
}

void E_TREE_evaluateInput(E_TREE *this, const char *str, TAG *root)
{
	char *copy = strdup(str);
	char *tmp = copy;
	
	E_TREE_evaluateFull(this, &tmp, root);
	
	free(copy);
}

void E_TREE_evaluateTags(E_TREE *this, TAG *tag)
{
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		E_N *node = &this->nodes[i];
		
		if(node->type == E_STR)
		{
			M_TAG *t = TAG_evaluateAbs(tag, node->data, NULL);
			
			if(t == NULL)
			{
				fprintf(stderr, "ERR: Couldn't evaluate '%s'.\nAbort.\n", (char *) node->data);
				exit(1);
			}
			
			node->type = E_INT;
			node->data = t;
		}
	}
}

void E_TREE_evaluateMeta(E_TREE *this, I_M *meta)
{
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		E_N *node = &this->nodes[i];
		
		if(node->type == E_INT)
		{
			int r = 0, j;
			for(j = 0 ; j < meta->tc && r == 0 ; j++)
			{
				r = r || M_TAG_compIntArr(node->data, &meta->tags[j]);
			}
			
			free(((M_TAG *) node->data)->path);
			free(node->data);
			
			node->type = E_BOOL;
			node->data = malloc(sizeof(int));
			*((int *) node->data) = r;
		}
	}
}

int E_TREE_evaluateFinal(E_TREE *this)
{
	int r = 0;
	
	I_S stack;
	I_S_init(&stack);
	
	int i, v, v2;
	for(i = 0 ; i < this->c ; i++)
	{
		E_N *node = &this->nodes[i];
		
		switch(node->type)
		{
			case E_INS:
				v = I_S_pop(&stack);
				switch(*((int *) node->data))
				{
					case _E_I_AND: I_S_push(&stack, I_S_pop(&stack) && v); break;
					case _E_I_OR:  I_S_push(&stack, I_S_pop(&stack) || v); break;
					case _E_I_NOT: I_S_push(&stack, v == 0 ? 1 : 0); break;
					case _E_I_XOR: 
						v2 = I_S_pop(&stack);
						I_S_push(&stack, (v == 0 && v2 != 0) || (v != 0 && v2 == 0));
						break;
				}
				break;
			case E_BOOL:
				I_S_push(&stack, *((int *) node->data));
				break;
			case E_STR:
			case E_INT:
			default:
				fprintf(stderr, "ERR: Unknown command %d.\nAbort.\n", node->type);
				exit(1);
				break;
		}
	}
	
	r = I_S_pop(&stack);
	
	if(I_S_size(&stack) != 0)
	{
		fprintf(stderr, "ERR: Stack after evaluation not empty (%d left).\nAbort.\n", I_S_size(&stack));
		exit(1);
	}
	
	I_S_dispose(&stack);
	
	return r;
}

int E_TREE_evaluate(E_TREE *this, const char *str, TAG *root, I_M *meta)
{
	E_TREE_evaluateInput(this, str, root);
	E_TREE_evaluateTags(this, root);
	E_TREE_evaluateMeta(this, meta);
	
	return E_TREE_evaluateFinal(this);
}

void E_TREE_dispose(E_TREE *this)
{
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		E_N_dispose(&this->nodes[i]);
	}
	
	free(this->nodes);
	
	E_TREE_init(this);
}

// # ==========================================================================

void E_TREE_push(E_TREE *this, const int t, const void *d)
{
	E_N node;
	node.type = t;
	node.data = (void *) d;
	
	this->nodes = realloc(this->nodes, ++this->c * sizeof(E_N));
	this->nodes[this->c - 1] = E_N_copy(&node);
}

void E_TREE_evaluateFull(E_TREE *this, char **src, TAG *root)
{
	E_TREE_evaluateTerm(this, src, root);
	
	char c = **src;
	
	switch(c)
	{
		case '&':
		case '|':
		case '^': break;
		case ')':
		case '\0': return;
		default:
			fprintf(stderr, "ERR: Unexpected char %c!\nAbort.\n", c);
			exit(1);
	}
	
	(*src)++;
	
	E_TREE_evaluateTerm(this, src, root);
	
	switch(c)
	{
		case '&': E_TREE_push(this, E_INS, &E_I_AND); break;
		case '|': E_TREE_push(this, E_INS, &E_I_OR);  break;
		case '^': E_TREE_push(this, E_INS, &E_I_XOR); break;
	}
}

void E_TREE_evaluateTerm(E_TREE *this, char **src, TAG *root)
{
	char str[1024];
	int i = 0;
	
	if(**src == '!' || **src == '~')
	{
		(*src)++;
		E_TREE_evaluateTerm(this, src, root);
		E_TREE_push(this, E_INS, &E_I_NOT);
		return;
	}
	else if(**src == '(')
	{
		(*src)++; // for '('
		E_TREE_evaluateFull(this, src, root);
		(*src)++; // for ')'
		return;
	}
	
	while(isA(**src))
	{
		str[i++] = **src;
		(*src)++;
	}
	str[i] = '\0';
	
	int l = 0;
	char **eval = TAG_expand(root, str, &l);
	
	if(l <= 0)
	{
		fprintf(stderr, "ERR: Couldn't resolve '%s'.\nAbort.\n", str);
		exit(1);
	}
	
	E_TREE_push(this, E_STR, eval[0]);
	
	for(i = 1 ; i < l ; i++)
	{
		E_TREE_push(this, E_STR, eval[i]);
		E_TREE_push(this, E_INS, &E_I_OR);
	}
	
	for(i = 0 ; i < l ; i++)
	{
		free(eval[i]);
	}
	free(eval);
}

// # --------------------------------------------------------------------------

void I_S_init(I_S *this)
{
	this->c = 0;
	this->v = NULL;
}

void I_S_push(I_S *this, int v)
{
	this->v = realloc(this->v, ++this->c * sizeof(int));
	this->v[this->c - 1] = v;
}

int I_S_pop(I_S *this)
{
	int r = this->v[--this->c];
	
	if(this->c > 0)
	{
		this->v = realloc(this->v, this->c * sizeof(int));
	}
	else
	{
		free(this->v);
		this->v = NULL;
	}
	
	return r;
}

int I_S_size(I_S *this)
{
	return this->c;
}

void I_S_dispose(I_S *this)
{
	free(this->v);
	I_S_init(this);
}

// # --------------------------------------------------------------------------

E_N E_N_copy(E_N *this)
{
	E_N ret;
	ret.type = this->type;
	
	int c;
	switch(ret.type)
	{
		case E_INS:
		case E_BOOL:
			ret.data = malloc(sizeof(int));
			*((int *) ret.data) = *((int *) this->data);
			break;
		case E_STR:
			ret.data = strdup(this->data);
			break;
		case E_INT:
			c = ((M_TAG *) this->data)->c;
			ret.data = malloc(sizeof(M_TAG));
			((M_TAG *) ret.data)->c = c;
			((M_TAG *) ret.data)->path = malloc(c * sizeof(int));
			memcpy(((M_TAG *) ret.data)->path, ((M_TAG *) this->data)->path, c * sizeof(int));
			break;
	}
	
	return ret;
}

void E_N_dispose(E_N *this)
{
	switch(this->type)
	{
		case E_INT:
			free(((M_TAG *) this->data)->path);
		case E_INS:
		case E_STR:
		case E_BOOL:
			free(this->data);
			break;
	}
	
	this->type = E_NONE;
	this->data = NULL;
}


