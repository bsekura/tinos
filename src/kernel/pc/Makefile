include ../makefile.inc

objs	:= pit.o pic.o cons.o font.o reboot.o kb.o fd.o ide.o \
	   nvram.o pci.o

all: pc.o

libpc.a: $(objs)
	ar rsc $@ $(objs)

pc.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) libpc.a pc.o *~

depend:
	makedepend $(INCLUDE_DIR) *.c
# DO NOT DELETE

cons.o: ../../../include/tinos/pc/cons.h ../../../include/tinos/pc/video.h
cons.o: ../../../include/sys/types.h ../../../include/string.h
cons.o: ../../../include/bcopy.h ../../../include/tinos/i386/inlines.h
fd.o: ../../../include/sys/types.h ../../../include/tinos/thread.h
fd.o: ../../../include/tinos/i386/setjmp.h ../../../include/tinos/i386/hat.h
fd.o: ../../../include/tinos/i386/inlines.h ../../../include/tinos/vm/vas.h
fd.o: ../../../include/tinos/vm/page.h ../../../include/tinos/i386/page.h
fd.o: ../../../include/sys/bits.h ../../../include/tinos/sema.h
fd.o: ../../../include/tinos/msg.h
font.o: ../../../include/tinos/i386/inlines.h ../../../include/sys/types.h
font.o: ../../../include/tinos/i386/spl.h ../../../include/string.h
font.o: ../../../include/bcopy.h
ide.o: ../../../include/tinos/sema.h ../../../include/tinos/pc/ide.h
ide.o: ../../../include/sys/types.h ../../../include/tinos/i386/inlines.h
kb.o: ../../../include/sys/types.h ../../../include/tinos/i386/inlines.h
kb.o: ../../../include/tinos/i386/spl.h ../../../include/tinos/thread.h
kb.o: ../../../include/tinos/i386/setjmp.h ../../../include/tinos/i386/hat.h
kb.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/vm/page.h
kb.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
kb.o: ../../../include/tinos/sema.h ../../../include/tinos/msg.h
kb.o: ../../../include/tinos/pc/kb.h
nvram.o: ../../../include/tinos/pc/nvram.h
nvram.o: ../../../include/tinos/i386/inlines.h ../../../include/sys/types.h
pci.o: ../../../include/sys/types.h
pic.o: ../../../include/tinos/pc/pic.h ../../../include/sys/types.h
pic.o: ../../../include/sys/bits.h ../../../include/tinos/i386/inlines.h
pit.o: ../../../include/tinos/pc/pit.h ../../../include/tinos/i386/inlines.h
pit.o: ../../../include/sys/types.h
reboot.o: ../../../include/tinos/i386/inlines.h ../../../include/sys/types.h
reboot.o: ../../../include/tinos/pc/kb.h
