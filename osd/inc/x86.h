#ifndef __TL_X86_H
#define	__TL_X86_H

#ifdef __cplusplus
extern "C"
{
#endif

unsigned inportb(unsigned short port);
void outportb(unsigned port, unsigned val);
unsigned disable(void);
void enable(void);

#ifdef __cplusplus
}
#endif

#endif

