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
 * $Id: types.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
/*
 * useful types
 * 
 */
#ifndef __types_h__
#define __types_h__

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;

typedef ulong size_t;

typedef void (*voidfun)();
typedef int (*intfun)();

typedef uchar  byte;
typedef ushort word;
typedef ulong  dword;

#ifndef NULL
#define NULL (0)
#endif

#endif /* __types_h__ */

