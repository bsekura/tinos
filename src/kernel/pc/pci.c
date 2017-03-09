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
 * $Id: pci.c,v 1.2 1998/03/01 21:07:01 bart Exp $
 *
 */
 
#include <sys/types.h>

/*
 * bio32 signature: "_32_"
 */
#define BIOS32_SIGNATURE	('_'+('3'<<8)+('2'<<16)+('_'<<24))
 
union bios32 {
	struct {
		ulong	signature;	/* */
		ulong	entry;		/* physical address */
		uchar	revision;	/* revision level */
		uchar	length;		/* length in paragraphs */
		uchar	checksum;	/* all bytes must add up to zero */
		uchar	reserved[5];
	} fields;
	char chars[16];
};

void
init_pci()
{
	union bios32* p;
	
	for(p = (union bios32*) 0xe0000; p < (union bios32*) 0x100000; p++) {
		if(p->fields.signature == BIOS32_SIGNATURE) {
			printf("pci_init(): at %08lx\n", (ulong) p);
			break;
		}
	}
}
