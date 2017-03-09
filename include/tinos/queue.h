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
 * $Id: queue.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

/*
 * macros for manipulating queues
 *
 */

#define enqueue(head, next, prev, e) 					\
{ 									\
	if(head == 0) { 						\
		head = e; 						\
		e->next = e->prev = head; 				\
	} else { 							\
		e->next = head; 					\
		e->prev = head->prev; 					\
		e->prev->next = e; 					\
		head->prev = e; 					\
	}								\
}

#define enq_head(head, next, prev, e) 					\
{ 									\
	if(head == 0) { 						\
		head = e; 						\
		e->next = e->prev = head; 				\
		return;	 						\
	} else {							\
		e->next = head; 					\
		e->prev = head->prev; 					\
		e->prev->next = e; 					\
		head = e; 						\
	}								\
}

#define enqueue_before(head, next, prev, before, e) 			\
{ 									\
	e->next = before; 						\
	e->prev = before->prev;						\
	before->prev->next = e; 					\
	before->prev = e; 						\
	if(before == head)						\
		head = e;						\
}

#define dequeue(head, next, prev, e, e_type_ptr) 			\
{ 									\
	if(e->next == e) { 						\
		head = 0; 						\
	} else { 							\
		register e_type_ptr __next = e->next; 			\
		register e_type_ptr __prev = e->prev; 			\
		__next->prev = __prev; 					\
		__prev->next = __next; 					\
		if(head == e) { 					\
			head = __next; 					\
		} 							\
	} 								\
}
	
