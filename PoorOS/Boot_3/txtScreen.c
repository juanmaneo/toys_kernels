/*--------------------------------------------------------------------------*/

#include "txtScreen.h"
#include "utils.h"
#include "timer.h"

#define TXT_BASE 0x000B8000
#define TXT_WIDTH 80
#define TXT_HEIGHT 25

static dword txtColumn=0;
static dword txtRow=0;
static TxtColor txtColor=TXT_GREEN;
static size_t txtTabSize=8;

void
setTxtColor(TxtColor color)
{
txtColor=color;
}

TxtColor
getTxtColor(void)
{
return txtColor;
}

void
setTxtTabSize(size_t tabSize)
{
if(tabSize&&(tabSize<(TXT_WIDTH>>1)))
  {
  txtTabSize=tabSize;
  }
}

size_t
getTxtTabSize(void)
{
return txtTabSize;
}

void
setTxtPos(dword column,
          dword row)
{
txtColumn=column%TXT_WIDTH;
txtRow=row%TXT_HEIGHT;
}

void
getTxtPos(dword * columnOut,
          dword * rowOut)
{
*columnOut=txtColumn;
*rowOut=txtRow;
}

size_t
getTxtWidth(void)
{
return TXT_WIDTH;
}

size_t
getTxtHeight(void)
{
return TXT_HEIGHT;
}

void
clearScreen(void)
{
memset((byte *)TXT_BASE,0,(TXT_WIDTH*TXT_HEIGHT)<<1);
txtColumn=0;
txtRow=0;
}

void
writeChar(char c)
{
switch(c)
  {
  case '\b':
    {
    if(txtColumn)
      {
      --txtColumn;
      }
    break;
    }
  case '\r':
    {
    txtColumn=0;
    break;
    }
  case '\n':
    {
    txtColumn=TXT_WIDTH;
    break;
    }
  case '\f':
    {
    clearScreen();
    break;
    }
  case '\t':
    {
    txtColumn=((txtColumn/txtTabSize)+1)*txtTabSize;
    break;
    }
  default:
    {
    byte * ptr=(byte *)TXT_BASE+(((TXT_WIDTH*txtRow)+txtColumn)<<1);
    ptr[0]=c;
    ptr[1]=txtColor;
    ++txtColumn;
    }
  }
if(txtColumn>=TXT_WIDTH)
  {
  txtColumn=0;
  if(txtRow==TXT_HEIGHT-1)
    {
    char * dst=(char *)TXT_BASE;
    char * src=dst+(TXT_WIDTH<<1);
    size_t nb=(TXT_WIDTH*(TXT_HEIGHT-1))<<1;
    memmove(dst,src,nb);
    dst+=nb;
    memset(dst,0,nb);
    }
  else
    {
    ++txtRow;
    }
  }
}

void
writeStr(const char * str)
{
while(*str)
  {
  writeChar(*(str++));
  }
}

void
writeHexaByte(byte value)
{
static char digits[]="0123456789ABCDEF";
writeChar(digits[(value>>4)&0x0F]);
writeChar(digits[value&0x0F]);
}

void
writeHexaWord(word value)
{
writeHexaByte((value>>8)&0x00FF);
writeHexaByte(value&0x00FF);
}

void
writeHexaDWord(dword value)
{
writeHexaByte((value>>24)&0x000000FF);
writeHexaByte((value>>16)&0x000000FF);
writeHexaByte((value>>8)&0x000000FF);
writeHexaByte(value&0x000000FF);
}

void
writeDecimalDWord(dword value)
{
static char buffer[0x10];
byte idx=0;
do
  {
  buffer[idx++]='0'+(value%10);
  value/=10;
  } while(value);
while(idx--)
  {
  writeChar(buffer[idx]);
  }
}

void
writeFloat(float value)
{
writeDouble(value);
}

void /* very ugly implementation !!!! */
writeDouble(double value)
{
dword i,prec,d;
if(value<0.0)
  {
  writeChar('-');
  value=-value;
  } 
i=(dword)value;
prec=1000000;
d=(dword)((value-i)*prec);
writeDecimalDWord(i);
writeChar('.');
if(d)
  {
  while(d<(prec/=10))
    {
    writeChar('0');
    }
  writeDecimalDWord(d);
  }
else
  {
  writeChar('0');
  }
}

void
beep(dword frequency,
     dword milliseconds)
{
byte b;
dword limit=getTime()+milliseconds;
dword val=0x001234DE/frequency;
outb(0x43,0xb6);
outb(0x42,val&0x000000FF);
outb(0x42,(val>>8)&0x000000FF);
inb(b,0x61);
outb(0x61,b|3);
while(getTime()<limit)
  {
  idle();
  }
outb(0x61,b);
}

/*--------------------------------------------------------------------------*/
