#include"filecheck.h"
#include<stdio.h>
#include<stdbool.h>

#if defined(_WIN32) || defined(_WIN64)

#include<windows.h>

static LPCWSTR strtowstr(const char* str){
    int wide_str_size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    WCHAR *wide_str = (WCHAR*)malloc(wide_str_size * sizeof(WCHAR));

    if(wide_str == NULL)return NULL;

    MultiByteToWideChar(CP_ACP, 0, str, -1, wide_str, wide_str_size);

    return (LPCWSTR)wide_str;
}

#else

#include<sys/stat.h>

#endif

bool dir_exists(const char *path){

#if defined(_WIN32) || defined(_WIN64)

    DWORD attributes = GetFileAttributesW(strtowstr(path));

    if(attributes == INVALID_FILE_ATTRIBUTES){
        return false;
    }else if(attributes & FILE_ATTRIBUTE_DIRECTORY){
        return true;
    }

    return false;

#else

    struct stat info;

    if(stat(path, &info)){
        return false;
    }else if(info.st_mode & S_IFDIR){
        return true;
    }

    return false;

#endif

}

bool file_exists(const char *path){
    FILE *file = fopen(path, "r");

    if(file){
        fclose(file);

        return true;
    }

    return false;
}