/*
 * Modification History
 *
 * 2000-October-14		Jason Rohrer
 * Created.
 *
 * 2001-January-27		Jason Rohrer
 * Converted to use MutexLock and added to minorGems source tree.
 * Changed tprintf to be static (the mutexes don't work otherwise).
 * Now we're closing the argument list.
 * Fixed so that it works with any number of arguments.
 * Changed name of print function to printf.
 *
 * 2004-March-31   Jason Rohrer
 * Fixed static memory leak.
 */

#include "minorGems/common.h"



#ifndef THREAD_SAFE_PRINTER_INCLUDED
#define THREAD_SAFE_PRINTER_INCLUDED

#include "MutexLock.h"
#include <stdio.h>

// for variable argument lists
#include <stdarg.h>

/**
 * Thread safe printf function.  Note that printf is actually thread safe 
 * anyway, so this is just to demonstrate and test locks.  It seems as
 * though printf _isn't_ thread safe on certain platforms, so this class
 * may be useful.
 *
 * @author Jason Rohrer
 */
class ThreadSafePrinter {

	public:
		
		static int printf( const char* inFormatString, ... );

	private:
		static MutexLock sLock;

	};

// initialize static members
MutexLock ThreadSafePrinter::sLock;	

inline int ThreadSafePrinter::printf( const char*inFormatString, ... ) {
		
	va_list argList;
	va_start( argList, inFormatString );
	
	sLock.lock();
	
	int returnVal = vprintf( inFormatString, argList );
	fflush( stdout );
	
	sLock.unlock();
	
	va_end( argList );

	return returnVal;
	}


#endif
