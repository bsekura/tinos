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
 * $Id: pool.c,v 1.2 1998/03/09 18:39:04 bart Exp $
 *
 */

#include <sys/types.h>
#include <pool.h>
#include <i386/page.h>

#define page_align(x)	((x) & 0xfffff000)

static ulong pool_count;
static ulong page_count;

void
init_pool(master_pool_t* mp, int sz)
{
	mp->pools = NULL;
	mp->hint = NULL;
	mp->count = 0;
	mp->size = sz;
	sema_init(&mp->s_lock);
	
	pool_count++;
}

/*
 * allocate a new pool for a given master pool
 * actually rips some real memory and initializes pool descriptor
 */
pool_desc_t*
new_pool(master_pool_t* mp)
{
	int c, n;
	pool_desc_t* p;
	ulong addr, size;
	void* page;

	/*
	 * see how many chunks will fit into a page
	 * take into account pool descriptor at the beginning of a page
	 */
	size = mp->size;
	n = (PAGESZ - sizeof(pool_desc_t)) / size;

	/*
	 * get some real memory
	 */
	page = (void*) alloc_page();
	if(!page) {
		printf("init_pool(): couldn't allocate page.\n");
		return (NULL);
	}

	page_count++;
	/*
	 * initialize pool descriptor
	 */
	p = (pool_desc_t*) page;
	p->master = mp;
	p->refcnt = 0;
	p->total = n;
	p->nfree = n;
	p->size = size;
	p->free = page + sizeof(pool_desc_t);

	/*
	 * update master pool information: list links and count
	 */
	p->next = mp->pools;
	mp->pools = p;
	mp->count++;

	/*
	 * initialize free list pointers for every chunk
	 * trailing with null
	 */
	addr = (ulong) p->free;
	for(c = 0; c < n - 1; c++) {
	      *((void**) addr) = (void*)(addr + size);
	      addr += size;
	}
	*((void**) addr) = NULL;
	
#if 0
	c = n;
	while(c--) {
		*((void**) addr) = (c == 0) ? NULL : (void*)(addr + size);
		addr += size;
	}
#endif

#if 0
	printf("new_pool(): page=%x, n=%d, free=%x\n",
		(ulong) page, n, (ulong) p->free);
#endif

	return (p);
}

/*
 * allocate single chunk from given master pool
 */
void*
alloc_pool(master_pool_t* mp)
{
	pool_desc_t* p;
	ulong addr;

	sema_p(&mp->s_lock);

	/*
	 * first check if hint is valid
	 * if not, go through pool list to find one containing some free chunks
	 * if no pools with free space, get a new one
	 * always update hint for future use
	 */
	p = mp->hint;
	if(p == NULL || p->free == NULL) {
		for(p = mp->pools; p; p = p->next) {
			if(p->free) {
				mp->hint = p;
				break;
			}
		}
		if(!p) {
			p = new_pool(mp);
			mp->hint = p;
		}
	}

	/*
	 * get a chunk and maintain pointers
	 */
	addr = (ulong) p->free;
	p->free = *((void**) addr);
	p->refcnt++;
	p->nfree--;

	sema_v(&mp->s_lock);

	return ((void*) addr);
}

/*
 * free chunk
 */
void
free_pool(void* addr)
{
	pool_desc_t* p;
	master_pool_t* mp;
	ulong pa;
	void* a;

	/*
	 * find the start of page, which is our pool descriptor
	 */
	pa = page_align((ulong) addr);
	p = (pool_desc_t*) pa;
	mp = p->master;

	sema_p(&mp->s_lock);

	/*
	 * free this chunk
	 */
	a = addr;
	*((void**) a) = p->free;
	p->free = a;
	p->nfree++;

	/*
	 * check reference count and free the whole pool if needed
	 */
	if(--p->refcnt == 0) {
		pool_desc_t* lp;

#if 0
		printf("free_pool(): refcnt=0, looking for pool_desc.\n");
#endif
		/*
		 * if this pool is a hint, update hint
		 * since this one is about to cease to exist
		 */
		if(mp->hint == p)
			mp->hint = p->next;

		/*
		 * find this pool in master pool linked list and unlink it
		 * first check out the head of the list
		 * if not sucessful, go through the whole list
		 */
		if(mp->pools == p) {
			mp->pools = p->next;
		}
		else {
			for(lp = mp->pools; lp; lp->next) {
				if(lp->next == p) {
					printf("got it, unlinking\n");
					lp->next = p->next;
					break;
				}
			}

			if(!lp) {
				printf("free_pool(): oops.\n");
			}
		}

		/*
		 * decrease the count and free memory of this pool
		 */
		mp->count--;
		free_page((ulong) p);
		page_count--;
	}

	sema_v(&mp->s_lock);
}

void
dump_pool(master_pool_t* mp)
{
	pool_desc_t* p;
	void* a;
	int c;

	printf("master: pools=%x, hint=%x, count=%d\n",
		(ulong) mp->pools, (ulong) mp->hint, mp->count);
	c = 0;
	for(p = mp->pools; p; p = p->next) {
		printf("pool[%d]: %x, free=%x, nfree=%d, refcnt=%d\n",
			c++, (ulong) p, (ulong) p->free, p->nfree, p->refcnt);
		a = p->free;
		while(a) {
			printf("%x ", *((void**) a));
			a = *((void**) a);
		}
		printf("\n");
	}
}

void
pool_stats()
{
	printf("pool_stats(): %d pools, %d pages\n",
		pool_count, page_count);
}
