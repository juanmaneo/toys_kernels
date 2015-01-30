.SUFFIXES: .asm

# defines
MAKEFILE=linux.mak
MAKEDEP=$(MAKEFILE)
LDSCRIPT=../krnl1m.ld
NASM	=nasm -f elf
CC	=gcc -g -Wall -W -O2 -nostdinc -fno-builtin
LD	=ld -g -T $(LDSCRIPT) -nostdlib
OBJS	=kstart.o main.o

# targets
all: krnl.x

install: krnl.x
	-umount ../mnt
	mount /dev/fd0 ../mnt
	cp -f krnl.x ../mnt/osd3.x

clean:
	rm -f *.o krnl.x krnl.sym krnl.lst

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
	nm $@ | sort >krnl.sym
	strip $@
