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
 * $Id: video.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef _VIDEO_H_
#define _VIDEO_H_

#define BLINK		0x8000

#define	BLACK		0x0000
#define BLUE		0x0100
#define GREEN		0x0200
#define CYAN		0x0300
#define RED		0x0400
#define MAGENTA		0x0500
#define BROWN		0x0600
#define WHITE		0x0700
#define	GRAY		0x0800
#define LTBLUE		0x0900
#define LTGREEN		0x0A00
#define LTCYAN		0x0B00
#define LTRED		0x0C00
#define LTMAGENTA	0x0D00
#define YELLOW		0x0E00
#define BWHITE		0x0F00

#define	BG_BLACK	0x0000
#define BG_BLUE		0x1000
#define BG_GREEN	0x2000
#define BG_CYAN		0x3000
#define BG_RED		0x4000
#define BG_MAGENTA	0x5000
#define BG_BROWN	0x6000
#define BG_WHITE	0x7000

#endif /* _VIDEO_H_ */
