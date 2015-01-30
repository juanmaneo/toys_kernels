/*============================================================================
MAIN KERNEL CODE

EXPORTS:
void kprintf(const char *fmt, ...);
void panic(const char *fmt, ...);
void fault(regs_t *regs);
int main(void);
============================================================================*/
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <string.h> /* NULL */
#include <x86.h> /* disable() */
#include <_printf.h> /* do_printf() */

/* the code for setvect() and getvect() in
KSTART.ASM depends on the layout of this structure */
typedef struct
{
	unsigned access_byte, eip;
} vector_t;

/* IMPORTS
from KSTART.ASM */
void getvect(vector_t *v, unsigned vect_num);
void setvect(vector_t *v, unsigned vect_num);

/* from VIDEO.C */
void putch(unsigned c);
void init_video(void);

/* from DEBUG.C */
void dump(unsigned char *data, unsigned count);

#define	SYSCALL_INT	0x30

/* the layout of this structure must match the order of registers
pushed and popped by the exception handlers in KSTART.ASM */
typedef struct
{
/* pushed by pusha */
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
/* pushed separately */
	unsigned ds, es, fs, gs;
	unsigned which_int, err_code;
/* pushed by exception. Exception may also push err_code.
user_esp and user_ss are pushed only if a privilege change occurs. */
	unsigned eip, cs, eflags, user_esp, user_ss;
} regs_t;
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
*****************************************************************************/
void panic(const char *fmt, ...)
{
	va_list args;

	disable(); /* interrupts off */
	va_start(args, fmt);
	(void)do_printf(fmt, args, kprintf_help, NULL);
	while(1)
		/* freeze */;
}
/*****************************************************************************
*****************************************************************************/
static const char * const _msg[] =
{
	"divide error", "debug exception", "NMI", "INT3",
	"INTO", "BOUND exception", "invalid opcode", "no coprocessor",
	"double fault", "coprocessor segment overrun",
		"bad TSS", "segment not present",
	"stack fault", "GPF", "page fault", "??",
	"coprocessor error", "alignment check", "??", "??",
	"??", "??", "??", "??",
	"??", "??", "??", "??",
	"??", "??", "??", "??",
	"IRQ0", "IRQ1", "IRQ2", "IRQ3",
	"IRQ4", "IRQ5", "IRQ6", "IRQ7",
	"IRQ8", "IRQ9", "IRQ10", "IRQ11",
	"IRQ12", "IRQ13", "IRQ14", "IRQ15",
	"syscall"
};
/*****************************************************************************
*****************************************************************************/
void fault(regs_t *regs)
{
	unsigned i;

	i = regs->which_int;
	kprintf("*** Asm exception #%X", i);
	if(i <= sizeof(_msg) / sizeof(_msg[0]))
		kprintf(" (%s)", _msg[i]);
	kprintf("\n");
	if(i < 0x20)
		panic("*** Goodbye (system halted, use reset button to end)");
}
/*****************************************************************************
*****************************************************************************/
static void c_fault(regs_t *regs)
{
	unsigned i;

	i = regs->which_int;
	kprintf("*** C exception #%X", i);
	if(i <= sizeof(_msg) / sizeof(_msg[0]))
		kprintf(" (%s)", _msg[i]);
	kprintf("\n");
	if(i < 0x20)
		panic("*** Goodbye (system halted, use reset button to end)");
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
	vector_t old_v, new_v;

	init_video();

	kprintf("Hello from OSD 6\n");
	kprintf("Interrupts have been set up by assembly language code.\n");
	kprintf("Trying INT %02Xh...\n", SYSCALL_INT);
	__asm__ __volatile__("int %0" : : "i"(SYSCALL_INT));

	kprintf("Saving INT %02Xh vector...\n", SYSCALL_INT);
	getvect(&old_v, SYSCALL_INT);
	kprintf("Installing new INT %02Xh handler...\n", SYSCALL_INT);
	new_v.eip = (unsigned)c_fault;
	new_v.access_byte = 0x8E; /* present, ring 0, '386 interrupt gate */
	setvect(&new_v, SYSCALL_INT);
	kprintf("Trying INT %02Xh...\n", SYSCALL_INT);
	__asm__ __volatile__("int %0" : : "i"(SYSCALL_INT));

	kprintf("Restoring old INT %02Xh vector...\n", SYSCALL_INT);
	setvect(&old_v, SYSCALL_INT);
	kprintf("Trying INT %02Xh...\n", SYSCALL_INT);
	__asm__ __volatile__("int %0" : : "i"(SYSCALL_INT));

	kprintf("Trying INT 17h...\n");
	__asm__ __volatile__("int %0" : : "i"(0x17));
/* this is not reached */
	return 0;
}
