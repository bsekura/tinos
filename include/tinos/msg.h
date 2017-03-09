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
 * $Id: msg.h,v 1.3 1998/03/01 21:05:46 bart Exp $
 *
 */

#ifndef __msg_h__
#define __msg_h__

#include <sys/types.h>
#include <sema.h>

/*
 * message flags
 */
#define	MF_INTERRUPT	(0x80)	/* a message carrying 
				   interrupt notification */
#define	MF_ONEWAY	(0x02)	/* we don't expect a reply */				  

#define MSGBUF_MAX	(32)

typedef struct message {
	ulong		flags;
	ulong		qref;	/* queue reference count */
	ulong		id;
	void*		data;
	ulong		len;
	byte		buf[MSGBUF_MAX];
	semaphore_t	lock;	/* message is still on the queue */	
	struct message* next;	/* message queue under the port */
	struct message* next_free;
} message_t;

typedef struct msg_desc {
	ulong	id;
	ulong	count;
	void*	data;
	ulong	len;
} msg_desc_t;

typedef struct port {
	ulong	      flags;
	
	message_t*    head;	/* message queue: head */
	message_t*    tail;	/* ... its tail */

	semaphore_t   lock;	/* to manipulate port safely */
	semaphore_t   recv;	/* serializing receive requests */
	
	message_t*    free_list; /* a list of free messages */
} port_t;

port_t* alloc_port();
message_t* alloc_msg();
void dump_port(port_t*);

int m_receive(port_t*, msg_desc_t*);
void do_send(port_t*, message_t*);
void m_send(port_t*, msg_desc_t*);

#endif /* __msg_h__ */
