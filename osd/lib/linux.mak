.SUFFIXES: .asm

# defines
MAKEFILE=linux.mak
MAKEDEP=$(MAKEFILE)
INCDIR	=../inc
NASM	=nasm -f elf -i$(INCDIR)/
CC	=gcc -g -Wall -W -O2 -nostdinc -fno-builtin -I$(INCDIR)

STDIO	=stdio/doprintf.o

STRING	=string/memsetw.o string/memcpy.o string/strlen.o

X86	=x86/inportb.o x86/outportb.o x86/disable.o x86/enable.o

CTYPE	=ctype/ctype.o

SETJMP	=setjmp/setjmp.o setjmp/longjmp.o

OBJS	=$(STDIO) $(STRING) $(X86) $(CTYPE) $(SETJMP)

# targets
all: libc.a

clean:
	rm -f libc.a $(OBJS)

# implicit rules
.asm.o:
	$(NASM) -o$@ $<

.c.o:
	$(CC) -c -o$@ $<

# dependencies
stdio/doprintf.o:	stdio/doprintf.c	$(MAKEDEP)

string/memsetw.o:	string/memsetw.c	$(MAKEDEP)

string/memcpy.o:	string/memcpy.c		$(MAKEDEP)

string/strlen.o:	string/strlen.c		$(MAKEDEP)

x86/inportb.o:		x86/inportb.c		$(MAKEDEP)

x86/outportb.o:		x86/outportb.c		$(MAKEDEP)

x86/disable.o:		x86/disable.c		$(MAKEDEP)

x86/enable.o:		x86/enable.c		$(MAKEDEP)

ctype/ctype.o:		ctype/ctype.c		$(MAKEDEP)

setjmp/setjmp.o:	setjmp/setjmp.asm	$(MAKEDEP)

setjmp/longjmp.o:	setjmp/longjmp.c	$(MAKEDEP)

# explicit rules
libc.a:	$(OBJS) $(MAKEDEP)
	ar rcs libc.a $(OBJS)
