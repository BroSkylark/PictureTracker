#include "include.h"

int checkNCreateDir(const char *dir)
{
	struct stat st = {0};
	
	if(stat(dir, &st) == -1)
	{
		return mkdir(dir, 0777);
	}
	
	return 1;
}

void merge(char *t, const char *s1, const char *s2)
{
	if(s1 == NULL || s2 == NULL)
	{
		fprintf(stderr, "ERR: Empty string merged.\n");
		exit(1);
	}
	
	strcpy(t, s1);
	strcpy(t + strlen(t), s2);
}

void strcpyv(char *target, const char *source, char v)
{
	int i;
	for(i = 0 ; source[i] != v && source[i] != '\0' ; i++)
	{
		target[i] = source[i];
	}
	
	target[i] = '\0';
}

