#include "arguments.h"

void ARG_init(ARG *this)
{
	this->c = 0;
	this->args = NULL;
	this->vals = NULL;
}

int ARG_find(ARG *this, const char *name)
{
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		if(strcmp(this->args[i], name) == 0)
		{
			return i;
		}
	}
	
	return -1;
}

void ARG_set(ARG *this, const char *name, const char *value)
{
	int p = ARG_find(this, name);
	
	if(p < 0)
	{
		this->args = realloc(this->args, ++this->c * sizeof(void *));
		this->vals = realloc(this->vals,   this->c * sizeof(void *));
		
		p = this->c - 1;
		this->args[p] = strdup(name);
	}
	else
	{
		free(this->vals[p]);
	}
	
	this->vals[p] = strdup(value);
}

void ARG_parse(ARG *this, int argc, const char * const *args)
{
	int i;
	for(i = 0 ; i < argc ; i++)
	{
		if(args[i][0] == '-' && args[i][1] == '-')
		{
			printf("Processing argument '%s' ...\n", args[i]);
			char name[256], value[256];
			
			strcpyv(name, args[i] + 2, '=');
			printf("\t> name:  '%s'\n", name);
			
			if(*(args[i] + 2 + strlen(name)) == '=')
			{
				strcpy(value, args[i] + 2 + strlen(name) + 1);
			}
			else
			{
				value[0] = '1';
				value[1] = '\0';
			}
			
			printf("\t> value: '%s'\n", value);
			
			ARG_set(this, name, value);
		}
		else
		{
			fprintf(stderr, "ERR: Invalid parameter: '%s'.\nAbort.\n", args[i]);
		}
	}
}

const char *ARG_getArg(ARG *this, const char *name)
{
	int p = ARG_find(this, name);
	
	return p < 0 ? NULL : this->vals[p];
}

void ARG_dispose(ARG *this)
{
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		free(this->args[i]);
		free(this->vals[i]);
	}
	
	free(this->args);
	free(this->vals);
	
	ARG_init(this);
}

