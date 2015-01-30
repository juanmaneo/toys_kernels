#ifndef __TL_STRING_H
#define	__TL_STRING_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <_size_t.h>
#include <_null.h>

void *memcpy(void *dst_ptr, const void *src_ptr, size_t count);
void *memsetw(void *dst, int val, size_t count);
size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif

#endif

