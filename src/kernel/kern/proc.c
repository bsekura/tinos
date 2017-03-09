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
 *
 * $Id: proc.c,v 1.2 1998/02/26 19:48:12 bart Exp $
 *
 */

#include <proc.h>
#include <boot.h>
#include <pool.h>
#include <queue.h>
#include <string.h>
#include <i386/seg.h>

extern struct boot_info_s* boot_info;
extern cache_desc_t* reserved_cache;

#define MAX_STACK	(4*PAGESZ)

static master_pool_t	proc_pool;

static void dump_exec_header(struct exec_hdr_s*);

/*
 * list of all the processes in the system
 */
static struct process* proc_list;


/*
 * init proc subsystem
 */
void
init_proc()
{
	init_pool(&proc_pool, sizeof(struct process));	
}

static ulong
get_pid()
{
	static ulong pid_counter = 1;
	return pid_counter++;
}

ulong
get_stack(struct process* p)
{
	p->stack = (ulong) (p->stack - MAX_STACK);
	if(p->stack < p->highest) {
		
		printf("get_stack(): no space for stack\n");
		return 0;
	}
	
	return (p->stack);	
}

struct process*
new_proc(struct process* parent)
{
	struct process* p;

	p = (struct process*) alloc_pool(&proc_pool);
	if(p == 0) {
		return NULL;
	}
	
	bzero(p, sizeof(struct process));
	p->pid = get_pid();
	p->vas = vas_alloc();
	p->parent = parent;

#if 0 /* bzero above does the job */
	p->brk = 0x00000000;	
	p->highest = 0x00000000;
#endif

	enqueue(proc_list, next, prev, p);
	
	/*
	 * stack begins at the top of virtual address
	 * space (leave one page slot empty)
	 */
	p->stack = (ulong) (-PAGESZ);

	return (p);
}

int
del_proc(struct process* p)
{
	if(!p)
		return 0;
		
	dequeue(proc_list, next, prev, p, struct process*);
}

static char*
basename(char* s)
{
	char* p = strrchr(s, '/');
	return (p ? p+1 : s);
}

static void
setup_args(char* dest, char* cmdline)
{
#define	MAX_ARGSZ (64)

	char* p, *d, *args;
	ulong argc;
	ulong* p_argc;
	char** argv;
	char tmp[256];
	
	p_argc = (ulong*) dest;
	dest += sizeof(ulong);
	
	args = dest;
	dest += MAX_ARGSZ;
	
	argv = (char**) dest;

	/*
	 * deal with the first argument 
	 * trim off path
	 */
	p = cmdline;
	d = tmp;
	while(*p && *p != ' ')
		*d++ = *p++;
	*d = 0;
	d = args;
	strcpy(d, basename(tmp));
	argv[0] = d - USER_BASE;
	d += strlen(tmp) + 1;
	argc = 1;
	
	/*
	 * pack the rest of args if any
	 */
	if(*p) {
		while(*p++ == ' ');
		p--;
		
		while(*p) {
			argv[argc++] = d - USER_BASE;
			while(*p && *p != ' ')
				*d++ = *p++;
			*d++ = 0;
			if(*p == 0)
				break;
			while(*p++ == ' ');
			p--;
		}
	}
	*p_argc = argc;
}

int
create_bootproc()
{
	int c;
	
	for(c = 0; c < boot_info->boot_count; c++) {
		
		vas_region_t* text, *data;
		struct process* p;
		struct tcb* t;
		struct exec_hdr_s e;
		ulong bss_vaddr;
		struct boot_task_s* b;
		
		b = &boot_info->boot_task[c];		
		if(!exec_header(b, &e)) {
			printf("create_bootproc(): wierd image, bailing out\n");
			continue;
		}

#ifdef _DEBUG		
		dump_exec_header(&e);
#endif	
		if(!(p = new_proc(0))) {
			printf("create_bootproc(): p == 0\n");
			return 0;
		}

		text = region_create(p->vas, e.text_vaddr, e.text_size,
			      	     	reserved_cache, e.text_vaddr);
		data = region_create(p->vas, e.data_vaddr, e.data_size,
			      		reserved_cache, e.data_vaddr);	     
			      
		/*
		 * bss region
		 * we check whether bss fits into data page
		 * in which case no separate region is necessary
		 */			     
		bss_vaddr = page_round(e.data_vaddr + e.data_size);
		if(e.bss_end > bss_vaddr) {
			region_create(p->vas, bss_vaddr, e.bss_end - bss_vaddr,
					reserved_cache, bss_vaddr);
		}
		p->brk = page_round(e.bss_end);
		p->highest = p->brk;

#ifdef _DEBUG		
		region_dump(p->vas);			   
#endif
		
		/*
		 * XXX temp kludge
		 *     map video memory
		 */
		#define VA_USER_VIDEO	(0x20000000)
		hat_map(p->vas->hat,
			VA_USER_VIDEO | USER_BASE, 
			0xb8000, 
			4, 
			PTE_WRITE|PTE_USER);

	#if 0		
		printf("text: %08x, data: %08x\n",
			*((ulong*) e.text_ptr), *((ulong*) e.data_ptr));
	#endif

		/*
		 * we have to commit text and initialized data pages
		 * of boot process because we don't know where it came
		 * from and we can't load it on demand. Those pages
		 * are wired in memory just like kernel pages
		 */			
		region_commit(text);
		region_commit(data);

		/*
		 * enter newly created virtual address space
		 * and copy text and initialized data to allocated regions
		 */		
		hat_enter(p->vas->hat);
		bcopy((void*) e.text_ptr, (void*) e.text_vaddr, e.text_size);
		bcopy((void*) e.data_ptr, (void*) e.data_vaddr, e.data_size);

		setup_args((char*) e.data_vaddr, (char*) b->cmdline);
		
		/*
		 * we just made a copy of this process text
		 * and initialized data, so we can return pages occupied
		 * by its image to the system
		 */
		reclaim_region(b->start,  b->start + b->size);

		/*
		 * create main thread
		 */
		printf("creating user thread...\n");		
		t = create_thread(p, e.entry);
		if(!t) {
			printf("create_bootproc(): cannot create thread\n");
		}
	}
	
	return 1;
}

void
proc_attach_thread(struct tcb* t)
{
	/*
	 * add thread to process list
	 * allocate stack area for it
	 * setup its stack pointer
	 */
	struct process* p;
	vas_region_t* stack;
	 
	p = t->proc;
	if(!p) {
		printf("proc_attach_thread(): p == null\n");
		return;
	}
	enqueue(p->threads, next_proc, prev_proc, t);
	get_stack(p);
	stack = region_create(p->vas, p->stack, MAX_STACK,
				reserved_cache, p->stack);			    
				
	t->stk = (p->stack - USER_BASE) + MAX_STACK - sizeof(long);					
}

void
proc_detach_thread(struct tcb* t)
{
	/*
	 * thread 't' has died
	 * clean up its pieces
	 *
	 * remove it from process list
	 * release any stack pages
	 * adjust free stack area
	 */
	struct process* p;
	 
	p = t->proc;
	if(!p) {
		printf("proc_dettach_thread(): p == null\n");
		return;
	}
	
	dequeue(p->threads, next_proc, prev_proc, t, struct tcb*);
}

int
kill_proc(struct process* p)
{
	/*
	 * first kill all threads attached to this process
	 * ('threads' list)
	 * once they're killed, we can safely destroy vas
	 */	

	return 0;
}

static void
dump_exec_header(struct exec_hdr_s* e)
{
	printf("text[vaddr,size,ptr]: %08x %08x %08x\n",
		e->text_vaddr, e->text_size, e->text_ptr);
	printf("data[vaddr,size,ptr]: %08x %08x %08x\n",
		e->data_vaddr, e->data_size, e->data_ptr);
	printf("entry: %08x, bss_end: %08x\n", 
		e->entry, e->bss_end);
}
  