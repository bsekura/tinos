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
 * $Id: reboot.c,v 1.2 1998/02/26 19:48:34 bart Exp $
 *
 */

#include <i386/inlines.h>
#include <pc/kb.h>

void
reboot_machine()
{
	int c;

	cli();
	printf("... rebooting ...\n");
	for(c = 0; c < 100000; c++) {
		if(inb(KB_STATUS_REG) == KBS_IN_DATA)
			break;
	}
	
	c = 1000000;
	while(c--);
	
	outb(KB_CMD_REG, KB_RESEND);
	
	for(;;);
}
