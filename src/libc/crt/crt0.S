/* TINOS Operating System
 * Copyright (c) 1996, 1997 Bart Sekura
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
 * $Id: crt0.S,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

	.data

argc:	.long	0
	.space	64
argv:	.space	8*4

	.text
	.globl 	_start
_start:

	pushl	$argv
	pushl	argc
	call	main

	addl	$8, %esp
	
_finito:
	jmp	_finito

