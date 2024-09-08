#ifndef CHECKEDFUNC_H
#define CHECKEDFUNC_H

#include<stdio.h>
#include<stdlib.h>

#define MALLOC_ERROR (1 << 0)
#define CALLOC_ERROR (1 << 1)
#define REALLOC_ERROR (1 << 2)
#define FWRITE_ERROR (1 << 3)
#define FREAD_ERROR (1 << 4)
#define FOPEN_ERROR (1 << 5)
#define FCLOSE_ERROR (1 << 6)

#define PRINT_ERROR(condition, error_code, error_value, format, ...) if(1){ \
    if(condition){ \
        fprintf(stderr, format, __VA_ARGS__); \
        *error_code |= error_value; \
    } \
}else NULL

#define CHECKED_MALLOC(ptr, size, type, error_code) if(1){ \
    (ptr) = (type*)malloc((size) * sizeof(type)); \
    \
    PRINT_ERROR(!(ptr), error_code, MALLOC_ERROR, "malloc failed on line %d\n", __LINE__); \
}else NULL

#define CHECKED_CALLOC(ptr, size, type, error_code) if(1){ \
    (ptr) = (type*)calloc((size), sizeof(type)); \
    \
    PRINT_ERROR(!(ptr), error_code, CALLOC_ERROR, "calloc failed on line %d\n", __LINE__); \
}else NULL

#define CHECKED_REALLOC(ptr, size, type, error_code) if(1){ \
    (ptr) = (type*)realloc(ptr, size); \
    \
    PRINT_ERROR(!(ptr), error_code, REALLOC_ERROR, "Realloc failed on line %d\n", __LINE__); \
}

#define CHECKED_FWRITE(ptr, size, count, file, error_code) if(1){ \
    int values_write = fwrite((ptr), (size), (count), (file)); \
    \
    PRINT_ERROR((values_write != (count)), error_code, FWRITE_ERROR, "fwrite failed on line %d\n", __LINE__); \
}else NULL

#define CHECKED_FREAD(ptr, size, count, file, error_code) if(1){ \
    int values_read = fread((ptr), (size), (count), (file)); \
    \
    PRINT_ERROR((values_read != (count)), error_code, FREAD_ERROR, "fread failed on line %d\n", __LINE__); \
}else NULL

#define CHECKED_FOPEN(ptr, file, mode, error_code) if(1){ \
    (ptr) = fopen((file), (mode)); \
    \
    PRINT_ERROR(!(ptr), error_code, FOPEN_ERROR, "fopen failed on line %d\n", __LINE__); \
}else NULL

#define CHECKED_FCLOSE(file, error_code) if(1){ \
    int _ret_val = fclose(file); \
    \
    PRINT_ERROR(_ret_val, error_code, FCLOSE_ERROR, "fclose failed on line %d", __LINE__); \
}else NULL

#endif