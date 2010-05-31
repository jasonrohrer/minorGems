/*
 * Modification History
 *
 * 2002-April-8   Jason Rohrer
 * Created.
 */

#include "minorGems/common.h"



#ifndef PRINT_UTILS_INCLUDED
#define PRINT_UTILS_INCLUDED



/**
 * A thread-safe version of printf.
 *
 * Note that printf is already thread-safe on certain platforms,
 * but does not seem to be thread-safe on Win32.
 *
 * @param inFormatString the format string to use.
 * @param ...  a variable argument list, with the same usage
 *   pattern as printf.
 */
int threadPrintF( const char* inFormatString, ... );



#endif
