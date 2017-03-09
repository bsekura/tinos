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
 * $Id: page.c,v 1.2 1998/03/09 18:39:09 bart Exp $
 *
 */

#include <sys/types.h>
#include <i386/page.h>
#include <sema.h>
#include <vm/page.h>
#include <boot.h>
#include <pool.h>

/*
 * external stuff from boot module
 * this module must be initialized after
 * boot information has been dealt with, so that we can
 * start page database at top_memory
 */
extern ulong top_memory;
extern struct boot_info_s* boot_info;

/*
 * useful macros
 * get page address given the address of its slot in page database
 * and opposite
 */
#define page_address(page) \
        (((ulong)(page - page_db)) << PGSHIFT)

#define page_desc(addr) \
        (&page_db[(addr >> PGSHIFT)])

/*
 * per-page structure that describes every physical page
 * available in the system is linked on one of the lists below
 */
 
static page_desc_t* page_db;    /* page frame database */
static ulong pdb_size;          /* ... its size */
static ulong max_pages;         /* max physical pages in the system */

static page_desc_t* free_pages;	/* head of free list */
static ulong free_count;	/* count of elements on free list */

static page_desc_t* active_pages;	/* head of active pages list */
static page_desc_t* active_tail;	/* ...its tail... */
static ulong active_count;		/* ...and element count */

static page_desc_t* detached_pages;	/* head of detached pages */
static page_desc_t* detached_tail;	/* ...tail */
static ulong detached_count;		/* ...and count */

/*
 * semaphore for accessing free pages list
 */
static semaphore_t freeq_lock;

/*
 * page cache pool
 */
static master_pool_t cache_pool;

/*
 * list of non-reclaimable system page caches
 */
static cache_desc_t* sys_cache_list;
cache_desc_t* reserved_cache;

/*
 * global hash map
 *
 * pages are entered into global hash map when they are
 * in use by some memory cache. hash map provides for fast lookup
 * given cache address and offset
 */
typedef struct {
        ulong flags;
        page_desc_t* pages;
} hash_slot_t;

static ulong hash_count;
static ulong hash_mask;
static hash_slot_t* hash_map;

#define hash_func(cache, offset) \
        ((((ulong)cache) + ((ulong)(offset >> PGSHIFT))) & hash_mask)

/*
 * number of reserved pages starting at 0
 * one for null page and one for kernel page directory
 */
#define NPG_RESERVED    (2)

/* forward */
static void reserve_region(ulong start, ulong end);
void cache_reserved(cache_desc_t* cache, ulong start, ulong end);
void cache_dump(cache_desc_t* cache);

/*
 * initialize page frame database
 * page frame database starts just after kernel
 * also reserve used up memory regions (kernel, boot tasks images)
 */
void
init_pages()
{
        int c;
        cache_desc_t* ch;
        ulong count;

        /*
         * sanity check first
         * we shouldn't be here before boot initialization
         */
        if(boot_info->magic != BOOT_INFO_MAGIC) {
                printf("fatal!, boot info not initialized...\n");
                while(1);
        }

        /*
         * calculate number of all physical page frames available
         * also determine page frame database size
         */
        max_pages = btop(boot_info->mem_size);
        pdb_size = max_pages * sizeof(page_desc_t);

        printf("pm_init(): total: %d, pages: %d, pdb_size :%d\n",
                boot_info->mem_size, max_pages, pdb_size);
        printf("pm_init(): initializing page database at: %08x ...",
                top_memory);

        /*
         * initialize page frame database area
         */
        top_memory = page_round(top_memory);
        page_db = (page_desc_t*) top_memory;
        bzero(page_db, pdb_size);

        /*
         * put all pages on the free list
         * leave out first two pages: null and kernel pgdir
         */
        for(c = NPG_RESERVED; c < max_pages; c++) {
                page_db[c].flags = PGF_FREE;
                page_db[c].next = &page_db[c + 1];
        }
        page_db[c - 1].next = NULL;

        /*
         * mark reserved pages as such
         */
        for(c = 0; c < NPG_RESERVED; c++) {
                page_db[c].flags = PGF_RESERVED;
                page_db[c].next = NULL;
        }

        /*
         * initialize free pages list head, count and lock
         */
        free_pages = &page_db[NPG_RESERVED];
        free_count = max_pages - NPG_RESERVED;
        sema_init(&freeq_lock);

        printf("done\n");

        /*
         * update top memory pointer by page frame database size
         */
        top_memory += page_round(pdb_size);
        printf("top_memory: %08x\n", top_memory);

	/*
	 * list counters
	 */
        active_count = 0;
        detached_count = 0;

        /*
         * initialize global hash map
 	 * first determine hash map size (power of 2, >= max_pages)
         */
        hash_count = 1;
        while(hash_count < max_pages)
                hash_count <<= 1;

        hash_mask = hash_count - 1;

        printf("hash_count = %d, hash map size = %d\n", 
                hash_count, hash_count * sizeof(hash_slot_t));

        hash_map = (hash_slot_t*) top_memory;
        top_memory += page_round(hash_count * sizeof(hash_slot_t));
        printf("top_memory = %08x\n", top_memory);

        for(c = 0; c < hash_count; c++) {
                hash_map[c].flags = 0;
                hash_map[c].pages = NULL;
        }               

        /*
         * reserve used up memory regions
         * starting at video memory until top memory
         * (including our page database)
         * also reserve region used by boot tasks images (if any)
         * it'll be reclaimed once they turn into real tasks
         */
        reserve_region(0xa0000, top_memory);
#if 0      
        if(boot_info->boot_task) {
                reserve_region(boot_info->boot_start, boot_info->boot_end);
        }
#endif       

        /*
         * pool of cache descriptors
         */
        init_pool(&cache_pool, sizeof(cache_desc_t));

	/*
	 * create a cache that will become a reserved cache
	 * it'll keep pages permanently wired (kernel)
	 */
        reserved_cache = cache_create();
        if(!reserved_cache) {
        	printf("init_page(): fatal, cannot create reserved cache\n");
        	return;
        }
        
        cache_reserved(reserved_cache, 0x100000, top_memory);
        for(c = 0; c < NPG_RESERVED; c++) {
	        cache_page(reserved_cache, c << PGSHIFT, c << PGSHIFT);
	}	      
}

/*
 * exclude given region of pages from the page database
 * and mark as reserved
 */
static void
reserve_region(ulong start, ulong end)
{
        ulong c, s, e;

        s = (start >> PGSHIFT);
        e = (end >> PGSHIFT);

#ifdef _DEBUG
        printf("pm_reserve(): from %d to %d\n", s, e);
#endif       
        for(c = s; c < e; c++) {
                page_db[c].flags = PGF_RESERVED;
        }
        
#ifdef _DEBUG
        printf("relinking from %08x to %08x\n",
                ((s-1) << PGSHIFT), (c << PGSHIFT));
#endif
               
        page_db[s - 1].next = &page_db[c];
        free_count -= (e-s);
}

void
reclaim_region(ulong start, ulong end)
{
	ulong c;

#ifdef _DEBUG	
	printf("reclaim_region(): %08x-%08x\n", start, end);
#endif
	for(c = page_addr(start); c < page_round(end); c += PAGESZ) {
		free_page(c);
	}
}

/*
 * get the page off the free list
 * mark it as used and return its physical address
 */
ulong
alloc_page()
{
        page_desc_t* p;

        sema_p(&freeq_lock);
        p = free_pages;
        if(p && p->flags == PGF_FREE) {
                free_pages = free_pages->next;
                free_count--;
                sema_v(&freeq_lock);

                p->next = NULL;
                p->flags = PGF_USED;

                return (((ulong)(p - page_db)) << PGSHIFT);
        }

        /*
         * something went wrong
         */
        sema_v(&freeq_lock);
        if(!p) {
                printf("get_free_page(): no free pages.\n");
                return (NULL);
        }
        if(p->flags != PGF_FREE) {
                printf("get_free_page(): oops, free list corrupted.\n");
                return (NULL);
        }
}

/*
 * put the page on the free list
 */
int
free_page(ulong addr)
{
        ulong i;

        i = (addr >> PGSHIFT);
        if(i > NPG_RESERVED && i < max_pages) {
                page_desc_t* p;

                sema_p(&freeq_lock);
                p = &page_db[i];
                p->next = free_pages;
                free_pages = p;

                p->flags = PGF_FREE;
                free_count++;

                sema_v(&freeq_lock);
                return (1);

        } else {
                printf("free_page(): bad address\n");
        }

        return (0);
}

/*
 * enqueue page (described by its descriptor)
 * to a cache list
 */
void
enqueue_cache(cache_desc_t* cache, page_desc_t* page)
{
        if(cache->tail) {
                cache->tail->next_cache = page;
        }
        page->next_cache = NULL;
        cache->tail = page;
        if(cache->head == NULL) {
                cache->head = page;
        }       
}

/*
 * create new page cache
 * returns newly created cache descriptor
 */
cache_desc_t*
cache_create()
{
        cache_desc_t* cache;

        cache = alloc_pool(&cache_pool);
        if(!cache) {
                printf("cache_create(): cannot alloc from cache pool\n");
                return (NULL);
        }

        cache->head = NULL;
        cache->tail = NULL;

        return (cache);
}

/*
 * cache reserved pages directly from page frame database
 */
void
cache_reserved(cache_desc_t* cache, ulong start, ulong end)
{
        ulong s, e, offset;
        page_desc_t* page;

        s = (start >> PGSHIFT);
        e = (end >> PGSHIFT);

        offset = start;
        for(page = &page_db[s]; page < &page_db[e]; page++) {
                cache_page(cache, page_address(page), offset);
                offset += PAGESZ;
        }
}

int
cache_page(cache_desc_t* cache, ulong page, ulong offset)
{
        hash_slot_t* h;
        page_desc_t* p;

        p = page_desc(page);

        /*
         * enqueue this page on cache page list
         */
        enqueue_cache(cache, p);
        p->cache = cache;
        p->offset = btop(offset);

        /*
         * put it on global hash map
         */
#ifdef _DEBUG         
        printf("cache_page(): hash = %d\n", hash_func(cache, offset));
#endif        
        h = &hash_map[hash_func(cache, offset)];
        p->next_hash = h->pages;
        h->pages = p;
        
        return (0);
}

/*
 * find a page given cache and memory object offset
 */ 
page_desc_t*
cache_lookup(cache_desc_t* cache, ulong offset)
{
        page_desc_t* p;
        hash_slot_t* h;
        ulong o;

        printf("cache_lookup(): hash = %d\n", hash_func(cache, offset));
        h = &hash_map[hash_func(cache, offset)];
        o = btop(offset);
        for(p = h->pages; p; p = p->next_hash) {
                if(p->cache == cache && p->offset == o)
                        break;
        }

        return (p);
}

void
cache_dump(cache_desc_t* cache)
{
        page_desc_t* p;

        p = cache->head;
        while(p) {
                printf("[%x:%x]", page_address(p), p->offset);
                p = p->next_cache;
        }
        printf("\n");
}

/*
 * cache_pagefault is called by vas layer when a region
 * with the faulting address has been found and it's
 * time for the region's cache to fill in missing
 * address space slots. depending on the type of the
 * cache we have to do the following here:
 *
 * zero-fill-on-demand cache:
 *	- allocate new core page
 *	- zero it
 *	- fill in the faulting address slot
 *
 * load-on-demand cache:
 *	- put the stub in the slot
 *	- allocate real page
 *	- ask memory object to load a page 
 *	- wait for io to complete
 *	- fill in the slot
 *
 * copy-on-write cache:
 *	- [TBD]
 */
 
int
cache_pagefault(cache_desc_t* cache, ulong va)
{
	return 0;
}

void
pm_dump()
{
        printf("max_pages : %d\n", max_pages);
        printf("free_count: %d\n", free_count);
}
