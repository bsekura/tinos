include ../makefile.inc

objs	:= blk.o buf.o ext2fs.o hash.o

all: dev.o

dev.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) *.o *~

depend:
	makedepend $(INCLUDE_DIR) *.c
# DO NOT DELETE

blk.o: ../../../include/sys/types.h ../../../include/tinos/pc/ide.h
