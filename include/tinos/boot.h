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
 * $Id: boot.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __boot_info_h__
#define __boot_info_h__

#include "../../src/boot/dos/boot.h"

#include <sys/types.h>

/*
 * boot task info
 */
struct boot_task_s {
	ulong  start;	/* start of image */
	ulong  size;	/* size of image */
	ulong  flags;
	ulong  cmdline;
};

#define BOOT_INFO_MAGIC 	(0xBAABCEED)
struct boot_info_s {
       ulong	   magic;
       ulong	   mem_size;	/* physical memory size */
       ulong	   kern_start;	/* kernel area start */
       ulong	   kern_end;	/* ...end */				
       ulong	   boot_start;	/* loaded boot tasks area start */
       ulong	   boot_end;	/* ...end */
       ulong	   boot_count;	/* how many boot tasks were loaded? */
       struct boot_task_s* boot_task; /* one per every boot task */
};

/*
 * generic executable header
 */
struct exec_hdr_s {
	ulong	text_vaddr;
	ulong	text_ptr;
	ulong	text_size;
	
	ulong	data_vaddr;
	ulong	data_ptr;
	ulong	data_size;
	
	ulong	entry;
	ulong	bss_end;
};

#endif /* __boot_info_h__ */
