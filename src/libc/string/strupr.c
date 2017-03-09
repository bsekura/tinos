/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
#include <ctype.h>

char *
strupr(char *s)
{
   char *pstr = s;

   while(*pstr != '\0') {
      *pstr = toupper(*pstr);
      pstr++;
   }

   return(s);
}
