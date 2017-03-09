/* 
 * Copyright (C) 1994 DJ Delorie
 */
#include <stdlib.h>

long double
atold(const char *ascii)
{
  return strtold(ascii, 0);
}
