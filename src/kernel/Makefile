# TINOS Operating System
# Copyright (c) 1996, 1997, 1998 Bart Sekura
# All Rights Reserved.
# 
# top level kernel makefile
#
# $Id: Makefile,v 1.2 1998/03/01 21:06:02 bart Exp $
#

include makefile.inc

subdirs 	:= c i386 pc kern vm dev

all: install

gcc.ver:
	@echo \#define GCC_VERSION \"`gcc -v 2>&1 | tail -1`\" > boot/gcc.version

#
# do all makefiles in all subdirectories
#
subdirs :
	@for dir in $(subdirs); \
		do (echo Building $$dir; make -C $$dir) || exit 1; \
	done

dep:
	@for dir in $(subdirs); \
		do (make -C $$dir depend); \
	done
	

kernel: subdirs
	$(LD) $(LDFLAGS) \
		i386/i386.o kern/kern.o vm/vm.o pc/pc.o c/c.o dev/dev.o \
		-o kernel

install: kernel
	cp kernel ../../bin/kernel

clean:
	@for dir in $(subdirs); \
		do (echo Cleaning $$dir; make -C $$dir clean); \
	done
	rm -f kernel *~





