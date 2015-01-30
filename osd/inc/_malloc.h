#ifndef __TL__MALLOC_H
#define	__TL__MALLOC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <_size_t.h>
#include <_null.h>

#define	MALLOC_MAGIC	0x6D92	/* must be < 0x8000 */

typedef struct _malloc		/* Turbo C	DJGPP */
{
	size_t size;		/* 2 bytes	 4 bytes */
	struct _malloc *next;	/* 2 bytes	 4 bytes */
	unsigned magic : 15;	/* 2 bytes total 4 bytes total */
	unsigned used : 1;
} malloc_t;		/* total   6 bytes	12 bytes */

extern malloc_t *_heap_bot, *_heap_top;

#ifdef __cplusplus
}
#endif

#endif
