include ../makefile.inc

objs	:= page.o vas.o

all: vm.o

libvm.a: $(objs)
	ar rsc $@ $(objs)

vm.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) libvm.a vm.o *~

depend:
	makedepend $(INCLUDE_DIR) *.c *.S
# DO NOT DELETE

page.o: ../../../include/sys/types.h ../../../include/tinos/i386/page.h
page.o: ../../../include/sys/bits.h ../../../include/tinos/sema.h
page.o: ../../../include/tinos/vm/page.h ../../../include/tinos/boot.h
page.o: ../../../src/boot/dos/boot.h ../../../include/tinos/pool.h
vas.o: ../../../include/sys/types.h ../../../include/tinos/i386/hat.h
vas.o: ../../../include/tinos/i386/inlines.h
vas.o: ../../../include/tinos/i386/spl.h ../../../include/tinos/i386/seg.h
vas.o: ../../../include/tinos/sema.h ../../../include/tinos/pool.h
vas.o: ../../../include/tinos/queue.h ../../../include/tinos/vm/page.h
vas.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
vas.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/thread.h
vas.o: ../../../include/tinos/i386/setjmp.h
