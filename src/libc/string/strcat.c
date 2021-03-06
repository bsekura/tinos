/* 
 * Copyright (C) 1994 DJ Delorie
 */
#include <string.h>

char *
strcat(char *s, const char *append)
{
  char *save = s;

  for (; *s; ++s);
  while ((*s++ = *append++));
  return save;
}
