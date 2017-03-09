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
 * $Id: frame.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __stack_frame_h__
#define __stack_frame_h__

/*
 * stack frame structures
 */

#ifndef __types_h__
#include <sys/types.h>
#endif

/*
 * standard stack frame - this is how stack looks upon
 * entry to kernel from user mode
 */
struct frame {
	ulong esds;	/* these we put lastly */

  	ulong edi;	/* these are our pushl's */
   	ulong esi; 
   	ulong ebp; 
  	ulong ebx;
   	ulong edx; 
   	ulong ecx; 
   	ulong eax;  

	ulong err;	/* this is either zero or some error code */

   	ulong eip;	/* this is extra bonus from Intel */
   	ulong ecs;
   	ulong eflags;
   	ulong esp;
  	ulong ess;
};

#endif /* __stack_frame_h__ */
