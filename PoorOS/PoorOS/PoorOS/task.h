//----------------------------------------------------------------------------

#ifndef POOROS_TASK_H
#define POOROS_TASK_H 1

#include "PoorOS/platform.h"

namespace PoorOS {

class System;

class Task
{
public:

  Task(void);

  virtual
  ~Task(void);

  virtual
  void
  pause(dword milliseconds=0xFFFFFFFF);

  virtual
  dword // milliseconds
  getWakeUpTime(void) const;

  virtual
  bool
  ended(void) const;

  virtual
  void
  main(void)=0;
  
protected:

  friend class System;

  byte * _stackSegment;
  byte * _stackPointer;
  dword _wakeUpTime;
  bool _ended;

private:

  static
  void
  _main(Task * task);

};

} // namespace PoorOS

#endif // POOROS_TASK_H

//----------------------------------------------------------------------------
