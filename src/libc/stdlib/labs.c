/* 
 * Copyright (C) 1994 DJ Delorie
 *
 */
#include <stdlib.h>

long
labs(long c)
{
  return c < 0 ? -c : c;
}
