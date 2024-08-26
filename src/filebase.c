#include"filebase.h"
#include<stdio.h>
#include<stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
    #include<windows.h>
    #include<direct.h>

    LPCWSTR strtowstr(const char* str){
        int wideStrSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
        WCHAR *wideStr = (WCHAR*)malloc(wideStrSize * sizeof(WCHAR));

        if(wideStr == NULL)return NULL;

        MultiByteToWideChar(CP_ACP, 0, str, -1, wideStr, wideStrSize);

        return (LPCWSTR)wideStr;
    }
#else
    #include<unistd.h>
    #include<sys/stat.h>
#endif

bool dirExists(const char *path){
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

bool fileExists(const char *path){
    FILE *file = fopen(path, "r");

    if(file){
        fclose(file);

        return true;
    }

    return false;
}