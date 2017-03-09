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
 * $Id: desc.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __desc_h__
#define __desc_h__

#include <sys/types.h>
#include <sys/bits.h>

struct gate_desc {
	ushort   offset_0;
   	ushort   selector;
  	ushort   type;
  	ushort   offset_16;
};

struct sys_desc {
   	ushort   limit_0_15;
   	ushort   base_0_15;
   	uchar    base_16_23;
   	uchar    access;
   	uchar    gd_limit_16_19;
   	uchar    base_24_31;
};

/*
 * these are bit settings for field access in sys_desc
 */
#define A_PRESENT	BIT(7)
/* descriptor priviledge level - bits 6-5 */
#define A_DPL_0		(0)
#define A_DPL_1		BIT(5)	
#define A_DPL_2		BIT(6)
#define A_DPL_3		BIT(6) | BIT(5)
/* descriptor type and access - bit 4 and bits 3-1 */
#define A_DATA		BIT(4)	
#define A_CODE		BIT(4) | BIT(3)
#define A_DATA_WRITE	BIT(4) | BIT(1)
#define A_CODE_READ	BIT(4) | BIT(3) | BIT(1)

#define A_ACCESSED	BIT(0)

/*
 * these bits are for field gd_limit_16_19
 */
#define SEG_PAGED	BIT(7)
#define SEG_32		BIT(6)


/*
 * pseudo descriptor is used to load descriptor registers
 * in the following inline functions
 */
struct pseudo_desc {
	ushort	pad;
	ushort	len;
	ulong	base;
};

inline static void
load_gdt(struct pseudo_desc* pdesc)
{
	asm volatile 
	(
		"lgdt %0" 
		:
		: "m" (pdesc->len)
	);
}

inline static void
load_idt(struct pseudo_desc* pdesc)
{
	asm volatile 
	(
		"lidt %0" 
		:
		: "m" (pdesc->len)
	);
}


void init_gdt();
void init_idt();


#endif /* __desc_h__ */
