/*--------------------------------------------------------------------------*/

#include "timer.h"

static volatile dword timeCount=0;

void
initTimer(void)
{
static bool initDone=false;
if(!initDone)
  {
  dword val=0x001234DE/1000; /* program system timer at 1kHz */
  maskInterrupts()
  outb(0x43,0x36);
  outb(0x40,val&0x000000FF);
  outb(0x40,(val>>8)&0x000000FF);
  unmaskInterrupts()
  initDone=true;
  }
}

dword /* milliseconds */
getTime(void)
{
dword result;
maskInterrupts();
result=timeCount;
unmaskInterrupts();
return result;
}

void
timerInterruptHandler(void)
{
++timeCount;
}

/*--------------------------------------------------------------------------*/
