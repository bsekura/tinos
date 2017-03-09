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
 * $Id: hat.c,v 1.3 1998/03/01 21:06:21 bart Exp $
 *
 */

/*
 * hardware address translation layer for Intel 80x86 (x = 3, 4, 5, 6 ...)
 * routines for managing machine depenent portion of virtual
 * memory subsystem
 */

#include <i386/hat.h>
#include <i386/page.h>
#include <i386/frame.h>
#include <i386/inlines.h>
#include <i386/spl.h>
#include <boot.h>
#include <thread.h>

static hat_desc_t kernel_hat;
hat_desc_t* kern_hat;

/*
 * actually initialize hat descriptor
 * don't have to allocate memory for it,
 * higher levels do that (it's a part of a vas structure)
 */
ulong
hat_create(hat_desc_t* hat)
{
	ulong page;
	ulong* p;

	page = alloc_page();
	if(page == 0) {
		printf("hat_create(): fatal, cannot alloc_page()...\n");
		return (0);
	}

	/* 
	 * clone kernel hat (kernel page dir)
	 */
	copy_page((void*) kern_hat->va_pd, (void*) page);

	hat->pa_pd = hat->va_pd = page;
	hat->page_count = 1;

	return (page);
}

/*
 * destroy hat structures
 * go through page directory and then 2nd level page table
 * and release any pages (pointed by table slots)
 */
void
hat_destroy(hat_desc_t* hat)
{
	ulong c, i, *pdir, *ptab;

	pdir = (ulong*) hat->va_pd;
	for(c = 0; c < NPTE; c++, pdir++) {
		if(*pdir) {
			ptab = (ulong*) pte_addr(*pdir);
			for(i = 0; i < NPTE; i++, ptab++) {
				if(*ptab) {
					free_page(pte_addr(*ptab));
					*ptab = 0;
					hat->page_count--;
				}
			}
			free_page((ulong) (ptab - NPTE));
		}
	}
	free_page((ulong) (pdir - NPTE));
}

/*
 * enter given hat (and stay there)
 * this causes the current running thread to
 * change the hat it belongs to
 *
 */
void
hat_enter(hat_desc_t* hat)
{
	spl_t s;
	
	if(hat) {
		extern struct tcb* cur_thread;
		
		s = splhi_save();
		cur_thread->hat = hat;
		hat_switch(hat);
		splx(s);
	}	
}

/*
 * map given physical memory region
 * no pages are allocated except for page tables
 *
 * used by kernel upon initialization
 */
void
hat_map(hat_desc_t* hat, ulong va, ulong pa_start, ulong npg, ulong perms)
{
	int ipd, ipt;
	ulong addr, *p;

	p = (ulong*) hat->va_pd;
	ipd = va2ipd((ulong) va);
	ipt = va2ipt((ulong) va);

#ifdef _HAT_DEBUG
	printf("hat_map(): ipd = %d, ipt = %d\n", ipd, ipt);
#endif

	addr = pa_start;
	while(1) {
		ulong c, ptab, *pt;

		ptab = pte_addr(p[ipd]);
		if(ptab == 0) {
			ptab = alloc_page();
			zero_page((void*) ptab);
			p[ipd] = ptab | PTE_PRESENT | perms;
			hat->page_count++;
		}

		pt = (ulong*) ptab;
		c = NPTE;
		if(ipt) {
			pt += ipt;
			c -= ipt;
		}
		if(c > npg)
			c = npg;

		npg -= c;
		while(c--) {
			*pt++ = addr | PTE_PRESENT | perms;
			addr += PAGESZ;
		}
		if(npg <= 0)
			break;

		ipd++;
		ipt = 0;
	}
}

/*
 * insert translation at given virtual address
 */
void
hat_insert(hat_desc_t* hat, ulong va, ulong page, ulong perms)
{
	ulong* p, ptab;
	int ipd;

	p = (ulong*) hat->va_pd;
	ipd = va2ipd(va);
	
#ifdef _HAT_DEBUG
	printf("hat_insert(): ipd = %d, ipt = %d\n", ipd, va2ipt(va));
#endif

	ptab = pte_addr(p[ipd]);
	if(ptab == 0) {
#ifdef _HAT_DEBUG
		printf("hat_insert(): allocating page table\n");
#endif
		ptab = alloc_page();
		zero_page((void*) ptab);
		p[ipd] = ptab | PTE_PRESENT | perms;
		hat->page_count++;
	}

	((ulong*) ptab)[va2ipt(va)] = page | PTE_PRESENT | perms;
}

/*
 * remove translation
 * gotta flush TLB here
 */
void
hat_remove(hat_desc_t* hat, ulong va)
{
	ulong* p, ptab;

	p = (ulong*) hat->va_pd;
	printf("hat_remove(): ipd = %d, ipt = %d\n", va2ipd(va), va2ipt(va));

	ptab = pte_addr(p[va2ipd(va)]);
	if(ptab == 0) {
		printf("hat_remove(): no page table\n");
		return;
	}

	/* remove the present bit */
	((ulong*) ptab)[va2ipt(va)] &= ~PTE_PRESENT;

	/* flush TLB */
	set_cr3(hat->pa_pd);
}

void
hat_chmod(hat_desc_t* hat, ulong va, ulong flags)
{
	ulong* p, ptab;

	p = (ulong*) hat->va_pd;
	printf("hat_chmod(): ipd = %d, ipt = %d\n", va2ipd(va), va2ipt(va));

	ptab = pte_addr(p[va2ipd(va)]);
	if(ptab == 0) {
		printf("hat_chmod(): no page table\n");
		return;
	}

	//((ulong*) ptab)[va2ipt(va)] &= ~PTE_PRESENT;

	/* flush TLB */
	//set_cr3(hat->pa_pd);
}

inline static ulong
hat_getpfn(ulong* pd, ulong va)
{
	ulong* pt;
	
	pt = (ulong*) pte_addr(pd[va2ipd(va)]);
	if(pt) {
		return pte_addr(pt[va2ipt(va)]);
	}
	
	return -1;
}

void
hat_copy(hat_desc_t* src, hat_desc_t* dst,
	 ulong va_src, ulong va_dst, ulong npg)
{
	int ipd_src, ipt_src, ipd_dst, ipt_dst;

	ipd_src = va2ipd(va_src);
	ipt_src = va2ipt(va_src);
	ipd_dst = va2ipd(va_dst);
	ipt_dst = va2ipt(va_dst);

	printf("hat_copy(): source [%d][%d] -> destination [%d][%d]\n",
		ipd_src, ipt_src, ipd_dst, ipt_dst);
}

void
hat_dump(hat_desc_t* hat)
{
	ulong c, i, *p, *ptab;

	p = (ulong*) hat->va_pd;
	printf("hat_dump(): page_count = %d\n", hat->page_count);
	for(c = 0; c < NPTE; c++) {
		if(*p) {
			printf("hat_dump(): at %d (%x): ", c, *p);

			ptab = (ulong*) pte_addr(*p);
			i = 0;
			while(*ptab == 0) {
				i++;
				*ptab++;
			}

			printf("[%d] ", i);
			for(i = 0; i < 6; i++) {
				printf("%x ", *ptab++);
			}
			printf("\n");
		}
		p++;
	}
}

static void
show_regs(struct frame* f)
{
	printf("eip: %08x ecs: %08x ess: %08x esp: %08x\n",
		f->eip, f->ecs, f->ess, f->esp);
	printf("eax: %08x ebx: %08x ecx: %08x edx: %08x\n",
		f->eax, f->ebx, f->ecx, f->edx);
}

#define PF_ERR_USER	(0x04)
#define PF_ERR_WRITE	(0x02)
#define PF_ERR_PROTECT	(0x01)

int vas_pagefault(int user, int write, int protection,
             	  ulong va, ulong pa);

/*
 * cpu page fault trap
 */
void
page_fault(ulong stack)
{
	struct frame* f = (struct frame*)&stack;
	ulong cr2, *pd;
	spl_t s;

	/*
	 * XXX
	 * at present I am paranoid and disable interrupts
	 * for the whole page fault code path
	 * in the future I hope to employ some intelligent
	 * locking scheme here...
	 */
	s = splhi_save();
	
	/*
	 * get faulting virtual address (register cr2)
	 * and a page directory of the current vas context
	 */
	cr2 = get_cr2();
	pd = (ulong*) get_cr3();
	
	/*
	 * let upper level handle the page fault
	 */
	if(!vas_pagefault(f->err & PF_ERR_USER,
		          f->err & PF_ERR_WRITE,
		          f->err & PF_ERR_PROTECT,
		          cr2,
		          hat_getpfn(pd, cr2))) {

		/*
		 * this page fault was not handled		       
		 * which is kinda fatal
		 */
		printf("\nUnhandled Page Fault.\n");
		printf("cr2: %08x err: %08x pfn: %08x\n", 
			cr2, f->err, hat_getpfn(pd, cr2));
		show_regs(f);

		printf("page fault occurred in ");
		if(f->err & PF_ERR_USER)
			printf("user mode, ");
		else
			printf("kernel mode, ");

		if(f->err & PF_ERR_WRITE)
			printf("write operation, ");
		else
			printf("read operation, ");

		if(f->err & PF_ERR_PROTECT)
			printf("protection violation.\n");
		else	
			printf("non-present page.\n");
		
		printf("ipd = %d, ipt = %d\n", 
			va2ipd(cr2), 
			va2ipt(cr2));

		/*
		 * hang infinitely
		 */		 
		while(1);
	}
	splx(s); 
}


/*
 * sets up kernel hat and starts paging
 * called only once upon system startup
 */
void
hat_init()
{
	extern struct boot_info_s* boot_info;
	extern ulong top_memory;
	ulong mem_pages, start;
	ulong page;

	/*
	 * initialize kernel hat
	 * cannot use hat_create() coz it actually clones
	 * kernel hat (the one we're about to create)
	 */
	kern_hat = &kernel_hat;

	page = alloc_page();
	if(page == 0) {
		printf("hat_init(): fatal, cannot alloc_page()...\n");
		return;
	}

	zero_page((void*) page);

	kern_hat->pa_pd = page;
	kern_hat->va_pd = page;
	kern_hat->page_count = 1;

	/* 
	 * now map some memory 
	 * actually, map all physical pages
	 */
	mem_pages = boot_info->mem_size / PAGESZ;
	hat_map(kern_hat, PAGESZ, PAGESZ, mem_pages, PTE_WRITE);
	
#if 0		
	start = page_addr(boot_info->kern_start);
	
	hat_map(kern_hat, PAGESZ, PAGESZ, 
		btop((start - PAGESZ)), 
		PTE_READONLY);
		
	hat_map(kern_hat, start, start, 
		btop((top_memory - start)), 
		PTE_WRITE);

	hat_map(kern_hat, top_memory, top_memory,
		btop((boot_info->mem_size - top_memory)), 
		PTE_READONLY);			
#endif
	
	/*hat_dump(kern_hat);*/
	printf("hat_kernel_init(): entering kernel_hat\n");

	/* start paging and enter kernel hat */
	asm volatile
	(
		"movl %%eax, %%cr3\n\t"
		"movl %%cr0, %%eax\n\t"
		"orl  $0x80010000, %%eax\n\t"
		"movl %%eax, %%cr0\n\t"
		"jmp  1f\n\t"
		"1:"
		:
		: "a" (kern_hat->pa_pd)
	);
}

/*
 * XXX those below are temp
 */
 
void
hat_kern_insert(ulong va, ulong page)
{
	hat_insert(kern_hat, va, page, PTE_WRITE);
}

void
hat_test()
{
	hat_desc_t h;

	hat_create(&h);
	hat_dump(&h);
}
