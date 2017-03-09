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
 * $Id: page.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
 
#ifndef __page_h__
#define __page_h__

#include <sys/types.h>
#include <sys/bits.h>

/*
 * this is kernel page dir location upon kernel boot
 */
#define KERNEL_PD	(0x1000)

/*
 * page size and shift bits
 */
#define PAGESZ          (4096)	/* size of a page in bytes */
#define NPTE            (1024)	/* number of page table entries */
#define PGSHIFT         (12)	/* page size shift for calculations */

/* extract physical frame address from page table entry */
#define pte_addr(x)     ((ulong)(x) & 0xFFFFF000)
#define page_addr(x)	((ulong)(x) & 0xfffff000)

#define page_round(x)   (((ulong)(x) + (PAGESZ-1)) & ~(PAGESZ-1))
#define ptob(x)         ((ulong)(x) << PGSHIFT)
#define btop(x)         ((ulong)(x) >> PGSHIFT)
#define btorp(x)        (((ulong)(x) + (PAGESZ-1)) >> PGSHIFT)
#define btorpt(x)	(((ulong)(x) + ((PAGESZ*1024)-1)) >> 22) 

/*
 * macros for retriving page dir and page table indexes
 * out of virtual address
 */
#define va2ipd(va)   ((((ulong)va) >> 22) & 0x3FF)
#define va2ipt(va)   ((((ulong)va) >> 12) & 0x3FF)

/*
 * page dir/table entry bits
 */

#define PTE_PRESENT	BIT(0)
#define PTE_WRITE	BIT(1)
#define PTE_USER	BIT(2)
#define PTE_ACCESSED	BIT(5)
#define PTE_DIRTY	BIT(6)

#define	PTE_READONLY	(0)
#define	PTE_KERNEL	(0)


inline static void
zero_page(void* page)
{
	asm volatile
	(
		"cld\n\t"
		"xorl %%eax, %%eax\n\t"
		"rep\n\t"
		"stosl"
		:
		: "D" (page), "c" (NPTE)
		: "di", "ax"
	);
}

inline static void
copy_page(void* src, void* dest)
{
	asm volatile
	(
		"cld\n\t"
		"rep\n\t"
		"movsl"
		:
		: "D" (dest), "S" (src), "c" (NPTE)
		: "di", "si"
	);
}
	
#endif /* __page_h__ */
