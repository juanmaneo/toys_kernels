/*============================================================================
MAIN KERNEL CODE

EXPORTS:
void kprintf(const char *fmt, ...);
int main(void);
============================================================================*/
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <string.h> /* NULL */
#include <x86.h> /* disable() */
#include <_printf.h> /* do_printf() */
#include "_krnl.h" /* regs_t */

/* IMPORTS
from KSTART.ASM */
void getvect(vector_t *v, unsigned vect_num);
void setvect(vector_t *v, unsigned vect_num);

/* from VIDEO.C */
extern console_t _vc[];

void blink(void);
void putch(unsigned c);
void init_video(void);

/* from KBD.C */
void keyboard_irq(void);
void init_keyboard(void);

/* from SCHED.C */
void schedule(void);
void init_tasks(void);

/* from DEBUG.C */
void dump_regs(regs_t *regs);
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
void fault(regs_t *regs)
{
	static const char * const msg[] =
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
/**/

	switch(regs->which_int)
	{
/* this handler installed at compile-time
Keyboard handler is installed at run-time (see below) */
	case 0x20:	/* timer IRQ 0 */
		blink();
/* reset hardware interrupt at 8259 chip */
		outportb(0x20, 0x20);
		break;
	default:
		kprintf("Exception #%u", regs->which_int);
		if(regs->which_int <= sizeof(msg) / sizeof(msg[0]))
			kprintf(" (%s)", msg[regs->which_int]);
		kprintf("\n");
		dump_regs(regs);
		panic("Goodbye (system halted, use reset button to end)");
	}
}
/*****************************************************************************
*****************************************************************************/
static void init_8259s(void)
{
	static const unsigned irq0_int = 0x20, irq8_int = 0x28;
/**/

/* Initialization Control Word #1 (ICW1) */
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
/* ICW2:
route IRQs 0-7 to INTs 20h-27h */
	outportb(0x21, irq0_int);
/* route IRQs 8-15 to INTs 28h-2Fh */
	outportb(0xA1, irq8_int);
/* ICW3 */
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
/* ICW4 */
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
/* enable IRQ0 (timer) and IRQ1 (keyboard) */
	outportb(0x21, ~0x03);
	outportb(0xA1, ~0x00);
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
	vector_t v;

	init_video();
	kprintf("Hello from OSD 9\n");
	init_keyboard();
	init_8259s();

	kprintf("Installing keyboard interrupt handler...\n");
/* we don't save the old vector */
	v.eip = (unsigned)keyboard_irq;
	v.access_byte = 0x8E; /* present, ring 0, '386 interrupt gate */
	setvect(&v, 0x21);

	init_tasks();

	kprintf("Enabling hardware interrupts...\n");
	enable();
	kprintf("* Type some text\n");
	kprintf("* Press F1, F2, etc. to change virtual console\n");
	kprintf("* Press Ctrl+Alt+Del to reboot\n");
/* idle task/thread */
	while(1)
	{
		schedule();
	}
	return 0;
}
