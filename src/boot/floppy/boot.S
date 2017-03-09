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
 * $Id: boot.S,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

/*
 * gas generates 32-bit code and our code here will be run
 * by BIOS while the processor is in a real mode. Gas will treat
 * all data and address references as a 32-bit values so we
 * have to prefix all of them using those bytes below.
 */

#define data32	.byte 0x66
#define	addr32	.byte 0x67

SIGNATURE=	0xaa55
BOOTSEG=	0x9000
BOOTSTACK=	0xfff0
SECT_NO=	17
STRACK=		18	/* sectors per track */
TRACKS=		10

SYSSEG=		0x1000

	.file	"boot.S"

	.text	
	.globl	_start
_start:
	/* 
	 * set up %ds 
	 */
	movw	%cs, %ax
	movw	%ax, %ds

	/* 
	 * set up %ss and %esp 
	 */
	data32
	movl	$BOOTSEG, %eax
	movw	%ax, %ss
	data32
	movl	$BOOTSTACK, %esp

	/* 
	 * reset the disk system 
	 */
	xorl	%eax, %eax
	xorl	%edx, %edx
	int	$0x13

	/*
	 * setup initial parameters
	 */
	data32
	movl	$SYSSEG, %eax
	movw	%ax, %es	/* segment */
	xorl	%ebx, %ebx	/* offset */
	movb	$SECT_NO, %al	/* initial number of sectors */

	/*
	 * we start reading from cylinder 0, sector 2
	 * which is right after boot sector you're looking at
	 */
	data32
	movl	$0x0002, %ecx	/* cyl 0, sector 2 */
	xorl	%edx, %edx	/* driver 0x0 (dl) head 0 (dh) */

	xorl	%edi, %edi

/*
 * BIOS call "INT 0x13 Function 0x2" to read sectors from disk into memory
 *	Call with	%ah = 0x2
 *			%al = number of sectors
 *			%ch = cylinder
 *			%cl = sector
 *			%dh = head
 *			%dl = drive (0x80 for hard disk, 0x0 for floppy disk)
 *			%es:%bx = segment:offset of buffer
 *	Return:
 *			%al = number of sectors actually read
 *			%cf = error
 */

load:
	movb	$0x2, %ah	/* function 2 */
	int	$0x13
	data32
	jb	read_error

	/*
 	 * print dot '.'
	 */
	data32
	pusha
	data32
	mov	$1, %ebx
	data32
	mov	$0xe2e, %eax
	int	$0x10
	data32
	popa

	/*
 	 * check whether %di is set
 	 * this indicates we gotta finish the track
         * from previous segment crossing
 	 */
	cmpw	$1, %di
	data32
	jne	forw

	/*
	 * %di is set, read all sectors left within current track
	 */
	xorl	%edi, %edi	/* clear flag */
	movl	%eax, %esi	/* save eax */
	xorl	%eax, %eax
	mov	%es, %ax        
	add	$0x10, %ah      /* update segment %es */
	mov	%ax, %es
	xor	%ebx, %ebx      /* segment offset */
	movl	%esi, %eax	/* restore eax */

	addb	%al, %cl	/* compute start sector */
	movb	$STRACK, %al	/* compute how many sectors left */
	subb	%cl, %al
	incb	%al		/* sector count fixup */
	data32
	jmp	load

forw:
	xorb	%ah, %ah
	shlw	$9, %ax		/* ax = sectors read * 512 */
	addw	%ax, %bx	/* update offset for the next read */

	xorl	%eax, %eax	/* see how much space we have */
	subl	%ebx, %eax	/* within the current segment */

	data32
	cmp	$0x2400, %eax	/* will the track fit? */
	data32
	jge	boundary_ok

#if 0
	/*
 	 * print '-'
	 */
	data32
	pusha
	data32
	mov	$1, %ebx
	data32
	mov	$0xe2d, %eax
	int	$0x10
	data32
	popa
#endif

	shrw	$9, %ax		/* ax = number of sectors we can read */
	data32
	movl	$1, %edi        /* set %di to finish the track later */
	data32
	jmp	go_read	

boundary_ok:
	movb	$STRACK, %al

go_read:
	movb	$1, %cl
	cmpb	$1, %dh		/* head 1? */
	data32
	je	head_0		/* yes, change to zero */
	movb	$1, %dh		/* set head to 1 */
	data32
	jmp	load
head_0:
	xorb	%dh, %dh	/* head = 0 */
	incb	%ch		/* increase track */
	cmpb	$TRACKS, %ch	
	data32
	je	finito
	data32
	jmp	load

finito:

	/* 
	 * kill floppy motor 
 	 */
	data32	
	mov	$0x3f2, %edx
	xorb	%al, %al
	outb	%al, %dx

	/*
	 * show the world we're alive
	 */
	data32
	mov 	$info, %esi
	data32
	call	message	

	/*
	 * enter protected mode
	 */
	cli
	addr32
	data32
	lgdt	gdtr	

	movl	%cr0, %eax
	data32
	orl 	$0x1, %eax
	movl	%eax, %cr0

	data32
	addr32
	ljmp	$0x08, $prot

prot:
	/*
	 * we're in protected mode now
	 * we can forget about those annoying prefixes (addr32, data32)
	 * what we do here is: we enable gate A20, relocate the code 
	 * from 0x10000 (65536) to 0x100000 (1M) and jump there
 	 */

	cld
	movl	$0x10, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	
	movl	$0x10000-4, %esp
	call	a20

	/*
	 * move the stuff we've read where due
 	 */
	movl	$0x10000, %esi
	movl	$0x100000, %edi
	movl	$0x10000, %ecx
	cld
	rep
	movsb

	mov	$0x100000, %eax
	jmp	*%eax

	/*
	 * NOTREACHED
	 */

oh_shit:
	incw	(0xb8000)	
	jmp 	oh_shit


/*
 * read_error
 */
read_error:
	data32
	mov	$err_read, %esi
	data32
	call	message

stop:
	data32
	jmp	stop

/*
 * message: write the error message in %ds:%esi to console
 */
message:
	/*
	 * Use BIOS "int 10H Function 0Eh" to write character in teletype mode
	 *	%ah = 0xe	%al = character
	 *	%bh = page	%bl = foreground color (graphics modes)
	 */

	data32
	pusha

	data32
	mov	$0x0001, %ebx
	cld

nextb:
	lodsb			/* load a byte into %al */
	cmpb	$0x0, %al
	data32
	je	done
	movb	$0xe, %ah
	int	$0x10		/* display a byte */
	data32
	jmp	nextb
done:
	data32
	popa

	data32
	ret

/*
 * enable A20 gate
 * NOTE: this is called after having switched to protected mode
 *	 so no prefixes (data32, addr32) are necessary
 */

a20:
	movw	$0x64, %dx
1:
	inb	%dx, %al
	testb	$2, %al
	jne	1b
	movb	$0x0d1, %al
	outb	%al, %dx

2:
	inb	%dx, %al
	testb	$2, %al
	jne	2b
	movb	$0x0df, %al
	movw	%dx, %bx
	movw	$0x60, %dx
	outb	%al, %dx
	movw	%bx, %dx

3:
	inb	%dx, %al
	testb	$2, %al
	jne	3b
	ret

/*
 * debug helper routine
 * dumps a hex word using BIOS teletype print char function
 *
 * %esi contains word to be printed
 */

#if 0

print_hex:
	data32
	pusha

	data32
	movl	$12, %ecx
L11:
	movl	%esi, %eax
	sarl	%cl, %eax
	andb	$15, %al
	cmpb	$9, %al
	data32
	jle 	L12
	addb	$87, %al
	data32
	jmp	L13

L12:
	addb	$48, %al
L13:
	movsbl	%al, %eax
	movb	$0xe, %ah
	data32
	movl	$0x0001, %ebx
	int	$0x10
	data32	
	addl	$-4, %ecx
	data32
	jns	L11
	data32
	popa
	data32
	ret
#endif

/* 
 * error messages 
 */

err_read:	
	.string	"Read error\r\n\0"
info:	
	.string	"Booting...\r\n\0"

	.align 4
gdtr:
	.word	8*4
	.long	gdt

	.align	4
gdt:
	.quad	0x0000000000000000	/* null */
	.quad	0x00cf9a000000ffff	/* 0x08 code 4GB */
	.quad	0x00cf92000000ffff	/* 0x10 data 4GB */
	.quad	0x0000000000000000	/* unused */


/* 
 * the last 2 bytes in the sector 0 contain the boot signature 
 */
	. = _start + 0x1fe
	.value	SIGNATURE
