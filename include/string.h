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
 * $Id: string.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>
#include <bcopy.h>

#ifdef __cplusplus
extern "C" {
#endif

char*    strcpy(char*, const char*);
char*    strncpy(char*, const char*, size_t);
char*    strchr(const char*, int);
char*    strrchr(const char*, int);
int      strlen(const char*);
int      strcmp(const char*, const char*);
int      strncmp(const char*, const char*, size_t);
ulong    strtoul(const char*, char**, int);
long     strtol(const char*, char**, int);
char*    strlwr(char*);
char*    strupr(char*);
size_t   strcspn(const char*, const char*);
int      strcoll(const char*, const char*);
char*    strpbrk(const char*, const char*);
size_t   strspn(const char*, const char*);
char*    strstr(const char*, const char*);
char*    strtok(char*, const char*);
size_t   strxfrm(char*, const char*, size_t);

void*    memchr(const void* s, int c, size_t n);

#ifndef	__GNUC__ /* gcc got them built in */

int      memcmp(const void* s1, const void* s2, size_t n);
void*    memset(void* dest, int c, size_t count);
void*    memcpy(void* dest, const void* src, size_t count);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H_ */
