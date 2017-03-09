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
 * $Id: pic.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
#ifndef __pic_h__
#define __pic_h__

#ifndef __types_h__
#include <sys/types.h>
#endif

#include <sys/bits.h>

/* programmable interrupt controler ports */

#define MASTER_PIC_CMD		(0x20)
#define MASTER_PIC_INTMASK	(0x21)

#define SECOND_PIC_CMD		(0xA0)
#define SECOND_PIC_INTMASK	(0xA1)

/* initialization command words */

#define ICW1_NEED_ICW4		BIT(0)
#define ICW1_NO_ICW4		(0)
#define ICW1_SINGLE		BIT(1)
#define ICW1_CASCADING		(0)
#define ICW1_4_BYTE_INTVEC	BIT(2)
#define ICW1_8_BYTE_INTVEC	(0)
#define ICW1_LEVEL_TRIG		BIT(3)
#define ICW1_EDGE_TRIG		(0)
#define ICW1_BASE		BIT(4)

#define ICW2_INTVEC_MASK	(BIT(7)|BIT(6)|BIT(5)|BIT(4)|BIT(3))

#define ICW3_INT_0_SLAVE	BIT(0)
#define ICW3_INT_1_SLAVE	BIT(1)
#define ICW3_INT_2_SLAVE	BIT(2)
#define ICW3_INT_3_SLAVE	BIT(3)
#define ICW3_INT_4_SLAVE	BIT(4)
#define ICW3_INT_5_SLAVE	BIT(5)
#define ICW3_INT_6_SLAVE	BIT(6)
#define ICW3_INT_7_SLAVE	BIT(7)

#define ICW3_SLAVE_ON_1		(0x01)
#define ICW3_SLAVE_ON_2		(0x02)

#define ICW4_80x86_MODE		BIT(0)
#define ICW4_8085_MODE		(0)
#define ICW4_AUTO_EOI		BIT(1)
#define ICW4_NORMAL_EOI		(0)
#define ICW4_SFN_MODE		BIT(4)
#define ICW4_SEQUENTIAL		(0)
#define ICW4_BUF_SLAVE		BIT(3)
#define ICW4_BUF_MASTER		BIT(2) | BIT(3)

#define OCW1_MASK_OFF_ALL	(0xFF)

#define OCW2_NON_SPEC_EOI	BIT(5)

void pic_init(ushort,ushort);
void enable_irq(ushort);
void disable_irq(ushort);

#endif	/* __pic_h__ */

