/*============================================================================
KEYBOARD ROUTINES

EXPORTS:
void keyboard_irq(void);
============================================================================*/
#include <conio.h> /* KEY_nnn */
#include <x86.h> /* inportb() */

/* IMPORTS: */
void putch(unsigned c);
/*****************************************************************************
*****************************************************************************/
static unsigned convert(unsigned code)
{
	static const unsigned char set1_map[] =
	{
/* 00 */0,	0x1B,	'1',	'2',	'3',	'4',	'5',	'6',
/* 08 */'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
/* 10 */'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
/* 1Dh is left Ctrl */
/* 18 */'o',	'p',	'[',	']',	'\n',	0,	'a',	's',
/* 20 */'d',	'f',	'g',	'h',	'j',	'k',	'l',	';',
/* 2Ah is left Shift */
/* 28 */'\'',	'`',	0,	'\\',	'z',	'x',	'c',	'v',
/* 36h is right Shift */
/* 30 */'b',	'n',	'm',	',',	'.',	'/',	0,	0,
/* 38h is left Alt, 3Ah is Caps Lock */
/* 38 */0,	' ',	0,	KEY_F1,	KEY_F2,	KEY_F3,	KEY_F4,	KEY_F5,
/* 45h is Num Lock, 46h is Scroll Lock */
/* 40 */KEY_F6,	KEY_F7,	KEY_F8,	KEY_F9,	KEY_F10,0,	0,	KEY_HOME,
/* 48 */KEY_UP,	KEY_PGUP,'-',	KEY_LFT,'5',	KEY_RT,	'+',	KEY_END,
/* 50 */KEY_DN,	KEY_PGDN,KEY_INS,KEY_DEL,0,	0,	0,	KEY_F11,
/* 58 */KEY_F12
	};
/**/

/* ignore set 1 break codes */
	if(code >= 0x80)
		return 0;
/* ignore invalid scan codes */
	if(code >= sizeof(set1_map) / sizeof(set1_map[0]))
		return 0;
/* convert raw scancode in code to unshifted ASCII */
	return set1_map[code];
}
/*****************************************************************************
*****************************************************************************/
void keyboard_irq(void)
{
	unsigned scan_code, temp;

/* you MUST read port 60h to clear the keyboard interrupt */
	scan_code = inportb(0x60);
	temp = convert(scan_code);
	if(temp != 0)
		putch(temp);
/* reset hardware interrupt at 8259 chip */
	outportb(0x20, 0x20);
}
