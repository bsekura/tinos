#
# TINOS Operating System
# Copyright (c) 1998 Bart Sekura
#
# Boot sector code makefile
#
# $Id: Makefile,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
#

all:	bootsect

bootsect: boot

boot: boot.o
	ld -o boot -Ttext 0x7c00 -e _start boot.o

bootsect:
	dd if=boot of=bootimg skip=6
	dd if=bootimg of=bootsect count=1

floppy: bootsect
	dd if=bootsect of=/dev/fd0

img: bootsect kern
	cat bootsect kern > img
	dd if=img of=/dev/fd0

kern: kernel
	dd if=kernel of=kern bs=116 skip=1

clean:
	rm -f *.o *~ boot bootsect bootimg
