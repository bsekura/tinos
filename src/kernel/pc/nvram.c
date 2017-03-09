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
 * $Id: nvram.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <pc/nvram.h>  

void
show_time()
{
	int h = nvread(4);
	int m = nvread(2);
	int s = nvread(0);

	h = (((h >> 4) & 0x0F) * 10) + (h & 0x0F);
	m = (((m >> 4) & 0x0F) * 10) + (m & 0x0F);
	s = (((s >> 4) & 0x0F) * 10) + (s & 0x0F);

	printf("TIME: %02d:%02d:%02d\n", h, m, s);
}

void
show_date()
{
	int y = nvread(9);
	int m = nvread(8);
	int d = nvread(7);
	int dw = nvread(6);

	y = (((y >> 4) & 0x0F) * 10) + (y & 0x0F);
	m = (((m >> 4) & 0x0F) * 10) + (m & 0x0F);
	d = (((d >> 4) & 0x0F) * 10) + (d & 0x0F);
	dw = (((dw >> 4) & 0x0F) * 10) + (dw & 0x0F);

	printf("DATE: %02d:%02d:%02d   weekday: %d\n", y, m, d, dw);
}

#if 0

void
get_time(struct __time* t)
{
   int hr = nvread(4);
   int min = nvread(2);
   int sec = nvread(0);
   int yr = nvread(9);
   int mon = nvread(8);
   int day = nvread(7);

   t->hr = (((hr >> 4) & 0x0F) * 10) + (hr & 0x0F);
   t->min = (((min >> 4) & 0x0F) * 10) + (min & 0x0F);
   t->sec = (((sec >> 4) & 0x0F) * 10) + (sec & 0x0F);
   t->yr = (((yr >> 4) & 0x0F) * 10) + (yr & 0x0F);
   t->mon = (((mon >> 4) & 0x0F) * 10) + (mon & 0x0F);
   t->day = (((day >> 4) & 0x0F) * 10) + (day & 0x0F);
}

#endif
