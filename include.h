#ifndef __INCLUDE_H
#define __INCLUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void strcpyv(char *, const char *, char);
int checkNCreateDir(const char *);
void merge(char *, const char *, const char *);
int copyFile(const char *, const char *);
const char *findLast(const char *, char);

#endif

