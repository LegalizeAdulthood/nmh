
/*
 * whatnow.c -- the nmh `WhatNow' shell
 *
 * This code is Copyright (c) 2002, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <libmh.h>

int
main (int argc, char **argv)
{
    if (nmh_init(argv[0], 1)) { return 1; }

    return WhatNow (argc, argv);
}
