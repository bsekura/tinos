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
 * $Id: tss.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <sys/types.h>
#include <i386/tss.h>
#include <i386/desc.h>
#include <i386/seg.h>
#include <i386/inlines.h>

extern struct sys_desc gdt[];

struct tss kernel_tss;

#define KERN_STKSZ	(1024)
static uchar kernel_tss_stack[KERN_STKSZ];

/* forward */
static void init_tss_desc(uint, ulong);
static void ktss_setup(struct tss*, ulong);

void 
init_kernel_tss()
{
	init_tss_desc(GDTSEL_KTSS, (ulong) &kernel_tss);
   	ktss_setup(&kernel_tss, 
		   (ulong) &kernel_tss_stack[KERN_STKSZ - sizeof(ulong)]);
	ltr(GDTSEL_KTSS);
}  
   
void 
panic(char* msg)
{
   	cli();
   	printf("%s\n", msg);
   	while(1);
}
   
static void 
init_tss_desc(uint sel, ulong tss_addr)
{
   	uint i = sel/8;

   	gdt[i].base_0_15 = tss_addr;
   	gdt[i].base_16_23 = (tss_addr >> 16);
   	gdt[i].base_24_31 = (tss_addr >> 24);
   	gdt[i].limit_0_15 = sizeof(struct tss) - 1;
   	gdt[i].gd_limit_16_19 = 0x00;
   	gdt[i].access = 0xE9;
}

static void 
ktss_setup(struct tss* ptss, ulong stk)
{
	/*
	 * in fact, I think we need to fill only
	 * a couple of those fields ...
	 */
   	ptss->eip = (ulong)&panic;
   	ptss->esp = stk;
   	ptss->cs = GDTSEL_KCODE;
   	ptss->ds = GDTSEL_KDATA;
   	ptss->es = GDTSEL_KDATA;
   	ptss->fs = GDTSEL_KDATA;
   	ptss->gs = GDTSEL_KDATA;
   	ptss->ss = GDTSEL_KDATA;
   	ptss->ss0 = GDTSEL_KDATA;
   	ptss->esp0 = stk;
   	ptss->trap = 0x00;
   	ptss->iomap = 0x00FF;
   	ptss->eflags = 0x00000202;
   	ptss->cr3 = 0x1000;
}

