include ../makefile.inc

objs	:= start.o gdt.o idt.o tss.o machtrap.o hat.o \
	   bcopy.o exec.o syscall.o migrate.o

all: i386.o

libi386.a: $(objs)
	ar rsc $@ $(objs)

i386.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) libi386.a i386.o *~

depend:
	makedepend $(INCLUDE_DIR) *.c *.S
# DO NOT DELETE

exec.o: ../../../include/sys/types.h ../../../include/tinos/i386/elf.h
exec.o: ../../../include/tinos/i386/hat.h
exec.o: ../../../include/tinos/i386/inlines.h
exec.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
exec.o: ../../../include/tinos/i386/seg.h ../../../include/tinos/i386/tss.h
exec.o: ../../../include/tinos/i386/spl.h ../../../include/tinos/i386/cpu.h
exec.o: ../../../include/tinos/thread.h ../../../include/tinos/i386/setjmp.h
exec.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/vm/page.h
exec.o: ../../../include/tinos/sema.h ../../../include/tinos/i386/exec.h
exec.o: ../../../include/tinos/boot.h ../../../src/boot/dos/boot.h
gdt.o: ../../../include/tinos/i386/desc.h ../../../include/sys/types.h
gdt.o: ../../../include/sys/bits.h ../../../include/tinos/i386/seg.h
hat.o: ../../../include/tinos/i386/hat.h ../../../include/sys/types.h
hat.o: ../../../include/tinos/i386/inlines.h
hat.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
hat.o: ../../../include/tinos/i386/frame.h ../../../include/tinos/i386/spl.h
hat.o: ../../../include/tinos/boot.h ../../../src/boot/dos/boot.h
hat.o: ../../../include/tinos/thread.h ../../../include/tinos/i386/setjmp.h
hat.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/vm/page.h
hat.o: ../../../include/tinos/sema.h
idt.o: ../../../include/tinos/i386/desc.h ../../../include/sys/types.h
idt.o: ../../../include/sys/bits.h ../../../include/tinos/i386/seg.h
idt.o: ../../../include/tinos/i386/inlines.h
migrate.o: ../../../include/tinos/i386/inlines.h ../../../include/sys/types.h
syscall.o: ../../../include/tinos/i386/frame.h
syscall.o: ../../../include/tinos/i386/seg.h
tss.o: ../../../include/sys/types.h ../../../include/tinos/i386/tss.h
tss.o: ../../../include/tinos/i386/desc.h ../../../include/sys/bits.h
tss.o: ../../../include/tinos/i386/seg.h
tss.o: ../../../include/tinos/i386/inlines.h
machtrap.o: ../../../include/tinos/i386/seg.h
