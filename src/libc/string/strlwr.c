/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
#include <ctype.h>

char *
strlwr(char *s)
{
   char *pstr = s;

   while(*pstr != '\0') {
      *pstr = tolower(*pstr);
      pstr++;
   }

   return(s);
}
