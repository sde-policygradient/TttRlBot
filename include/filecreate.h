#ifndef FILECREATE_H
#define FILECREATE_H

#if defined(_WIN32) || defined(_WIN64)

#include<direct.h>

#define MKDIR(x) _mkdir(x)

#else

#include<sys/stat.h>

#define MKDIR(x) mkdir(x, S_IRWXU)

#endif

#endif