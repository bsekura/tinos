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
 * $Id: buf.c,v 1.1 1998/03/09 18:40:07 bart Exp $
 *
 */
 
#include "buf.h"
#include "blk.h"

#include <i386/page.h>

#define	MAX_BUF	(PAGESZ/sizeof(struct buffer_head))

#define HASH_TABLE_SIZE	(32)
#define	HASH_FUNC(dev,blk)	(((dev)^(blk))%HASH_TABLE_SIZE)

static ulong 	buf_size;
static ulong	buf_page_count;
static ulong	buf_pages[MAX_BUF];
static ulong	buf_per_page;

static struct buffer_head* buffers;
static struct buffer_head* hash_table[HASH_TABLE_SIZE];
static struct buffer_head* free_list;

#if 0
#define _HASH
#endif

#ifdef _HASH
#include "hash.h"
static struct hash_table* buf_hash;
#endif

static void buf_dump_free();

int
buf_init(ulong block_size)
{
	int c, i;
	ulong page, count;

	/*
	 * see how many blocks will fit into one page
	 * and how many pages we will need to allocate
	 */
	buf_size = block_size;	
	buf_per_page = PAGESZ / buf_size;
	buf_page_count = MAX_BUF / buf_per_page;
	printf("buf_per_page = %d, buf_page_count = %d\n",
		buf_per_page, buf_page_count);
		
	page = alloc_page();
	if(!page)
		return 0;
		
	buffers = (struct buffer_head*) page;

	count = 0;
	for(c = 0; c < buf_page_count; c++) {
	
		page = alloc_page();
		if(!page) {
			printf("cannot alloca page\n");
			return 0;
		}	

		buf_pages[c] = page;
		for(i = 0; i < buf_per_page; i++) {
			buffers[count].dev = 0;
			buffers[count].blk = 0;
			buffers[count].data = (void*) page;
			bzero(buffers[count].data, buf_size);
			buffers[count].next_free = &buffers[count + 1];
			buffers[count].prev_free = &buffers[count - 1];
			count++;
			page += buf_size;
		}
	}
	buffers[count - 1].next_free = 0;
	buffers[0].prev_free = &buffers[count - 1];
	
	if(count != MAX_BUF) {
		printf("oops, count != MAX_BUF\n");
	}

	free_list = buffers;	
	//buf_dump_free();
	
	for(c = 0; c < HASH_TABLE_SIZE; c++) {
		hash_table[c] = (struct buffer_head*) 0;
	}

#ifdef _HASH	
	hash_init();
	buf_hash = hash_alloc(64);
	if(!buf_hash) {
		printf("buf_init(): couldn't alloc buf_hash\n");
		return 0;
	}
#endif
	
	return 1;
}

void
buf_destroy()
{
	int c;

	for(c = 0; c < buf_page_count; c++) {
		if(buf_pages[c]) {
			free_page(buf_pages[c]);
		}
		buf_pages[c] = 0;
	}
		
	free_page((ulong) buffers);	
	buffers = (void*) 0;			
}

/*
 * insert buffer at the head of hash queue
 */
static void
buf_insert_hash(struct buffer_head* bh, int ihash)
{
	bh->next = hash_table[ihash];
   	hash_table[ihash] = bh;
   	if(bh->next)
       		bh->next->prev = bh;
}

/*
 * remove buffer from its hash queue
 */
static void
buf_remove_hash(struct buffer_head* bh)
{
	int ihash;
	
	ihash = HASH_FUNC(bh->dev, bh->blk);
   	if(bh->next)
      		bh->next->prev = bh->prev;
   	if(bh->prev)
      		bh->prev->next = bh->next;

   	if(hash_table[ihash] == bh)
	      hash_table[ihash] = bh->next;
	      
  	bh->next = bh->prev = NULL;
}

/*
 * enqueue at the end of free list
 */
static void
buf_insert_free(struct buffer_head* bh)
{
//#if 0
   	bh->next_free = free_list;
   	if(free_list) {
      		bh->prev_free = free_list->prev_free;
      		if(free_list->prev_free)
         		free_list->prev_free->next_free = bh;
	      	free_list->prev_free = bh;
   	}
//#endif

	/*
	if(free_list == 0) {
		free_list = bh;
		bh->next_free = bh->prev_free = free_list;
		return;
	}
	
	bh->next_free = free_list;
	bh->prev_free = free_list->prev_free;
	bh->prev_free->next_free = bh;
	free_list->prev_free = bh;
	*/
}

/*
 * remove from free list
 */
static void
buf_remove_free(struct buffer_head* bh)
{
//#if 0
   	if(bh->next_free)
      		bh->next_free->prev_free = bh->prev_free;
   	if(bh->prev_free)
      		bh->prev_free->next_free = bh->next_free;

   	if(free_list == bh)
      		free_list = bh->next_free;
		
   	bh->next_free = bh->prev_free = NULL;
//#endif

	/*
	if(bh->next_free == free_list) {
		free_list = 0;
	} else {
		struct buffer_head* next = bh->next_free;
		struct buffer_head* prev = bh->prev_free;
		
		next->prev_free = prev;
		prev->next_free = next;
		if(free_list == bh) {
			free_list = next;
		}
	}
	*/
}

/*
 * get first free buffer from the head of free buffers list
 */ 
static struct buffer_head*
buf_get_free()
{
	struct buffer_head* bh;

   	bh = free_list;
   	if(bh == NULL)
      		return (NULL);

   	buf_remove_free(bh); 
   	return (bh);
}

/*
 * look for buffer in hash table
 */
static struct buffer_head*
buf_get_hash(ulong dev, ulong blk, ulong ihash)
{
   	struct buffer_head* bh;

   	for(bh = hash_table[ihash]; bh; bh = bh->next)
      		if(bh->dev == dev && bh->blk == blk)
         		return (bh);

   	return (NULL);
}

static void
buf_dump_free()
{
	struct buffer_head* bh;
	
	bh = free_list;
	while(bh) {
		printf("x ");
		bh = bh->next_free;
	}
	printf("\n");
}

static struct buffer_head*
buf_getblk(ulong dev, ulong blk)
{
	ulong ihash;
	struct buffer_head* bh;
	
	ihash = HASH_FUNC(dev, blk);
	bh = buf_get_hash(dev, blk, ihash);
	if(bh) {
		//printf("buf_getblk(): found in hash_table %08lx\n", 
		//	(ulong) bh);
		
		buf_remove_free(bh);
		return (bh);
	}
	
	//printf("buf_getblk(): getting free buffer\n");
	bh = buf_get_free();
	if(!bh) {
		printf("buf_getblk(): no free buffers\n");
		for(;;);
	}
	
	if(bh->dev == dev && bh->blk == blk)
		return (bh);
		
	buf_remove_hash(bh);
	bh->dev = dev;
	bh->blk = blk;
	bh->flags = 0;
	buf_insert_hash(bh, ihash);
	
	return (bh);
}

#ifndef _HASH

struct buffer_head*
bread(ulong dev, ulong blk)
{
	struct buffer_head* bh;
	
	bh = buf_getblk(dev, blk);
	if(!bh) {
		printf("bread(): buf_getblk() gave us null\n");
		return 0;
	}
	
	if(bh->flags) {
		return (bh);
	}

	//printf("bread(): reading block %d\n", bh->blk);
	if(!devblk_read(dev, bh->blk, 1, bh->data)) {
		printf("bread(): devblk_read() failed\n");
		return 0;
	}	
	bh->flags = 1;
	
	
	return (bh);
}

void
brelse(struct buffer_head* bh)
{
	if(!bh) {
		printf("brelse(): oops, bh == null\n");
		return;
	}
	
	buf_insert_free(bh);
}

#else

struct buffer_head*
bread(ulong dev, ulong blk)
{
	struct buffer_head* bh;
	
	bh = hash_lookup(buf_hash, blk);
	if(bh) {
		//printf("bread(): hash_lookup() found a buffer\n");
		return (bh);
	}
	
	bh = buffers++;
	if(!bh) {
		printf("bread(): malloc(bh) failed\n");
		return 0;
	}
	
	bh->dev = dev;
	bh->blk = blk;
	if(!bh->data) {
		printf("bread(): bh->data is null\n");
		return 0;
	}
	
	if(!devblk_read(dev, bh->blk, 1, bh->data)) {
		printf("bread(): devblk_read() failed\n");
		return 0;
	}	

	hash_insert(buf_hash, blk, bh);
	return (bh);	
}

void
brelse(struct buffer_head* bh)
{
	hash_remove(buf_hash, bh->blk);
}

#endif /* _HASH */
