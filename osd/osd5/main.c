/*============================================================================
MAIN KERNEL CODE

EXPORTS:
void kprintf(const char *fmt, ...);
int main(void);
============================================================================*/
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <string.h> /* NULL */
#include <_printf.h> /* do_printf() */

/* IMPORTS: */
void putch(unsigned c);
void init_video(void);
/*****************************************************************************
*****************************************************************************/
static int kprintf_help(unsigned c, void **ptr)
{
	putch(c);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
void kprintf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	(void)do_printf(fmt, args, kprintf_help, NULL);
	va_end(args);
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
	int score = 4, years;

	init_video();
	kprintf("Hello from OSD 5\n");
	years = -7;
	kprintf("%u score and %i years ago...\n", score, -years);
	kprintf("-1L == 0x%lX == octal %lo\n", -1L, -1L);
	kprintf("<%-08s> and <%08s> justified strings\n", "left", "right");
	kprintf("Goodbye. Use reset button to end\n");
/* return to KSTART.ASM, which will freeze */
	return 0;
}
