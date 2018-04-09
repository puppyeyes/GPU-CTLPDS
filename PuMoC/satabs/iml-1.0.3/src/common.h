/* ---------------------------------------------------------------------
 *
 * -- Integer Matrix Library (IML)
 *    (C) Copyright 2004, 2006 All Rights Reserved
 *
 * -- IML routines -- Version 1.0.1 -- November, 2006
 *
 * Author         : Zhuliang Chen
 * Contributor(s) : Arne Storjohann
 * University of Waterloo -- School of Computer Science
 * Waterloo, Ontario, N2L3G1 Canada
 *
 * ---------------------------------------------------------------------
 *
 * -- Copyright notice and Licensing terms:
 *
 *  Redistribution  and  use in  source and binary forms, with or without
 *  modification, are  permitted provided  that the following  conditions
 *  are met:
 *
 * 1. Redistributions  of  source  code  must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce  the above copyright
 *    notice,  this list of conditions, and the  following disclaimer in
 *    the documentation and/or other materials provided with the distri-
 *    bution.
 * 3. The name of the University,  the IML group,  or the names of its
 *    contributors  may not be used to endorse or promote products deri-
 *    ved from this software without specific written permission.
 *
 * -- Disclaimer:
 *
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO,  PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO-
 * RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (IN-
 * CLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */



/*
 *  common.h provides the basic library header include and memory management
 *  functions. 
 *
 *  It is written based on the examples from Gary V. Vaughan's Autobook.
 *
 */

#ifndef IML_COMMON_H
#define IML_COMMON_H 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>

#if HAVE_MATH_H
#  include <math.h>
#endif

#if STDC_HEADERS || HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#if HAVE_TIME_H
#  include <time.h>
#endif

#if defined __CYGWIN32__ && !defined __CYGWIN__
#  define __CYGWIN__ __CYGWIN32__
#endif
#if defined __CYGWIN__ && !defined WIN32
#  define WIN32
#endif

#ifdef __GNUC__
#  ifndef const
#    define const       __const
#  endif
#  ifndef signed
#    define signed      __signed
#  endif
#else
#  ifdef __STDC__
#    undef  signed
#    define signed
#  endif
#endif

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS  0
#  define EXIT_FAILURE  1
#endif

#if !HAVE_BZERO && HAVE_MEMSET
# define bzero(buf, bytes)    ((void) memset (buf, 0, bytes))
#endif

#define XCALLOC(type, num)                                  \
        ((type *) xcalloc ((num), sizeof(type)))
#define XMALLOC(type, num)                                  \
        ((type *) xmalloc ((num) * sizeof(type)))
#define XREALLOC(type, p, num)                              \
        ((type *) xrealloc ((p), (num) * sizeof(type)))
#define XFREE(stale)                            do {        \
        if (stale) { free (stale);  stale = 0; }            \
                                                } while (0)

void *xcalloc  (size_t num, size_t size);
void *xmalloc  (size_t num);
void *xrealloc (void *p, size_t num);

#endif /* !IML_COMMON_H */

