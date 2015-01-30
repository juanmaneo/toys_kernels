#define	VGA_MISC_READ	0x3CC
/*****************************************************************************
*****************************************************************************/
__inline__ unsigned char inportb(unsigned short port)
{
	unsigned char ret_val;

	__asm__ __volatile__("inb %1,%0"
		: "=a"(ret_val)
		: "d"(port));
	return ret_val;
}
/*****************************************************************************
*****************************************************************************/
void *memcpy(void *dst_ptr, const void *src_ptr, unsigned count)
{
	void *ret_val = dst_ptr;
	const char *src = (const char *)src_ptr;
	char *dst = (char *)dst_ptr;

	for(; count != 0; count--)
		*dst++ = *src++;
	return ret_val;
}
/*****************************************************************************
for MinGW32
*****************************************************************************/
#ifdef __WIN32__
#if __GNUC__<3
#error Do not use MinGW GCC 2.x with NASM
#endif
int __main(void) { return 0; }
void _alloca(void) { }
#endif
/*****************************************************************************
*****************************************************************************/
int main(void)
{
	static const char msg1[] = "H e l l o   f r o m   O S D   3 ";
	static const char msg2[] = "G o o d b y e .   "
		"U s e   r e s e t   b u t t o n   t o   e n d ";
/**/
	unsigned vga_fb_adr;

/* check for monochrome or color VGA emulation */
	if((inportb(VGA_MISC_READ) & 0x01) != 0)
		vga_fb_adr = 0xB8000;
	else
		vga_fb_adr = 0xB0000;
/* display message on top line */
	memcpy((char *)(vga_fb_adr + 0), msg1, sizeof(msg1) - 1);
/* display message on second line */
	memcpy((char *)(vga_fb_adr + 160), msg2, sizeof(msg2) - 1);
/* return to KSTART.ASM, which will freeze */
	return 0;
}
