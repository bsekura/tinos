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
 * $Id: pool.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __pool_h__
#define __pool_h__

#include <sema.h>

/*
 * this describes single page pool of equal size chunks
 * it's located at the beginning of the page
 */
typedef struct pool_desc {
	int			size;	/* chunk size */
	int			refcnt;	/* reference count */
	int			nfree;	/* free chunks left */
	int			total;	/* total number of chunks */
	void*			free;	/* free chunks list head */
	struct master_pool*	master;	/* master pool to which we belong to */
	struct pool_desc*	next;	/* list link */
} pool_desc_t;

/*
 * master pool descriptor
 * links single page pools together 
 */
typedef struct master_pool {
	pool_desc_t*	pools;	/* pool list */
	pool_desc_t*	hint;	/* hint to avoid list traversal */
	int		count;	/* how many pools we allocated */
	int		size;	/* chunk size */
	semaphore_t 	s_lock; /* serializing semaphore */
} master_pool_t;

/*
 * pool operations
 */
#ifdef __cplusplus
extern "C" {
#endif

void 		init_pool(master_pool_t* mp, int sz);
pool_desc_t* 	new_pool(master_pool_t* mp);
void*	 	alloc_pool(master_pool_t* mp);
void	 	free_pool(void* addr);

#ifdef __cplusplus
}
#endif

#endif /* __pool_h__ */
