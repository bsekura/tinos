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
 * $Id: font.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

/*
 *  This is adopted from Linux
 *
 *  linux/drivers/char/vga.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *			1995  Jay Estabrook
 */
 
#include <i386/inlines.h>
#include <i386/spl.h>
#include <sys/types.h>
#include <string.h>

#define FONT_SIZE (8192)

int set_font(char*);
int cons_adjust_height(ulong);

void
load_font(char* buf, ulong unit)
{
	static char font[FONT_SIZE];
	uint c;
	char* p;
	
	memset(font, 0, FONT_SIZE);
	p = buf;
	for(c = 0; c < 256; c++) {
		bcopy(p, font+(32*c), unit);
	 	p += unit;
	}

	set_font(font);		
	cons_setlines(cons_adjust_height(unit));
}


#define colourmap ((char *)0xa0000)
/* Pauline Middelink <middelin@polyware.iaf.nl> reports that we
   should use 0xA0000 for the bwmap as well.. */
#define blackwmap ((char *)0xa0000)
#define cmapsz 8192
#define seq_port_reg (0x3c4)
#define seq_port_val (0x3c5)
#define gr_port_reg (0x3ce)
#define gr_port_val (0x3cf)

int 
set_font(char * arg)
{
	int i;
	char *charmap;
	int beg;
	spl_t s;

	charmap = colourmap;
	beg = 0x0e;

	s = splhi_save();
	outb( seq_port_reg, 0x00 );   /* First, the sequencer */
	outb( seq_port_val, 0x01 );   /* Synchronous reset */
	outb( seq_port_reg, 0x02 );
	outb( seq_port_val, 0x04 );   /* CPU writes only to map 2 */
	outb( seq_port_reg, 0x04 );
	outb( seq_port_val, 0x07 );   /* Sequential addressing */
	outb( seq_port_reg, 0x00 );
	outb( seq_port_val, 0x03 );   /* Clear synchronous reset */

	outb( gr_port_reg, 0x04 );    /* Now, the graphics controller */
	outb( gr_port_val, 0x02 );    /* select map 2 */
	outb( gr_port_reg, 0x05 );
	outb( gr_port_val, 0x00 );    /* disable odd-even addressing */
	outb( gr_port_reg, 0x06 );
	outb( gr_port_val, 0x00 );    /* map start at A000:0000 */

	for (i=0; i<cmapsz ; i++)
		*(charmap+i) = *(arg+i);

	outb( seq_port_reg, 0x00 );   /* First, the sequencer */
	outb( seq_port_val, 0x01 );   /* Synchronous reset */
	outb( seq_port_reg, 0x02 );
	outb( seq_port_val, 0x03 );   /* CPU writes to maps 0 and 1 */
	outb( seq_port_reg, 0x04 );
	outb( seq_port_val, 0x03 );   /* odd-even addressing */
	outb( seq_port_reg, 0x00 );
	outb( seq_port_val, 0x03 );   /* clear synchronous reset */

	outb( gr_port_reg, 0x04 );    /* Now, the graphics controller */
	outb( gr_port_val, 0x00 );    /* select map 0 for CPU */
	outb( gr_port_reg, 0x05 );
	outb( gr_port_val, 0x10 );    /* enable even-odd addressing */
	outb( gr_port_reg, 0x06 );
	outb( gr_port_val, beg );     /* map starts at b800:0 or b000:0 */
	splx(s);

	return 0;
}

static ulong video_scan_lines = 25*16;
static ulong video_font_height;

static ushort video_port_reg = 0x3d4;
static ushort video_port_val = 0x3d5;

/*
 * Adjust the screen to fit a font of a certain height
 *
 * Returns < 0 for error, 0 if nothing changed, and the number
 * of lines on the adjusted console if changed.
 */
int
cons_adjust_height(unsigned long fontheight)
{
	int rows, maxscan;
	unsigned char ovr, vde, fsr, curs, cure;
	spl_t s;

	video_font_height = fontheight;

	rows = video_scan_lines/fontheight; 
	maxscan = rows*fontheight - 1;		
	/* Scan lines to actually display-1 */

	/* Reprogram the CRTC for the new font size
	   Note: the attempt to read the overflow register will fail
	   on an EGA, but using 0xff for the previous value appears to
	   be OK for EGA text modes in the range 257-512 scan lines, so I
	   guess we don't need to worry about it.

	   The same applies for the spill bits in the font size and cursor
	   registers; they are write-only on EGA, but it appears that they
	   are all don't care bits on EGA, so I guess it doesn't matter. */

	s = splhi_save();
	outb( video_port_reg, 0x07 );		/* CRTC overflow register */
	ovr = inb(video_port_val);
	outb( video_port_reg, 0x09 );		/* Font size register */
	fsr = inb(video_port_val);
	outb( video_port_reg, 0x0a );		/* Cursor start */
	curs = inb(video_port_val);
	outb( video_port_reg, 0x0b );		/* Cursor end */
	cure = inb(video_port_val);

	vde = maxscan & 0xff;			/* Vertical display end reg */
	ovr = (ovr & 0xbd) +			/* Overflow register */
	      ((maxscan & 0x100) >> 7) +
	      ((maxscan & 0x200) >> 3);
	fsr = (fsr & 0xe0) + (fontheight-1);    /*  Font size register */
	curs = (curs & 0xc0) + fontheight - (fontheight < 10 ? 2 : 3);
	cure = (cure & 0xe0) + fontheight - (fontheight < 10 ? 1 : 2);

	outb( video_port_reg, 0x07 );		/* CRTC overflow register */
	outb( video_port_val, ovr );
	outb( video_port_reg, 0x09 );		/* Font size */
	outb( video_port_val, fsr );
	outb( video_port_reg, 0x0a );		/* Cursor start */
	outb( video_port_val, curs );
	outb( video_port_reg, 0x0b );		/* Cursor end */
	outb( video_port_val, cure );
	outb( video_port_reg, 0x12 );		/* Vertical display limit */
	outb( video_port_val, vde );
	splx(s);

	return rows;
}

