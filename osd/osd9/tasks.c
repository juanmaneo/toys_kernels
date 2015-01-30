/*============================================================================
TASKS

EXPORTS:
void task1(void);
void task2(void);
void task3(void);
void task4(void);
============================================================================*/
#include "_krnl.h"

/* IMPORTS
from VIDEO.C */
void putch_help(console_t *con, unsigned c);

/* from SCHED.C */
extern task_t *_curr_task;

void schedule(void);
/*****************************************************************************
*****************************************************************************/
static int write(const unsigned char *str, unsigned len)
{
	unsigned i;

	for(i = 0; i < len; i++)
	{
		putch_help(_curr_task->vc, *str);
		str++;
	}
	return i;
}
/*****************************************************************************
*****************************************************************************/
static void yield(void)
{
	schedule();
}
/*****************************************************************************
*****************************************************************************/
#define	WAIT	0xFFFFFL

static void wait(void)
{
	unsigned long wait;

	for(wait = WAIT; wait != 0; wait--)
		/* nothing */;
}
/*****************************************************************************
*****************************************************************************/
void task1(void)
{
	static const unsigned char msg_a[] = "hello from task 1 ";
	static const unsigned char msg_b[] = "task 1 ";
/**/

	write(msg_a, sizeof(msg_a));
	wait();
	while(1)
	{
		yield();
		write(msg_b, sizeof(msg_b));
		wait();
	}
}
/*****************************************************************************
*****************************************************************************/
void task2(void)
{
	static const unsigned char msg_a[] = "hola de tarea 2 ";
	static const unsigned char msg_b[] = "tarea 2 ";
/**/

	write(msg_a, sizeof(msg_a));
	wait();
	while(1)
	{
		yield();
		write(msg_b, sizeof(msg_b));
		wait();
	}
}
/*****************************************************************************
*****************************************************************************/
void task3(void)
{
	static const unsigned char msg_a[] = "Hallo von Aufgabe 3 ";
	static const unsigned char msg_b[] = "Aufgabe 3 ";
/**/

	write(msg_a, sizeof(msg_a));
	wait();
	while(1)
	{
		yield();
		write(msg_b, sizeof(msg_b));
		wait();
	}
}
/*****************************************************************************
this character -->	ƒ
is the reason we use unsigned char instead of char
*****************************************************************************/
void task4(void)
{
	static const unsigned char msg_a[] = "Bonjour de tƒche 4 ";
	static const unsigned char msg_b[] = "tƒche 4  ";
/**/

	write(msg_a, sizeof(msg_a));
	wait();
	while(1)
	{
		yield();
		write(msg_b, sizeof(msg_b));
		wait();
	}
}
