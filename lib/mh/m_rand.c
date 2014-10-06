/*
 * m_rand -- provides pseudorandom bytes
 *
 * This code is Copyright (c) 2012, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <libmh.h>

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

static long seeded = 0L;

int
m_rand (unsigned char *buf, size_t n) {

  if (! seeded) {
    FILE *devurandom;
    unsigned int seed;

    if ((devurandom = fopen ("/dev/urandom", "r"))) {
      if (fread (&seed, sizeof (seed), 1, devurandom) == 1) seeded = 1;
      fclose (devurandom);
    }

    if (! seeded) {
      /* This seed calculation is from Helmut G. Katzgraber, "Random
         Numbers in Scientific Computing:  An Introduction",
         arXiv:1005.4117v1 [physics.comp-ph], 22 May 2010, p. 19.
         time() and getpid() shouldn't fail on POSIX platforms. */
      seed = abs ((int) ((time (0) * 181 * ((getpid ()-83) * 359)) % 104729));
      seeded = 1;
    }

    srand (seed);
  }

  while (n > 0) {
    int rnd = rand ();
    unsigned char *rndp = (unsigned char *) &rnd;
    unsigned int i;

    for (i = 0; i < sizeof rnd  &&  n > 0;  ++i, --n) {
      *buf++ = *rndp++;
    }
  }

  return 0;
}
