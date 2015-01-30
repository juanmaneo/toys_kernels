.SUFFIXES: .asm

# defines
MAKEFILE=ming.mak
MAKEDEP=$(MAKEFILE)
INCDIR	=../inc
LDSCRIPT=../krnl1m.ld
NASM	=nasm -f win32 -dUNDERBARS=1 -i$(INCDIR)/
CC	=gcc -g -Wall -W -O2 -nostdinc -fno-builtin -I$(INCDIR)
LD	=ld -g -T $(LDSCRIPT) -nostdlib
LIBC	=../lib/libc.a
OBJS	=kstart.o main.o video.o debug.o

# targets
all: krnl.x

install: krnl.x
	command /c copy /y krnl.x a:\osd6.x

clean:
	deltree /y *.o krnl.x krnl.sym krnl.lst

# implicit rules
.asm.o:
	$(NASM) -o$@ $<

.c.o:
	$(CC) -c -o$@ $<

# dependencies
kstart.o:	kstart.asm	$(MAKEDEP)

main.o:		main.c		$(MAKEDEP)

video.o:	video.c		$(MAKEDEP)

debug.o:	debug.c		$(MAKEDEP)

# explicit rules
$(LIBC): ../lib/$(MAKEFILE)
	make -C ../lib -f $(MAKEFILE)

krnl.x: $(OBJS) $(LDSCRIPT) $(LIBC)	$(MAKEDEP)
	$(LD) -o$@ $(OBJS) $(LIBC)
	objdump --source $@ >krnl.lst
	nm --line-numbers $@ | sort >krnl.sym
	strip $@
