/* ffs -- find first set bit in a word, counted from least significant end.
   For Intel 80x86, x>=3.
   Copyright (C) 1991, 1992 Free Software Foundation, Inc.
   Contributed by Torbjorn Granlund (tege@sics.se).

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <string.h>

int
ffs(int x)
{
  int cnt;
  int tmp;

  asm ("xorl %0,%0\n"		/* Set CNT to zero.  */
       "bsfl %2,%1\n"		/* Count low bits in X and store in %1.  */
       "jz 1f\n"		/* Jump if OK, i.e. X was non-zero.  */
       "leal 1(%3),%0\n"	/* Return bsfl-result plus one on %0.  */
       "1:" : "=a" (cnt), "=r" (tmp) : "rm" (x), "1" (tmp));

  return cnt;
}
