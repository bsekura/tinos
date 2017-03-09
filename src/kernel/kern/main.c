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
 * kernel main entry
 *
 * $Id: main.c,v 1.2 1998/03/01 21:06:49 bart Exp $
 *
 */

#include <sys/types.h>
#include <i386/page.h>
#include <i386/spl.h>
#include <i386/multiboot.h>
#include <thread.h>
#include <irq.h>

/*
 * kernel idle thread
 * currently doesn't do much :-)
 */
void
idle_thread()
{
	extern void kb_isr();
	ushort* p = (ushort*) 0xb8000;

	printf("idle thread here...\n");	
	
	enable_irq(0);
	interrupt_register(1, kb_isr);
	create_bootproc();

	for(;;) {	
		p[79]++;
	}
}

/*
 * main entry point
 */

int
main(ulong info_start, ulong magic)
{
	int c, base, ext;
	ulong total;
	struct tcb* t;
	extern uchar _start[], edata[], end[];
	extern void syscon();

	printf("TINOS kernel: %s %s [%x:%x:%x]\n",
		__DATE__, __TIME__, (ulong)_start, (ulong)edata, (ulong)end);
		
	if(magic == MULTIBOOT_VALID) {
		printf("we're loaded by a multiboot compliant loader! (is it you, grub?)\n");
		//multiboot_dump(info_start);
		multiboot_init((struct multiboot_info*) info_start);
		//bootinfo_dump();		
		//while(1);
	} else {
		printf("info start: %08x\n", info_start);
		dosboot_init(info_start);
	}

	init_gdt();
	init_idt();
	init_kernel_tss();
	
	init_pci();

	init_pages();

	init_vas();
	init_pit();
	init_pic();
	init_msg();
	init_thread();
	init_proc();
	init_timeout();
	
	kb_init();

	/*
	 * create two initial kernel threads:
	 * idle and system console
	 */
	t = create_thread(0, (ulong) &idle_thread);
	create_thread(0, (ulong) &syscon);

	/*	
	 * enable interrupts and roll the dice ...
	 */
	spl0();
	thread_execute(t);
	
	/*
	 * this point is unreachable
	 * if we somehow got here, we're in trouble
	 */
	printf("oops!\n");
	c = 1000000000;
	while(c--);
	reboot_machine();
	
	return 0;
}
