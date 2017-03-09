/* 
 * Copyright (C) 1995 DJ Delorie
 */
#include <string.h>

char *
stpcpy(char *_dest, const char *_src)
{
  if (!_dest || !_src)
    return 0;
  while ((*_dest++ = *_src++));
  return --_dest;
}
