#ifndef __TL__KRNL_H
#define	__TL__KRNL_H

#ifdef __cplusplus
extern "C"
{
#endif

#define	MAX_VC	12

/* the code for setvect() and getvect() in
KSTART.ASM depends on the layout of this structure */
typedef struct
{
	unsigned access_byte, eip;
} vector_t;

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

typedef struct	/* circular queue */
{
	unsigned char *data;
	unsigned size, in_base, in_ptr/*, out_base*/, out_ptr;
} queue_t;

typedef struct
{
/* virtual console input */
	queue_t keystrokes;
/* virtual console output */
	unsigned esc, attrib, csr_x, csr_y, esc1, esc2, esc3;
	unsigned short *fb_adr;
} console_t;

#ifdef __cplusplus
}
#endif

#endif
