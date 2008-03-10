/* Copyright (C) 1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@arthur.rhein-neckar.de>, 1999.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <errno.h>
#include <malloc.h>
#include <stdio.h>

#define merror(msg) return msg;

char* tst_malloc (void)
{
  void *p, *q;
  int save;

  errno = 0;

  p = malloc (-1);
  save = errno;

  if (p != NULL)
    merror ("1: malloc (-1) succeeded.");

  if (p == NULL && save != ENOMEM)
    merror ("2: errno is not set correctly");

  p = malloc (10);
  if (p == NULL)
    merror ("3: malloc (10) failed.");

  /* realloc (p, 0) == free (p).  */
  p = realloc (p, 0);
  if (p != NULL)
    merror ("4: realloc (p, 0) failed.");

  p = malloc (0);
  if (p == NULL)
    merror ("5: malloc (0) failed.");

  p = realloc (p, 0);
  if (p != NULL)
    merror ("6: realloc (p, 0) failed.");

  p = malloc (513 * 1024);
  if (p == NULL)
    merror ("7: malloc (513K) failed.");

  q = malloc (-512 * 1024);
  if (q != NULL)
    merror ("8: malloc (-512K) succeeded.");

  free (p);

  return NULL;
}
