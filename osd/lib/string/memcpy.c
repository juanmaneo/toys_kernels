#include <string.h> /* size_t */
/*****************************************************************************
*****************************************************************************/
void *memcpy(void *dst_ptr, const void *src_ptr, size_t count)
{
	void *ret_val = dst_ptr;
	const char *src = (const char *)src_ptr;
	char *dst = (char *)dst_ptr;

/* copy up */
	for(; count != 0; count--)
		*dst++ = *src++;
	return ret_val;
}
