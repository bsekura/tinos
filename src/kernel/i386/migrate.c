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
 * $Id: migrate.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

/*
 * migrating threads test
 * this stuff should go to i386 
 *
 */ 

#include <i386/inlines.h>

struct t_frame {
	ulong	eip, ecs, efl, esp, ess;
};

static ulong hello_stack[100];
static struct t_frame* saved_frame;

static void
hello()
{
	printf("hello\n");
	asm volatile 
	(
		"cli\n\t"
		"movl saved_frame, %%esp\n\t"
		"iret"
		:
		:
	);
}

#if 0
void
migrate()
{
	printf("ok, migrating...\n");
	cli();
	stack_migrate((ulong) &hello_stack[90], (ulong) &hello);
	printf("oops\n");
	while(1);
}
#endif


void
sys_trap(ulong stack)
{
	struct t_frame* f = (struct t_frame*) &stack;
	printf("syscall trap\n");
	printf("ess = %08x, ecs = %08x, efl = %08x, esp = %08x, eip = %08x\n",
		f->ess, f->ecs, f->efl, f->esp, f->eip);

	cli();
	saved_frame = f;
	stack_migrate((ulong) &hello_stack[90], (ulong) &hello);
}

void
trap()
{
	printf("calling trap...\n");
	call_trap();
	printf("ok, back from trap\n");
}

