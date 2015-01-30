/*--------------------------------------------------------------------------*/

#ifndef PLATFORM_H
#define PLATFORM_H 1

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

typedef unsigned int size_t;
typedef int ssize_t;

typedef unsigned char bool;
#define true 1
#define false 0

#define maskInterrupts() \
        asm volatile ("pushf\n\tcli" : : );

#define unmaskInterrupts() \
        asm volatile ("popf" : : );

#define idle() \
        asm volatile ("hlt" : : );

#define inb(result,port) \
        asm volatile ("inb %%dx,%%al" : "=a" (result) : "d"  (port));

#define outb(port,value) \
        asm volatile ("outb %%al,%%dx" : : "d" (port), "a" (value));

#endif /* PLATFORM_H */

/*--------------------------------------------------------------------------*/
