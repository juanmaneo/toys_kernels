/*============================================================================
KEYBOARD ROUTINES

EXPORTS:
void keyboard_irq(void);
void init_keyboard(void);
============================================================================*/
#include <conio.h> /* KEY_nnn */
#include <x86.h> /* outportb(), inportb() */
#include "_krnl.h" /* MAX_VC */

/* IMPORTS
from VIDEO.C */
extern console_t _vc[];

void select_vc(unsigned which_vc);
void putch(unsigned c);

/* from MAIN.C */
void kprintf(const char *fmt, ...);

/* "raw" set 1 scancodes from PC keyboard. Keyboard info here:
http://www.execpc.com/~geezer/osd/kbd */
#define	RAW1_LEFT_CTRL		0x1D
#define	RAW1_RIGHT_CTRL		0x1D	/* same as left */
#define	RAW1_LEFT_SHIFT		0x2A
#define	RAW1_RIGHT_SHIFT	0x36
#define	RAW1_LEFT_ALT		0x38
#define	RAW1_RIGHT_ALT		0x38	/* same as left */
#define	RAW1_CAPS_LOCK		0x3A
#define	RAW1_F1			0x3B
#define	RAW1_F2			0x3C
#define	RAW1_F3			0x3D
#define	RAW1_F4			0x3E
#define	RAW1_F5			0x3F
#define	RAW1_F6			0x40
#define	RAW1_F7			0x41
#define	RAW1_F8			0x42
#define	RAW1_F9			0x43
#define	RAW1_F10		0x44
#define	RAW1_NUM_LOCK		0x45
#define	RAW1_SCROLL_LOCK	0x46
#define	RAW1_F11		0x57
#define	RAW1_F12		0x58

#define	KBD_BUF_SIZE		64
/*****************************************************************************
*****************************************************************************/
static void reboot(void)
{
	unsigned temp;

	disable();
/* flush the keyboard controller */
	do
	{
		temp = inportb(0x64);
		if((temp & 0x01) != 0)
		{
			(void)inportb(0x60);
			continue;
		}
	} while((temp & 0x02) != 0);
/* pulse the CPU reset line */
	outportb(0x64, 0xFE);
/* ...and if that didn't work, just halt */
	while(1)
		/* nothing */;
}
/*****************************************************************************
*****************************************************************************/
static void write_kbd(unsigned adr, unsigned data)
{
	unsigned long timeout;
	unsigned stat;

/* Linux code didn't have a timeout here... */
	for(timeout = 500000L; timeout != 0; timeout--)
	{
		stat = inportb(0x64);
/* loop until 8042 input buffer empty */
		if((stat & 0x02) == 0)
			break;
	}
	if(timeout != 0)
		outportb(adr, data);
}
/*****************************************************************************
*****************************************************************************/
static unsigned convert(unsigned key)
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
	static unsigned short kbd_status, saw_break_code;
/**/
	unsigned short temp;

/* check for break key (i.e. a key is released) */
	if(key >= 0x80)
	{
		saw_break_code = 1;
		key &= 0x7F;
	}
/* the only break codes we're interested in are Shift, Ctrl, Alt */
	if(saw_break_code)
	{
		if(key == RAW1_LEFT_ALT || key == RAW1_RIGHT_ALT)
			kbd_status &= ~KBD_META_ALT;
		else if(key == RAW1_LEFT_CTRL || key == RAW1_RIGHT_CTRL)
			kbd_status &= ~KBD_META_CTRL;
		else if(key == RAW1_LEFT_SHIFT || key == RAW1_RIGHT_SHIFT)
			kbd_status &= ~KBD_META_SHIFT;
		saw_break_code = 0;
		return 0;
	}
/* it's a make key: check the "meta" keys, as above */
	if(key == RAW1_LEFT_ALT || key == RAW1_RIGHT_ALT)
	{
		kbd_status |= KBD_META_ALT;
		return 0;
	}
	if(key == RAW1_LEFT_CTRL || key == RAW1_RIGHT_CTRL)
	{
		kbd_status |= KBD_META_CTRL;
		return 0;
	}
	if(key == RAW1_LEFT_SHIFT || key == RAW1_RIGHT_SHIFT)
	{
		kbd_status |= KBD_META_SHIFT;
		return 0;
	}
/* Scroll Lock, Num Lock, and Caps Lock set the LEDs. These keys
have on-off (toggle or XOR) action, instead of momentary action */
	if(key == RAW1_SCROLL_LOCK)
	{
		kbd_status ^= KBD_META_SCRL;
		goto LEDS;
	}
	if(key == RAW1_NUM_LOCK)
	{
		kbd_status ^= KBD_META_NUM;
		goto LEDS;
	}
	if(key == RAW1_CAPS_LOCK)
	{
		kbd_status ^= KBD_META_CAPS;
LEDS:		write_kbd(0x60, 0xED);	/* "set LEDs" command */
		temp = 0;
		if(kbd_status & KBD_META_SCRL)
			temp |= 1;
		if(kbd_status & KBD_META_NUM)
			temp |= 2;
		if(kbd_status & KBD_META_CAPS)
			temp |= 4;
		write_kbd(0x60, temp);	/* bottom 3 bits set LEDs */
		return 0;
	}
/* ignore invalid scan codes */
	if(key >= sizeof(set1_map) / sizeof(set1_map[0]))
		return 0;
/* convert raw scancode in key to unshifted ASCII in temp */
	temp = set1_map[key];
/* defective keyboard? non-US keyboard? more than 104 keys? */
	if(temp == 0)
		return temp;
/* handle the three-finger salute */
	if((kbd_status & KBD_META_CTRL) && (kbd_status & KBD_META_ALT) &&
		(temp == KEY_DEL))
	{
		kprintf("\n""\x1B[42;37;1m""*** rebooting!");
		reboot();
	}
/* I really don't know what to do yet with Alt, Ctrl, etc. -- punt */
	return temp;
}
/*****************************************************************************
*****************************************************************************/
void keyboard_irq(void)
{
	unsigned key, i;

/* get scancode from port 0x60 */
	key = inportb(0x60);
/* if it's F1, F2 etc. switch to the appropriate virtual console */
	switch(key)
	{
	case RAW1_F1:
		i = 0;
		goto SWITCH_VC;
	case RAW1_F2:
		i = 1;
		goto SWITCH_VC;
	case RAW1_F3:
		i = 2;
		goto SWITCH_VC;
	case RAW1_F4:
		i = 3;
		goto SWITCH_VC;
	case RAW1_F5:
		i = 4;
		goto SWITCH_VC;
	case RAW1_F6:
		i = 5;
		goto SWITCH_VC;
	case RAW1_F7:
		i = 6;
		goto SWITCH_VC;
	case RAW1_F8:
		i = 7;
		goto SWITCH_VC;
	case RAW1_F9:
		i = 8;
		goto SWITCH_VC;
	case RAW1_F10:
		i = 9;
		goto SWITCH_VC;
	case RAW1_F11:
		i = 10;
		goto SWITCH_VC;
	case RAW1_F12:
		i = 11;
SWITCH_VC:
		select_vc(i);
		break;
	default:
		i = convert(key);
		if(i != 0)
			putch(i);
		break;
	}
/* reset hardware interrupt at 8259 chip */
	outportb(0x20, 0x20);
}
/*****************************************************************************
*****************************************************************************/
void init_keyboard(void)
{
	static unsigned char buffers[KBD_BUF_SIZE * MAX_VC];
/**/
	int i;

	for(i = 0; i < MAX_VC; i++)
	{
		_vc[i].keystrokes.data = buffers + KBD_BUF_SIZE * i;
		_vc[i].keystrokes.size = KBD_BUF_SIZE;
	}
	kprintf("init_kbd: %u buffers, %u bytes each\n",
		MAX_VC, KBD_BUF_SIZE);
}
