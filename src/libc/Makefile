# TINOS Operating System
# Copyright (c) 1996, 1997, 1998 Bart Sekura
# All Rights Reserved.
# 
# top level kernel makefile
#
# $Id: Makefile,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
#

include makefile.inc

subdirs 	:= crt ctype mach stdio stdlib string

all: install

#
# do all makefiles in all subdirectories
#
subdirs :
	@(for dir in $(subdirs); \
		do (echo Building $$dir; cd $$dir; make; cd ..); \
	done)

dep:
	@(for dir in $(subdirs); \
		do (cd $$dir; make depend; cd ..); \
	done)
	

libc.a: subdirs
	ar -rs $@ \
		ctype/ctype.o mach/mach.o string/string.o \
		stdio/stdio.o stdlib/stdlib.o

install: libc.a
	cp crt/crt0.o ../../lib
	cp libc.a ../../lib

clean:
	@(for dir in $(subdirs); \
		do (echo Cleaning $$dir; cd $$dir; make clean; cd ..); \
	done)	
	rm -f libc.a *~

