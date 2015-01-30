##############################################################################

all : cdrom.img

#---- floppy/cdrom images ----

floppy.img : main.asm boot.asm
	@echo
	nasm -f bin main.asm -o floppy.img
	@echo

cdrom.img : floppy.img
	@echo
	mkhybrid -b floppy.img -boot-load-size 2880 -o cdrom.img floppy.img
	@echo

#---- execution ----

runFloppy : floppy.img
	@echo
	qemu -fda floppy.img -boot a
	@echo

runCdrom : cdrom.img
	@echo
	qemu -cdrom cdrom.img -boot d
	@echo

run : runCdrom

#---- cleaning ----

clean :
	rm -rf floppy.img cdrom.img

##############################################################################
