/* 
 * Copyright (C) 1995 DJ Delorie
 */
#include <string.h>

int
bcmp(const void *ptr1, const void *ptr2, int len)
{
   return (memcmp(ptr1, ptr2, len));
}
