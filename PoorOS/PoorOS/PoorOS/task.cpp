//----------------------------------------------------------------------------

#include "PoorOS/task.h"

#include "PoorOS/system.h"

namespace PoorOS {

Task::Task(void)
: _stackSegment(new byte[System::STACK_SIZE]),
  _stackPointer((byte *)0),
  _wakeUpTime(0xFFFFFFFF),
  _ended(false)
{
System::theSystem->_addTask(this);
}

Task::~Task(void)
{
System::theSystem->_removeTask(this);
delete[] _stackSegment;
}

void
Task::pause(dword milliseconds)
{
_wakeUpTime=(milliseconds==0xFFFFFFFF
             ? 0xFFFFFFFF
             : System::theSystem->getTime()+milliseconds);
System::theSystem->schedule();
}

dword // milliseconds
Task::getWakeUpTime(void) const
{
return _wakeUpTime;
}

bool
Task::ended(void) const
{
return _ended;
}

// void
// Task::main(void)=0

void
Task::_main(Task * task) // Static, Private
{
task->main();
task->_ended=true;
for(;;)
  {
  task->pause();
  }
}

} // namespace PoorOS

//----------------------------------------------------------------------------
