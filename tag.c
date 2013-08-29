#include "tag.h"

void TAG_init(TAG *this)
{
	memset(this->name, 0, TAG_NAME_SIZE);
	this->favs = 0;
	this->tags = NULL;
	this->tc   = 0;
}

int TAG_findTag(TAG *this, TAG t)
{
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		if(strcmp(this->tags[i].name, t.name) == 0)
		{
			return i;
		}
	}
	
	return -1;
}

void TAG_read(TAG *this, FILE *f)
{
	fread(this, sizeof(TAG), 1, f);
	
	this->tags = this->tc > 0 ? malloc(this->tc * sizeof(TAG)) : NULL;
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		TAG_read(&this->tags[i], f);
	}
}

void TAG_write(TAG *this, FILE *f)
{
	fwrite(this, sizeof(TAG), 1, f);
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		TAG_write(&this->tags[i], f);
	}
}

void TAG_addTag(TAG *this, const char *path, int override)
{
	while(*path == '.') path++;
	
	if(*path == '\0') return;
	
	TAG t;
	TAG_init(&t);
	strcpyv(t.name, path, '.');
	path += strlen(t.name);
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		if(strcmp(this->tags[i].name, t.name) == 0)
		{
			TAG_addTag(&this->tags[i], path, override);
			return;
		}
	}
	
	if(*path != '\0')
	{
		if(override)
		{
			TAG_addTag(&t, path, override);
		}
		else
		{
			return;
		}
	}
	
	this->tags = realloc(this->tags, ++this->tc * sizeof(TAG));
	this->tags[this->tc - 1] = t;
}

int TAG_removeTag(TAG *this, const char *path, int override)
{
	while(*path == '.') path++;
	
	if(*path == '\0') return 1;
	
	char buf[TAG_NAME_SIZE];
	strcpyv(buf, path, '.');
	path += strlen(buf);
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		if(strcmp(this->tags[i].name, buf) == 0)
		{
			if(*path == '\0')
			{
				if(this->tags[i].tc == 0 || override)
				{
					TAG_dispose(&this->tags[i]);
					if(--this->tc - i > 0)
					{
						memmove(this->tags + i, this->tags + i + 1, (this->tc - i) * sizeof(TAG));
					}
					
					if(this->tc > 0)
					{
						this->tags = realloc(this->tags, this->tc * sizeof(TAG));
					}
					else
					{
						free(this->tags);
						this->tags = NULL;
					}
					
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return TAG_removeTag(&this->tags[i], path, override);
			}
		}
	}
	
	return -1;
}

void TAG_listTags(TAG *this, int indent)
{
	int i;
	for(i = 0 ; i < indent ; i++) printf(" ");
	
	printf("- %s (%d)\n", this->name, this->favs);
	
	for(i = 0 ; i < this->tc ; i++)
	{
		TAG_listTags(&this->tags[i], indent + 2);
	}
}

void TAG_dispose(TAG *this)
{
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		TAG_dispose(&this->tags[i]);
	}
	
	free(this->tags);
	
	TAG_init(this);
}

M_TAG *TAG_evaluateAbs(TAG *this, const char *str, M_TAG *tag)
{
	while(*str == '.') str++;
	
	if(*str == '\0') return tag;
	
	if(tag == NULL)
	{
		tag = malloc(sizeof(M_TAG));
		tag->c = 0;
		tag->path = NULL;
	}
	
	char buf[TAG_NAME_SIZE];
	strcpyv(buf, str, '.');
	str += strlen(buf);
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		if(strcmp(this->tags[i].name, buf) == 0)
		{
			tag->path = realloc(tag->path, ++tag->c * sizeof(int));
			tag->path[tag->c - 1] = i;
			
			return TAG_evaluateAbs(&this->tags[i], str, tag);
		}
	}
	
	free(tag->path);
	free(tag);
	
	return NULL;
}

M_TAG *TAG_evaluatePart(TAG *this, const char *str, M_TAG *tags, int *l)
{
	if(*str == '.')
	{
		M_TAG *tag = TAG_evaluateAbs(this, str, NULL);
		
		if(tag != NULL)
		{
			tags = realloc(tags, ++(*l) * sizeof(M_TAG));
			tags[*l - 1] = *tag;
			free(tag);
		}
		
		return tags;
	}
	
	char buf[TAG_NAME_SIZE];
	strcpyv(buf, str, '.');
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		if(strcmp(this->tags[i].name, buf) == 0)
		{
			tags = TAG_evaluatePart(&this->tags[i], str + strlen(buf), tags, l);
		}
		
		tags = TAG_evaluatePart(&this->tags[i], str, tags, l);
	}
	
	return tags;
}

