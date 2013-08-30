#include "include.h"
#include "arguments.h"
#include "tracker.h"

const char * const DEFAULT[] = {"--profile=test"};
const int DEF_C = 1;
const char *A_PROFILE = "profile";
const char *A_IMPORT = "import";
const char *A_REIMPORT = "reimport";
const char *A_AUTOGEN = "auto-generation";
const char *A_SEARCH = "search";
const char *A_LISTT = "list-tags";
const char *A_LISTI = "list-images";
const char *A_NEWTAG = "new-tag";
const char *A_ADDTAG = "add-tag";
const char *A_REMTAG = "remove-tag";
const char *A_DELTAG = "delete-tag";
const char *A_FAV = "favor";
const char *A_OVERRIDE = "override";
const char *A_TAG = "tag";
const char *A_IMAGE = "image";
const char *DIR_IMAGES = "images";
const char *DIR_IMPORT = "import";

int main(int argc, const char * const argv[])
{
	//
	ARG args;
	ARG_init(&args);
	ARG_parse(&args, DEF_C, DEFAULT); 
	ARG_parse(&args, argc - 1, argv + 1);
	
//	printf("Done parsing.\n\n");
	
	const char *path = ARG_getArg(&args, A_PROFILE);
	const char *fn = ".profile";
	
	chdir(path);
	checkNCreateDir(DIR_IMAGES);
	checkNCreateDir(DIR_IMPORT);
	
	TRACKER t;
	TRACKER_init(&t);
	
	TRACKER_read(&t, fn);
	
	#define GETA(a) ARG_getArg(&args, a)
	
//	printf("$$$ Evaluating...\n");
	
	if(GETA(A_IMPORT))
	{
//		printf("$ Trying to import images...\n");
		TRACKER_import(&t, DIR_IMPORT, DIR_IMAGES);
	}
	else if(GETA(A_REIMPORT))
	{
//		printf("$ Trying to reimport images...\n");
		TRACKER_reimport(&t, DIR_IMAGES);
	}
	
	if(GETA(A_SEARCH))
	{
//		printf("$ Trying to search ...\n");
		TRACKER_search(&t, GETA(A_SEARCH));
	}
	else if(GETA(A_LISTT))
	{
//		printf("$ Trying to list tags...\n");
		TRACKER_listTags(&t);
	}
	else if(GETA(A_LISTI))
	{
//		printf("$ Trying to list image meta...\n");
		if(GETA(A_IMAGE))
		{
			int id;
			sscanf(GETA(A_IMAGE), "%i", &id);
			TRACKER_listImage(&t, id);
		}
		else
		{
			TRACKER_listImages(&t);
		}
	}
	else if(GETA(A_FAV))
	{
		int id;
		sscanf(GETA(A_IMAGE), "%i", &id);
//		printf("$ Trying to favor image ...\n");
		TRACKER_favImage(&t, id);
	}
	else if(GETA(A_DELTAG))
	{
		TRACKER_deleteTag(&t, GETA(A_TAG), GETA(A_OVERRIDE) ? 1 : 0);
	}
	else if(GETA(A_ADDTAG))
	{
		int id;
		sscanf(GETA(A_IMAGE), "%i", &id);
//		printf("$ Trying to tag image...\n");
		TRACKER_tagImage(&t, id, GETA(A_TAG));
	}
	else if(GETA(A_REMTAG))
	{
		int id;
		sscanf(GETA(A_IMAGE), "%i", &id);
//		printf("$ Trying to remove tag from image...\n");
		TRACKER_untagImage(&t, id, GETA(A_TAG));
	}
	else if(GETA(A_NEWTAG))
	{
//		printf("$ Trying to add new tag...\n");
		TRACKER_addTag(&t, GETA(A_TAG), GETA(A_AUTOGEN) ? 1 : 0);
	}
	
//	printf("$$$ Done evaluating.\n");
	
	#undef GETA
	
	TRACKER_flush(&t);
	TRACKER_dispose(&t);
	ARG_dispose(&args);
	
	return EXIT_SUCCESS;
}

