.SUFFIXES: .asm

# defines
MAKEFILE=dj.mak
MAKEDEP=$(MAKEFILE)
LDSCRIPT=../krnl1m.ld
NASM	=nasm -f coff -dUNDERBARS=1
CC	=gcc -g -Wall -W -O2 -nostdinc -fno-builtin
LD	=ld -g -T $(LDSCRIPT) -nostdlib
OBJS	=kstart.o main.o

# targets
all: krnl.x

install: krnl.x
	copy /y krnl.x a:\osd3.x

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

# explicit rules
krnl.x: $(OBJS) $(LDSCRIPT)	$(MAKEDEP)
	$(LD) -o$@ $(OBJS)
	objdump --source $@ >krnl.lst
	nm --line-numbers $@ | sort >krnl.sym
	strip $@
