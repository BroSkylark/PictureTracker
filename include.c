#include "include.h"

const char *findLast(const char *s, char v)
{
	int i, j = -1;
	for(i = 0 ; s[i] != '\0' ; i++)
	{
		if(s[i] == v || (j < 0 && s[i] == '\0'))
		{
			j = i;
		}
	}
	
	return s + j;
}

int copyFile(const char *src, const char *trg)
{
	FILE *read = fopen(src, "rb");
	FILE *write = fopen(trg, "wb");
	
	if(read == NULL || write == NULL)
	{
		fprintf(stderr, "ERR: Cannot copy file '%s'.\nAbort.\n", src);
		return -1;
	}
	
	fseek(read, 0, SEEK_END);
	int l = ftell(read);
	fseek(read, 0, SEEK_SET);
	
	char *buf = malloc(l);
	fread(buf, 1, l, read);
	fwrite(buf, 1, l, write);
	
	free(buf);
	fclose(read);
	fclose(write);
	
	return 1;
}

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

