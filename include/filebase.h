#ifndef FILEBASE_H
#define FILEBASE_H

#include<stdio.h>
#include<stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
    #include<windows.h>
    #include<direct.h>

    #define MKDIR(x) _mkdir(x)

    LPCWSTR strtowstr(const char* str);
#else
    #include<unistd.h>
    #include<sys/stat.h>

    #define MKDIR(x) mkdir(x)
#endif

bool dirExists(const char *path);

bool fileExists(const char *path);

#endif