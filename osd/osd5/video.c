/*============================================================================
TEXT VIDEO ROUTINES

EXPORTS:
void putch(unsigned char c);
void init_video(void);
============================================================================*/
#include <string.h> /* memcpy(), memsetw() */
#include <x86.h> /* outportb(), inportb() */

/* IMPORTS:
from MAIN.C */
void kprintf(const char *fmt, ...);

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
		memcpy(_vga_fb_adr, _vga_fb_adr + temp * _vc_width,
			(_vc_height - temp) * _vc_width * 2);
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
	_attrib = 0x40; /* black on red */
	_vc_width = 80;
	_vc_height = 25;
	kprintf("  init_video: %s emulation, %u x %u, framebuffer at "
		"0x%lX\n", (_crtc_io_adr == 0x3D4) ? "color" : "mono",
		_vc_width, _vc_height, _vga_fb_adr);
}
