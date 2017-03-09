#
# $Id: makefile,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
#

CC= bcc
AS= tasm
CFLAGS= -ml -O2 -I\bc45\include -L\bc45\lib
ASFLAGS= -mx

.c.obj:
	$(CC) $(CFLAGS) -c $*.c
.asm.obj:
	$(AS) $(ASFLAGS) $*.asm

OBJS= load.obj a20.obj jump.obj

boot: 	$(OBJS)
	$(CC) $(CFLAGS) -eboot.exe $(OBJS)

clean:
	del *.obj
	del boot.exe
