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
 * $Id: hat.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __hat_h__
#define __hat_h__

#include <sys/types.h>
#include <i386/inlines.h>

typedef struct {
        ulong pa_pd;
        ulong va_pd;
        ulong page_count;
} hat_desc_t;

/*
 * switch address space (if needed)
 */
inline static void
hat_switch(hat_desc_t* hat)
{
	if(get_cr3() != hat->pa_pd)
		set_cr3(hat->pa_pd);
}

void hat_enter(hat_desc_t* hat);

#endif /* __hat_h__ */
