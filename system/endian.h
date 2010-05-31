/*
 * Modification History
 *
 * 2002-May-25   Jason Rohrer
 * Created.
 *
 * 2004-January-12   Jason Rohrer
 * Added support for metrowerks win32 compiler.
 *
 * 2009-April-3   Jason Rohrer
 * OpenBSD support.
 */

#include "minorGems/common.h"



/**
 * Include this file to define __BYTE_ORDER
 *
 * After this has been included, __BYTE_ORDER will be either
 * __LITTLE_ENDIAN  or
 * __BIG_ENDIAN
 */



#ifdef __FreeBSD__
#include <machine/endian.h>


#elif defined(__NetBSD__)
#include <sys/endian.h>


#elif defined(__OpenBSD__)
#include <sys/types.h>
#include <machine/endian.h>


// default BSD case
#elif defined(BSD)
#include <machine/endian.h>



#elif defined(SOLARIS)
// Code for Solaris defs adapted from:
// MD5 message-digest algorithm.
// by Colin Plumb in 1993, no copyright is claimed.

//each solaris is different -- this won't work on 2.6 or 2.7
# include <sys/isa_defs.h>

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321

#ifdef _LITTLE_ENDIAN
#define __BYTE_ORDER __LITTLE_ENDIAN

#else // default to big endian
#define __BYTE_ORDER __BIG_ENDIAN
#endif

// end solaris case



#elif defined(WIN_32) || \
      ( defined(__MWERKS__) && defined(__INTEL__) )  // windows case
#define __LITTLE_ENDIAN 1234
#define __BYTE_ORDER __LITTLE_ENDIAN

// end windows case



#else
// linux case
#include <endian.h>

// end linux case



#endif
// end of all system-specific cases





// BSD calls it BYTE_ORDER, linux calls it __BYTE_ORDER
#ifndef __BYTE_ORDER
#define __BYTE_ORDER  BYTE_ORDER
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN  LITTLE_ENDIAN
#endif

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN  BIG_ENDIAN
#endif




