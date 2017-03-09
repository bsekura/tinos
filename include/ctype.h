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
 * $Id: ctype.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

/*
 * I believe this comes (or is derived) from DJ Delorie's code
 * Copyright (c) 1994 DJ Delorie
 */

#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <sys/types.h>

#define _C_ALN 		(0x0001)
#define _C_ALP		(0x0002)
#define _C_CTL		(0x0004)
#define _C_DIG		(0x0008)
#define _C_GRA		(0x0010)
#define _C_LOW		(0x0020)
#define _C_PRN		(0x0040)
#define _C_PUN		(0x0080)
#define _C_SPC		(0x0100)
#define _C_UPE		(0x0200)
#define _C_HEX		(0x0400)

extern ushort __ctab[];
extern uchar __toupper_tab[];
extern uchar __tolower_tab[];

#define isalnum(c) (__ctab[((c)&0xff)+1] & _C_ALN)
#define isalpha(c) (__ctab[((c)&0xff)+1] & _C_ALP)
#define iscntrl(c) (__ctab[((c)&0xff)+1] & _C_CTL)
#define isdigit(c) (__ctab[((c)&0xff)+1] & _C_DIG)
#define isgraph(c) (__ctab[((c)&0xff)+1] & _C_GRA)
#define islower(c) (__ctab[((c)&0xff)+1] & _C_LOW)
#define isprint(c) (__ctab[((c)&0xff)+1] & _C_PRN)
#define ispunct(c) (__ctab[((c)&0xff)+1] & _C_PUN)
#define isspace(c) (__ctab[((c)&0xff)+1] & _C_SPC)
#define isupper(c) (__ctab[((c)&0xff)+1] & _C_UPE)
#define isxdigit(c) (__ctab[((c)&0xff)+1] & _C_HEX)

#define isascii(c) ((uchar)(c) <= 0x7F)

#define tolower(c) (__tolower_tab[((c)&0xff)+1])
#define toupper(c) (__toupper_tab[((c)&0xff)+1])

#endif /* _CTYPE_H_ */
