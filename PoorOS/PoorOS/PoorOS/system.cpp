//----------------------------------------------------------------------------

#include "PoorOS/system.h"

#include "PoorOS/console.h"
#include "PoorOS/task.h"

//----------------------------------------------------------------------------

extern byte ** __currentStack;

namespace PoorOS {

System::System(void)
: _systemStack((byte *)0),
  _console((Console *)0),
  _timeCount(0),
  _tasks(),
  _nbTasks(0),
  _currentTask(-1)
{
__currentStack=&_systemStack;
dword val=0x001234DE/1000; // program system timer at 1kHz
outb(0x43,0x36); 
outb(0x40,val&0x000000FF);
outb(0x40,(val>>8)&0x000000FF);
}

System::~System(void)
{
while(_nbTasks)
  {
  delete _tasks[_nbTasks-1];
  }
if(_console)
  {
  delete _console;
  }
System::theSystem=(System *)0;
}

// bool // destroy system
// System::main(void)=0

void
System::setConsole(Console * console)
{
_console=console;
}

const Console *
System::getConsole(void) const
{
return _console;
}

Console *
System::accessConsole(void)
{
return _console;
}

dword // milliseconds since startup
System::getTime(void) const
{
dword result;
maskInterrupts();
result=_timeCount;
unmaskInterrupts();
return result;
}

void
System::schedule(dword milliseconds)
{
dword limit=getTime()+milliseconds;
for(;;)
  {
  dword result;
  syscall_0(result,System::SYSCALL_SCHEDULE);
  if(getTime()<limit)
    {
    idle(); // make CPU sleep until an interrupt occurs
    }
  else
    {
    break;
    }
  }
}

void
System::_addTask(Task * task) // Protected
{
dword result;
syscall_1(result,System::SYSCALL_ADD_TASK,task);
}

void
System::_removeTask(Task * task) // Protected
{
dword result;
syscall_1(result,System::SYSCALL_REMOVE_TASK,task);
}

void
System::_interruptHandler(dword interrupt) // Protected
{
maskInterrupts();
switch(interrupt)
  {
  case 0x00: // Divide by zero (Intel exception)
  case 0x01: // Trace / Debug (Intel exception)
  case 0x02: // NMI (Intel exception)
  case 0x03: // Breakpoint instruction (Intel exception)
  case 0x04: // Into overflow (Intel exception)
  case 0x05: // Bound overflow (Intel exception)
  case 0x06: // Invalid opcode (Intel exception)
  case 0x07: // Unavailable coprocessor (Intel exception)
  case 0x08: // Double fault (Intel exception)
  case 0x09: // Coprocessor segment overflow (Intel exception)
  case 0x0A: // Invalid task segment (Intel exception)
  case 0x0B: // Missing segment (Intel exception)
  case 0x0C: // Stack fault (Intel exception)
  case 0x0D: // General protection fault (Intel exception)
  case 0x0E: // Page fault (Intel exception)
  case 0x0F: // Reserved (Intel exception)
  case 0x10: // Coprocessor error (Intel exception)
  case 0x11: // Bad alignment (Intel exception)
  case 0x12: // Reserved (Intel exception)
  case 0x13: // Reserved (Intel exception)
  case 0x14: // Reserved (Intel exception)
  case 0x15: // Reserved (Intel exception)
  case 0x16: // Reserved (Intel exception)
  case 0x17: // Reserved (Intel exception)
  case 0x18: // Reserved (Intel exception)
  case 0x19: // Reserved (Intel exception)
  case 0x1A: // Reserved (Intel exception)
  case 0x1B: // Reserved (Intel exception)
  case 0x1C: // Reserved (Intel exception)
  case 0x1D: // Reserved (Intel exception)
  case 0x1E: // Reserved (Intel exception)
    {
    break;
    }
  case 0x1F: // Reserved (Intel exception)
    {
    _manageSyscall((System::StackFrame *)(*__currentStack));
    break;
    }
  case 0x20: // System timer - i8254 (Master PIC, irq 0)
    {
    if(!(++_timeCount%10))
      {
      _schedule(); // 100Hz
      }
    break;
    }
  case 0x21: // Keyboard controller - i8042 (Master PIC, irq 1)
    {
    byte sc;
    do
      {
      inb(sc,0x64);
      } while((sc&0x01)==0);
    inb(sc,0x60);
    if(_console)
      {
      _console->_manageScanCode(sc);
      }
    break;
    }
  case 0x22: // Slave PIC (Master PIC, irq 2)
  case 0x23: // Serial ports 2:2F8h-2FFh and 4:2E8h-2EFh (Master PIC, irq 3)
  case 0x24: // Serial ports 1:3F8h-3FFh and 3:3E8h-3EFh (Master PIC, irq 4)
  case 0x25: // Parallel port 2:378h/278h (Master PIC, irq 5)
  case 0x26: // Floppy disk controller (Master PIC, irq 6)
  case 0x27: // Parallel port 1:3BCh/378h (Master PIC, irq 7)
  case 0x28: // Real time clock - MC146818 (Slave PIC, irq 8)
  case 0x29: // Unused / redirecte cascade (Slave PIC, irq 9)
  case 0x2A: // USB / general adapter (Slave PIC, irq 10)
  case 0x2B: // General adapter (Slave PIC, irq 11)
  case 0x2C: // Mouse port / general adapter (Slave PIC, irq 12)
  case 0x2D: // Math coprocessor NMI (Slave PIC, irq 13)
  case 0x2E: // Primary IDE Hard disk (Slave PIC, irq 14)
  case 0x2F: // Secondary IDE Hard disk (Slave PIC, irq 15)
  default:
    {
    break;
    }
  }
unmaskInterrupts();
}

void
System::_manageSyscall(System::StackFrame * frame) // Protected
{
// read syscall number in eax
// read syscall arguments in ebx,ecx,edx,esi,edi
// write syscall result in eax
switch(frame->eax)
  {
  case SYSCALL_SCHEDULE:
    {
    _schedule();
    frame->eax=0;
    break;
    }
  case SYSCALL_ADD_TASK:
    {
    Task * task=(Task *)frame->ebx;
    if(_nbTasks<System::NB_TASK_MAX)
      {
      _tasks[_nbTasks++]=task;
      task->_stackSegment=new byte[System::STACK_SIZE];
      dword * top=(dword *)(task->_stackSegment+System::STACK_SIZE);
      *(--top)=(dword)task; // argument for Task::_main()
      *(--top)=(dword)0; // dummy return address
      top-=sizeof(System::StackFrame)/sizeof(dword);
      System::StackFrame * f=(System::StackFrame *)top;
      f->gs=0x10;                 // data segment descriptor
      f->fs=0x10;                 // ...
      f->es=0x10;                 // ...
      f->ds=0x10;                 // ...
      f->edi=0;
      f->esi=0;
      f->ebp=0;
      f->esp=(dword)top;          // unused but correct
      f->ebx=0;
      f->edx=0;
      f->ecx=0;
      f->eax=0;
      f->eip=(dword)&Task::_main; // start execution here
      f->cs=0x08;                 // code segment descriptor
      f->eflags=0x00000202;       // interrupts are enabled
      asm volatile ("finit" : : );
      asm volatile ("fsave (%%eax)" : : "a" (top) ); // initial math context
      task->_stackPointer=(byte *)top;
      frame->eax=0;
      }
    else
      {
      frame->eax=0xFFFFFFFF;
      }
    break;
    }
  case SYSCALL_REMOVE_TASK:
    {
    Task * task=(Task *)frame->ebx;
    for(size_t i=_nbTasks;i--;)
      {
      if(_tasks[i]==task)
        {
        _tasks[i]=_tasks[--_nbTasks];
        if(_currentTask==(int)i) // destroying current task --> no return
          {
          delete[] task->_stackSegment; // still using it until syscall end
          free(task); // yes, it's ugly !
          }
        _currentTask=_nbTasks;
        _schedule();
        break;
        }
      }
    frame->eax=0;
    break;
    }
  default:
    {
    frame->eax=0xFFFFFFFF;
    break;
    }
  }
}

void
System::_schedule(void) // Protected
{
for(;;)
  {
  if(++_currentTask>=(int)_nbTasks)
    {
    _currentTask=-1;
    __currentStack=&_systemStack;
    break;
    }
  if(_timeCount>=_tasks[_currentTask]->_wakeUpTime)
    {
    __currentStack=&_tasks[_currentTask]->_stackPointer;
    break;
    }
  }
}

void
System::_wrapMain(void) // Static, Protected
{
if(System::theSystem&&System::theSystem->main())
  {
  delete System::theSystem;
  }
}

void
System::_wrapInterruptHandler(dword interrupt) // Static, Protected
{
if(System::theSystem)
  {
  System::theSystem->_interruptHandler(interrupt);
  }
}

} // namespace PoorOS

//----------------------------------------------------------------------------
