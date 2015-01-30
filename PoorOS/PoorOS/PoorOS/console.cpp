//----------------------------------------------------------------------------

#include "PoorOS/console.h"

#include "PoorOS/system.h"

namespace PoorOS {

#define TXT_PTR(column,row) \
        ((byte *)Console::TXT_BASE+(((Console::TXT_WIDTH*row)+column)<<1))

static
inline
void
keyIndexAndBit(byte key,
               byte & indexOut,
               byte & bitOut)
{
indexOut=key>>3;
bitOut=1<<(key&0x07);
}

Console::Console(void)
: _column(0),
  _row(0),
  _color(Console::TXT_GREEN),
  _reverseColor(_color<<4),
  _savedColor(_color),
  _tabSize(8),
  _keys(),
  _input(),
  _inputReady(0)
{
memset((void *)_keys,0,0x10);
memset((void *)_input,0,Console::TXT_INPUT_SIZE);
clear();
}

Console::~Console(void)
{
byte b;
inb(b,0x61);
outb(0x61,b&0xFC); // stop beep if any
}

void
Console::clear(void)
{
byte * ptr=(byte *)Console::TXT_BASE;
memset(ptr,0,(Console::TXT_WIDTH*Console::TXT_HEIGHT)<<1);
ptr[1]=_reverseColor;
_column=0;
_row=0;
_savedColor=_color;
}

void
Console::writeChar(char c)
{
byte * ptr=TXT_PTR(_column,_row);
switch(c)
  {
  case '\b':
    {
    if(_column)
      {
      ptr[1]=_savedColor;
      --_column;
      }
    break;
    }
  case '\r':
    {
    ptr[1]=_savedColor;
    _column=0;
    break;
    }
  case '\n':
    {
    ptr[1]=_savedColor;
    _column=Console::TXT_WIDTH;
    break;
    }
  case '\f':
    {
    clear();
    break;
    }
  case '\t':
    {
    ptr[1]=_savedColor;
    _column=((_column/_tabSize)+1)*_tabSize;
    break;
    }
  default:
    {
    ptr[0]=c;
    ptr[1]=_color;
    ++_column;
    }
  }
if(_column>=Console::TXT_WIDTH)
  {
  _column=0;
  if(_row==Console::TXT_HEIGHT-1)
    {
    byte * dst=(byte *)Console::TXT_BASE;
    byte * src=dst+(Console::TXT_WIDTH<<1);
    size_t nb=(Console::TXT_WIDTH*(Console::TXT_HEIGHT-1))<<1;
    memmove(dst,src,nb);
    dst+=nb;
    memset(dst,0,nb);
    }
  else
    {
    ++_row;
    }
  }
ptr=TXT_PTR(_column,_row);
_savedColor=ptr[1];
ptr[1]=_reverseColor;
}

void
Console::writeStr(const char * src)
{
while(*src)
  {
  writeChar(*(src++));
  }
}

void
Console::writeHexaByte(byte value)
{
static char digits[]="0123456789ABCDEF";
writeChar(digits[(value>>4)&0x0F]);
writeChar(digits[value&0x0F]);
}

void
Console::writeHexaWord(word value)
{
writeHexaByte((value>>8)&0x00FF);
writeHexaByte(value&0x00FF);
}

void
Console::writeHexaDWord(dword value)
{
writeHexaByte((value>>24)&0x000000FF);
writeHexaByte((value>>16)&0x000000FF);
writeHexaByte((value>>8)&0x000000FF);
writeHexaByte(value&0x000000FF);
}

void
Console::writeDecimalByte(byte value)
{
writeDecimalDWord(value);
}

void
Console::writeDecimalWord(word value)
{
writeDecimalDWord(value);
}

void
Console::writeDecimalDWord(dword value)
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
Console::writeFloat(float value)
{
writeDouble(value);
}

void // very ugly implementation !!!!
Console::writeDouble(double value)
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
Console::beep(dword frequency,
              dword milliseconds)
{
byte b;
dword val=0x001234DE/frequency;
outb(0x43,0xb6);
outb(0x42,val&0x000000FF);
outb(0x42,(val>>8)&0x000000FF);
inb(b,0x61);
outb(0x61,b|3);
System::theSystem->schedule(milliseconds);
outb(0x61,b);
}

void
Console::setCursor(dword column,
                   dword row)
{
byte * ptr=TXT_PTR(_column,_row);
ptr[1]=_savedColor;
_column=column%Console::TXT_WIDTH;
_row=row%Console::TXT_HEIGHT;
ptr=TXT_PTR(_column,_row);
_savedColor=ptr[1];
ptr[1]=_reverseColor;
}

void
Console::getCursor(dword & columnOut,
                   dword & rowOut) const
{
columnOut=_column;
rowOut=_row;
}

size_t // nb columns
Console::getWidth(void) const
{
return Console::TXT_WIDTH;
}

size_t // nb rows
Console::getHeight(void) const
{
return Console::TXT_HEIGHT;
}

void
Console::setColor(Console::Color color)
{
byte c=(byte)color;
c&=0x0000000F;
if(c)
  {
  _color=c;
  _reverseColor=c<<4;
  byte * ptr=TXT_PTR(_column,_row);
  ptr[1]=_reverseColor;
  }
}

Console::Color // current color
Console::getColor(void) const
{
return (Console::Color)_color;
}

void
Console::setTabSize(size_t tabSize)
{
if(tabSize&&(tabSize<(Console::TXT_WIDTH>>1)))
  {
  _tabSize=tabSize;
  }
}

size_t // current tab size
Console::getTabSize(void) const
{
return _tabSize;
}

size_t // nb chars in input buffer
Console::inputReady(void) const
{
size_t result;
maskInterrupts();
result=_inputReady;
unmaskInterrupts();
return result;
}

char // next char in input buffer or \0
Console::getChar(void)
{
char result='\0';
maskInterrupts();
if(_inputReady)
  {
  result=_input[0];
  memmove((void *)_input,(const void *)(_input+1),--_inputReady);
  }
unmaskInterrupts();
return result;
}

bool // key is pressed
Console::testKey(byte key)
{
bool result;
byte index;
byte bit;
keyIndexAndBit(key,index,bit);
maskInterrupts();
result=_keys[index]&bit;
unmaskInterrupts();
return result;
}

void
Console::_manageScanCode(byte sc) // Protected
{
switch(sc)
  {
  case 0xE0:
    {
    break;
    }
  default:
    {
    if(sc&0x80)
      {
      sc&=0x7F;
      byte index;
      byte bit;
      keyIndexAndBit(sc,index,bit);
      _keys[index]&=~bit;
      }
    else
      {
      byte index;
      byte bit;
      keyIndexAndBit(sc,index,bit);
      _keys[index]|=bit;
      char c=_translateKey(sc);
      if(c)
        {
        if(_inputReady<Console::TXT_INPUT_SIZE)
          {
          _input[_inputReady++]=c;
          }
        else
          {
          // FIXME: should beep ?
          }
        }
      }
    }
  }
}

#ifdef MY_KEYBOARD
char // FIXME: ugly hardcoded keymap for _MY_ keyboard !
Console::_translateKey(byte key) // Protected
{
char c='\0';
bool shift=(testKey(0x2A)||testKey(0x36));
bool alt=testKey(0x38);
bool control=testKey(0x1D);
switch(key)
  {
  case 0x01: c='\x1B'; break;
  case 0x02: c=(alt ? '¹' : (shift ? '1' : '&')); break;
  case 0x03: c=(alt ? '~' : (shift ? '2' : 'é')); break;
  case 0x04: c=(alt ? '#' : (shift ? '3' : '"')); break;
  case 0x05: c=(alt ? '{' : (shift ? '4' : '\'')); break;
  case 0x06: c=(alt ? '[' : (shift ? '5' : '(')); break;
  case 0x07: c=(alt ? '|' : (shift ? '6' : '-')); break;
  case 0x08: c=(alt ? '`' : (shift ? '7' : 'è')); break;
  case 0x09: c=(alt ? '\\' : (shift ? '8' : '_')); break;
  case 0x0A: c=(alt ? '^' : (shift ? '9' : 'ç')); break;
  case 0x0B: c=(alt ? '@' : (shift ? '0' : 'à')); break;
  case 0x0C: c=(alt ? ']' : (shift ? '°' : ')')); break;
  case 0x0D: c=(alt ? '}' : (shift ? '+' : '=')); break;
  case 0x0E: c='\b'; break;
  case 0x0F: c='\t'; break;
  case 0x10: c=(shift ? 'A' : 'a'); break;
  case 0x11: c=(shift ? 'Z' : 'z'); break;
  case 0x12: c=(shift ? 'E' : 'e'); break;
  case 0x13: c=(shift ? 'R' : 'r'); break;
  case 0x14: c=(shift ? 'T' : 't'); break;
  case 0x15: c=(shift ? 'Y' : 'y'); break;
  case 0x16: c=(shift ? 'U' : 'u'); break;
  case 0x17: c=(shift ? 'I' : 'i'); break;
  case 0x18: c=(shift ? 'O' : 'o'); break;
  case 0x19: c=(shift ? 'P' : 'p'); break;
  case 0x1A: c='^'; break;
  case 0x1B: c=(alt ? '¤' : (shift ? '£' : '$')); break;
  case 0x1C: c='\n'; break;
  case 0x1E: c=(shift ? 'Q' : 'q'); break;
  case 0x1F: c=(shift ? 'S' : 's'); break;
  case 0x20: c=(shift ? 'D' : 'd'); break;
  case 0x21: c=(shift ? 'F' : 'f'); break;
  case 0x22: c=(shift ? 'G' : 'g'); break;
  case 0x23: c=(shift ? 'H' : 'h'); break;
  case 0x24: c=(shift ? 'J' : 'j'); break;
  case 0x25: c=(shift ? 'K' : 'k'); break;
  case 0x26: c=(shift ? 'L' : 'l'); break;
  case 0x27: c=(shift ? 'M' : 'm'); break;
  case 0x28: c=(shift ? '%' : 'ù'); break;
  case 0x29: c='²'; break;
  case 0x2B: c=(shift ? 'µ' : '*'); break;
  case 0x2C: c=(shift ? 'W' : 'w'); break;
  case 0x2D: c=(shift ? 'X' : 'x'); break;
  case 0x2E: c=(shift ? 'C' : 'c'); break;
  case 0x2F: c=(shift ? 'V' : 'v'); break;
  case 0x30: c=(shift ? 'B' : 'b'); break;
  case 0x31: c=(shift ? 'N' : 'n'); break;
  case 0x32: c=(shift ? '?' : ','); break;
  case 0x33: c=(shift ? '.' : ';'); break;
  case 0x34: c=(shift ? '/' : ':'); break;
  case 0x35: c=(shift ? '§' : '!'); break;
  case 0x39: c=' '; break;
  case 0x53: c='\x7f'; break;
  case 0x56: c=(shift ? '>' : '<'); break;
  }
if(control)
  {
  if((c>='@')&&(c<='Z'))
    {
    c-='@';
    }
  else if((c>='`')&&(c<='z'))
    {
    c-='`';
    }
  }
return c;
}
#else
char // FIXME: ugly hardcoded keymap for US keyboard !
Console::_translateKey(byte key) // Protected
{
char c='\0';
bool shift=(testKey(0x2A)||testKey(0x36));
// bool alt=testKey(0x38);
bool control=testKey(0x1D);
switch(key)
  {
  case 0x01: c='\x1B'; break;
  case 0x02: c=(shift ? '!' : '1'); break;
  case 0x03: c=(shift ? '@' : '2'); break;
  case 0x04: c=(shift ? '#' : '3'); break;
  case 0x05: c=(shift ? '$' : '4'); break;
  case 0x06: c=(shift ? '%' : '5'); break;
  case 0x07: c=(shift ? '^' : '6'); break;
  case 0x08: c=(shift ? '&' : '7'); break;
  case 0x09: c=(shift ? '*' : '8'); break;
  case 0x0A: c=(shift ? '(' : '9'); break;
  case 0x0B: c=(shift ? ')' : '0'); break;
  case 0x0C: c=(shift ? '_' : '-'); break;
  case 0x0D: c=(shift ? '+' : '='); break;
  case 0x0E: c='\b'; break;
  case 0x0F: c='\t'; break;
  case 0x10: c=(shift ? 'Q' : 'q'); break;
  case 0x11: c=(shift ? 'W' : 'w'); break;
  case 0x12: c=(shift ? 'E' : 'e'); break;
  case 0x13: c=(shift ? 'R' : 'r'); break;
  case 0x14: c=(shift ? 'T' : 't'); break;
  case 0x15: c=(shift ? 'Y' : 'y'); break;
  case 0x16: c=(shift ? 'U' : 'u'); break;
  case 0x17: c=(shift ? 'I' : 'i'); break;
  case 0x18: c=(shift ? 'O' : 'o'); break;
  case 0x19: c=(shift ? 'P' : 'p'); break;
  case 0x1A: c=(shift ? '{' : '['); break;
  case 0x1B: c=(shift ? '}' : ']'); break;
  case 0x1C: c='\n'; break;
  case 0x1E: c=(shift ? 'A' : 'a'); break;
  case 0x1F: c=(shift ? 'S' : 's'); break;
  case 0x20: c=(shift ? 'D' : 'd'); break;
  case 0x21: c=(shift ? 'F' : 'f'); break;
  case 0x22: c=(shift ? 'G' : 'g'); break;
  case 0x23: c=(shift ? 'H' : 'h'); break;
  case 0x24: c=(shift ? 'J' : 'j'); break;
  case 0x25: c=(shift ? 'K' : 'k'); break;
  case 0x26: c=(shift ? 'L' : 'l'); break;
  case 0x27: c=(shift ? ':' : ';'); break;
  case 0x28: c=(shift ? '"' : '\''); break;
  case 0x29: c=(shift ? '~' : '`'); break;
  case 0x2B: c=(shift ? '|' : '\\'); break;

  case 0x2C: c=(shift ? 'Z' : 'z'); break;
  case 0x2D: c=(shift ? 'X' : 'x'); break;
  case 0x2E: c=(shift ? 'C' : 'c'); break;
  case 0x2F: c=(shift ? 'V' : 'v'); break;
  case 0x30: c=(shift ? 'B' : 'b'); break;
  case 0x31: c=(shift ? 'N' : 'n'); break;
  case 0x32: c=(shift ? 'M' : 'm'); break;
  case 0x33: c=(shift ? '<' : ','); break;
  case 0x34: c=(shift ? '>' : '.'); break;
  case 0x35: c=(shift ? '?' : '/'); break;
  case 0x39: c=' '; break;
  case 0x53: c='\x7f'; break;
  }
if(control)
  {
  if((c>='@')&&(c<='Z'))
    {
    c-='@';
    }
  else if((c>='`')&&(c<='z'))
    {
    c-='`';
    }
  }
return c;
}
#endif

} // namespace PoorOS

//----------------------------------------------------------------------------
