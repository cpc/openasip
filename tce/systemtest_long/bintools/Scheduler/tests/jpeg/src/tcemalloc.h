#ifndef TCE_LIB_HH
#define TCE_LIB_HH

#include <sys/types.h>

void* malloc(size_t nbytes);
void* calloc(size_t num, size_t size);
void* realloc(void* cp, size_t nbytes);
void free(void* cp);

#endif

