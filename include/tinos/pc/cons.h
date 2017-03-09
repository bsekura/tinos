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
 * $Id: cons.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
#ifndef _CONS_H_
#define _CONS_H_

#include <pc/video.h>

#define     VID_COLOR_BASE 0xb8000

/*
 * CRT controller ports (color)
 */
#define     CRT_INDEX   (0x03D4)
#define     CRT_DATA 	(0x03D5)

/*
 * CRT controller registers
 */
#define CR_CURSLINE1   (0x0A)
#define CR_CURSLINE2   (0x0B)
#define CR_CURSOR_HI   (0x0E)
#define CR_CURSOR_LO   (0x0F)

/*
 * display modes
 */
#define     DEF_MODE (LTGREEN|BG_BLACK)

void clrscr();
void putc(unsigned char);
void ungetc();
void print(char*);
void printf(const char*,...);

#endif /* _CONS_H_ */
