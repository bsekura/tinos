include ../makefile.inc

objs	:= basename.o bcmp.o ffs.o index.o memccpy.o memchr.o memset.o \
	   memcpy.o memcmp.o rindex.o stpcpy.o strcat.o strchr.o \
	   strcmp.o strcoll.o strcspn.o strdup.o stricmp.o \
	   strlen.o strlwr.o strncat.o strncmp.o strncpy.o strnicmp.o \
	   strpbrk.o strrchr.o strsep.o strspn.o strstr.o strtok.o \
	   strupr.o strxfrm.o

all: string.o

string.o: $(objs)
	ld -r -o $@ $(objs)

clean:
	rm -f $(objs) string.o

