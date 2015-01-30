/*--------------------------------------------------------------------------*/

#ifndef TIMER_H
#define TIMER_H

#include "platform.h"

void
initTimer(void);

dword /* milliseconds */
getTime(void);

void
timerInterruptHandler(void);

#endif /* TIMER_H */

/*--------------------------------------------------------------------------*/
