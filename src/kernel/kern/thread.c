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
 * $Id: thread.c,v 1.3 1998/03/01 21:06:49 bart Exp $
 *
 */

#include <thread.h>
#include <sema.h>
#include <pool.h>
#include <proc.h>
#include <i386/exec.h>
#include <i386/hat.h>
#include <i386/page.h>
#include <i386/inlines.h>
#include <i386/spl.h>

extern struct tcb* cur_thread;
extern hat_desc_t* kern_hat;

static master_pool_t thread_pool;

/* 
 * thread queue
 */
struct tcb* ready_threads;
ulong n_threads;
ulong ready_count;

/*
 * initialize thread control block pool
 */
void
init_thread()
{
        init_pool(&thread_pool, sizeof(struct tcb));
        ready_threads = NULL;
        n_threads = 0;
	ready_count = 0;
}

/*
 * add a thread to the list
 * it also ends up on the ready queue
 */
static inline void
enqueue_ready(struct tcb* t)
{
	if(ready_threads) {
		t->next = ready_threads;
		t->prev = ready_threads->prev;
		t->prev->next = t;
		ready_threads->prev = t;

		t->next_ready = ready_threads;
		t->prev_ready = ready_threads->prev_ready;
		t->prev_ready->next_ready = t;
		ready_threads->prev_ready = t;

		return;		
	}
	ready_threads = t;
	t->next = t->prev = t;
	t->next_ready = t->prev_ready = t;
}

/*
 * remove thread from the list (and ready queue)
 */
static inline void
remove_ready(struct tcb* t)
{
	if(t->next != t) {
		register struct tcb* next = t->next;
		register struct tcb* prev = t->prev;
		next->prev = prev;
		prev->next = next;
		if(ready_threads == t) {
			ready_threads = next;
		}

		/*
	 	 * take care of ready list too
		 */
		next = t->next_ready;
		prev = t->prev_ready;
		next->prev_ready = prev;
		prev->next_ready = next;

		return;
	}
	ready_threads = NULL;
}

/*
 * when linking or unlinking a thread from a ready list
 * we don't have to take care of the head of the list
 * since no such thing exists. There is always one thread
 * ready to run (idle thread) linked on this list.
 * Assuming that, the code gets easier and quicker.
 */
static inline void
link(struct tcb* t)
{
	t->next_ready = ready_threads;
	t->prev_ready = ready_threads->prev_ready;
	t->prev_ready->next_ready = t;
	ready_threads->prev_ready = t;
}

static inline void
unlink(struct tcb* t)
{
	register struct tcb* next = t->next_ready;
	register struct tcb* prev = t->prev_ready;
	next->prev_ready = prev;
	prev->next_ready = next;
}

/*
 * take a nap
 */
void
thread_sleep(struct tcb* t)
{
	spl_t s;

        if(t->state == TS_WAITING) {
                printf("thread_sleep(): warning, the guy sleeps already...\n");
                return;
        }

	s = splhi_save();
        t->state = TS_WAITING;
        t->cur_prio = t->prio;
	unlink(t);
	ready_count--;
	splx(s);
}

/*
 * wake up, it's time to work again
 */
void
thread_wakeup(struct tcb* t)
{
	spl_t s;

        if(t->state == TS_READY) {
                printf("thread_wakeup(): warning, the guy is up already...\n");
                return;
        }
	
	s = splhi_save();
        t->state = TS_READY;
	link(t);
	ready_count++;
	splx(s);
}


/*
 * create new thread
 */
struct tcb*
create_thread(struct process* proc, ulong start_addr)
{
        extern ulong sched_tick;
        struct tcb* t;
        spl_t s;

        t = (struct tcb*) alloc_pool(&thread_pool);
        if(!t) {
               printf("create_thread(): cannot alloc from pool\n");
               return (NULL);
        }

	/*
	 * initialize thread control block
	 * 'proc' might be null for kernel threads
	 */
        bzero(t, sizeof(struct tcb));        
        t->proc = proc;

	/*
	 * initial values for typical scheduler fields
	 */
        t->prio = DEFAULT_PRIO;
        t->cur_prio = DEFAULT_PRIO;
        t->cnt = 0;
        t->quantum = DEFAULT_QUANTA;
        t->stamp = sched_tick;
        t->state = TS_READY;
        t->ticks = 0;
        

	/*
	 * setup kernel stack
	 * every thread has a kernel stack regardless if it
	 * is a kernel thread or regular user mode thread
	 *
	 * for user mode threads, this stack is used when
	 * entering kernel mode (its user mode state is pushed on it)
	 * for kernel threads, this is actual working stack
	 * NOTE: kernel stack is only one page long, there is no
	 *	 mechanism to grow kernel stack at the moment
	 *	 therefore kernel threads mustn't be stack hungry 
	 *
	 */
        t->kstack = alloc_page();
        if( !t->kstack ) {
                printf("create_thread(): fatal, cannot allocate kstack.\n");
                return (NULL);
        }

        zero_page((void*) t->kstack);
	t->entry = start_addr;
	t->kstack = t->kstack + PAGESZ - sizeof(long);
        t->ctx->esp = t->kstack;

	if(proc) {
		t->vas = proc->vas;
		t->hat = t->vas->hat;

		/*
		 * this will add newly created thread
		 * to its process and allocate stack for it
		 */
		proc_attach_thread(t);		
		
		/*
		 * when this thread will be chosen by scheduler to run
		 * longjmp() will switch to this routine (defined in exec.c)
		 * which makes an intrasegment jump to user mode
		 */
		t->ctx->eip = (ulong) exec_user;
		
	} else {	
		/*
		 * this is a kernel thread, point it to kernel hat
		 * kernel threads have no vas
		 */
	        t->hat = kern_hat;
        	t->vas = (vas_desc_t*) 0;
        	
        	/*
        	 * thread will start immediately
        	 */
	        t->ctx->eip = t->entry;
        }

	/*
	 * add it to the ready queue
	 */
        s = splhi_save();
        enqueue_ready(t);
        n_threads++;
	ready_count++;
        splx(s);

        return (t);
}

/*
 * roll with this thread
 */
void
thread_execute(struct tcb* t)
{
        if(t == cur_thread) {
                printf("thread_execute(): trying to exec running thread.\n");
                return;
        }

        splhi();
        cur_thread = t;
        t->quantum = DEFAULT_QUANTA;
        longjmp(t->ctx, 0);
}

/*
 * terminate current thread
 * assumes splhi()
 */
void
thread_die()
{
	struct tcb* t;

	t = cur_thread;
	remove_ready(t);
	ready_count--;
	if(t->proc) {
		proc_detach_thread(t);
	}

	/*
	 * free_pool() actually thrashes tcb structure
	 * but it's ok, coz schedule() never references
	 * clobbred fields
	 */
	free_pool(t);
	schedule();
	
	/*
	 * should never happen...
	 */
	printf("thread_die(): dammit\n");
	for(;;);
}

/*
 * delete specified thread
 * if it's currently running one, calls thread_die() above
 */
void
delete_thread(struct tcb* t)
{
        spl_t s;

        printf("freeing: %08x\n", (ulong) t);
        s = splhi_save();
        if(t == cur_thread)
        	thread_die();
        	
        remove_ready(t);
	ready_count--;
        splx(s);

        free_pool(t);
}

void
thread_set_ready(struct tcb* t)
{
        spl_t s;

        s = splhi_save();
        enqueue_ready(t);
        splx(s);
}

void
thread_remove_ready(struct tcb* t)
{
        spl_t s;

        s = splhi_save();
        remove_ready(t);
        splx(s);
}

void
dump_free_threads()
{
}

void
dump_ready_threads()
{
        struct tcb* t;

        t = ready_threads;
        if(!t) {
                printf("no ready threads.\n");
                return;
        }

        printf("tcb      s ticks    cnt pri bpr preempt  sema\n");
        do {
                char state;

                switch(t->state) {
                        case TS_READY: state = 'R'; break;
                        case TS_WAITING: state = 'W'; break;
                        default: state = '?'; break;
                }
                printf("%08x %c %08x %03d %03d %03d %08x %08x %08x\n",
                        (ulong)t, state, t->ticks,
                        t->cnt, t->cur_prio, t->prio, t->preempt,
                        t->s_wait, t->stamp);
                t = t->next;
        }
        while(t != ready_threads);
#if 0
	printf("quantum: %d\n", DEFAULT_QUANTA - (ready_count >> 3));
#endif
}
