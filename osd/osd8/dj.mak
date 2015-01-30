.SUFFIXES: .asm

# defines
MAKEFILE=dj.mak
MAKEDEP=$(MAKEFILE)
INCDIR	=../inc
LDSCRIPT=../krnl1m.ld
NASM	=nasm -f coff -dUNDERBARS=1 -i$(INCDIR)/
CC	=gcc -g -Wall -W -O2 -nostdinc -fno-builtin -I$(INCDIR)
LD	=ld -g -T $(LDSCRIPT) -nostdlib
LIBC	=../lib/libc.a
OBJS	=kstart.o main.o video.o debug.o kbd.o

# targets
all: krnl.x

install: krnl.x
	copy /y krnl.x a:\osd8.x

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

kbd.o:		kbd.c		$(MAKEDEP)

# explicit rules
$(LIBC): ../lib/$(MAKEFILE)
	make -C ../lib -f $(MAKEFILE)

krnl.x: $(OBJS) $(LDSCRIPT) $(LIBC)	$(MAKEDEP)
	$(LD) -o$@ $(OBJS) $(LIBC)
	objdump --source $@ >krnl.lst
	nm --line-numbers $@ | sort >krnl.sym
	strip $@
