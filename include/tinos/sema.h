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
 * $Id: sema.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __sema_h__
#define __sema_h__

struct tcb;

struct semaphore {
	int		count;
	struct tcb* 	wait_q;
};

typedef struct semaphore semaphore_t;

void sema_init(semaphore_t* s);
void sema_setval(semaphore_t* s, int val);
void sema_p(semaphore_t*);
void sema_v(semaphore_t*);

#endif /* __sema_h__ */
