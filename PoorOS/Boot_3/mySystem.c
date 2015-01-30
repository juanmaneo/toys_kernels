/*--------------------------------------------------------------------------*/

#include "utils.h"
#include "timer.h"
#include "txtScreen.h"
#include "keyboard.h"

/*--------------------------------------------------------------------------*/

typedef struct
  {
  void * data;
  size_t capacity;
  size_t size;
  } Block;

void
initBlock(Block * b)
{
b->data=(void *)0;
b->capacity=0;
b->size=0;
}

void
eraseBlock(Block * b)
{
if(b->data)
  {
  free(b->data);
  b->data=(void *)0;
  }
b->capacity=0;
b->size=0;
}

Block *
createBlock(void)
{
Block * b=(Block*)malloc(sizeof(Block));
initBlock(b);
return b;
}

void
destroyBlock(Block * b)
{
eraseBlock(b);
free(b);
}

void
resizeBlock(Block * b,
            size_t size)
{
if(b->data)
  {
  if(b->capacity>=size)
    {
    b->size=size;
    }
  else
    {
    void * data=malloc(size);
    memcpy(data,b->data,b->size);
    free(b->data);
    b->data=data;
    b->size=size;
    b->capacity=size;
    }
  }
else
  {
  b->data=malloc(size);
  b->size=size;
  b->capacity=size;
  }
}

size_t /* old size */
appendBlock(Block * b,
            const void * data,
            size_t size)
{
size_t oldSize=b->size;
resizeBlock(b,oldSize+size);
memcpy(((byte *)b->data)+oldSize,data,size);
return oldSize;
}

void
writeBlock(Block * b,
           size_t offset,
           const void * data,
           size_t size)
{
memcpy((byte *)b->data+offset,data,size);
}

void
readBlock(const Block * b,
          size_t offset,
          void * data,
          size_t size)
{
memcpy(data,(byte *)b->data+offset,size);
}

/*--------------------------------------------------------------------------*/

void
mainCode(void)
{
const char * prompt="--> ";
Block * str;
Block vect;
initTimer();
str=createBlock();
initBlock(&vect);
writeStr(prompt);
for(;;)
  {
  if(inputReady())
    {
    char c=getChar();
    switch(c)
      {
      case '\b':
        {
        if(str->size)
          {
          --(str->size);
          writeStr("\b \b");
          }
        else
          {
          beep(300,100);
          }
        break;
        }
      case '\n':
        {
        writeChar(c);
        c='\0';
        appendBlock(str,&c,sizeof(char));
        if(!strcmp(str->data,"time"))
          {
          writeDecimalDWord(getTime());
          writeChar('\n');
          }
        else if(!strcmp(str->data,"clear"))
          {
          clearScreen();
          }
        else if(!strcmp(str->data,"show"))
          {
          showAlloc();
          }
        else if(!strcmp(str->data,"free"))
          {
          size_t i;
          for(i=0;i<vect.size;i+=sizeof(Block *))
            {
            Block * s;
            readBlock(&vect,i,&s,sizeof(Block *));
            destroyBlock(s);
            }
          eraseBlock(&vect);
          }
        else if(strlen(str->data))
          {
          TxtColor color;
          size_t i;
          appendBlock(&vect,&str,sizeof(Block *));
          str=createBlock();
          color=getTxtColor();
          for(i=0;i<vect.size;i+=sizeof(Block *))
            {
            Block * s;
            readBlock(&vect,i,&s,sizeof(Block *));
            setTxtColor(1+(i%15));
            writeDecimalDWord(s->size);
            writeStr("\t<");
            writeStr((const char *)s->data);
            writeStr(">\n");
            }
          setTxtColor(color);
          }
        eraseBlock(str);
        writeStr(prompt);
        break;
        }
      default:
        {
        writeChar(c);
        appendBlock(str,&c,sizeof(char));
        }
      }
    }
  else
    {
    idle();
    }
  }
}

/*--------------------------------------------------------------------------*/

void
interruptHandler(unsigned int interrupt)
{
switch(interrupt)
  {
  case 0x00: /* Divide by zero (Intel exception) */
  case 0x01: /* Trace / Debug (Intel exception) */
  case 0x02: /* NMI (Intel exception) */
  case 0x03: /* Breakpoint instruction (Intel exception) */
  case 0x04: /* Into overflow (Intel exception) */
  case 0x05: /* Bound overflow (Intel exception) */
  case 0x06: /* Invalid opcode (Intel exception) */
  case 0x07: /* Unavailable coprocessor (Intel exception) */
  case 0x08: /* Double fault (Intel exception) */
  case 0x09: /* Coprocessor segment overflow (Intel exception) */
  case 0x0A: /* Invalid task segment (Intel exception) */
  case 0x0B: /* Missing segment (Intel exception) */
  case 0x0C: /* Stack fault (Intel exception) */
  case 0x0D: /* General protection fault (Intel exception) */
  case 0x0E: /* Page fault (Intel exception) */
  case 0x0F: /* Reserved (Intel exception) */
  case 0x10: /* Coprocessor error (Intel exception) */
  case 0x11: /* Bad alignment (Intel exception) */
  case 0x12: /* Reserved (Intel exception) */
  case 0x13: /* Reserved (Intel exception) */
  case 0x14: /* Reserved (Intel exception) */
  case 0x15: /* Reserved (Intel exception) */
  case 0x16: /* Reserved (Intel exception) */
  case 0x17: /* Reserved (Intel exception) */
  case 0x18: /* Reserved (Intel exception) */
  case 0x19: /* Reserved (Intel exception) */
  case 0x1A: /* Reserved (Intel exception) */
  case 0x1B: /* Reserved (Intel exception) */
  case 0x1C: /* Reserved (Intel exception) */
  case 0x1D: /* Reserved (Intel exception) */
  case 0x1E: /* Reserved (Intel exception) */
  case 0x1F: /* Reserved (Intel exception) */
    {
    break;
    }
  case 0x20: /* System timer - i8254 (Master PIC, irq 0) */
    {
    timerInterruptHandler();
    break;
    }
  case 0x21: /* Keyboard controller - i8042 (Master PIC, irq 1) */
    {
    keyboardInterruptHandler();
    break;
    }
  case 0x22: /* Slave PIC (Master PIC, irq 2) */
  case 0x23: /* Serial ports 2:2F8h-2FFh and 4:2E8h-2EFh (Master PIC, irq 3) */
  case 0x24: /* Serial ports 1:3F8h-3FFh and 3:3E8h-3EFh (Master PIC, irq 4) */
  case 0x25: /* Parallel port 2:378h/278h (Master PIC, irq 5) */
  case 0x26: /* Floppy disk controller (Master PIC, irq 6) */
  case 0x27: /* Parallel port 1:3BCh/378h (Master PIC, irq 7) */
  case 0x28: /* Real time clock - MC146818 (Slave PIC, irq 8) */
  case 0x29: /* Unused / redirecte cascade (Slave PIC, irq 9) */
  case 0x2A: /* USB / general adapter (Slave PIC, irq 10) */
  case 0x2B: /* General adapter (Slave PIC, irq 11) */
  case 0x2C: /* Mouse port / general adapter (Slave PIC, irq 12) */
  case 0x2D: /* Math coprocessor NMI (Slave PIC, irq 13) */
  case 0x2E: /* Primary IDE Hard disk (Slave PIC, irq 14) */
  case 0x2F: /* Secondary IDE Hard disk (Slave PIC, irq 15) */
  default:
    {
    break;
    }
  }
}

/*--------------------------------------------------------------------------*/
