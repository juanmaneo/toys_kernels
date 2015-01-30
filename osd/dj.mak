MAKEFILE=dj.mak

all:
	make -f $(MAKEFILE) -C osd3
	make -f $(MAKEFILE) -C osd4
	make -f $(MAKEFILE) -C osd5
	make -f $(MAKEFILE) -C osd6
	make -f $(MAKEFILE) -C osd7
	make -f $(MAKEFILE) -C osd8
	make -f $(MAKEFILE) -C osd9

install:
	make -f $(MAKEFILE) -C osd3 install
	make -f $(MAKEFILE) -C osd4 install
	make -f $(MAKEFILE) -C osd5 install
	make -f $(MAKEFILE) -C osd6 install
	make -f $(MAKEFILE) -C osd7 install
	make -f $(MAKEFILE) -C osd8 install
	make -f $(MAKEFILE) -C osd9 install

clean:
	make -f $(MAKEFILE) -C lib  clean
	make -f $(MAKEFILE) -C osd3 clean
	make -f $(MAKEFILE) -C osd4 clean
	make -f $(MAKEFILE) -C osd5 clean
	make -f $(MAKEFILE) -C osd6 clean
	make -f $(MAKEFILE) -C osd7 clean
	make -f $(MAKEFILE) -C osd8 clean
	make -f $(MAKEFILE) -C osd9 clean
