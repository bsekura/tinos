#include <stdlib.h>

double
atof(const char *ascii)
{
  return strtod(ascii, 0);
}
