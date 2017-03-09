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
 * kernel console support
 *
 * $Id: cons.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <pc/cons.h>
#include <sys/types.h>
#include <string.h>
#include <i386/inlines.h>

static short* vidmem = (void*)0xb8000;
static uint curpos = 0;

#define DEF_COLS	(80)
#define DEF_LINES	(25)

static uint lines = DEF_LINES;
static uint screen_size = DEF_COLS * DEF_LINES;

void
cons_setlines(uint l)
{
	lines = l;
	screen_size = DEF_COLS * lines;
}

void 
cons_clrscr()
{
   int c;

   for(c = 0; c < screen_size; c++)
      vidmem[c] = 0x0700 | ' ';
}

void 
cons_setposxy(int x, int y)
{
   curpos = (y * DEF_COLS) + x;
}  

void
cons_setpos(int pos)
{
   curpos = pos;
}

int
cons_getpos()
{
   return (curpos);
}

void
update_curpos()
{
   outb(CRT_INDEX, CR_CURSOR_HI);
   outb(CRT_DATA, curpos>>8);
   outb(CRT_INDEX, CR_CURSOR_LO);
   outb(CRT_DATA, curpos);
}

static void
get_curpos()
{
   uchar lo = 0, hi = 0;

   outb(CRT_INDEX, CR_CURSOR_HI);
   hi = inb(CRT_DATA);
   outb(CRT_INDEX, CR_CURSOR_LO);
   lo = inb(CRT_DATA);

   curpos = (hi<<8) + lo;
}  

void 
cons_putc(unsigned char ch)
{
   int c;
   
   if(ch == '\r')
      return;

   if( ch == '\n' ) {
      curpos = ( curpos + DEF_COLS ) / DEF_COLS * DEF_COLS;
   } else {
      vidmem[curpos++] = DEF_MODE | ch;
   }

   if( curpos >= screen_size ) {
      bcopy(vidmem + DEF_COLS, vidmem, ((screen_size - DEF_COLS) * 2));
      for(c = screen_size - DEF_COLS; c <= screen_size; c++)
         vidmem[ c ] = DEF_MODE | ' ';
      curpos -= DEF_COLS;
   }

   update_curpos();
}

void
cons_ungetc()
{
   curpos--;
   vidmem[curpos] = DEF_MODE | ' ';

   update_curpos();
}

void 
cons_print(char* str)
{
	get_curpos();
   	while(*str)
      		cons_putc(*str++);
}

