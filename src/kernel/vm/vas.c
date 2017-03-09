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
 * $Id: vas.c,v 1.2 1998/03/09 18:39:09 bart Exp $
 *
 */

#include <sys/types.h>
#include <i386/hat.h>
#include <i386/spl.h>
#include <i386/seg.h>
#include <sema.h>
#include <pool.h>
#include <queue.h>
#include <vm/page.h>
#include <vm/vas.h>
#include <thread.h>

/* 
 * vas and region structure pool 
 */
static master_pool_t vas_pool;
static master_pool_t region_pool;

static vas_desc_t* vas_list;	/* list of all vases in the system */
vas_desc_t* kern_vas;		/* kernel vas */

/*
 * initialize vas structures
 * create new pool and initialize kernel vas
 */
void
init_vas()
{
	extern hat_desc_t* kern_hat;
	vas_desc_t* vas;

	/*
	 * initialize vas descriptors pool and allocate kernel vas
	 */
	init_pool(&vas_pool, sizeof(vas_desc_t));
	vas = alloc_pool(&vas_pool);
	if(!vas) {
		 printf("vas_init(): oops, cannot alloc from pool\n");
		 return;
	}

	/*
	 * kernel hat is initialized directly at hat layer
	 */
	hat_init();
	vas->hat = kern_hat;

	vas->rg_list = NULL;
	vas->last_reg = NULL;

	sema_init(&vas->vas_lock);
	vas->refcnt = 0;

	enqueue(vas_list, next, prev, vas);
	kern_vas = vas;

	/*
	 * initialize region pool
	 */
	init_pool(&region_pool, sizeof(vas_region_t));
}

/*
 * allocate new vas
 */
vas_desc_t*
vas_alloc()
{
	spl_t s;
	vas_desc_t* vas;

	vas = (vas_desc_t*) alloc_pool(&vas_pool);
	if(!vas) {
		 printf("vas_alloc(): cannot alloc vas_desc_t.\n");
		 return (NULL);
	}

	/* initialize hat structure */
	vas->hat = &vas->hat_desc;
	hat_create(vas->hat);

	vas->rg_list = NULL;
	vas->last_reg = NULL;

	sema_init(&vas->vas_lock);
	vas->refcnt = 0;
	
	s = splhi_save();
	enqueue(vas_list, next, prev, vas);
	splx(s);

	return (vas);
}

vas_region_t*
region_create(vas_desc_t* vas, ulong start, ulong size,
	      cache_desc_t* cache, ulong offset)
{
	int found;
	vas_region_t* rg, *r;

	size += (start - page_addr(start));
	start = page_addr(start);
		
	/*
	 * first, determine where to insert new region
	 * to keep the list sorted
	 * and check for overlapping
	 */
	found = 0;
	if(vas->rg_list) {
		r = vas->rg_list;
		do {
			if(r->start > start) {

				if(start + size > r->start) {
					printf("overlapping\n");
					return 0;
				}
				rg = r->prev;
				if((rg != r) &&
				   (rg->start + rg->size > start)) {
					printf("overlapping\n");
					return 0;
				}

				found = 1;
				break;
			}
			r = r->next;
		} while(r != vas->rg_list);

		if(!found) {
			rg = vas->rg_list->prev;
			if(rg->start + rg->size > start) {
				printf("overlapping\n");
				return 0;
			}
		}
	}

	rg = alloc_pool(&region_pool);
	if(!rg) {
		printf("region_create(): fatal, couldn't alloc from pool\n");
		return 0;
	}

	rg->vas = vas;
	rg->start = start;
	rg->size = size;
	rg->cache = cache;
	rg->offset = offset;

	if(found) {
		enqueue_before(vas->rg_list, next, prev, r, rg);
	} else {
		enqueue(vas->rg_list, next, prev, rg);
	}
	
	return (rg);
}

/*
 * this routine should coalesce region to minimize
 * vas overhead
 */
void
region_coalesce()
{
	/* TBD */
}

/*
 * commit the region
 * fill in all its page slots with real memory
 */
void
region_commit(vas_region_t* rg)
{
	ulong va, page;
	hat_desc_t* hat;

	hat = rg->vas->hat;
	va = rg->start;
	while(va < rg->start + rg->size) {
		page = alloc_page();
		if(!page) {
			printf("fatal, cannot alloc page\n");
			return;
		}
		//printf("region_commit: page = %08x\n", page);
		cache_page(rg->cache, page, page);
		hat_insert(hat, va, page, PTE_WRITE|PTE_USER);
		va += PAGESZ;
	}
} 

void
vas_dump()
{
	spl_t s;
	vas_desc_t* vas;
	
	s = splhi_save();
	vas = vas_list;
	if(vas) {
		do {
			printf("vas: %08x\n", (ulong) vas);
			vas = vas->next;
		} while(vas != vas_list);
	}
	splx(s);
}

void
region_dump(vas_desc_t* vas)
{
	spl_t s;
	vas_region_t* rg;
	
	s = splhi_save();
	rg = vas->rg_list;
	if(rg) {
		do {
			printf("region: [%08x:%08x]\n", rg->start, rg->size);
			rg = rg->next;
		} while(rg != vas->rg_list);
	}
	splx(s);	
}

/*
 * XXX temp
 */
void
rg_test(ulong start, ulong size)
{
	extern cache_desc_t* reserved_cache;
	static vas_desc_t* vas;

	if(!vas) {
		printf("rg_test(): allocating vas...\n");
		vas = vas_alloc();
		if(!vas) {
			return;
		}
	}

	printf("creating region... %x, %x\n", start, size);
	region_create(vas, start, size, reserved_cache, start);
	region_dump(vas);
	region_commit(vas->rg_list);
	hat_switch(vas->hat);
}

/*
 * handle a pagefault
 * called from hat layer
 */
int
vas_pagefault(int user, int write, int protection,
		ulong va, ulong pa)
{

	/*
	 * 1. find a region with faulting address (va)
	 * 2. check the cache type of this region
	 * 3. if zero-fill-on-demand, get a zero page and insert
	 */
	 
	if(user || (va & USER_BASE)) {
		extern struct tcb* cur_thread;
		vas_desc_t* vas;
		vas_region_t* rg;
		vas_region_t* found;
		ulong page;
	
		//printf("vas_pagefault(): user mode, va = %08x\n", va);

		/*
		 * first check the last region if set
		 * or go through the list of regions under this
		 * vas and find a region where faulting virtual
		 * address is; sets 'found' to a region with
		 * a faulting address upon completion;
		 * if found is null, segmentation fault is raised
		 */
		found = 0;
		vas = cur_thread->vas;		
		if((rg = vas->last_reg)
		   && va >= rg->start 
		   && (va < (rg->start + rg->size))) {
		   	//printf("pf in cached region\n");
			found = rg;
			
		} else {

			rg = vas->rg_list;
			if(rg) {
				do {
					//printf("region: [%08x:%08x]\n", 
					//	rg->start, rg->size);
					if(va >= rg->start
					   && (va < (rg->start + rg->size))) {									   	
						//printf("found\n");
						found = rg;
						break;
					}
					
					rg = rg->next;
				}		
				while(rg != vas->rg_list);
			}
		}

		/*
		 * this user thread accessed memory 
		 * it doesn't own. kill sonofabitch
		 */
		if(!found) {
			printf("segmentation fault (%08lx)\n", va);
			thread_die();
			return 0;
		}
		

		/*				
		 * setup hint for next time
		 */
		vas->last_reg = rg;
					
		/*
		 * get a real page
		 */
		page = alloc_page();
		if(page == 0) {
			printf("fatal!, cannot alloc page\n");
			return 0;
		}

		zero_page((void*) page);					
		cache_page(rg->cache, page, page);
					
		/*
		 * insert it in a missing slot
		 */ 
		hat_insert(vas->hat,
			   va,
			   page,
			   PTE_USER|PTE_WRITE);
						   						  
		return 1;						  
	}
	
	return 0;
}
