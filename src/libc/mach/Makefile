include ../makefile.inc  
CFLAGS	+= -I../../include/tinos

objs	= cons.o bcopy.o

all: mach.o

mach.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) *.o *~
