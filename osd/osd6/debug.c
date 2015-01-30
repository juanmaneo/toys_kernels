/*============================================================================
DEBUG FUNCTIONS

EXPORTS:
void dump(unsigned char *data, unsigned count)
============================================================================*/
/* IMPORTS
from MAIN.C */
void kprintf(const char *fmt, ...);
/*****************************************************************************
*****************************************************************************/
#define BPERL		16	/* byte/line for dump */

void dump(unsigned char *data, unsigned count)
{
	unsigned char byte1, byte2;

	while(count != 0)
	{
		for(byte1 = 0; byte1 < BPERL; byte1++)
		{
			if(count == 0)
				break;
			kprintf("%02X ", data[byte1]);
			count--;
		}
		kprintf("\t");
		for(byte2 = 0; byte2 < byte1; byte2++)
		{
			if(data[byte2] < ' ')
				kprintf("%c", '.');
			else
				kprintf("%c", data[byte2]);
		}
		kprintf("\n");
		data += BPERL;
	}
}
