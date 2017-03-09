include ../makefile.inc

objs	:= boot.o irq.o main.o msg.o pool.o proc.o \
	   sched.o sema.o syscon.o thread.o syscall.o  

all: kern.o

libkern.a: $(objs)
	ar rsc $@ $(objs)

kern.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) libkern.a kern.o *~

depend:
	makedepend $(INCLUDE_DIR) *.c *.S
# DO NOT DELETE

boot.o: ../../../include/sys/types.h ../../../include/sys/coff.h
boot.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
boot.o: ../../../include/tinos/pc/nvram.h
boot.o: ../../../include/tinos/i386/inlines.h ../../../include/tinos/boot.h
boot.o: ../../../src/boot/dos/boot.h ../../../include/tinos/i386/multiboot.h
boot.o: ../../../include/tinos/i386/mb_info.h ../../../include/stdio.h
boot.o: ../../../include/stdarg.h ../../../include/string.h
boot.o: ../../../include/bcopy.h
irq.o: ../../../include/tinos/irq.h ../../../include/sys/types.h
irq.o: ../../../include/tinos/pc/pic.h ../../../include/sys/bits.h
irq.o: ../../../include/tinos/i386/frame.h
main.o: ../../../include/sys/types.h ../../../include/tinos/i386/page.h
main.o: ../../../include/sys/bits.h ../../../include/tinos/i386/spl.h
main.o: ../../../include/tinos/i386/multiboot.h
main.o: ../../../include/tinos/i386/mb_info.h ../../../include/tinos/thread.h
main.o: ../../../include/tinos/i386/setjmp.h
main.o: ../../../include/tinos/i386/hat.h
main.o: ../../../include/tinos/i386/inlines.h ../../../include/tinos/vm/vas.h
main.o: ../../../include/tinos/vm/page.h ../../../include/tinos/sema.h
main.o: ../../../include/tinos/irq.h
msg.o: ../../../include/stdio.h ../../../include/stdarg.h
msg.o: ../../../include/tinos/thread.h ../../../include/sys/types.h
msg.o: ../../../include/tinos/i386/setjmp.h ../../../include/tinos/i386/hat.h
msg.o: ../../../include/tinos/i386/inlines.h ../../../include/tinos/vm/vas.h
msg.o: ../../../include/tinos/vm/page.h ../../../include/tinos/i386/page.h
msg.o: ../../../include/sys/bits.h ../../../include/tinos/sema.h
msg.o: ../../../include/tinos/sched.h ../../../include/tinos/i386/spl.h
msg.o: ../../../include/tinos/msg.h ../../../include/tinos/pool.h
pool.o: ../../../include/sys/types.h ../../../include/tinos/pool.h
pool.o: ../../../include/tinos/sema.h ../../../include/tinos/i386/page.h
pool.o: ../../../include/sys/bits.h
proc.o: ../../../include/tinos/proc.h ../../../include/sys/types.h
proc.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/vm/page.h
proc.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
proc.o: ../../../include/tinos/sema.h ../../../include/tinos/i386/hat.h
proc.o: ../../../include/tinos/i386/inlines.h ../../../include/tinos/thread.h
proc.o: ../../../include/tinos/i386/setjmp.h ../../../include/tinos/boot.h
proc.o: ../../../src/boot/dos/boot.h ../../../include/tinos/pool.h
proc.o: ../../../include/tinos/queue.h ../../../include/string.h
proc.o: ../../../include/bcopy.h ../../../include/tinos/i386/seg.h
sched.o: ../../../include/tinos/i386/tss.h ../../../include/sys/types.h
sched.o: ../../../include/tinos/i386/frame.h
sched.o: ../../../include/tinos/i386/inlines.h
sched.o: ../../../include/tinos/i386/spl.h ../../../include/tinos/sema.h
sched.o: ../../../include/tinos/thread.h ../../../include/tinos/i386/setjmp.h
sched.o: ../../../include/tinos/i386/hat.h ../../../include/tinos/vm/vas.h
sched.o: ../../../include/tinos/vm/page.h ../../../include/tinos/i386/page.h
sched.o: ../../../include/sys/bits.h ../../../include/tinos/queue.h
sema.o: ../../../include/tinos/i386/inlines.h ../../../include/sys/types.h
sema.o: ../../../include/tinos/i386/spl.h ../../../include/tinos/sema.h
sema.o: ../../../include/tinos/thread.h ../../../include/tinos/i386/setjmp.h
sema.o: ../../../include/tinos/i386/hat.h ../../../include/tinos/vm/vas.h
sema.o: ../../../include/tinos/vm/page.h ../../../include/tinos/i386/page.h
sema.o: ../../../include/sys/bits.h ../../../include/tinos/sched.h
sema.o: ../../../include/tinos/queue.h
syscall.o: ../../../include/tinos/msg.h ../../../include/sys/types.h
syscall.o: ../../../include/tinos/sema.h
syscon.o: ../../../include/sys/types.h ../../../include/string.h
syscon.o: ../../../include/bcopy.h ../../../include/tinos/pc/cons.h
syscon.o: ../../../include/tinos/pc/video.h ../../../include/tinos/thread.h
syscon.o: ../../../include/tinos/i386/setjmp.h
syscon.o: ../../../include/tinos/i386/hat.h
syscon.o: ../../../include/tinos/i386/inlines.h
syscon.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/vm/page.h
syscon.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
syscon.o: ../../../include/tinos/sema.h ../../../include/tinos/pool.h
syscon.o: ../../../include/tinos/msg.h
thread.o: ../../../include/tinos/thread.h ../../../include/sys/types.h
thread.o: ../../../include/tinos/i386/setjmp.h
thread.o: ../../../include/tinos/i386/hat.h
thread.o: ../../../include/tinos/i386/inlines.h
thread.o: ../../../include/tinos/vm/vas.h ../../../include/tinos/vm/page.h
thread.o: ../../../include/tinos/i386/page.h ../../../include/sys/bits.h
thread.o: ../../../include/tinos/sema.h ../../../include/tinos/pool.h
thread.o: ../../../include/tinos/proc.h ../../../include/tinos/i386/exec.h
thread.o: ../../../include/tinos/boot.h ../../../src/boot/dos/boot.h
thread.o: ../../../include/tinos/i386/spl.h
