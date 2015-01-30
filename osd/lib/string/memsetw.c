#include <string.h> /* size_t */
/*****************************************************************************
*****************************************************************************/
void *memsetw(void *dst, int val, size_t count)
{
	unsigned short *temp = (unsigned short *)dst;

	for( ; count != 0; count--)
		*temp++ = val;
	return dst;
}
