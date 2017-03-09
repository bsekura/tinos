# TINOS Operating System
# Copyright (c) 1996, 1997, 1998 Bart Sekura
# All Rights Reserved.
# 
# top level makefile
#
# $Id: Makefile,v 1.2 1998/03/01 21:11:18 bart Exp $
#

dirs	= kernel libc test

all:
	@for dir in $(dirs); \
		do (cd $$dir && make all) || exit 1; \
	done


clean:
	@for dir in $(dirs); \
		do (cd $$dir && make clean); \
	done
	rm -f *~

	