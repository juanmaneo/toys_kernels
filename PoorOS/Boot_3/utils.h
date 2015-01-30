/*--------------------------------------------------------------------------*/

#ifndef UTILS_H
#define UTILS_H 1

#include "platform.h"

void *
memcpy(void * dest,
       const void * src,
       size_t n);

void *
memmove(void * dest,
        const void * src,
        size_t n);

void *
memset(void * dest,
       int c,
       size_t n);

size_t
strlen(const char * s);

char *
strcpy(char * dest,
       const char * src);

char *
strncpy(char * dest,
        const char * src,
        size_t n);

char *
strcat(char * dest,
       const char * src);

char *
strncat(char * dest,
        const char * src,
        size_t n);

int
strcmp(const char * s1,
       const char * s2);

void *
malloc(size_t size);

void
free(void * mem);

void
showAlloc(void);

#endif /* UTILS_H */

/*--------------------------------------------------------------------------*/
