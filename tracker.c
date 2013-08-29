#include "tracker.h"

void I_M_init(I_M*);
void I_M_fav(I_M*, TAG *);
void I_M_reconstruct(I_M*, TAG*);
void I_M_list(I_M*, TAG *);
void I_M_read(I_M*, FILE *);
void I_M_write(I_M*, FILE *);
void I_M_dispose(I_M*);

int TRACKER_getNewMetaIndex(TRACKER*);

void TRACKER_init(TRACKER *this)
{
	this->mc      = 0;
	this->meta    = NULL;
	this->profile = NULL;
	this->autoC   = 0;
	
	TAG_init(&this->root);
	this->root.name[0] = '.';
}

void TRACKER_import(TRACKER *this, const char *source_dir, const char *target_dir)
{
	struct dirent *dp;
	DIR *dfd;

 	if((dfd = opendir(source_dir)) == NULL)
	{
		fprintf(stderr, "ERR: Can't open %s\n", source_dir);
		exit(1);
	}
	
	char filename_old[128], filename_new[128];
	
	printf("Import started ...\n");

	while((dp = readdir(dfd)) != NULL)
	{
		struct stat stbuf;
		
		sprintf(filename_old, "%s/%s", source_dir, dp->d_name);
		
		if(stat(filename_old, &stbuf) == -1)
		{
			printf("Unable to stat file: %s\n", filename_old);
			continue;
		}

		if((stbuf.st_mode & S_IFMT) == S_IFDIR) // if is directory
		{
			continue;
		}
		else
		{
			int m = TRACKER_getNewMetaIndex(this);
			sprintf(this->meta[m].name, "%d%s", m, findLast(filename_old, '.'));
			
			sprintf(filename_new, "%s/%d%s", target_dir, m, findLast(filename_old, '.'));
			copyFile(filename_old, filename_new);
			remove(filename_old);
			
			printf("> Moved '%s' to '%s'.\n", filename_old, filename_new);
		}
	}
}

void TRACKER_reimport(TRACKER *this, const char *dir)
{
	struct dirent *dp;
	DIR *dfd;

 	if((dfd = opendir(dir)) == NULL)
	{
		fprintf(stderr, "ERR: Can't open %s\n", dir);
		exit(1);
	}
	
	char filename[128];
	
	printf("Import started ...\n");

	while((dp = readdir(dfd)) != NULL)
	{
		struct stat stbuf;
		
		sprintf(filename, "%s/%s", dir, dp->d_name);
		
		if(stat(filename, &stbuf) == -1)
		{
			printf("Unable to stat file: %s\n", filename);
			continue;
		}

		if((stbuf.st_mode & S_IFMT) == S_IFDIR) // if is directory
		{
			continue;
		}
		else
		{
			int m = -1;
			strcpy(filename, dp->d_name);
			sscanf(filename, "%i", &m);
			
			if(m < 0)
			{
				fprintf(stderr, "ERR: Imvalid filename for reimport: '%s'.\nAbort.\n", filename);
				exit(1);
			}
			
			if(m >= this->mc)
			{
				int i = this->mc;
				this->mc = m + 1;
				this->meta = realloc(this->meta, this->mc * sizeof(I_M));
				
				for(; i < this->mc ; i++)
				{
					I_M_init(&this->meta[i]);
				}
			}
			
			strcpy(this->meta[m].name, filename);
			
			printf("> Reimported '%s' as image #%d.\n", filename, m);
		}
	}
}

void TRACKER_addTag(TRACKER *this, const char *tag)
{
	TAG_addTag(&this->root, tag, this->autoC);
}

void TRACKER_tagImage(TRACKER *this, int image, const char *tag)
{
	M_TAG *t = TAG_evaluateAbs(&this->root, tag, NULL);
	
	if(t == NULL)
	{
		fprintf(stderr, "ERR: No tag found for '%s'\nAbort.\n", tag);
		return;
	}
	
	I_M *m = &this->meta[image];
	m->paths = realloc(m->paths, ++m->tc * sizeof(char *));
	m->tags  = realloc(m->tags,    m->tc * sizeof(M_TAG));
	m->paths[m->tc - 1] = strdup(tag);
	
	m->tags[m->tc - 1] = *t;
	free(t);
}

void TRACKER_favImage(TRACKER *this, int image)
{
	I_M_fav(&this->meta[image], &this->root);
}

void TRACKER_listTags(TRACKER *this)
{
	printf("> Tags:\n");
	TAG_listTags(&this->root, 2);
	printf("\n");
}

void TRACKER_listImages(TRACKER *this)
{
	int i;
	for(i = 0 ; i < this->mc ; i++)
	{
		I_M_list(&this->meta[i], &this->root);
	}
}

void TRACKER_read(TRACKER *this, const char *fn)
{
	this->profile = strdup(fn);
	
	FILE *read = fopen(fn, "rb");
	
	if(read == NULL)
	{
		TRACKER_flush(this);
		
		return;
	}
		
	TAG_read(&this->root, read);
	
	fread(&this->mc, sizeof(int), 1, read);
	
	this->meta = this->mc > 0 ? malloc(this->mc * sizeof(I_M)) : NULL;
	
	int i;
	for(i = 0 ; i < this->mc ; i++)
	{
		I_M_read(&this->meta[i], read);
		I_M_reconstruct(&this->meta[i], &this->root);
	}
	
	fclose(read);
}

void TRACKER_flush(TRACKER *this)
{
	FILE *write = fopen(this->profile, "wb");
	
	if(write == NULL)
	{
		fprintf(stderr, "ERR: Couldn't write to file '%s'.\nAbort.\n", this->profile);
		exit(1);
	}
		
	TAG_write(&this->root, write);

	fwrite(&this->mc, sizeof(int), 1, write);
	
	int i;
	for(i = 0 ; i < this->mc ; i++)
	{
		I_M_write(&this->meta[i], write);
	}
	
	fclose(write);
}

void TRACKER_dispose(TRACKER *this)
{
	TAG_dispose(&this->root);
	
	int i;
	for(i = 0 ; i < this->mc ; i++)
	{
		I_M_dispose(&this->meta[i]);
	}
	
	free(this->profile);
	
	TRACKER_init(this);
}

// # --------------------------------------------------------------------------

int TRACKER_getNewMetaIndex(TRACKER *this)
{
	int i;
	for(i = 0 ; i < this->mc ; i++)
	{
		if(this->meta[i].valid != 1)
		{
			return i;
		}
	}
	
	this->meta = realloc(this->meta, ++this->mc * sizeof(I_M));
	I_M_init(&this->meta[this->mc - 1]);
	
	return this->mc - 1;
}

// # ==========================================================================

void I_M_init(I_M *this)
{
	memset(this->name, '\0', META_NAME_SIZE);
	this->valid = 1;
	this->tc    = 0;
	this->tags  = NULL;
	this->paths = NULL;
}

void I_M_fav(I_M *this, TAG *root)
{
	int i, j;
	for(i = 0 ; i < this->tc ; i++)
	{
		TAG *tag = root;
		M_TAG *m = &this->tags[i];
		
		for(j = 0 ; j < m->c ; j++)
		{
			tag = &tag->tags[m->path[j]];
			tag->favs++;
		}
	}
}

void I_M_reconstruct(I_M *this, TAG *tag)
{
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		M_TAG *t = TAG_evaluateAbs(tag, this->paths[i], NULL);
		if(t == NULL) continue;
		this->tags[i] = *t;
		free(t);
	}
}

void I_M_printList(M_TAG *this, TAG *root)
{
	int i;
	for(i = 0 ; i < this->c ; i++)
	{
		root = &root->tags[this->path[i]];
		printf(".%s", root->name);
	}
}

void I_M_list(I_M *this, TAG *root)
{
	printf("Image '%s':\n", this->name);
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		printf("  - ");
		I_M_printList(&this->tags[i], root);
		printf("\n");
	}
	
	printf("\n");
}

void I_M_read(I_M *this, FILE *f)
{
	fread(this, sizeof(I_M), 1, f);
	
	this->tags  = this->tc > 0 ? malloc(this->tc * sizeof(M_TAG))  : NULL;
	this->paths = this->tc > 0 ? malloc(this->tc * sizeof(char *)) : NULL;
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		int l;
		fread(&l, sizeof(int), 1, f);
		
		this->paths[i] = l > 0 ? malloc(l + 1) : NULL;
		
		if(l > 0)
		{
			fread(this->paths[i], 1, l, f);
			this->paths[i][l] = '\0';
		}
	}
}

void I_M_write(I_M *this, FILE *f)
{
	fwrite(this, sizeof(I_M), 1, f);
	
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		int l = strlen(this->paths[i]);
		fwrite(&l, sizeof(int), 1, f);
		
		if(l > 0)
		{
			fwrite(this->paths[i], 1, l, f);
		}
	}
}

void I_M_dispose(I_M *this)
{
	int i;
	for(i = 0 ; i < this->tc ; i++)
	{
		free(this->tags[i].path);
		free(this->paths[i]);
	}
	
	free(this->tags);
	free(this->paths);
	
	I_M_init(this);
}

