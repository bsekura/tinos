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
 * $Id: syscall.c,v 1.2 1998/03/01 21:06:49 bart Exp $
 *
 */
 
#include <msg.h>

void
sample_syscall(int x, char* s)
{
	printf("sample_syscall(): x = %d, s = %s\n", x, s);
}

int
kbisr_syscall()
{
	extern port_t* kb_port;
	//extern message_t* kb_isr_msg;
	msg_desc_t desc;
	
	m_receive(kb_port, &desc);
	return desc.count;
}
