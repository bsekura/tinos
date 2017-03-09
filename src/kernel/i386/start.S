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
 * kernel starts its life here
 *
 * $Id: start.S,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

/*
 * initial stack
 */
	.data       

   	.globl	_os_stack
   	.align 	4, 0x90
   	.space 	0x100
_os_stack:  
   	.long 0

/*
 * let's roll
 */   
	.text

	.globl 	_start
	.align 	4, 0x90
_start:
	jmp	boot_entry

	/*
	 * multiboot compliant header 
 	 * the layout of this thing depends on image format
	 * for aout formats we need to specify more
	 * alignment is important also 
	 */

	.align 4, 0x90
boot_hdr: 

	.long	0x1BADB002		/* magic */
#ifdef __ELF__
	.long   0x00000000		/* no flags if ELF */
	.long	0-0x1BADB002-0x00000000	/* checksum */
#else   /* aout kludge */
	.long	0x00010000		/* feature flags */
	.long	0-0x1BADB002-0x00010000	/* checksum */
	.long	boot_hdr		/* header_addr */
	.long	_start			/* load_addr */
	.long	edata			/* load_end_addr */
	.long	end			/* bss_end_addr */
	.long	boot_entry		/* entry */
#endif

	/* the actual code starts here ... */
boot_entry:

	/*
	 * clear NT, learned the hard way...
	 */
   	pushl 	$0
   	popfl

   	/*
    	 * setup our stack
    	 */
   	lea   	_os_stack, %edx
   	movl  	%edx, %esp

	/*
	 * mulitboot compliant loader (read: grub)
	 * should put magic value in %eax
	 */
	pushl	%eax

   	/*
    	 * clear bss
    	 */
   	xorl  	%eax, %eax
   	movl  	$edata, %edi
   	movl  	$end, %ecx
   	subl  	%edi, %ecx
   	cld
   	rep   
   	stosb

   	/*
    	 * call our C code initialization ...
    	 */
   	pushl 	%ebx
   	call  	main

	/*
	 * NOTREACHED
	 */
darn:
	incw	(0xb8000)
	jmp	darn
