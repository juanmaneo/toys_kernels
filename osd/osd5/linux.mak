.SUFFIXES: .asm

# defines
MAKEFILE=linux.mak
MAKEDEP=$(MAKEFILE)
INCDIR	=../inc
LDSCRIPT=../krnl1m.ld
NASM	=nasm -f elf -i$(INCDIR)/
CC	=gcc -g -Wall -W -O2 -nostdinc -fno-builtin -I$(INCDIR)
LD	=ld -g -T $(LDSCRIPT) -nostdlib
LIBC	=../lib/libc.a
OBJS	=kstart.o main.o video.o

# targets
all: krnl.x

install: krnl.x
	-umount ../mnt
	mount /dev/fd0 ../mnt
	cp -f krnl.x ../mnt/osd5.x

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

video.o:	video.c		$(MAKEDEP)

# explicit rules
$(LIBC):
	make -C ../lib -f $(MAKEFILE)

krnl.x: $(OBJS) $(LDSCRIPT) $(LIBC)	$(MAKEDEP)
	$(LD) -o$@ $(OBJS) $(LIBC)
	objdump --source $@ >krnl.lst
	nm $@ | sort >krnl.sym
	strip $@
