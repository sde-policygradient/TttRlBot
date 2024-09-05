#ifndef FILECHECK_H
#define FILECHECK_H

#include<stdio.h>
#include<stdbool.h>

#if defined(_WIN32) || defined(_WIN64)

#include<windows.h>
#include<direct.h>

#else

#include<unistd.h>
#include<sys/stat.h>

#endif

bool dir_exists(const char *path);

bool file_exists(const char *path);

#endif