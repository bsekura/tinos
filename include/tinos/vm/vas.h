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
 * $Id: vas.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __vas_h__
#define __vas_h__


#include <vm/page.h>
#include <sys/types.h>
#include <i386/hat.h>
#include <sema.h>

struct vas_region {
	struct vas_desc*	vas;
        ulong                   start;
        ulong                   size;
	cache_desc_t*		cache;
	ulong			offset;
        struct vas_region*      next;
        struct vas_region*      prev;
};

typedef struct vas_region vas_region_t;

struct semaphore;

struct vas_desc {
        hat_desc_t              hat_desc; /* underlying MMU stuff */
        hat_desc_t*             hat;      /* ... I prefer -> to . */
        vas_region_t*           rg_list;  /* doubly linked list of regions */
        vas_region_t*           last_reg; /* most recently accessed region */
        semaphore_t       	vas_lock;
        int                     refcnt;
        struct vas_desc*        next;
        struct vas_desc*        prev;
};

typedef struct vas_desc vas_desc_t;

/*
 * prototypes
 */

void		vas_init();
vas_desc_t* 	vas_alloc();
vas_region_t*	region_create(vas_desc_t* vas, ulong start, ulong size,
			      cache_desc_t* cache, ulong offset);


#endif /* __vas_h__ */
