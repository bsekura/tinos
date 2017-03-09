/* TINOS Operating System
 * Copyright (c) 1996, 1997, 1998 Bart Sekura
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
 * $Id: hash.c,v 1.1 1998/03/09 18:40:08 bart Exp $
 *
 */
 
/*
 * ripped off from VSTa
 * Copyright (c) 1993-1996 Andy Valencia
 *
 */
 
#include "hash.h"

#include <pool.h>
#include <i386/page.h>

#define	HASH_FUNC(key, mask)	(((key)^((key)>>2)^((key)>>6)) & mask)

static master_pool_t hash_table_pool;
static master_pool_t hash_node_pool;

void
hash_init()
{
	init_pool(&hash_table_pool, sizeof(struct hash_table));
	init_pool(&hash_node_pool, sizeof(struct hash_node));
}

struct hash_table*
hash_alloc(ulong count)
{
	struct hash_table* h;
	
	h = (struct hash_table*) alloc_pool(&hash_table_pool);
	if(h) {
		h->count = count;
		h->mask = h->count - 1;
		h->refcnt = 0;
		h->nodes = (struct hash_node**) alloc_page();
		if(h->nodes) {
			bzero(h->nodes, PAGESZ);
		} else
			printf("hash_alloc(): alloc_page() null (h->nodes)\n");	
	} else
		printf("hash_alloc(): couldn't alloc hash_table\n");
	
	return (h);
}

void
hash_free(struct hash_table* h)
{
	if(h) {
		int c;
		struct hash_node* node, *next_node;
	
		node = next_node = (struct hash_node*) 0;
		for(c = 0; c < h->count; h++) {
			for(node = h->nodes[c]; node; node = next_node) {
				next_node = node->next;
				free_pool(node);
				h->refcnt--;
			}
		}
		free_page((ulong) h->nodes);
		free_pool(h);
	}
}

int
hash_insert(struct hash_table* h, ulong key, void* data)
{
	struct hash_node* node;

	if(!h)
		return (0);
		
	node = (struct hash_node*) alloc_pool(&hash_node_pool);
	if(node) {
		ulong i;
	
		i = HASH_FUNC(key, h->mask);
		node->key = key;
		node->data = data;
		node->next = h->nodes[i];
		h->nodes[i] = node;
		
		h->refcnt++;
		return (1);
	}
	
	return (0);
}

int
hash_remove(struct hash_table* h, ulong key)
{
	struct hash_node** node, *n;
	int i;

	if(!h)
		return (0);

	i = HASH_FUNC(key, h->mask);
	node = &h->nodes[i];
	n = *node;
	while(n) {
		if(n->key == key) {		
			*node = n->next;
			free_pool(n);
			
			h->refcnt--;
			return (1);
		}
		node = &n->next;
		n = *node;
	}
	
	return (0);
}

void*
hash_lookup(struct hash_table* h, ulong key)
{
	struct hash_node* node;
	int i;

	if(!h)
		return (0);
		
	i = HASH_FUNC(key, h->mask);
	for(node = h->nodes[i]; node; node = node->next) {
		if(node->key == key) {
			return (node->data);
		}
	}
	
	return (void*) 0;
}
