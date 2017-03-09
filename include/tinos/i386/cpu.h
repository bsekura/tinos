/* TINOS Operating System
 * Copyright (c) 1996, 1997, 1998 Bart Sekura
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and 
 * this permission notice appear in all copies of the software, 
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * $Id: cpu.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __cpu_h__
#define __cpu_h__

/*
 * i386 processor related stuff (register bits, etc.)
 */
 
/*
 * i386 eflags register bits
 */
 
#define	EFL_INTS	(0x00000200)
#define	EFL_IOPL	(0x00003000)
#define	EFL_NT		(0x00004000)
#define	EFL_V86		(0x00020000)

#endif /* __cpu_h__ */
