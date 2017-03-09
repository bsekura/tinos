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
 * $Id: gdt.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <i386/desc.h>
#include <i386/seg.h>

/*
 *  GDT
 */

struct sys_desc gdt[] = {
/* 0x00 null        */  
	{ 0, 0, 0, 0, 0, 0 },
/* 0x08 kernel code 
   base: 0x00000000 limit: 0x3FFFFFFF (1GB-1) */
	{ 
	/* limit 0-16 */	0xFFFF, 
	/* base 0-15 */		0x0000, 
	/* base 16-23 */	0x00, 
	/* access */		(A_PRESENT | A_DPL_0 | A_CODE_READ), 
	/* gd, limit 16-19 */	(SEG_PAGED | SEG_32 | 0x3),
	/* base 24-31 */	0x00
	},
/* 0x10 kernel data - full 4GB mapping
   base: 0x00000000 limit: 0xFFFFFFFF (4GB-1) */
	{
	/* limit 0-16 */	0xFFFF, 
	/* base 0-15 */		0x0000, 
	/* base 16-23 */	0x00, 
	/* access */		(A_PRESENT | A_DPL_0 | A_DATA_WRITE), 
	/* gd, limit 16-19 */	(SEG_PAGED | SEG_32 | 0xF), 
	/* base 24-31 */	0x00
	},
/* 0x18 user code   
   base: 0x40000000 limit: 0xFFFFFFFF (3GB-1) */  
	{
	/* limit 0-16 */	0xFFFF, 
	/* base 0-15 */		0x0000, 
	/* base 16-23 */	0x00, 
	/* access */		(A_PRESENT | A_DPL_3 | A_CODE_READ), 
	/* gd, limit 16-19 */	(SEG_PAGED | SEG_32 | 0xF), 
	/* base 24-31 */	0x40
	},
/* 0x20 user data - same as above  */  
	{
	/* limit 0-16 */	0xFFFF, 
	/* base 0-15 */		0x0000, 
	/* base 16-23 */	0x00, 
	/* access */		(A_PRESENT | A_DPL_3 | A_DATA_WRITE), 
	/* gd, limit 16-19 */	(SEG_PAGED | SEG_32 | 0xF), 
	/* base 24-31 */	0x40
	},
/* 0x28 tss - filled later */  
	{0},
/* reserved */
	{0}
};

static struct pseudo_desc gdt_desc;

#if 0
void
dump_gdt_slot(int i)
{
	struct sys_desc* gdt_slot = &gdt[i];
	ulong l1 = *((ulong*)gdt_slot);
	ulong l2 = *(((ulong*)gdt_slot) + 1);

	printf("GDT[%d]: %08x%08x\n", i, l2, l1);
}
#endif

void
init_gdt()
{
#if 0
	int c;
	for(c = 1; c < 7; c++)
		dump_gdt_slot(c);
#endif

	gdt_desc.len = sizeof(gdt) - 1;
	gdt_desc.base = (ulong)&gdt;

	load_gdt(&gdt_desc);

	/* reload segment registers */
	asm volatile 
	(
		"ljmp %0, $1f\n\t"
		"1: "
		:
		: "i" (GDTSEL_KCODE)
	);

	asm volatile 
	(
		"movl %0, %%eax\n\t"
		"movw %%ax, %%ds\n\t"
		"movw %%ax, %%es\n\t"
		"movw %%ax, %%ss\n\t"
		"movw %%ax, %%fs\n\t"
		"movw %%ax, %%gs"
		:
		: "i" (GDTSEL_KDATA)
	);
}
