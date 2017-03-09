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
 *
 * $Id: irq.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <irq.h>
#include <pc/pic.h>
#include <i386/frame.h>

struct irq_dispatch_slot {
	ulong	stray;
	voidfun	handler;
};

#define IRQ_NO	(16)
static struct irq_dispatch_slot irq_dispatch_table[IRQ_NO];

void
init_irq()
{
}

void
interrupt(ulong stack)
{
	ulong no = ((struct frame*) &stack)->err;
	
	if(irq_dispatch_table[no].handler)
		irq_dispatch_table[no].handler();
	else
		irq_dispatch_table[no].stray++;
}

int
interrupt_register(ulong no, voidfun handler)
{
	if(no < 1 || no >= 16) {
		printf("interrupt_register(): weird interrupt number\n");
		return 0;
	}
	
	if(!handler) {
		printf("interrupt_register(): null handler\n");
		return 0;
	}
	
	irq_dispatch_table[no].handler = handler;
	enable_irq(no);
	return 1;
}

int
interrupt_unregister(ulong no)
{
	if(no < 1 || no >= 16) {
		printf("interrupt_unregister(): weird interrupt number\n");
		return 0;
	}
	
	irq_dispatch_table[no].handler = (voidfun) 0;
	disable_irq(no);
	return 1;
}
