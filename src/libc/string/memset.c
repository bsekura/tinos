#include <string.h>

void*
memset(void* dest, int c, size_t count)
{
   if(count) {
      char* d = dest;
      do {
         *d++ = c;
      } while(--count);
   }
   return(dest);
}
