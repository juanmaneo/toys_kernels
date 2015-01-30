/*--------------------------------------------------------------------------*/

#include "keyboard.h"
#include "utils.h"

#if 0
#define MY_KEYBOARD
#endif

#define KBD_INPUT_SIZE 0x100

#define KEY_INDEX_AND_BIT(key,index,bit) \
  { (index)=(key)>>3; (bit)=1<<((key)&0x07); }

static volatile byte keys[0x10]={0};
static volatile char input[KBD_INPUT_SIZE]={0};
static volatile size_t inputCount=0;

size_t /* nb chars in input buffer */
inputReady(void)
{
size_t result;
maskInterrupts();
result=inputCount;
unmaskInterrupts();
return result;
}

char /* next char in input buffer or \0 */
getChar(void)
{
char result='\0';
maskInterrupts();
if(inputCount)
  {
  result=input[0];
  memmove((void *)input,(const void *)(input+1),--inputCount);
  }
unmaskInterrupts();
return result;
}

bool /* key is pressed */
testKey(byte key)
{
bool result;
byte index;
byte bit;
KEY_INDEX_AND_BIT(key,index,bit);
maskInterrupts();
result=keys[index]&bit;
unmaskInterrupts();
return result;
}

#ifdef MY_KEYBOARD
static
char /* FIXME: ugly hardcoded keymap for _MY_ keyboard ! */
translateKey(byte key)
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
static
char /* FIXME: ugly hardcoded keymap for US keyboard ! */
translateKey(byte key)
{
char c='\0';
bool shift=(testKey(0x2A)||testKey(0x36));
/* bool alt=testKey(0x38); */
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

void
keyboardInterruptHandler(void)
{
byte sc;
do
  {
  inb(sc,0x64);
  } while((sc&0x01)==0);
inb(sc,0x60);
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
      byte index;
      byte bit;
      sc&=0x7F;
      KEY_INDEX_AND_BIT(sc,index,bit);
      keys[index]&=~bit;
      }
    else
      {
      char c;
      byte index;
      byte bit;
      KEY_INDEX_AND_BIT(sc,index,bit);
      keys[index]|=bit;
      c=translateKey(sc);
      if(c&&(inputCount<KBD_INPUT_SIZE))
        {
        input[inputCount++]=c;
        }
      else
        {
        /* FIXME: should beep ? */
        }
      }
    }
  }
}

/*--------------------------------------------------------------------------*/
