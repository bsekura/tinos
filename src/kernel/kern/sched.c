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
 * $Id: sched.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <i386/tss.h>
#include <i386/frame.h>
#include <i386/inlines.h>
#include <i386/spl.h>
#include <sema.h>
#include <thread.h>
#include <queue.h>

/*
 * XXX temp debug
 * we use it to blink on the screen during certain activities
 * (now that's a professional debugging)
 */
static ushort* screen = (ushort*) 0xb8000;

/*
 * need it to set the next kernel stack (tss.esp0)
 * (ring 0) during context switch
 */
extern struct tss kernel_tss;

/* 
 * thread queue and ready threads counter
 * (defined in thread.c)
 */
extern struct tcb* ready_threads;
extern ulong ready_count;

/* 
 * current running thread 
 */
struct tcb* cur_thread;

/* 
 * statistics 
 */
static uint user_ticks;
static uint kern_ticks;
static uint nested_clocks;
static uint cur_returns;
static uint sched_adjusts;
static uint sched_recalcs;

/*
 * are we nesting clock interrupts?
 * this one effectively solves that
 */
static int in_clock = 0;

static ulong ticks_counter;

/*
 * cpu usage aging factor
 */
#define CNT_SHIFT	(1)

/*
 * used to determine if a second passed 
 * since we last ran cpu aging routine
 */
static ulong sched_timer;

/*
 * how many times we ran the abovementioned routine
 * (also used to stamp threads when they're awarded a timeslice
 *  from scheduler to determine possible starvation)
 */
ulong sched_tick;

/*
 * time recording (microseconds, seconds)
 */
static ulong us, sec;

/*
 * this may be set outside scheduler to indicate
 * that the current thread should be preempted
 * even though its timeslice hasn't expired
 * coz there are potentially more important threads
 * hanging around waiting to be activated
 */
ulong sched_preempt;

/*
 * switch context from current thread to the one specified
 * also switch address space if needed
 */
inline static void
context_switch(struct tcb* t)
{
	if(cur_thread == t) {
		printf("context_switch(): switching to current...\n");
		return;
	}

	cur_thread = t;
	kernel_tss.esp0 = t->kstack;

	hat_switch(t->hat);
	t->preempt++;
	longjmp(t->ctx, 1);
}

/*
 * age cpu usage periodically
 * currently called once per second from clock interrupt
 *
 */
static void
sched_aging()
{
	struct tcb* t;
	spl_t s;

	/*
	 * some simple statistics
	 */
	sched_tick++;
	sched_recalcs++;

	/*
	 * go through the list of ready threads
	 * age current cpu usage and recalculate new priority
	 */
	t = ready_threads;
	s = splhi_save();
	while((t = t->next) != ready_threads) {
		 t->cnt >>= CNT_SHIFT;

		 /* 
		  * calc new priority 
		  */
		 t->cur_prio = t->prio - (t->cnt >> 1);
		 if(t->cur_prio < 0) {
				t->cur_prio = 0;
		 }
	}
	splx(s);

	/*
	 * XXX
	 * periodically, we should check for starving threads
	 * tcb->stamp gets updated with current sched_tick
	 * every time scheduler choses this tcb to run
	 * it's easy to check then whether the thread is almost dead
	 * - its stamp differs substantially from current sched_tick
	 */
}

/*
 * this function is a main scheduler activity
 */
void
schedule()
{
	struct tcb* t, *next;
	int c;

	splhi();
	t = cur_thread;
	t->cur_prio = t->prio - (t->cnt >> 1);
	if(t->cur_prio < 0) {
		t->cur_prio = 0;
	}

	c = -1;
	next = t = ready_threads;
	while((t = t->next_ready) != ready_threads) {
		if(t->cur_prio > c) {
			c = t->cur_prio;
			next = t;
		}
	}

	next->stamp = sched_tick;

	if(next->quantum <= 0) {
		/*
		 * quantum decreases when the number of 
		 * running threads increases
		 * treshold is 8 (2^3)
		 */
		next->quantum += (DEFAULT_QUANTA - (ready_count >> 3));
		if(next->quantum < MIN_QUANTA) {
			next->quantum = MIN_QUANTA;
		}
	}

	if(next != cur_thread) {
		in_clock = 0;
		/* save context of current thread */
		if(setjmp(cur_thread->ctx)) {
		       return;
		}

		/* switch to next */
		context_switch(next);
	}
	cur_returns++;
	spl0();
}


/*
 * record some time related statistics
 */	
static void
bump_time(int user)
{
	sched_timer++;
	us += 10000;
	if(us >= 1000000) {
		us -= 1000000;
		sec++;
	}

	if(user) {
		user_ticks++;
	} else {
		kern_ticks++;
	}
	cur_thread->ticks++;
}

void check_timeout();

/*
 * clock interrupt
 * interrupts are disabled until we enable them explicitly
 */
void
clock(ulong stack)
{
	struct tcb* t;
	struct frame* f = (struct frame*) &stack;
	
	//screen[0]++;
	/* bail out if reentered */
	if(in_clock) {
		//screen[10]++;
		nested_clocks++;
		return;
	}

	/* count the tick ... */
	ticks_counter++;
	in_clock = 1;
	spl0();

	bump_time(USERMODE(f));

	/* increase cpu usage and decrease quantum */
	t = cur_thread;
	if(t != ready_threads) {
		t->cnt++;
	}
	t->quantum--;

	/* need to age cpu usage ? */
	if(sched_timer >= 100) {
		sched_aging();
		sched_timer = 0;
	}

	check_timeout();

	if(t->quantum <= 0 || sched_preempt--) {
		//screen[1]++;
		schedule();
	}

	splhi();
	in_clock = 0;
}

void
sched_stat()
{
	printf("nested_clocks: %d\n", nested_clocks);
	printf("sched_adjusts: %d\n", sched_adjusts);
	printf("sched_recalcs: %d\n", sched_recalcs);
	printf("cur_returns  : %d\n", cur_returns);
	printf("kernel ticks : %d\n", kern_ticks);
	printf("ticks counter : %d\n", ticks_counter);
}

void
sched_uptime()
{
	printf("ticks counter : %d\n", ticks_counter);
	printf("us: %d, sec: %d\n", us, sec);
	printf("time: %02d:%02d:%02d:%02d\n",
		sec/3600, sec/60, sec%60, us/1000);
}

static ulong t1, t2;

void
timer_start()
{
	splhi();
	t1 = ticks_counter;
	//printf("timer_start(): t1 = %d\n", t1);
	spl0();
}

void
timer_stop()
{
	ulong usec;

	splhi();
	t2 = ticks_counter;
	usec = (t2 - t1) * 10000;
	printf("Timer: %d useconds elapsed (%d usec [10000])\n",
		usec, usec/10000);
	spl0();
}

typedef struct timeout {
	int state;
	ulong ticks;
	struct semaphore* s_tmout;
	struct timeout* next;
	struct timeout* prev;
} timeout_t;

static timeout_t* tmout_q;
static timeout_t trailer;
#define MAX_TIMEOUTS	(10)
static timeout_t tm_pool[MAX_TIMEOUTS];

#define TM_FREE 	(0)
#define TM_ALLOC	(1)

void
init_timeout()
{
	int c;
	timeout_t* t;

	for(c = 0, t = &tm_pool[0]; c < MAX_TIMEOUTS; c++, t++) {
		t->state = TM_FREE;
	}

	t = &trailer;
	t->ticks = 0xFFFFFFFF;
	t->state = TM_ALLOC;

	/* enqueue trailer */
	enqueue(tmout_q, next, prev, t);
}

void
set_timeout(ulong ticks)
{
	int c;
	timeout_t* t, *next;
	spl_t s;

	/*
	 * simple hunt for free timeout slot
	 * gotta do it at splhi
	 */
	s = splhi_save();
	for(c = 0, t = &tm_pool[0]; c < MAX_TIMEOUTS; c++, t++) {
		if(t->state == TM_FREE)
			break;
	}

	if(c == MAX_TIMEOUTS - 1) {
		printf("set_timeout(): no more space\n");
		splx(s);
		return;
	}

	/*
	 * initialize this timeout
	 * set its ticks value to current system ticks plus
	 * the amount of ticks it wants to wait; makes searching
	 * the timeout queue easier
	 */
	t->state = TM_ALLOC;
	t->ticks = ticks_counter + ticks;
	sema_init(t->s_tmout);
	sema_setval(t->s_tmout, 0);

	/*
	 * go through the timeout queue until we reach
	 * the one with higher timeout ticks than this one.
	 * this results in having timeout queue sorted
	 * by timeout ticks in ascending order
	 */
	next = tmout_q;
	while(1) {
		if(next->ticks > t->ticks)
			break;
		next = next->next;
	}

	/*
	 * insert this one before the one found above
	 */
	t->next = next;
	t->prev = next->prev;
	next->prev->next = t;
	next->prev = t;
	if(tmout_q == next) {
		tmout_q = t;
	}

	splx(s);

	/*
	 * now actually wait for the timeout to elapse
	 * we have previously initialized the semaphore
	 * to non-singnaled state, so we block here
	 */
	sema_p(t->s_tmout);
}

void
check_timeout()
{
	timeout_t* t;
	spl_t s;

	/*
	 * go through timeouts queue and release those that expired
	 */
	s = splhi_save();
	for(t = tmout_q; ticks_counter >= t->ticks; t = t->next) {
		sema_v(t->s_tmout);
		dequeue(tmout_q, next, prev, t, timeout_t*);
	}
	splx(s);
}

void
dump_timeout()
{
	timeout_t* t;

	t = tmout_q;
	while(1) {
		printf("t->ticks = %d\n", t->ticks);
		if((t = t->next) == tmout_q)
			break;
	}
}
