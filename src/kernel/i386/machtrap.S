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
 * $Id: machtrap.S,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <i386/seg.h>

/*
 * generic macros for saving and restoring registers
 * note that this is the same for all traps (cpu traps, interrupts)
 */
#define SAVE_REGS			\
	pushl	%eax			;\
	pushl	%ecx			;\
	pushl	%edx			;\
	pushl	%ebx			;\
	pushl	%ebp			;\
	pushl	%esi			;\
	pushl	%edi			;\
	pushw	%ds			;\
	pushw	%es			;\
	movw	%ss, %ax		;\
	movw	%ax, %ds		;\
	movw	%ax, %es

#define RESTORE_REGS		\
	popw	%es		;\
	popw	%ds		;\
	popl	%edi		;\
	popl	%esi		;\
	popl	%ebp		;\
	popl	%ebx		;\
	popl	%edx		;\
	popl	%ecx		;\
	popl	%eax		


/*
 * macros for generating entry points for cpu traps and interrupts
 * we push supplied code value for ones without it
 */
#define ENTRY(name, code)	\
	.globl t_##name		;\
	.align 4 		;\
t_##name:			;\
	pushl $code		;\
	cld			;\
	SAVE_REGS


#define ENTRY_WITH_ERROR(name)	\
	.globl t_##name		;\
	.align 4 		;\
t_##name:			;\
	cld			;\
	SAVE_REGS


/*
 * leaving is the same for every trap type
 * we adjust stack pointer because of error code (or custom code)
 */
#define LEAVE			\
	RESTORE_REGS		;\
	addl $4, %esp		;\
	iret

/*
 * now the proper macros for cpu traps, cpu traps with error code
 * and hardware interrupts
 * system call entries are the same as cpu traps without error code
 */
#define TRAP(name) 		 \
	ENTRY(name, 0) 		;\
	call name		;\
	LEAVE				

#define TRAP_WITH_ERROR(name)	\
	ENTRY_WITH_ERROR(name)	;\
	call name		;\
	LEAVE

/*
 *
 * ISR and ISR_2 are for direct interrupt dispatch
 * they will call C routine 'name'
 */
#define ISR(name, code)		\
	ENTRY(name, code)	;\
	movl $0x20, %eax	;\
	outb %al, $0x20		;\
	call name		;\
	LEAVE

#define ISR_2(name, code)	\
	ENTRY(name, code)	;\
	movl $0x20, %eax	;\
	outb %al, $0x20		;\
	outb %al, $0xA0		;\
	call name		;\
	LEAVE


/*
 * macros below dispatch to 'interrupt' routine (defined in irq.c) 
 * with supplied number of the interrupt
 */
#define INTERRUPT(name, code)	\
	ENTRY(name, code)	;\
	movl $0x20, %eax	;\
	outb %al, $0x20		;\
	call interrupt		;\
	LEAVE

#define INTERRUPT_2(name, code)	\
	ENTRY(name, code)	;\
	movl $0x20, %eax	;\
	outb %al, $0x20		;\
	outb %al, $0xA0		;\
	call interrupt		;\
	LEAVE


/*
 * now the stubs themselves  ...
 */

TRAP_WITH_ERROR(page_fault)
TRAP(default_trap)

ISR(clock, 0)

INTERRUPT(interrupt_01, 1)
INTERRUPT(interrupt_02, 2)
INTERRUPT(interrupt_03, 3)
INTERRUPT(interrupt_04, 4)
INTERRUPT(interrupt_05, 5)
INTERRUPT(interrupt_06, 6)
INTERRUPT(interrupt_07, 7)

INTERRUPT_2(interrupt_08, 8)
INTERRUPT_2(interrupt_09, 9)
INTERRUPT_2(interrupt_10, 10)
INTERRUPT_2(interrupt_11, 11)
INTERRUPT_2(interrupt_12, 12)
INTERRUPT_2(interrupt_13, 13)
INTERRUPT_2(interrupt_14, 14)
INTERRUPT_2(interrupt_15, 15)

TRAP(_sample_syscall)
TRAP(_kbisr_syscall)

	.globl	call_trap
	.align	4, 0x90
call_trap:
	int $0x90
	ret

	.globl	syscall_trap
	.align	4, 0x90
syscall_trap:
	call sys_trap	
	iret
