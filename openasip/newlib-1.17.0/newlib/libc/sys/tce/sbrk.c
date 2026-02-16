/* Version of sbrk for no operating system.  */

#include <_syslist.h>

void *
_sbrk (int incr)
{ 
   extern char   _end; /* Set by linker.  */
   static char * heap_end; 
   char *        prev_heap_end; 

   if (heap_end == 0)
     heap_end = &_end; 

   prev_heap_end = heap_end; 
   heap_end += incr; 

   return (void *) prev_heap_end; 
} 
