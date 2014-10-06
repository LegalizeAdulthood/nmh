
/*
 * copy.c -- copy a string and return pointer to NULL terminator
 *
 * This code is Copyright (c) 2002, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <libmh.h>

char *
copy(const char *from, char *to)
{
    while ((*to = *from)) {
	to++;
	from++;
    }
    return (to);
}
