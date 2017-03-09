/* 
 * Copyright (C) 1994 DJ Delorie
 */
#include <string.h>

char *
strcpy(char *to, const char *from)
{
  char *save = to;

  for (; (*to = *from); ++from, ++to);
  return save;
}
