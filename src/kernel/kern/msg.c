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
 * $Id: msg.c,v 1.4 1998/03/09 18:39:04 bart Exp $
 *
 */

/*
 * messaging primitives
 * general rule is simplicity and efficiency over extended functionality
 * these routines basically synchronize "talking" threads and
 * pass data around
 *
 * the design of this messaging module is heavily 
 * influenced by VSTa - an excellent microkernel by Andy Valencia
 * (written from scratch, however)
 *
 */

#include <stdio.h>
#include <thread.h>
#include <sched.h>
#include <i386/inlines.h>
#include <i386/spl.h>
#include <msg.h>
#include <pool.h>

static master_pool_t	port_pool;
static master_pool_t	msg_pool;

static void m_putfree(port_t*, message_t*);
static message_t* m_getfree(port_t*);

void
init_msg()
{
	init_pool(&port_pool, sizeof(port_t));
	init_pool(&msg_pool, sizeof(message_t));
}

message_t*
alloc_msg()
{
	message_t* m;

	m = alloc_pool(&msg_pool);
	if(!m) {
	       printf("alloc_msg(): cannot alloc from pool\n");
	       return (NULL);
	}
	bzero(m, sizeof(message_t));
	sema_init(&m->lock);
	sema_setval(&m->lock, 0);
	return (m);
}

port_t*
alloc_port()
{
	int c, free_list_size;
	port_t* port;

	port = (port_t*) alloc_pool(&port_pool);
	if(!port) {
		  printf("alloc_port(): cannot allocate\n");
		  return (NULL);
	}

	/* 
	 * init queue pointers 
	 */
	port->head = NULL;
	port->tail = NULL;

	/*
	 * port semaphore
	 */
	sema_init(&port->lock);

	/*
	 * 'receive' semaphore
	 * - it's initially probieren'ed
	 */
	sema_init(&port->recv);
	sema_setval(&port->recv, 0);
	
	/*
	 * initialize free list
	 */
	free_list_size = 8;
	port->free_list = 0;
	for(c = 0; c < free_list_size; c++) {
		message_t* m;
		
		if((m = alloc_msg()))
			m_putfree(port, m);
		else {
			printf("alloc_port(): warning, cannot alloc_msg()\n");
			break;
		}			
	}		

	return (port);
}

static void
m_putfree(port_t* port, message_t* m)
{
	spl_t s;

	s = splhi_save();
	m->next_free = port->free_list;
	port->free_list = m;
	splx(s);
}

static message_t*
m_getfree(port_t* port)
{
	message_t* m;
	spl_t s;

	s = splhi_save();
	m = port->free_list;
	if(m) {
		port->free_list = port->free_list->next_free;
		m->next_free = 0;
	}		

	splx(s);	
	return (m);
}

/*
 * enqueue message
 */
inline static void
m_enq(port_t* port, message_t* m)
{
	if(port->tail) {
		port->tail->next = m;
	}

	m->next = NULL;
	port->tail = m;
	if(!port->head) {
		port->head = m;
	}
}

/*
 * remove message
 */
inline static void
m_deq(port_t* port, message_t* m)
{
	if(port->head == m) {
		port->head = m->next;
	}
	else {
		message_t* qm;

		for(qm = port->head; qm; qm = qm->next) {
			if(qm->next == m) {
				qm->next = m->next;
				if(port->tail == m) {
					port->tail = qm;
				}
				break;
			}
		}
	}
}

void
dump_port(port_t* port)
{
	int c;
	message_t* m;

	printf("dump_port(): msgs = ");
	sema_p(&port->lock);
	c = 0;
	for(m = port->head; m; m = m->next) {
		printf("%d, ", m->id);
		if(++c > 10)
			break;
	}
	sema_v(&port->lock);
	printf("\n");
}

int
m_receive(port_t* port, msg_desc_t* desc)
{
	message_t* msg;
	spl_t s;

	/*
	 * check out 'receive' gate - is there something for us?
	 * will block if not
	 */
	sema_p(&port->recv);

	/*
	 * there's a message for us, access port...
	 */
	//sema_p(&port->lock);
	s = splhi_save();

	/*
	 * get a message
  	 */
	msg = port->head;
	if(!msg) {
		printf("m_receive(): message queue is empty...\n");
		//sema_v(&port->lock);
		splx(s);
		return (-1);
	}
	//printf("m_receive(): message = %d\n", msg->msg_id);
	port->head = msg->next;
	
	/*
	 * release port
	 */
	//sema_v(&port->lock);
	splx(s);
	
	desc->count = msg->qref;
	msg->qref = 0;
	desc->id = msg->id;	
	
	/*
	 * nothing else to do if it's an interrupt message
	 */
	if(msg->flags & MF_INTERRUPT)
		return 1;

	/*
	 * if there's data to copy - do it
	 * but check if the receiving side provided a buffer for it
	 */
	if(msg->data && desc->data) {
		bcopy(msg->data, desc->data,
		      msg->len > desc->len ? desc->len : msg->len);
	}
	
	/*
	 * release the guy that sent this message
	 */
	sema_v(&msg->lock);

	return 1;
}

void
do_send(port_t* port, message_t* msg)
{
	spl_t s;
	 
	/*
	 * check the queue ref count of the message
	 * if it's already hanging on the queue - get out
 	 */
 	msg->qref++;
 	if(msg->qref > 1) {
 		printf("#");
 		return;
 	}
 	
	/*
	 * specialized code path of message sending routine
	 * used when sending interrupt messages
	 *
	 * in our uniprocessor implementation, we can take
	 * advantage of not having to lock the port in order 
	 * to manipulate it (enqueue a message) when sending
	 * interrupts since we're already at splhi.
	 *
	 */
 	if(msg->flags & MF_INTERRUPT) {
 		m_enq(port, msg);
 		sema_v(&port->recv);
 		
 		return;
 	}

	/*
	 * usual message; lock the port before fiddling with it
	 */ 	
	//sema_p(&port->lock);
	s = splhi_save();
	m_enq(port, msg);
	//sema_v(&port->lock);
	splx(s);

	/*
	 * signal 'receive' semaphore - there's stuff to pick up
	 */
	sema_v(&port->recv);
	
	/*
	 * wait for message to be taken off the queue
	 * by the server thread
	 * 
	 * XXX how 'bout some timeout facility?   
	 */
	sema_p(&msg->lock);

	/*
	 * release the message structure
	 */
	//sema_p(&port->lock);
	m_putfree(port, msg);
	//sema_v(&port->lock);
}

/*
 * send a message
 * desc contains data passed by a client
 * we allocate our internal msg descriptor
 * and call do_send()
 */
void
m_send(port_t* port, msg_desc_t* desc)
{
	message_t* msg;

	msg = m_getfree(port);
	if(!msg) {
		/*
		 * XXX temp
		 * we should sleep here waiting for free
		 * message slots
		 */
		 printf("m_send(): no free messages\n");
		 return;
	}
	
	msg->id = desc->id;
	if(desc->data) {
		msg->len = desc->len > MSGBUF_MAX ? MSGBUF_MAX : desc->len;
		bcopy(desc->data, msg->buf, msg->len);
		msg->data = msg->buf;
	} else
		msg->data = (void*) 0;
		
	do_send(port, msg);
}
