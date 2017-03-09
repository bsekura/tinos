#include <string.h>

void*
memcpy(void* dest, const void* src, size_t count)
{
   bcopy(src, dest, count);
   return(dest);
}
