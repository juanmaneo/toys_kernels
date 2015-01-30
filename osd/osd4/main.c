/*============================================================================
STRING.H C LIBRARY FUNCTIONS
============================================================================*/
#define NULL 0

typedef unsigned size_t;
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
/*****************************************************************************
*****************************************************************************/
void *memsetw(void *dst, int val, size_t count)
{
	unsigned short *temp = (unsigned short *)dst;

	for( ; count != 0; count--)
		*temp++ = val;
	return dst;
}
/*****************************************************************************
*****************************************************************************/
size_t strlen(const char *str)
{
	size_t ret_val;

	for(ret_val = 0; *str != '\0'; str++)
		ret_val++;
	return ret_val;
}
/*============================================================================
STDARG.H C LIBRARY FUNCTIONS
============================================================================*/
/* width of stack == width of int */
#define	STACKITEM	int

typedef unsigned char *va_list;

/* round up width of objects pushed on stack. The expression before the
& ensures that we get 0 for objects of size 0. */
#define	VA_SIZE(TYPE)					\
	((sizeof(TYPE) + sizeof(STACKITEM) - 1)	\
		& ~(sizeof(STACKITEM) - 1))

/* &(LASTARG) points to the LEFTMOST argument of the function call
(before the ...) */
#define	va_start(AP, LASTARG)	\
	(AP=((va_list)&(LASTARG) + VA_SIZE(LASTARG)))

#define va_end(AP)	/* nothing */

#define va_arg(AP, TYPE)	\
	(AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))
/*============================================================================
STDIO.H C LIBRARY FUNCTIONS
============================================================================*/
/*****************************************************************************
name:	do_printf
action:	minimal subfunction for ?printf, calls function
	'fn' with arg 'ptr' for each character to be output
returns:total number of characters output
notes:	does not handle long long (64-bit) values, far pointers, floats,
	precision part of field width, leading sign, or leading blanks.
*****************************************************************************/
/* flags used in processing format string */
#define		PR_LJ	0x01	/* left justify */
#define		PR_CA	0x02	/* use A-F instead of a-f for hex */
#define		PR_SG	0x04	/* signed numeric conversion (%d vs. %u) */
#define		PR_32	0x08	/* long (32-bit) numeric conversion */
#define		PR_16	0x10	/* short (16-bit) numeric conversion */
#define		PR_WS	0x20	/* PR_SG set and num was < 0 */
#define		PR_LZ	0x40	/* pad left with '0' instead of ' ' */
#define		PR_FP	0x80	/* pointers are far */

/* largest number handled is 2^32-1, lowest radix handled is 8.
2^32-1 in base 8 has 11 digits (add 5 for trailing NUL and for slop) */
#define		PR_BUFLEN	16

typedef int (*fnptr_t)(unsigned c, void **helper);

int do_printf(const char *fmt, va_list args, fnptr_t fn, void *ptr)
{
	unsigned state, flags, radix, actual_wd, count, given_wd;
	char *where, buf[PR_BUFLEN];
	long num;

	state = flags = count = given_wd = 0;
/* begin scanning format specifier list */
	for(; *fmt; fmt++)
	{
		switch(state)
		{
/* STATE 0: AWAITING % */
		case 0:
			if(*fmt != '%')	/* not %... */
			{
				fn(*fmt, &ptr);	/* ...just echo it */
				count++;
				break;
			}
/* found %, get next char and advance state to check if next char is a flag */
			state++;
			fmt++;
			/* FALL THROUGH */
/* STATE 1: AWAITING FLAGS (%-0) */
		case 1:
			if(*fmt == '%')	/* %% */
			{
				fn(*fmt, &ptr);
				count++;
				state = flags = given_wd = 0;
				break;
			}
			if(*fmt == '-')
			{
				if(flags & PR_LJ)/* %-- is illegal */
					state = flags = given_wd = 0;
				else
					flags |= PR_LJ;
				break;
			}
/* not a flag char: advance state to check if it's field width */
			state++;
/* check now for '%0...' */
			if(*fmt == '0')
			{
				flags |= PR_LZ;
				fmt++;
			}
			/* FALL THROUGH */
/* STATE 2: AWAITING (NUMERIC) FIELD WIDTH */
		case 2:
			if(*fmt >= '0' && *fmt <= '9')
			{
				given_wd = 10 * given_wd +
					(*fmt - '0');
				break;
			}
/* not field width: advance state to check if it's a modifier */
			state++;
			/* FALL THROUGH */
/* STATE 3: AWAITING MODIFIER CHARS (FNlh) */
		case 3:
			if(*fmt == 'F')
			{
				flags |= PR_FP;
				break;
			}
			if(*fmt == 'N')
				break;
			if(*fmt == 'l')
			{
				flags |= PR_32;
				break;
			}
			if(*fmt == 'h')
			{
				flags |= PR_16;
				break;
			}
/* not modifier: advance state to check if it's a conversion char */
			state++;
			/* FALL THROUGH */
/* STATE 4: AWAITING CONVERSION CHARS (Xxpndiuocs) */
		case 4:
			where = buf + PR_BUFLEN - 1;
			*where = '\0';
			switch(*fmt)
			{
			case 'X':
				flags |= PR_CA;
				/* FALL THROUGH */
/* xxx - far pointers (%Fp, %Fn) not yet supported */
			case 'x':
			case 'p':
			case 'n':
				radix = 16;
				goto DO_NUM;
			case 'd':
			case 'i':
				flags |= PR_SG;
				/* FALL THROUGH */
			case 'u':
				radix = 10;
				goto DO_NUM;
			case 'o':
				radix = 8;
/* load the value to be printed. l=long=32 bits: */
DO_NUM:				if(flags & PR_32)
					num = va_arg(args, unsigned long);
/* h=short=16 bits (signed or unsigned) */
				else if(flags & PR_16)
				{
					if(flags & PR_SG)
						num = va_arg(args, short);
					else
						num = va_arg(args, unsigned short);
				}
/* no h nor l: sizeof(int) bits (signed or unsigned) */
				else
				{
					if(flags & PR_SG)
						num = va_arg(args, int);
					else
						num = va_arg(args, unsigned int);
				}
/* take care of sign */
				if(flags & PR_SG)
				{
					if(num < 0)
					{
						flags |= PR_WS;
						num = -num;
					}
				}
/* convert binary to octal/decimal/hex ASCII
OK, I found my mistake. The math here is _always_ unsigned */
				do
				{
					unsigned long temp;

					temp = (unsigned long)num % radix;
					where--;
					if(temp < 10)
						*where = temp + '0';
					else if(flags & PR_CA)
						*where = temp - 10 + 'A';
					else
						*where = temp - 10 + 'a';
					num = (unsigned long)num / radix;
				}
				while(num != 0);
				goto EMIT;
			case 'c':
/* disallow pad-left-with-zeroes for %c */
				flags &= ~PR_LZ;
				where--;
				*where = (char)va_arg(args, char);
				actual_wd = 1;
				goto EMIT2;
			case 's':
/* disallow pad-left-with-zeroes for %s */
				flags &= ~PR_LZ;
				where = va_arg(args, char *);
EMIT:
				actual_wd = strlen(where);
				if(flags & PR_WS)
					actual_wd++;
/* if we pad left with ZEROES, do the sign now */
				if((flags & (PR_WS | PR_LZ)) ==
					(PR_WS | PR_LZ))
				{
					fn('-', &ptr);
					count++;
				}
/* pad on left with spaces or zeroes (for right justify) */
EMIT2:				if((flags & PR_LJ) == 0)
				{
					while(given_wd > actual_wd)
					{
						fn(flags & PR_LZ ? '0' :
							' ', &ptr);
						count++;
						given_wd--;
					}
				}
/* if we pad left with SPACES, do the sign now */
				if((flags & (PR_WS | PR_LZ)) == PR_WS)
				{
					fn('-', &ptr);
					count++;
				}
/* emit string/char/converted number */
				while(*where != '\0')
				{
					fn(*where++, &ptr);
					count++;
				}
/* pad on right with spaces (for left justify) */
				if(given_wd < actual_wd)
					given_wd = 0;
				else given_wd -= actual_wd;
				for(; given_wd; given_wd--)
				{
					fn(' ', &ptr);
					count++;
				}
				break;
			default:
				break;
			}
		default:
			state = flags = given_wd = 0;
			break;
		}
	}
	return count;
}
/*============================================================================
X86.H C LIBRARY FUNCTIONS
============================================================================*/
/*****************************************************************************
*****************************************************************************/
__inline__ unsigned inportb(unsigned port)
{
	unsigned ret_val;

	__asm__ __volatile__("inb %w1,%b0"
		: "=a"(ret_val)
		: "d"(port));
	return ret_val;
}
/*****************************************************************************
*****************************************************************************/
__inline__ void outportb(unsigned port, unsigned val)
{
	__asm__ __volatile__("outb %b0,%w1"
		:
		: "a"(val), "d"(port));
}
/*============================================================================
TEXT VIDEO ROUTINES

EXPORTS:
void putch(unsigned char c);
void init_video(void);
============================================================================*/
//#include <string.h> /* memcpy(), memsetw() */
//#include <x86.h> /* outportb(), inportb() */

#define	VGA_MISC_READ	0x3CC

static unsigned short *_vga_fb_adr;
static unsigned _crtc_io_adr, _attrib, _csr_x, _csr_y, _vc_width, _vc_height;
/*****************************************************************************
*****************************************************************************/
static void scroll(void)
{
	unsigned blank, temp;

	blank = 0x20 | (_attrib << 8);
/* scroll up */
	if(_csr_y >= _vc_height)
	{
		temp = _csr_y - _vc_height + 1;
		memcpy(_vga_fb_adr,	_vga_fb_adr + temp * _vc_width,
			(_vc_height - temp) * _vc_width);
/* blank the bottom line of the screen */
		memsetw(_vga_fb_adr + (_vc_height - temp) * _vc_width,
			blank, _vc_width);
		_csr_y = _vc_height - 1;
	}
}
/*****************************************************************************
*****************************************************************************/
static void move_csr(void)
{
	unsigned temp;

	temp = _csr_y * _vc_width + _csr_x;
	outportb(_crtc_io_adr + 0, 14);
	outportb(_crtc_io_adr + 1, temp >> 8);
	outportb(_crtc_io_adr + 0, 15);
	outportb(_crtc_io_adr + 1, temp);
}
/*****************************************************************************
*****************************************************************************/
void putch(unsigned c)
{
	unsigned att;

	att = _attrib << 8;
/* backspace */
	if(c == 0x08)
	{
		if(_csr_x != 0)
			_csr_x--;
	}
/* tab */
	else if(c == 0x09)
		_csr_x = (_csr_x + 8) & ~(8 - 1);
/* carriage return */
	else if(c == '\r')	/* 0x0D */
		_csr_x = 0;
/* line feed */
//	else if(c == '\n')	/* 0x0A */
//		_csr_y++;
/* CR/LF */
	else if(c == '\n')	/* ### - 0x0A again */
	{
		_csr_x = 0;
		_csr_y++;
	}
/* printable ASCII */
	else if(c >= ' ')
	{
		unsigned short *where;

		where = _vga_fb_adr + (_csr_y * _vc_width + _csr_x);
		*where = c | att;
		_csr_x++;
	}
	if(_csr_x >= _vc_width)
	{
		_csr_x = 0;
		_csr_y++;
	}
	scroll();
	move_csr();
}
/*****************************************************************************
*****************************************************************************/
void init_video(void)
{
/* check for monochrome or color VGA emulation */
	if((inportb(VGA_MISC_READ) & 0x01) != 0)
	{
		_vga_fb_adr = (unsigned short *)0xB8000L;
		_crtc_io_adr = 0x3D4;
	}
	else
	{
		_vga_fb_adr = (unsigned short *)0xB0000L;
		_crtc_io_adr = 0x3B4;
	}
	_attrib = 0x30; /* black on cyan */
	_vc_width = 80;
	_vc_height = 25;
}
/*============================================================================
MAIN KERNEL CODE

EXPORTS:
extern unsigned long _virt_to_phys;

void kprintf(const char *fmt, ...);
int main(void);
============================================================================*/
//#include <_printf.h> /* do_printf() */
//#include <stdarg.h> /* va_list, va_start(), va_end() */

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

	kprintf("Hello from OSD 4\n");
	years = -7;
	kprintf("%u score and %i years ago...\n", score, -years);
	kprintf("-1L == 0x%lX == octal %lo\n", -1L, -1L);
	kprintf("<%-08s> and <%08s> justified strings\n", "left", "right");
	kprintf("Goodbye. Use reset button to end\n");
/* return to KSTART.ASM, which will freeze */
	return 0;
}

