//----------------------------------------------------------------------------

#ifndef POOROS_SYSTEM_H
#define POOROS_SYSTEM_H 1

#include "utils.h"

namespace PoorOS {

class Console;
class Task;

class System
{
public:

  static
  System *
  theSystem; // should de initialized in the application

  System(void);

  virtual
  ~System(void);

  virtual
  bool // destroy system
  main(void)=0;

  virtual
  void
  setConsole(Console * console);

  virtual
  const Console *
  getConsole(void) const;

  virtual
  Console *
  accessConsole(void);

  virtual
  dword // milliseconds since startup
  getTime(void) const;

  virtual
  void
  schedule(dword milliseconds=0);

protected:

  friend class Task;

  struct StackFrame
    {
    byte fctx[7*4+8*10]; // fsave/frstor
    dword gs;            // push/pop
    dword fs;            // push/pop
    dword es;            // push/pop
    dword ds;            // push/pop
    dword edi;           // pusha/popa
    dword esi;           //   ...
    dword ebp;           //   ...
    dword esp;           //   ...
    dword ebx;           //   ...
    dword edx;           //   ...
    dword ecx;           //   ...
    dword eax;           //   ...
    dword eip;           // int/iret
    dword cs;            //   ...
    dword eflags;        //   ...
    };

  enum
    {
    NB_TASK_MAX=100,
    STACK_SIZE=0x4000
    };

  virtual
  void
  _addTask(Task * task);

  virtual
  void
  _removeTask(Task * task);

  virtual
  void
  _interruptHandler(dword interrupt);

  enum Syscall
    {
    SYSCALL_SCHEDULE,
    SYSCALL_ADD_TASK,
    SYSCALL_REMOVE_TASK
    };

  virtual
  void
  _manageSyscall(System::StackFrame * frame);

  virtual
  void
  _schedule(void);

  static
  void
  _wrapMain(void);

  static
  void
  _wrapInterruptHandler(dword interrupt);

  byte * _systemStack;
  Console * _console;
  volatile dword _timeCount;
  Task * _tasks[System::NB_TASK_MAX];
  size_t _nbTasks;
  int _currentTask;
};

} // namespace PoorOS

#endif // POOROS_SYSTEM_H

//----------------------------------------------------------------------------
