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
 *
 * $Id: idt.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <i386/desc.h>
#include <i386/seg.h>
#include <i386/inlines.h>
#include <sys/types.h>

#define IDT_SIZE (256)
struct gate_desc idt[IDT_SIZE];

/*
 * interrupts and traps
 */

/*
 * system calls
 */

/*
 * trap types
 */
#define     TRAP_GATE      (0x8F00)
#define     IRQ_GATE       (0x8E00)
#define     USER_TRAP_GATE (0xEF00)

/*
 * generic dummy exception handlers
 * XXX temporary, change that...
 */
#define TRAP_STUB(name, msg) \
   	void _x##name() { cli(); printf(##msg); while(1); } 
   
TRAP_STUB(divide, "exception: divide by zero\n")
TRAP_STUB(debug, "exception: debug\n")
TRAP_STUB(nmi, "exception: nmi\n")
TRAP_STUB(breakpoint, "exception: breakpoint\n")
TRAP_STUB(overflow, "exception: overflow\n")
TRAP_STUB(bounds, "exception: bounds\n")
TRAP_STUB(opcode, "exception: opcode\n")
TRAP_STUB(mathgone, "exception: mathgone\n")
TRAP_STUB(double, "exception: double\n")
TRAP_STUB(mathover, "exception: mathover\n")
TRAP_STUB(tss, "exception: tss\n")
TRAP_STUB(segment, "exception: segment\n")
TRAP_STUB(stack, "exception: stack\n")
TRAP_STUB(general, "exception: general\n")
TRAP_STUB(page, "exception: page\n")
TRAP_STUB(matherr, "exception: matherr\n")

#define TRAP_ENTRY(name) ((ulong)&_x##name)

#define IRQ(x) ((x)+32)

extern void t_page_fault();

/*
 * cpu traps table
 */
static ulong cpu_trap[] = {
   	TRAP_ENTRY(divide),
   	TRAP_ENTRY(debug),
   	TRAP_ENTRY(nmi),
   	TRAP_ENTRY(breakpoint),
   	TRAP_ENTRY(overflow),
   	TRAP_ENTRY(bounds),
   	TRAP_ENTRY(opcode),
   	TRAP_ENTRY(mathgone),
   	TRAP_ENTRY(double),
   	TRAP_ENTRY(mathover),
   	TRAP_ENTRY(tss),
   	TRAP_ENTRY(segment),
   	TRAP_ENTRY(stack),
   	TRAP_ENTRY(general),
   	(ulong) &t_page_fault,
   	TRAP_ENTRY(matherr)
};
#define NTRAP (sizeof(cpu_trap)/sizeof(ulong))

extern void t_default_trap();

void
default_trap()
{
	printf("default_trap.\n");
	while(1);
}

extern void t_clock(), t_kb_isr(), t_fd_isr();

#define INTERRUPT(x) t_interrupt_##x
#define INTERRUPT_STUB(x) extern void t_interrupt_##x()

INTERRUPT_STUB(01);
INTERRUPT_STUB(02);
INTERRUPT_STUB(03);
INTERRUPT_STUB(04);
INTERRUPT_STUB(05);
INTERRUPT_STUB(06);
INTERRUPT_STUB(07);
INTERRUPT_STUB(08);
INTERRUPT_STUB(09);
INTERRUPT_STUB(10);
INTERRUPT_STUB(11);
INTERRUPT_STUB(12);
INTERRUPT_STUB(13);
INTERRUPT_STUB(14);
INTERRUPT_STUB(15);

/*
 * hardware interrupts table
 */
static ulong cpu_isr[] = {
   	(ulong) &t_clock,   		/*  0 32 clock */
#if 0   	 
   	(ulong) &t_kb_isr,        	/*  1 33 keyboard */
#endif   	
   	(ulong) &INTERRUPT(01),		/*  1 33 keyboard */
   	(ulong) &INTERRUPT(02),    	/*  2 34 */
   	(ulong) &INTERRUPT(03),    	/*  3 35 */ 
   	(ulong) &INTERRUPT(04),    	/*  4 36 */
   	(ulong) &INTERRUPT(05),    	/*  5 37 */
   	(ulong) &INTERRUPT(06),   	/*  6 38 floppy */
   	(ulong) &INTERRUPT(07),    	/*  7 39 */
   	(ulong) &INTERRUPT(08),    	/*  8 40 */
   	(ulong) &INTERRUPT(09),    	/*  9 41 */
   	(ulong) &INTERRUPT(10),    	/* 10 42 */
   	(ulong) &INTERRUPT(11),    	/* 11 43 */
   	(ulong) &INTERRUPT(12),    	/* 12 44 */
   	(ulong) &INTERRUPT(13),    	/* 13 45 */
   	(ulong) &INTERRUPT(14), 	/* 14 46 primary ide */
   	(ulong) &INTERRUPT(15)  	/* 15 47 secondary ide */
};
#define NISR (sizeof(cpu_isr)/sizeof(ulong))

#define SYSCALL_TRAP	(0x90)
extern void syscall_trap();
#define SAMPLE_SYSCALL	(0x91)
extern void t__sample_syscall();
#define KBISR_SYSCALL	(0x92)
extern void t__kbisr_syscall();

/*
 * system calls
 */
static struct __syscall {
   	int   vect;
   	ulong func;
} syscall[] = {
	{ SYSCALL_TRAP, (ulong) &syscall_trap },
	{ SAMPLE_SYSCALL, (ulong) &t__sample_syscall },
	{ KBISR_SYSCALL, (ulong) &t__kbisr_syscall }
};
#define NSYSCALL (sizeof(syscall)/sizeof(struct __syscall))

/*
 * fill up idt slot
 */
static void 
setup_gate(int i, ulong p_hand, ushort type)
{
   	idt[i].offset_0 = p_hand;
   	idt[i].selector = GDTSEL_KCODE;
   	idt[i].type = type;
   	idt[i].offset_16 = (p_hand >> 16);
}

/*
 * initialize interrupt descriptor table
 */
void 
init_idt()
{
	static struct pseudo_desc idt_desc;
   	int c;

   	/*
   	 * upon init, fill all the slots with dummy isr
   	 */
   	for(c = 0; c < IDT_SIZE; c++) 
   		setup_gate(c, (ulong) &t_default_trap, TRAP_GATE);

   	/* 
   	 * CPU exceptions 
   	 */
   	for(c = 0; c < NTRAP; c++)
   		setup_gate(c, cpu_trap[c], TRAP_GATE);

   	/* 
   	 * hardware IRQs 
   	 */
   	for(c = 0; c < NISR; c++)
   		setup_gate(IRQ(c), cpu_isr[c], IRQ_GATE);
   
   	/* 
   	 * system calls 
   	 */
   	for(c = 0; c < NSYSCALL; c++)
   		setup_gate(syscall[c].vect, syscall[c].func, USER_TRAP_GATE);

	/* now load the descriptor */
	idt_desc.len = sizeof(idt) - 1;
	idt_desc.base = (ulong) &idt;
	load_idt(&idt_desc);
}
   
