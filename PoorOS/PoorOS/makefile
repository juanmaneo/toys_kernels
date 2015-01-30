##############################################################################

TARGET=mySystem

TEXT_ORG=0x800
MEM_SIZE=128
GFX_MODE=0

all : cdrom.img

#---- floppy/cdrom images ----

floppy.img : ${TARGET} PoorOS/boot.asm
	@echo
	nasm -DLOAD_ADDR=${TEXT_ORG} \
             -DBIN_SIZE=`ls -l ${TARGET} | awk '{print $$5}'` \
             -DGFX_MODE=${GFX_MODE} \
             -f bin -o boot PoorOS/boot.asm
	@# to be read, the last used sector must be filled up to 512 bytes
	dd if=boot of=floppy.img bs=512 conv=sync
	dd if=${TARGET} of=floppy.img bs=512 conv=sync seek=1
	@echo

cdrom.img : floppy.img
	@echo
	mkhybrid -b floppy.img -boot-load-size 2880 -o cdrom.img floppy.img
	@echo

#---- execution ----

runFloppy : floppy.img
	@echo
	qemu -m ${MEM_SIZE} -fda floppy.img -boot a -soundhw pcspk
	@echo

runCdrom : cdrom.img
	@echo
	qemu -m ${MEM_SIZE} -cdrom cdrom.img -boot d -soundhw pcspk
	@echo

run : runCdrom

#---- source files ----

POOROS_FILES=PoorOS/system.cpp \
             PoorOS/utils.cpp \
             PoorOS/task.cpp \
             PoorOS/console.cpp

FILES=${POOROS_FILES} \
      ${TARGET}.cpp

##############################################################################

CC=g++ -nostdinc -nostdinc++ \
       -fno-use-cxa-atexit -fno-builtin \
       -fno-rtti -fno-exceptions

LD=ld -nostdlib -T PoorOS/linker.ld

DEPFLAGS=-MM

CCFLAGS=-W -Wall -pedantic -I. -O3 -DGFX_MODE=${GFX_MODE}

LDFLAGS=-Ttext ${TEXT_ORG}

##############################################################################

#---- object files ----

OBJECTS=${FILES:.cpp=.o}

#---- startup ----

PoorOS/start.o : PoorOS/start.asm
	@echo
	@echo "======== PoorOS/start.asm ========"
	nasm -f aout -DMEM_SIZE=${MEM_SIZE} \
             -o PoorOS/start.o PoorOS/start.asm
	@echo

#---- link ----

${TARGET} : ${OBJECTS} PoorOS/start.o PoorOS/linker.ld
	@echo
	@echo "======== ${TARGET} ========"
	${LD} -o ${TARGET} ${OBJECTS} ${LDFLAGS}
	@echo

#---- default rule (.cpp --> .o) ----

.cpp.o :
	@echo
	@echo "======== $< ========"
	${CC} ${CCFLAGS} -c $< -o $*.o
	@echo

#---- dependencies ----

-include .depend

dep : ${FILES}
	@echo "======== Building dependencies ========"
	@rm -f .depend
	@for i in ${FILES} ; do \
	  echo $$i ; \
	  ${CC} ${DEPFLAGS} ${CCFLAGS} $$i > .tmpdepend ; \
	  OBJNAME=`echo $$i | sed -e s%\\\.cpp%.o% ` ; \
	  cat .tmpdepend | \
	  sed -e s%`basename $$i .cpp`\\\.o%$$OBJNAME% >> .depend ; \
	  echo  >> .depend ; \
	done
	@rm -f .tmpdepend

#---- cleaning ----

clean :
	rm -f ${TARGET} PoorOS/start.o ${OBJECTS} .depend \
              cdrom.img floppy.img boot
	touch * PoorOS/*

##############################################################################
