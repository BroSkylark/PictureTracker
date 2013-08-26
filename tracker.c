#include "tracker.h"

int I_M_read(I_M*, FILE *);
int I_M_write(I_M*, FILE *);

void TRACKER_init(TRACKER *this)
{
	this->profile = NULL;
	this->tags    = NULL;
	this->meta    = NULL;
	this->tc      = 0;
	this->mc      = 0;
}

void TRACKER_import(TRACKER *this, const char *dir)
{
 struct dirent *dp;
 DIR *dfd;

 if ((dfd = opendir(dir)) == NULL)
 {
  fprintf(stderr, "Can't open %s\n", dir);
  return 0;
 }

 char filename_qfd[100] ;
 char new_name_qfd[100] ;

 while ((dp = readdir(dfd)) != NULL)
 {
  struct stat stbuf ;
  sprintf( filename_qfd , "%s/%s",dir,dp->d_name) ;
  if( stat(filename_qfd,&stbuf ) == -1 )
  {
   printf("Unable to stat file: %s\n",filename_qfd) ;
   continue ;
  }

  if ( ( stbuf.st_mode & S_IFMT ) == S_IFDIR )
  {
   continue;
   // Skip directories
  }
  else
  {
   char* new_name = get_new_name( dp->d_name ) ;// returns the new string
                                                   // after removing reqd part
   sprintf(new_name_qfd,"%s/%s",dir,new_name) ;
   rename( filename_qfd , new_name_qfd ) ;
  }
 }
}

int TRACKER_read(TRACKER *this, const char *fn)
{
	this->profile = strdup(fn);
	FILE *profile = fopen(this->profile, "rb");
	
	if(profile == NULL)
	{
		return TRACKER_flush(this);
	}
	
	fread(&this->tc, sizeof(int), 1, profile);
	
	if(this->tc > 0)
	{
		this->tags = malloc(this->tc * sizeof(TAG));
		fread(this->tags, sizeof(TAG), this->tc, profile);
	}
	
	fread(&this->mc, sizeof(int), 1, profile);
	
	if(this->mc > 0)
	{
		this->meta = malloc(this->mc * sizeof(I_M));
	
		int i;
		for(i = 0 ; i < this->mc ; i++)
		{
			if(I_M_read(&this->meta[i], profile) < 0)
			{
				fprintf(stderr, "ERR: Invalid meta read (@%d).\nAbort.\n", i);
				fclose(profile);
				exit(1);
			}
		}
	}
	
	fclose(profile);
	
	return 1;
}

int TRACKER_flush(TRACKER *this)
{
	FILE *profile = fopen(this->profile, "wb");
	
	if(profile == NULL) return -1;
	
	fwrite(&this->tc, sizeof(int), 1, profile);
	
	if(this->tc > 0)
	{
		fwrite(this->tags, sizeof(TAG), this->tc, profile);
	}
	
	fwrite(&this->mc, sizeof(int), 1, profile);
	
	if(this->mc > 0)
	{
		int i;
		for(i = 0 ; i < this->mc ; i++)
		{
			if(I_M_write(&this->meta[i], profile) < 0)
			{
				fprintf(stderr, "ERR: Invalid meta write (@%d).\nAbort.\n", i);
				fclose(profile);
				exit(1);
			}
		}
	}
	
	fclose(profile);
	
	return 1;
}

void TRACKER_dispose(TRACKER *this)
{
	int i;
	for(i = 0 ; i < this->mc ; i++)
	{
		free(this->meta[i].tags);
	}
	
	free(this->profile);
	free(this->tags);
	free(this->meta);
}

// # ==========================================================================

int I_M_read(I_M *this, FILE *f)
{
	fread(this, sizeof(I_M), 1, f);
	this->tags = malloc(this->tc * sizeof(struct meta_tag));
	fread(this->tags, sizeof(struct meta_tag), this->tc, f);
	
	return 1;
}

int I_M_write(I_M *this, FILE *f)
{
	struct meta_tag *tmp = this->tags;
	this->tags = NULL;
	
	fwrite(this, sizeof(I_M), 1, f);
	fwrite(tmp, sizeof(struct meta_tag), this->tc, f);
	
	this->tags = tmp;
	
	return 1;
}

