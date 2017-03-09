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
 * $Id: bcopy.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
 
#ifndef __bcopy_h__
#define __bcopy_h__

/*
 * bzero(), bcopy() - very useful ones
 */

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void bcopy(const void *src, void *dest, size_t n);
void bzero(void *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* __bcopy_h__ */
