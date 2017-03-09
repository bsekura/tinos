# TINOS Operating System
# Copyright (c) 1996, 1997, 1998 Bart Sekura
# All Rights Reserved. 
#
# default rules
#
# $Id: makefile.inc,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
#

%.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

#
# defines
#

CC 		= gcc
LD		= ld
DEFS		=
OPTS		= -O2  -fomit-frame-pointer
MACHOPTS	= -march=pentium
INCLUDE_DIR	= -I../../../include -I../../../include/tinos

CFLAGS	= $(DEFS) -fno-builtin -nostdinc $(OPTS) $(INCLUDE_DIR)
LDFLAGS = -nostdlib -Ttext 0x100000
