include ../makefile.inc

objs	:= ctype.o string.o vsprintf.o

all: c.o

libc.a: $(objs)
	ar rsc $@ $(objs)

c.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) libc.a c.o *~

depend:
	makedepend (INCLUDE_DIR) *.c *.S
# DO NOT DELETE

ctype.o: /usr/include/ctype.h /usr/include/features.h
ctype.o: /usr/include/sys/cdefs.h /usr/include/endian.h
ctype.o: /usr/include/bytesex.h
string.o: /usr/include/limits.h /usr/include/features.h
string.o: /usr/include/sys/cdefs.h /usr/include/posix1_lim.h
string.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
string.o: /usr/include/ctype.h /usr/include/endian.h /usr/include/bytesex.h
string.o: /usr/include/string.h
vsprintf.o: /usr/include/sys/types.h /usr/include/linux/types.h
vsprintf.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
vsprintf.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
vsprintf.o: /usr/include/string.h /usr/include/features.h
vsprintf.o: /usr/include/sys/cdefs.h /usr/include/ctype.h
vsprintf.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/stdio.h
vsprintf.o: /usr/include/libio.h /usr/include/_G_config.h
