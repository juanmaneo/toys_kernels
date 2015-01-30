/*============================================================================
TASK CREATION AND SCHEDULING

EXPORTS:
extern task_t *_curr_task;

void schedule(void);
void init_tasks(void);
============================================================================*/
#include <setjmp.h> /* setjmp(), longjmp() */
#include "_krnl.h" /* console_t */

/* IMPORTS
from MAIN.C */
void kprintf(const char *fmt, ...);

/* from VIDEO.C */
extern console_t _vc[];

/* from TASKS.C */
void task1(void);
void task2(void);
void task3(void);
void task4(void);

#define	MAX_TASK	16
#define	USER_STACK_SIZE	512
/* jmp_buf (E)IP and (E)SP register names for various environments:
			Tinylib		TurboC	DJGPP	Linux (glibc5) */
#define	JMPBUF_IP	eip	/*	j_ip	__eip	__pc	*/
#define	JMPBUF_SP	esp	/*	j_sp	__esp	__sp	*/
#define	JMPBUF_FLAGS	eflags  /*	j_flag	__eflags ?	*/

task_t *_curr_task;
static task_t _tasks[MAX_TASK];
/*****************************************************************************
*****************************************************************************/
void schedule(void)
{
	static unsigned current;
/**/

/* setjmp() returning nonzero means we came here through hyperspace
from the longjmp() below -- just return */
	if(setjmp(_curr_task->state) != 0)
		return;
/* find next runnable task */
	do
	{
		current++;
		if(current >= MAX_TASK)
			current = 0;
		_curr_task = _tasks + current;
	} while(_curr_task->status != TS_RUNNABLE);
/* jump to new task */
	longjmp(_curr_task->state, 1);
}
/*****************************************************************************
*****************************************************************************/
#define	NUM_TASKS	5

void init_tasks(void)
{
	static unsigned char stacks[NUM_TASKS][USER_STACK_SIZE];
	static unsigned entry[NUM_TASKS] =
	{
		0,			(unsigned)task1,
		(unsigned)task2,	(unsigned)task3,
		(unsigned)task4
	};
/**/
	unsigned adr, i;

	kprintf("init_tasks:\n");
/* for the user tasks, initialize saved state... */
	for(i = 1; i < NUM_TASKS; i++)
	{
		(void)setjmp(_tasks[i].state);
/* ...especially the stack pointer */
		adr = (unsigned)(stacks[i] + USER_STACK_SIZE);
		_tasks[i].state[0].JMPBUF_SP = adr;
/* ...and program counter */
		_tasks[i].state[0].JMPBUF_IP = entry[i];
/* set EFLAGS value to enable interrupts */
		_tasks[i].state[0].JMPBUF_FLAGS = 0x200;
/* allocate a virtual console to this task */
		_tasks[i].vc = _vc + i;
/* mark it runnable */
		_tasks[i].status = TS_RUNNABLE;
	}
/* mark task #0 (idle task) runnable */
	_tasks[0].status = TS_RUNNABLE;
/* set _curr_task so schedule() will save state of task #0 */
	_curr_task = _tasks + 0;
}
