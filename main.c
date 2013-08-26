#include "include.h"
#include "arguments.h"
#include "tracker.h"

const char * const DEFAULT[] = {"--profile=."};
const int DEF_C = 1;
const char *A_PROFILE = "profile";
const char *A_IMPORT = "import";
const char *DIR_IMAGES = "images";
const char *DIR_IMPORT = "import";

int main(int argc, const char * const argv[])
{
	//
	ARG args;
	ARG_init(&args);
	ARG_parse(&args, DEF_C, DEFAULT); 
	ARG_parse(&args, argc - 1, argv + 1);
	
	printf("Done parsing.\n");
	
	const char *path = ARG_getArg(&args, A_PROFILE);
	const char *fn = ".profile";
	
	chdir(path);
	checkNCreateDir(DIR_IMAGES);
	checkNCreateDir(DIR_IMPORT);
	
	TRACKER t;
	TRACKER_init(&t);
	
	TRACKER_read(&t, fn);
	
	if(ARG_getArg(&args, A_IMPORT))
	{
		TRACKER_import(&t, DIR_IMPORT);
	}
	
	TRACKER_dispose(&t);
	ARG_dispose(&args);
	
	return EXIT_SUCCESS;
}

