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
 * programmable interrupt controller
 * 
 * $Id: pic.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <pc/pic.h>
#include <i386/inlines.h>

#define SLAVE_IRQ	8
#define MASTER_SLAVE	2

static ushort irq_mask = 0xFFFF;

void
init_pic()
{
	ushort master, slave;

	master = 0x20;
	slave = 0x28;

	outb(MASTER_PIC_CMD, ICW1_BASE | ICW1_NEED_ICW4);
	outb(MASTER_PIC_INTMASK, ICW2_INTVEC_MASK & master);
	outb(MASTER_PIC_INTMASK, ICW3_INT_2_SLAVE);
	outb(MASTER_PIC_INTMASK, ICW4_80x86_MODE);
	outb(MASTER_PIC_INTMASK, OCW1_MASK_OFF_ALL);
	outb(MASTER_PIC_CMD, OCW2_NON_SPEC_EOI);
	
	outb(SECOND_PIC_CMD, ICW1_BASE | ICW1_NEED_ICW4);
	outb(SECOND_PIC_INTMASK, ICW2_INTVEC_MASK & slave);
	outb(SECOND_PIC_INTMASK, ICW3_SLAVE_ON_2);
	outb(SECOND_PIC_INTMASK, ICW4_80x86_MODE);
	outb(SECOND_PIC_INTMASK, OCW1_MASK_OFF_ALL);
	outb(SECOND_PIC_CMD, OCW2_NON_SPEC_EOI);
}

void
enable_irq(ushort irq_no)
{
	irq_mask &= ~(1 << irq_no);
	if(irq_no >= SLAVE_IRQ) {
		irq_mask &= ~(1 << MASTER_SLAVE);
	}
	
	outb(MASTER_PIC_INTMASK, irq_mask & 0xff);
	outb(SECOND_PIC_INTMASK, (irq_mask >> 8) & 0xff);
}

void
disable_irq(ushort irq_no)
{
	irq_mask |= (1 << irq_no);
	if(irq_no >= SLAVE_IRQ) {
		irq_mask |= (1 << MASTER_SLAVE);
	}
	outb(MASTER_PIC_INTMASK, irq_mask & 0xff);
	outb(SECOND_PIC_INTMASK, (irq_mask >> 8) & 0xff);
}

