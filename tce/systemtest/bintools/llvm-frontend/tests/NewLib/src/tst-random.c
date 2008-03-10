/* Test program for random(), srandom(), initstate(), setstate()
   Written by Michael J. Fischer, August 21, 2000
   Placed in the public domain.  */

/* This program primarily tests the correct functioning of srandom()
   and setstate().  The strategy is generate and store a set of random
   sequences, each with a specified starting seed.  Then each sequence
   is regenerated twice and checked against the stored values.

   First they are regenerated one sequence at a time, using srandom()
   to set the initial state.  A discrepency here would suggest that
   srandom() was failing to completely initialize the random number
   generator.

   Second the sequences are regenerated in an interleaved order.
   A state vector is created for each sequence using initstate().
   setstate() is used to switch from sequence to sequence during
   the interleaved generation.  A discrepency here would suggest
   a problem with either initstate() failing to initialize the
   random number generator properly, or the failure of setstate()
   to correctly save and restore state information.  Also, each
   time setstate() is called, the returned value is checked for
   correctness (since we know what it should be).

   Note:  We use default state vector for sequence 0 and our own
   state vectors for the remaining sequences.  This is to give a check
   that the value returned by initstate() is valid and can indeed be
   used in the future.  */

/* Strategy:
   1.  Use srandom() followed by calls on random to generate a set of
       sequences of values.
   2.  Regenerate and check the sequences.
   3.  Use initstate() to create new states.
   4.  Regenerate the sequences in an interleaved manner and check.
*/

/*
 * Modified to test newlib rand and srand
 */

#include <stdlib.h>
#include <stdio.h>

const int degree = 128;		/* random number generator degree (should
				   be one of 8, 16, 32, 64, 128, 256) */
const int nseq = 3;		/* number of test sequences */
const int nrnd = 50;		/* length of each test sequence */
const unsigned int seed[3] = { 0x12344321U, 0xEE11DD22U, 0xFEDCBA98 };

char* fail (const char *msg, int s, int i);

char*
tst_random (void)
{
  long int rnd[nseq][nrnd];	/* pseudorandom numbers */
  int s;			/* sequence index */
  int i;			/* element index */

/*  printf ("Begining random package test using %d sequences of length %d.\n",
    nseq, nrnd); */

  /* 1. Generate and store the sequences.  */
/*  printf ("Generating random sequences.\n"); */
  for (s = 0; s < nseq; ++s)
    {
        srand ( seed[s] );
        for (i = 0; i < nrnd; ++i)
            rnd[s][i] = rand ();
    }
  
  /* 2. Regenerate and check.  */
/*  printf ("Regenerating and checking sequences.\n"); */
  for (s = 0; s < nseq; ++s)
  {
      srand (seed[s]);
      for (i = 0; i < nrnd; ++i)
          if (rnd[s][i] != rand ())
              return fail ("first regenerate test", s, i);
  }
  
  return NULL;
}

char*
fail (const char *msg, int s, int i)
{
    static char buf[100];
    snprintf (buf, sizeof(buf), "%s (seq %d, pos %d).\n", msg, s, i);
    return buf;
}
