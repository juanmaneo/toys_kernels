//----------------------------------------------------------------------------

#ifndef POOROS_PLATFORM_H
#define POOROS_PLATFORM_H 1

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

typedef unsigned int size_t;
typedef int ssize_t;

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

#define syscall_0(result,number) \
        asm volatile ("int $0x1F" : "=a" (result) \
                                  : "a" (number))

#define syscall_1(result,number,arg1) \
        asm volatile ("int $0x1F" : "=a" (result) \
                                  : "a" (number)  \
                                  , "b" (arg1))

#define syscall_2(result,number,arg1) \
        asm volatile ("int $0x1F" : "=a" (result) \
                                  : "a" (number)  \
                                  , "b" (arg1)    \
                                  , "c" (arg2))

#define syscall_3(result,number,arg1,arg2,arg3)   \
        asm volatile ("int $0x1F" : "=a" (result) \
                                  : "a" (number)  \
                                  , "b" (arg1)    \
                                  , "c" (arg2)    \
                                  , "d" (arg3))

#endif // POOROS_PLATFORM_H

//----------------------------------------------------------------------------
