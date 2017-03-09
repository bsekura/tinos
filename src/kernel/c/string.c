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
 * from VSTa code
 * Copyright (c) 1996 Andy Valencia
 *
 * $Id: string.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <limits.h>
#include <ctype.h>
#include <string.h>

#ifdef Isspace
#undef Isspace
#endif /* Isspace */
#define Isspace(c) ((c == ' ') || (c == '\t') || (c=='\n') || (c=='\v') || \
			(c == '\r') || (c == '\f'))

char*
strcpy(char* dest, const char* src)
{
	char* p = dest;

	while (*p++ = *src++);

	return(dest);
}

char*
strncpy(char* dest, const char* src, size_t len)
{
	char* p = dest, *lim;

	lim = p+len;
	while (p < lim) {
		if ((*p++ = *src++) == '\0') {
			break;
		}
	}
	return(dest);
}

int
strlen(const char *p)
{  
   	int x = 0;
                     
	if (p == 0) {
		return(0);
	}            
                     
	while (*p++) 
		++x; 
	return(x);   
}                    
                     

int 
strcmp(const char* s1, const char* s2)
{
	while(*s1 == *s2) {
		if(!*s1++)
			return 0;
		s2++;
	}
	return 1;
}

int 
strncmp(const char* s1, const char* s2, size_t nbytes)
{
	while(nbytes && (*s1++ == *s2)) {
		if(*s2++ == '\0') {
			return(0);
		}
		nbytes -= 1;
	}
	if(nbytes == 0 ) {
		return(0);
	}
	return(s1[-1] - s2[0]);
}

char *
strcat(char *dest, const char *src)
{
	char *p;

	for (p = dest; *p; ++p)
		;
	while (*p++ = *src++)
		;
	return(dest);
}

char *
strncat(char *dest, const char *src, int len)
{
	char *p;
	const char *lim;

	lim = src+len;
	for (p = dest; *p; ++p)
		;
	while (src < lim) {
		if ((*p++ = *src++) == '\0') {
			return(dest);
		}
	}
	*p++ = '\0';
	return(dest);
}

/*
 * strchr()
 *	Return pointer to first occurence, or 0
 */
char *
strchr(const char *p, int c)
{
	int c2;

	do {
		c2 = *p++;
		if (c2 == c) {
			return((char *)(p-1));
		}
	} while (c2);
	return(0);
}

/*
 * strrchr()
 *	Like strchr(), but search backwards
 */
char *
strrchr(const char *p, int c)
{
	char *q = 0, c2;

	do {
		c2 = *p++;
		if (c == c2) {
			q = (char *)p;
		}
	} while (c2);
	return(q ? (q-1) : 0);
}

ulong
strtoul(const char *s, char **ptr, int base)
{
	ulong total = 0;
	uint digit;
	int radix;
	const char *start = s;
	int did_conversion = 0;
	int overflow = 0;
	int minus = 0;
	ulong maxdiv, maxrem;

	if (s == NULL) {
		/*__seterr(ERANGE);*/
		if (!ptr) {
			*ptr = (char *) start;
		}
		return(0L);
	}

	while (Isspace (*s)) {
		s++;
	}

	if (*s == '-') {
		s++;
		minus = 1;
	} else if (*s == '+') {
		s++;
	}

	radix = base;
	if (base == 0 || base == 16) {
		/*
		 * try to infer radix from the string (assume decimal).
		 * accept leading 0x or 0X for base 16.
		 */
		if (*s == '0') {
			did_conversion = 1;
			if (base == 0) {
				radix = 8;	/* guess it's octal */
			}
			s++;			/* (but check for hex) */
			if (*s == 'X' || *s == 'x') {
				did_conversion = 0;
				s++;
				radix = 16;
			}
		}
	}
	if (radix == 0) {
		radix = 10;
	}
	
	maxdiv = ULONG_MAX / radix;
	maxrem = ULONG_MAX % radix;

	while ((digit = *s) != 0) {
		if (digit >= '0' && digit <= '9' && digit < ('0' + radix)) {
			digit -= '0';
		} else if (radix > 10) {
			if (digit >= 'a' && digit < ('a' + radix - 10)) {
				digit = digit - 'a' + 10;
			} else if (digit >= 'A' &&
					digit < ('A' + radix - 10)) {
				digit = digit - 'A' + 10;
			} else {
				break;
			}
		} else {
			break;
		}
		did_conversion = 1;
		if (total > maxdiv || (total == maxdiv && digit > maxrem)) {
			overflow = 1;
		}
		total = (total * radix) + digit;
		s++;
	}
	if (overflow) {
		/*__seterr(ERANGE);*/
		if (ptr != NULL) {
			*ptr = (char *) s;
		}
		return(ULONG_MAX);
	}
	if (ptr != NULL) {
		*ptr = (char *) ((did_conversion) ? (char *) s : start);
	}
	return(minus ? - total : total);
}

long
strtol(const char *s, char **ptr, int base)
{
	int minus = 0;
	ulong tmp;
	const char *start = s;
	char *eptr;

	if (s == NULL) {
		/*__seterr(ERANGE);*/
		if (!ptr) {
			*ptr = (char *) start;
		}
		return(0L);
	}

	while (Isspace (*s)) {
		s++;
	}
	if (*s == '-') {
		s++;
		minus = 1;
	} else if (*s == '+') {
		s++;
	}

	/*
	 * Let strtoul do the hard work.
	 */
	tmp = strtoul(s, &eptr, base);
	if (ptr != NULL) {
		*ptr = (char *)((eptr == s) ? start : eptr);
	}
	if (tmp > (minus ? - (ulong) LONG_MIN : (ulong) LONG_MAX)) {
		/*__seterr(ERANGE);*/
		return(minus ? LONG_MIN : LONG_MAX);
	}
	return(minus ? (long) -tmp : (long) tmp);
}

int
atoi(const char *p)
{
	return((int)strtol(p, (char **)NULL, 10));
}

long
atol(const char *p)
{
	return(strtol(p, (char **)NULL, 10));
}

char *
strlwr(char *s)
{
	char *pstr = s;

	while(*pstr != '\0') {
		*pstr = tolower(*pstr);
		pstr++;
	}

	return(s);
}

char *
strupr(char *s)
{
	char *pstr = s;

	while(*pstr != '\0') {
		*pstr = toupper(*pstr);
		pstr++;
	}

	return(s);
}

void*
memset(void* dest, int c, size_t count)
{
	if(count) {
		char* d = dest;
		do {
			*d++ = c;
		} while(--count);
	}
	return(dest);
}

#ifndef __GNUC__

void*
memcpy(void* dest, const void* src, size_t count)
{
	bcopy(src, dest, count);
	return(dest);
}

#endif
	