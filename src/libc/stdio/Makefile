include ../makefile.inc  

objs	= vsprintf.o

all: stdio.o

stdio.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) *.o *~
