#include <stdlib.h>

long
atol(const char *str)
{
  return strtol(str, 0, 10);
}
