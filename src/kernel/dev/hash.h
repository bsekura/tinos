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
 * $Id: hash.h,v 1.1 1998/03/09 18:40:08 bart Exp $
 *
 */

/*
 * ripped off from VSTa
 * Copyright (c) 1993-1996 Andy Valencia
 */
  
#ifndef __hash_h__
#define __hash_h__

#include <sys/types.h>

struct hash_table {
	ulong	count;
	ulong	mask;
	ulong	refcnt;
	struct hash_node** nodes;
};

struct hash_node {
	ulong	key;
	void*	data;
	struct hash_node* next;
	ulong	pad;
};

struct hash_table* hash_alloc(ulong count);
void hash_free(struct hash_table* h);
int hash_insert(struct hash_table* h, ulong key, void* data); 
int hash_remove(struct hash_table* h, ulong key); 
void* hash_lookup(struct hash_table* h, ulong key);

#endif /* __hash_h__ */

