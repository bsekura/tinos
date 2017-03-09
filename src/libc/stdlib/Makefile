include ../makefile.inc  

objs	= abs.o atof.o atoi.o atol.o atold.o bsearch.o div.o \
	  labs.o ldiv.o qsort.o rand.o random.o strtod.o \
	  strtol.o strtold.o strtoul.o errno.o

all: stdlib.o

stdlib.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) *.o *~
