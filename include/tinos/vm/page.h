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
 *
 */

#ifndef __vm_page_h__
#define __vm_page_h__

#include <sys/types.h>
#include <i386/page.h>
#include <sema.h>

/*
 * this describes every page frame of physical memory
 */
typedef struct page_desc {
        ulong flags;
        struct page_desc* next;		/* free, active, detached list */
        struct page_desc* next_hash; 	/* global hash map */
        struct page_desc* next_cache; 	/* cache page list */
	struct cache_desc* cache;
	ulong offset;
} page_desc_t;

#define PGF_FREE        (0x00)
#define PGF_RESERVED    (0x02)
#define PGF_USED        (0x04)
#define PGF_DETACHED	(0x08)

typedef struct cache_desc {
        ulong           flags;
	ulong		storage_id;
        page_desc_t*	head;
	page_desc_t*	tail;
	ulong		page_count;
} cache_desc_t;

cache_desc_t* cache_create();

#endif /* __vm_page_h__ */
